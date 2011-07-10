#  ***** GPL LICENSE BLOCK *****
#
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>.
#  All rights reserved.
#  ***** GPL LICENSE BLOCK *****

####------------------------------------------------------------------------------------------------------------------------------------------------------
#### HEADER
####------------------------------------------------------------------------------------------------------------------------------------------------------

bl_info = {
    "name"              : "SHIFT - Export",
    "author"            : "BBC",
    "version"           : (1,0),
    "blender"           : (2, 5, 3),
    "api"               : 31236,
    "category"          : "Object",
    "location"          : "Tool Shelf",
    "warning"           : '',
    "wiki_url"          : "",
    "tracker_url"       : "",
    "description"       : "SHIFT Exporter"}

import io
import os
import bpy
import sys
import math
import time
import cmath
import struct
import shutil
import ctypes
import random
import mathutils

from math       import radians
from ctypes     import *
from mathutils  import *
from bpy.props  import *

####------------------------------------------------------------------------------------------------------------------------------------------------------
#### FUNCTIONS
####------------------------------------------------------------------------------------------------------------------------------------------------------

####------------------------------------------------------------------------------------------------------------------------------------------------------
#### STRIP
####------------------------------------------------------------------------------------------------------------------------------------------------------

def strip (faces, onesided, onestrip):

    adj_faces = []
    adj_edges = []

    count = len (faces)

    # BUILDING BASIC FACE AND EDGE INFO
    
    for i, f in enumerate (faces):

        # vertex indices from input
        ref0 = f [0];   ref1 = f [1];   ref2 = f [2];

        # [0][1][2] are vertices
        # [3][4][5] are adjanced faces
        # [6][7][8] each one of (0,1,2) to connect adjanced faces with what edge
        adj_faces.append ([ref0, ref1, ref2, -1, -1, -1, -1, -1, -1])

        # we write edges vertex indices ordered (first lower than second)
        # this helps comparing edges (we dont have to check opposite direction)
        
        # [0] - first vertex index
        # [1] - second vertex index
        # [2] - owner face index (adj_faces)
        if (ref0 < ref1):   adj_edges.append ((ref0, ref1, i))
        else:               adj_edges.append ((ref1, ref0, i))
        if (ref0 < ref2):   adj_edges.append ((ref0, ref2, i))
        else:               adj_edges.append ((ref2, ref0, i))
        if (ref1 < ref2):   adj_edges.append ((ref1, ref2, i))
        else:               adj_edges.append ((ref2, ref1, i))

    # COLLECTING ADJACENT FACES AND CONNECTION INFO

    # one more dummy at the end
    # we need it for adding last edge in the loop below
    adj_edges.append ((-1, -1, -1))

    # sorting edges in order ref0 -> ref1
    adj_edges.sort (key = lambda adj_edges: adj_edges [1])
    adj_edges.sort (key = lambda adj_edges: adj_edges [0])

    # first edge [0]
    prevref0 = adj_edges [0][0]
    prevref1 = adj_edges [0][1]

    # face of first edge
    tmp = [adj_edges [0][2], 0, 0]; c = 1

    # look for similar edges
    for i in range (1, len (adj_edges)):

        ref0 = adj_edges [i][0]	# vertex ref #1
        ref1 = adj_edges [i][1]	# vertex ref #2
        face = adj_edges [i][2]	# owner face

        # is this edge same as previous one ?
        # all equal edges have same order of indices so we dont have to check other direction
        if  (ref0 == prevref0 and ref1 == prevref1):

            # more than 2 equal edges !
            if (c > 2):
                
                # we dont support this case 3 or more faces sharing the same edge

                # clean up
                adj_faces [:] = []
                adj_edges [:] = []

                # only works with manifold meshes (i.e. an edge is not shared by more than 2 triangles)
                return - 1

            # we save face reference and advance count of equal edges
            tmp [c] = face;	c += 1

        # this edge is different, so no more of last edge        
        # there are 2 equal edges so we have our adjanced face
        elif (c == 2):

            # these triangles have two edges which are equal 
            tri0 = adj_faces [tmp [0]]
            tri1 = adj_faces [tmp [1]]

            # edge 0
            if   (tri0 [0] == prevref0 and tri0 [1] == prevref1):   edge0 = 0
            elif (tri0 [0] == prevref1 and tri0 [1] == prevref0):   edge0 = 0
            elif (tri0 [0] == prevref0 and tri0 [2] == prevref1):   edge0 = 1
            elif (tri0 [0] == prevref1 and tri0 [2] == prevref0):   edge0 = 1
            elif (tri0 [1] == prevref0 and tri0 [2] == prevref1):   edge0 = 2
            elif (tri0 [1] == prevref1 and tri0 [2] == prevref0):   edge0 = 2

            # edge 1
            if   (tri1 [0] == prevref0 and tri1 [1] == prevref1):   edge1 = 0
            elif (tri1 [0] == prevref1 and tri1 [1] == prevref0):   edge1 = 0
            elif (tri1 [0] == prevref0 and tri1 [2] == prevref1):   edge1 = 1
            elif (tri1 [0] == prevref1 and tri1 [2] == prevref0):   edge1 = 1
            elif (tri1 [1] == prevref0 and tri1 [2] == prevref1):   edge1 = 2
            elif (tri1 [1] == prevref1 and tri1 [2] == prevref0):   edge1 = 2

            # set both triangles ref. to adj. triangle plus which face is connecting them
            # we save the reference of other triangle 
            tri0 [3 + edge0] = tmp [1]; tri0 [6 + edge0] = edge1
            tri1 [3 + edge1] = tmp [0]; tri1 [6 + edge1] = edge0

            # reset for next edge
            tmp [0] = face;  c = 1

        # we have only one boundary edge
        elif (c == 1):

            # no adjancency here
            
            # reset for next edge
            tmp [0] = face;  c = 1
            
        prevref0 = ref0
        prevref1 = ref1

    # clean up
    adj_edges [:] = []

    # FACE USAGE TAGS AND CONECTIVITY

    # tags contains True if the face has already been included in a strip
    tags = [False for i in range (count)]

    # number of connections for each face, second value is index of face
    connectivity = [[0, i] for i in range (count)]

    # we use conectivity information to help to start with boundary faces (which have less connections)

    # compute number of adjacent triangles for each face
    for i in range (count):

        t = adj_faces [i]

        # if index of adjanced face is -1 we have less connections
        if (t [3] != - 1): connectivity [i][0] += 1
        if (t [4] != - 1): connectivity [i][0] += 1
        if (t [5] != - 1): connectivity [i][0] += 1

    # sorting faces by number of connections
    connectivity.sort (key = lambda connectivity: connectivity [0])         ###, reverse = True)

    # BUILDING STRIPS

    index   = 0     # index of first face in 'connectivity'
    done    = 0     # count of faces already transformed into strips

    strips  = []    # list of strips

    # three temporary strips plus their faces (to mark them later as used)
    
    tmpfaces  = [[],[],[]]
    tmpstrips = [[],[],[]]
    
    # we work until we used all faces (count)
    while (done < count):

        # searching for face that was not alerady added into strip (tag = False)
        while ((index < count) and tags [connectivity [index][1]]):	index += 1

        # index of our first face
        first = connectivity [index][1]

        # our first face tuple
        firstface = adj_faces [first]
        
        # vertices of edges
        # for every one of tree strips we use different starting edge
        refs0 = (firstface [0], firstface [2], firstface [1])
        refs1 = (firstface [1], firstface [0], firstface [2])

        firstlengths = [0,0,0]   # lengths of first parts 

        # we will generate 3 different strips and later we choose best one and use that
        
        # compute 3 strips
        for j in range (3):

            # create a local copy of the tags
            tmptags = list (tags)

            def trackstrip (face, oldest, middle, strip, faces):

                strip.append (oldest)	# First vertex index of the strip
                strip.append (middle)   # Second vertex index of the strip

                loop = True

                while (loop):

                    # Get the third index of a face, given two of them
                    t = adj_faces [face]

                    if   (t [0] == oldest and t [1] == middle):	newest = t [2]
                    elif (t [0] == middle and t [1] == oldest):	newest = t [2]
                    elif (t [0] == oldest and t [2] == middle):	newest = t [1]
                    elif (t [0] == middle and t [2] == oldest):	newest = t [1]
                    elif (t [1] == oldest and t [2] == middle):	newest = t [0]
                    elif (t [1] == middle and t [2] == oldest):	newest = t [0]

                    # Get the edge ID and use it to catch the link to adjacent face.
                    if   (t [0] == middle and t [1] == newest):	edge = 0
                    elif (t [0] == newest and t [1] == middle):	edge = 0
                    elif (t [0] == middle and t [2] == newest):	edge = 1
                    elif (t [0] == newest and t [2] == middle):	edge = 1
                    elif (t [1] == middle and t [2] == newest):	edge = 2
                    elif (t [1] == newest and t [2] == middle):	edge = 2

                    # adding new vertex
                    strip.append (newest)
                    
                    # adding new face (one vertex -> one new face)
                    faces.append (face)

                    # tagging this new face as used
                    tmptags [face] = True

                    # adjanced face index on 'edge' side
                    link = t [3 + edge]

                    # If the face is no more connected, we're done...
                    # - 1 means that adjanced face was not set
                    if (link == - 1):   loop = False

                    # link gives us the new face index.
                    else:

                        face = link
                        
                        # if this new face was already added into strip we are done
                        if (tmptags [face]): loop = False

                    # shift the indices and wrap
                    oldest = middle
                    middle = newest

            # we process each strip in two direction from the same edge and thats why is dividen into two 'parts'
            
            # track first part of the strip
            trackstrip (first, refs0 [j], refs1 [j], tmpstrips [j], tmpfaces [j])

            l = len (tmpstrips [j]);    firstlengths [j] = l
            
            # we reverse strip so first face will be last
            # reverse first part of the strip
            tmpstrips [j].reverse ()
            tmpfaces  [j].reverse ()

            nref0 = tmpstrips [j][l - 3]
            nref1 = tmpstrips [j][l - 2]

            # we remove last ('first') face indices cause they are going to be added again
            tmpstrips [j].pop ()
            tmpstrips [j].pop ()
            tmpstrips [j].pop ()
            tmpfaces  [j].pop ()
                                    
            # track second part of the strip
            # we start from the same face than before but vertex indices are in reversed order (going other direction from the same starting edge)
            # we only add faces that has not been tagged (as used)
            trackstrip (first, nref0, nref1, tmpstrips [j], tmpfaces [j])
        
        # we use longest strip
        # the longer strip the better
        # shorter strips can leave more isolated faces
        # our face count is always the same so we want faces as much as possible in one continuous strip
        longest	= len (tmpfaces [0]);      best = 0
        
        if (len (tmpfaces [1]) > longest): longest = len (tmpfaces [1]);  best = 1
        if (len (tmpfaces [2]) > longest): longest = len (tmpfaces [2]);  best = 2

        # number of faces added
        done += longest

        # for each face we use (from selected best strip) we tag them as used
        for j in range (len (tmpfaces [best])):    tags [tmpfaces [best][j]] = True

        # flip strip if needed (if the length of the first part of the strip is odd, the strip must be reversed)
        if (onesided and firstlengths [best] & 1):

            # reversing strip
            tmpstrips [best].reverse ()

            # do we have more faces ?
            if (longest > 1):

                # if the position of the original face in this new reversed strip is odd, you're done
                npos = longest - firstlengths [best]

                # if the position of the original face in this new reversed strip is even, replicate the first index
                if (npos & 1):

                    # adding one more index at the end (which is now start cause we reversed order) of strip to close it
                    tmpstrips [best].insert (0, tmpstrips [best][0])

        # copy best strip in the strip buffers
        strips.append (list (tmpstrips [best]))
    
        # clean up
        tmpfaces  [0][:] = []
        tmpfaces  [1][:] = []
        tmpfaces  [2][:] = []
        tmpstrips [0][:] = []
        tmpstrips [1][:] = []
        tmpstrips [2][:] = []

    # clean up
    tmpstrips    [:] = []
    tmpfaces     [:] = []   
    connectivity [:] = []
    tmptags      [:] = []
    tags         [:] = []

    # link all strips in a single one
    if (onestrip):

        # list to store the single strip
        single = []

        # loop over strips and link them together
        for k in range (len (strips)):
            
            # nothing to do for the first strip, we just copy it
            if (k > 0):
                
                # this is not the first strip, so we must copy two void vertices between the linked strips

                # last added is doubled
                single.append (single [len (single) - 1])

                # double start of this new strip 
                single.append (strips [k][0])

                # linking two strips may flip their culling. If the user asked for single-sided strips we must fix that
                if (onesided):
                    
                    # culling has been inverted only if length is odd
                    if (len (single) & 1):
                        
                        # we can fix culling by replicating the first vertex once again...
                        if (strips [k][0] != strips [k][1]):    single.append (strips [k][0])
                        
                        # ...but if flipped strip already begin with a replicated vertex, we just can skip it.
                        else:   strips [k].pop (0)
                                            
            # copy strip
            for i in strips [k]:    single.append (i)

        # remove strips
        strips [:][:] = []

        return single

    return strips

####------------------------------------------------------------------------------------------------------------------------------------------------------
#### EXPORT MESH
####------------------------------------------------------------------------------------------------------------------------------------------------------

def processExportMesh (mesh, filepath):

    # count quads
    q = 0
    for f in mesh.faces:
        
        try:    f.vertices [3]; q += 1
        except: continue
    
    # primitive mode
    if q > 0:

        if q == len (mesh.faces):   mode = 'QUADS'
        
        else:            
            print ('ERROR | Mesh :', mesh.name, ' containts invalid combination of triangles and quads.')
            return -1
        
    else:   mode = 'TRIANGLES'

    # active scene
    scene = bpy.context.scene

    # name of this mesh
    name = mesh.name

    # material
    try:    mat = mesh.materials [0]
    except: mat = None
    
    # opening new binary file
    stream = io.open (filepath, mode = 'wb')
            
    export_tangents = False
    export_normals  = False
    export_colors   = False
    export_uv1      = False    
    export_uv2      = False
    export_uv3      = False
    
    # export properties
    if mat:
        
        if   'Solid'        in mat.name:
            
            export_tangents = True
            export_normals  = True
            export_uv1      = True

            # morphing use second channel
            try:        mesh ['morph_position'];  export_uv2 = True
            except:     pass
            try:        mesh ['morph_normal'];  export_uv2 = True
            except:     pass
            try:        mesh ['morph_uv'];  export_uv2 = True
            except:     pass
            
        elif 'Terrain'      in mat.name:
            
            export_tangents = True
            export_normals  = True
            export_uv1      = True
            export_uv2      = True
            
            # morphing use third channel
            try:        mesh ['morph_position'];  export_uv3 = True
            except:     pass
            try:        mesh ['morph_normal'];  export_uv3 = True
            except:     pass
            try:        mesh ['morph_uv'];  export_uv3 = True
            except:     pass
            
        elif 'Enviroment'   in mat.name:

            export_uv1      = True
                                            
        elif 'Foliage'      in mat.name:
            
            export_tangents = True
            export_normals  = True
            export_uv1      = True
            
        elif 'Grass'        in mat.name:
        
            export_uv1      = True
    
    # UV precisions

    if (export_uv1):

        export_uv1 = 16
        try:
            if   mesh ['uv1_precision'] == 'hi':
                export_uv1 = 32
            elif mesh ['uv1_precision'] == 'lo':
                export_uv1 = 16
                
        except: pass
        
    if (export_uv2):
        
        export_uv2 = 16
        try:
            if   mesh ['uv2_precision'] == 'hi':
                export_uv2 = 32
            elif mesh ['uv2_precision'] == 'lo':
                export_uv2 = 16
                
        except: pass
        
    if (export_uv3):
        
        export_uv3 = 16
        try:
            if   mesh ['uv3_precision'] == 'hi':
                export_uv3 = 32
            elif mesh ['uv3_precision'] == 'lo':
                export_uv3 = 16
                
        except: pass

    # save ref.
    mesho = mesh

    # make a copy of this mesh
    mesh = mesh.copy ()

    # create temporary object
    tmpo = bpy.data.objects.new ("tempobj", mesh)

    # link temporary object to the scene
    scene.objects.link (tmpo)

    # set active object
    scene.objects.active = tmpo
    
    #------------------------------------------------------------------------------------
    # COLLECTING MESH ELEMENTS OF SHARED VERTICES
    #------------------------------------------------------------------------------------

    # helping class for shared vertices attributes
    class shared:

        def __init__ (self):
            
            self.index       = - 1
            self.smoothing   = False
            
            self.valid       = False
            
            self.vertex      = None
            self.normal      = None

            self.tangent     = None
            self.color       = None
            self.uv1         = None
            self.uv2         = None
            self.uv3         = None

            self.next        = None

    # initializing shared list
    sharedl = []

    for v in mesh.vertices:
        sharedl.append (shared ())
    
    # active vertex color layer
    if export_colors:
        try:    colorlayer = mesh.vertex_colors ['main'].data             
        except:

            print ('ERROR | Mesh :', name, ' missing color channel.')
            return -1

    uv1layer = None
    uv2layer = None
    uv3layer = None

    # uv layer 1
    if export_uv1:
        try:    uv1layer = mesh.uv_textures ['main1'].data
        except:
            try:
                uv1layera = mesh.uv_textures ['main1a'].data
                uv1layerb = mesh.uv_textures ['main1b'].data
            except:
                print ('ERROR | Mesh :', name, ' missing or incomplete uv1 channel.')
                return -1

    # uv layer 2
    if export_uv2:
        try:    uv2layer = mesh.uv_textures ['main2'].data
        except:
            try:
                uv2layera = mesh.uv_textures ['main2a'].data
                uv2layerb = mesh.uv_textures ['main2b'].data
            except:
                print ('ERROR | Mesh :', name, ' missing or incomplete uv2 channel.')
                return -1

    # uv layer 3
    if export_uv3:
        try:    uv3layer = mesh.uv_textures ['main3'].data
        except:
            try:
                uv3layera = mesh.uv_textures ['main3a'].data
                uv3layerb = mesh.uv_textures ['main3b'].data
            except:
                print ('ERROR | Mesh :', name, ' missing or incomplete uv3 channel.')
                return -1
            
    faces = []
    
    if   mode == 'TRIANGLES':
        
        # traversing faces data
        for i, f in enumerate (mesh.faces):

            # face vertices        
            p0 = mesh.vertices [f.vertices [0]]
            p1 = mesh.vertices [f.vertices [1]]
            p2 = mesh.vertices [f.vertices [2]]
        
            # if this vertex node has been already filled with another face, we have to create new one
            s1 = sharedl [f.vertices [0]]
            while (s1.next): s1 = s1.next
            if (s1.valid):
                s1.next = shared (); s1 = s1.next
            s1.vertex      = p0.co; s1.smoothing   = f.use_smooth;  s1.valid       = True

            s2 = sharedl [f.vertices [1]]
            while (s2.next): s2 = s2.next
            if (s2.valid):
                s2.next = shared (); s2 = s2.next
            s2.vertex      = p1.co; s2.smoothing   = f.use_smooth;  s2.valid       = True
        
            s3 = sharedl [f.vertices [2]]
            while (s3.next): s3 = s3.next
            if (s3.valid):
                s3.next = shared (); s3 = s3.next
            s3.vertex      = p2.co; s3.smoothing   = f.use_smooth;  s3.valid       = True
        
            # our face list contains only references to shared elements
            faces.append ((s1,s2,s3))
            
            # collecting face data..                            
            if export_uv1:
                if uv1layer:    s1.uv1 = uv1layer [i].uv1;  s2.uv1 = uv1layer [i].uv2;  s3.uv1 = uv1layer [i].uv3
                else:
                    s1.uv1 = mathutils.Vector ((uv1layera [i].uv1 [0], uv1layera [i].uv1 [1], uv1layerb [i].uv1 [0], uv1layerb [i].uv1 [1]))
                    s2.uv1 = mathutils.Vector ((uv1layera [i].uv2 [0], uv1layera [i].uv2 [1], uv1layerb [i].uv2 [0], uv1layerb [i].uv2 [1]))
                    s3.uv1 = mathutils.Vector ((uv1layera [i].uv3 [0], uv1layera [i].uv3 [1], uv1layerb [i].uv3 [0], uv1layerb [i].uv3 [1]))
            if export_uv2:
                if uv2layer:    s1.uv2 = uv2layer [i].uv1;  s2.uv2 = uv2layer [i].uv2;  s3.uv2 = uv2layer [i].uv3
                else:
                    s1.uv2 = mathutils.Vector ((uv2layera [i].uv1 [0], uv2layera [i].uv1 [1], uv2layerb [i].uv1 [0], uv2layerb [i].uv1 [1]))
                    s2.uv2 = mathutils.Vector ((uv2layera [i].uv2 [0], uv2layera [i].uv2 [1], uv2layerb [i].uv2 [0], uv2layerb [i].uv2 [1]))
                    s3.uv2 = mathutils.Vector ((uv2layera [i].uv3 [0], uv2layera [i].uv3 [1], uv2layerb [i].uv3 [0], uv2layerb [i].uv3 [1]))
            if export_uv3:
                if uv3layer:    s1.uv3 = uv3layer [i].uv1;  s2.uv3 = uv3layer [i].uv2;  s3.uv3 = uv3layer [i].uv3
                else:
                    s1.uv3 = mathutils.Vector ((uv3layera [i].uv1 [0], uv3layera [i].uv1 [1], uv3layerb [i].uv1 [0], uv3layerb [i].uv1 [1]))
                    s2.uv3 = mathutils.Vector ((uv3layera [i].uv2 [0], uv3layera [i].uv2 [1], uv3layerb [i].uv2 [0], uv3layerb [i].uv2 [1]))
                    s3.uv3 = mathutils.Vector ((uv3layera [i].uv3 [0], uv3layera [i].uv3 [1], uv3layerb [i].uv3 [0], uv3layerb [i].uv3 [1]))
                    
            if export_colors:
                s1.color = colorlayer [i].color1
                s2.color = colorlayer [i].color2
                s3.color = colorlayer [i].color3
            
            if export_normals:
                if f.use_smooth:
                    s1.normal = mathutils.Vector (p0.normal)
                    s2.normal = mathutils.Vector (p1.normal)
                    s3.normal = mathutils.Vector (p2.normal)
                else:
                    s1.normal = mathutils.Vector (f.normal)
                    s2.normal = mathutils.Vector (f.normal)
                    s3.normal = mathutils.Vector (f.normal)

            if export_tangents:
                
                # compute the edge and uv differentials                
                dp0  = p1.co - p0.co;   duv0 = s2.uv1 - s1.uv1
                dp1  = p2.co - p0.co;   duv1 = s3.uv1 - s1.uv1            

                # denominator
                denom = duv0 [1] * duv1 [0] - duv0 [0] * duv1 [1]

                # compute tangent
                if (denom < 0.0):   tangent = dp0 * duv1 [1] - dp1 * duv0 [1]
                else:               tangent = dp1 * duv0 [1] - dp0 * duv1 [1]

                # normalize
                if (tangent.length != 0.0): tangent.normalize ()

                tangent = mathutils.Vector ((1.0, 1.0, 1.0))

                s1.tangent = tangent
                s2.tangent = tangent
                s3.tangent = tangent
                
    elif mode == 'QUADS':

        # traversing faces data
        for i, f in enumerate (mesh.faces):
            
            # face vertices        
            p0 = mesh.vertices [f.vertices [0]]
            p1 = mesh.vertices [f.vertices [1]]
            p2 = mesh.vertices [f.vertices [2]]
            p3 = mesh.vertices [f.vertices [3]]

            # if this vertex node has been already filled with another face, we have to create new one
            s1 = sharedl [f.vertices [0]]
            while (s1.next): s1 = s1.next
            if (s1.valid):
                s1.next = shared (); s1 = s1.next
            s1.vertex      = p0.co;
            s1.smoothing   = f.use_smooth;
            s1.valid       = True

            s2 = sharedl [f.vertices [1]]
            while (s2.next): s2 = s2.next
            if (s2.valid):
                s2.next = shared (); s2 = s2.next
            s2.vertex      = p1.co;
            s2.smoothing   = f.use_smooth;
            s2.valid       = True
        
            s3 = sharedl [f.vertices [2]]
            while (s3.next): s3 = s3.next
            if (s3.valid):
                s3.next = shared (); s3 = s3.next
            s3.vertex      = p2.co;
            s3.smoothing   = f.use_smooth;
            s3.valid       = True
            
            s4 = sharedl [f.vertices [3]]
            while (s4.next): s4 = s4.next
            if (s4.valid):
                s4.next = shared (); s4 = s4.next
            s4.vertex      = p3.co;
            s4.smoothing   = f.use_smooth;
            s4.valid       = True
            
            # our face list contains only references to shared elements
            faces.append ((s1,s2,s3,s4))

            # collecting face data..                            
            if export_uv1:
                if uv1layer:    s1.uv1 = uv1layer [i].uv1;  s2.uv1 = uv1layer [i].uv2;  s3.uv1 = uv1layer [i].uv3;  s4.uv1 = uv1layer [i].uv4                
                else:
                    s1.uv1 = mathutils.Vector ((uv1layera [i].uv1 [0], uv1layera [i].uv1 [1], uv1layerb [i].uv1 [0], uv1layerb [i].uv1 [1]))
                    s2.uv1 = mathutils.Vector ((uv1layera [i].uv2 [0], uv1layera [i].uv2 [1], uv1layerb [i].uv2 [0], uv1layerb [i].uv2 [1]))
                    s3.uv1 = mathutils.Vector ((uv1layera [i].uv3 [0], uv1layera [i].uv3 [1], uv1layerb [i].uv3 [0], uv1layerb [i].uv3 [1]))
                    s4.uv1 = mathutils.Vector ((uv1layera [i].uv4 [0], uv1layera [i].uv4 [1], uv1layerb [i].uv4 [0], uv1layerb [i].uv4 [1]))
            if export_uv2:
                if uv2layer:    s1.uv2 = uv2layer [i].uv1;  s2.uv2 = uv2layer [i].uv2;  s3.uv2 = uv2layer [i].uv3;  s4.uv2 = uv2layer [i].uv4
                else:
                    s1.uv2 = mathutils.Vector ((uv2layera [i].uv1 [0], uv2layera [i].uv1 [1], uv2layerb [i].uv1 [0], uv2layerb [i].uv1 [1]))
                    s2.uv2 = mathutils.Vector ((uv2layera [i].uv2 [0], uv2layera [i].uv2 [1], uv2layerb [i].uv2 [0], uv2layerb [i].uv2 [1]))
                    s3.uv2 = mathutils.Vector ((uv2layera [i].uv3 [0], uv2layera [i].uv3 [1], uv2layerb [i].uv3 [0], uv2layerb [i].uv3 [1]))
                    s4.uv2 = mathutils.Vector ((uv2layera [i].uv4 [0], uv2layera [i].uv4 [1], uv2layerb [i].uv4 [0], uv2layerb [i].uv4 [1]))
            if export_uv3:
                if uv3layer:    s1.uv3 = uv3layer [i].uv1;  s2.uv3 = uv3layer [i].uv2;  s3.uv3 = uv3layer [i].uv3;  s4.uv3 = uv3layer [i].uv4
                else:
                    s1.uv3 = mathutils.Vector ((uv3layera [i].uv1 [0], uv3layera [i].uv1 [1], uv3layerb [i].uv1 [0], uv3layerb [i].uv1 [1]))
                    s2.uv3 = mathutils.Vector ((uv3layera [i].uv2 [0], uv3layera [i].uv2 [1], uv3layerb [i].uv2 [0], uv3layerb [i].uv2 [1]))
                    s3.uv3 = mathutils.Vector ((uv3layera [i].uv3 [0], uv3layera [i].uv3 [1], uv3layerb [i].uv3 [0], uv3layerb [i].uv3 [1]))
                    s4.uv3 = mathutils.Vector ((uv3layera [i].uv4 [0], uv3layera [i].uv4 [1], uv3layerb [i].uv4 [0], uv3layerb [i].uv4 [1]))
                    
            if export_colors:
                s1.color = colorlayer [i].color1
                s2.color = colorlayer [i].color2
                s3.color = colorlayer [i].color3
                s4.color = colorlayer [i].color4
            
            if export_normals:
                if f.use_smooth:
                    s1.normal = mathutils.Vector (p0.normal)
                    s2.normal = mathutils.Vector (p1.normal)
                    s3.normal = mathutils.Vector (p2.normal)
                    s4.normal = mathutils.Vector (p3.normal)
                else:
                    s1.normal = mathutils.Vector (f.normal)
                    s2.normal = mathutils.Vector (f.normal)
                    s3.normal = mathutils.Vector (f.normal)
                    s4.normal = mathutils.Vector (f.normal)

            if export_tangents:
                
                # compute the edge and uv differentials                
                dp0  = p1.co - p0.co;   duv0 = s2.uv1 - s1.uv1
                dp1  = p2.co - p0.co;   duv1 = s3.uv1 - s1.uv1            

                # denominator
                denom = duv0 [1] * duv1 [0] - duv0 [0] * duv1 [1]

                # compute tangent
                if (denom < 0.0):   tangent = dp0 * duv1 [1] - dp1 * duv0 [1]
                else:               tangent = dp1 * duv0 [1] - dp0 * duv1 [1]

                # normalize
                if (tangent.length != 0.0): tangent.normalize ()

                tangent = mathutils.Vector ((1.0, 1.0, 1.0))

                s1.tangent = tangent
                s2.tangent = tangent
                s3.tangent = tangent
                s4.tangent = tangent
            
    #------------------------------------------------------------------------------------
    # SMOOTHING AND MERGEING SEPARATED VERTICES BY DATA
    #------------------------------------------------------------------------------------

    index = 0

    for v in sharedl:
        node = v
        while node:
            if node.valid:
                
                # count of merged elements
                counter = 1

                snode = v
        
                # searching for nodes with the same values
                while (snode):

                    # looking for node to merge and smooth with
                    if ((snode.index < 0) and (snode != node)):

                        if (not export_normals) or (export_normals and (snode.smoothing == node.smoothing) and \
                                (snode.smoothing or ((not snode.smoothing) and (snode.normal.dot (node.normal) > 0.999)))):

                            # Checking UVs
                            if (((not export_uv1) or (export_uv1 and (snode.uv1 - node.uv1).length < 0.001)) and \
                                ((not export_uv2) or (export_uv2 and (snode.uv2 - node.uv2).length < 0.001)) and \
                                ((not export_uv3) or (export_uv3 and (snode.uv3 - node.uv3).length < 0.001))):

                                # normal accumulation
                                if export_normals:
                                    node.normal  [0] += snode.normal  [0]
                                    node.normal  [1] += snode.normal  [1]
                                    node.normal  [2] += snode.normal  [2]

                                # color accumulation
                                if export_colors:
                                    node.color [0] += snode.color [0]
                                    node.color [1] += snode.color [1]
                                    node.color [2] += snode.color [2]

                                # setting reference to new index
                                snode.index = index

                                # marking as deleted
                                snode.valid = False
                                
                                # advancing counter
                                counter += 1
                    
                    # next node
                    snode = snode.next

                # average color
                if export_colors:
                    node.color [0] /= counter
                    node.color [1] /= counter
                    node.color [2] /= counter

                # advancing index
                node.index = index; index += 1;

                # we have to check if vector is zero length, otherwise we get wrong values
                
                # normalizing
                if export_normals:
                    if (node.normal.length  != 0.0): node.normal.normalize ()

                # normalizing
                if export_tangents:
                    
                    # now we are going to make our tangent perpendicular to normal 

                    # Gram-Schmidt orthogonalization
                    node.tangent = node.tangent - node.normal * node.tangent.dot (node.normal)

                    # normalizing
                    if (node.tangent.length != 0.0): node.tangent.normalize ()                    

            # next node
            node = node.next

    # new count of vertices
    cvertices = index

    #------------------------------------------------------------------------------------
    # BUILDING TRIANGLE STRIP
    #------------------------------------------------------------------------------------

    if mode == 'TRIANGLES':

        # building triangle indices
        triangles = []

        for f in faces:
            triangles.append ((f [0].index, f [1].index, f [2].index))

        # count of triangles
        ctriangles = len (triangles)

        if (len (triangles) < 65536):

            # building triangle strip
            indices = strip (triangles, True, True)

            # check result           
            if ((type (indices) != int) and (len (indices) < (ctriangles * 3))):

                mode = 'TRIANGLE_STRIP'
                
                cindices = len (indices)
                
                if cindices > 65536:
                    
                    print ('ERROR | Mesh : ', name, ' has generated too many indices.')
                    return -1
                                
                # log
                print ('Mesh : %-15s' % name, ' Faces : %-5i' % len (faces), ' Vertices : %-5i' % cvertices, ' Indices : ', cindices, '(TS)')            

        if mode == 'TRIANGLES':

            cindices = len (faces) * 3
            
            if cindices > 65536:
                
                print ('ERROR | Mesh : ', name, ' has generated too many indices.')
                return -1
                        
            # log
            print ('Mesh : %-15s' % name, ' Faces : %-5i' % len (faces), ' Vertices : %-5i' % cvertices, ' Indices : ', cindices, '(T)')

        # clean up
        triangles [:] = []
            
    elif mode == 'QUADS':
        
        cindices = len (faces) * 4
        
        # log
        print ('Mesh : %-15s' % name, ' Faces : %-5i' % len (faces), ' Vertices : %-5i' % cvertices, ' Indices : ', cindices, '(Q)')
                
    #------------------------------------------------------------------------------------
    # BUILDING LINEAR VERTEX DATA LISTS
    #------------------------------------------------------------------------------------

    vertices = [0 for i in range (cvertices * 3)]

    if export_tangents: tangents = [0 for i in range (cvertices * 3)]
    if export_normals:  normals  = [0 for i in range (cvertices * 3)]
    if export_colors:   colors   = [0 for i in range (cvertices * 3)]
    
    if export_uv1:
        if uv1layer:    uv1      = [0 for i in range (cvertices * 2)]
        else:           uv1      = [0 for i in range (cvertices * 4)]
    if export_uv2:
        if uv2layer:    uv2      = [0 for i in range (cvertices * 2)]
        else:           uv2      = [0 for i in range (cvertices * 4)]
    if export_uv3:
        if uv3layer:    uv3      = [0 for i in range (cvertices * 2)]
        else:           uv3      = [0 for i in range (cvertices * 4)]

    for v in sharedl:
        node = v
        while node:
            if node.valid:

                index = node.index * 3
                
                # vertex position
                vertices [index    ] = node.vertex [0]
                vertices [index + 1] = node.vertex [1]
                vertices [index + 2] = node.vertex [2]

                # short range is <-32767, 32767> insead of <-32768, 32767> for simplicity

                # vertex tangent
                if export_tangents:
                    tangents [index    ] = int (math.floor (127.5 + max (- 1.0, min (1.0, node.tangent [0])) * 127.5))
                    tangents [index + 1] = int (math.floor (127.5 + max (- 1.0, min (1.0, node.tangent [1])) * 127.5))
                    tangents [index + 2] = int (math.floor (127.5 + max (- 1.0, min (1.0, node.tangent [2])) * 127.5))
                    
                # vertex normal
                if export_normals:
                    normals  [index    ] = int (math.floor (max (- 1.0, min (1.0, node.normal [0])) * 32767.0))
                    normals  [index + 1] = int (math.floor (max (- 1.0, min (1.0, node.normal [1])) * 32767.0))
                    normals  [index + 2] = int (math.floor (max (- 1.0, min (1.0, node.normal [2])) * 32767.0))

                # vertex color
                if export_colors:
                    colors [index    ] = max (0, min (255, int (math.floor (255.0 * node.color [0]))))
                    colors [index + 1] = max (0, min (255, int (math.floor (255.0 * node.color [1]))))
                    colors [index + 2] = max (0, min (255, int (math.floor (255.0 * node.color [2]))))

                # vertex uv1                    
                if   export_uv1 == 16:
                    if uv1layer:
                        index = node.index * 2
                        uv1 [index    ] = max (- 32767, min (32767, int (math.floor (node.uv1 [0] * 32767.0 * 0.1))))
                        uv1 [index + 1] = max (- 32767, min (32767, int (math.floor (node.uv1 [1] * 32767.0 * 0.1))))
                    else:
                        index = node.index * 4
                        uv1 [index    ] = max (- 32767, min (32767, int (math.floor (node.uv1 [0] * 32767.0 * 0.1))))
                        uv1 [index + 1] = max (- 32767, min (32767, int (math.floor (node.uv1 [1] * 32767.0 * 0.1))))
                        uv1 [index + 2] = max (- 32767, min (32767, int (math.floor (node.uv1 [2] * 32767.0 * 0.1))))
                        uv1 [index + 3] = max (- 32767, min (32767, int (math.floor (node.uv1 [3] * 32767.0 * 0.1))))
                        
                elif export_uv1 == 32:
                    if uv1layer:
                        index = node.index * 2
                        uv1 [index    ] = node.uv1 [0]
                        uv1 [index + 1] = node.uv1 [1]
                    else:
                        index = node.index * 4
                        uv1 [index    ] = node.uv1 [0]
                        uv1 [index + 1] = node.uv1 [1]
                        uv1 [index + 2] = node.uv1 [2]
                        uv1 [index + 3] = node.uv1 [3]

                # vertex uv2
                if   export_uv2 == 16:
                    if uv2layer:
                        index = node.index * 2
                        uv2 [index    ] = max (- 32767, min (32767, int (math.floor (node.uv2 [0] * 32767.0 * 0.1))))
                        uv2 [index + 1] = max (- 32767, min (32767, int (math.floor (node.uv2 [1] * 32767.0 * 0.1))))
                    else:
                        index = node.index * 4
                        uv2 [index    ] = max (- 32767, min (32767, int (math.floor (node.uv2 [0] * 32767.0 * 0.1))))
                        uv2 [index + 1] = max (- 32767, min (32767, int (math.floor (node.uv2 [1] * 32767.0 * 0.1))))
                        uv2 [index + 2] = max (- 32767, min (32767, int (math.floor (node.uv2 [2] * 32767.0 * 0.1))))
                        uv2 [index + 3] = max (- 32767, min (32767, int (math.floor (node.uv2 [3] * 32767.0 * 0.1))))
                        
                elif export_uv2 == 32:
                    if uv2layer:
                        index = node.index * 2
                        uv2 [index    ] = node.uv2 [0]
                        uv2 [index + 1] = node.uv2 [1]
                    else:
                        index = node.index * 4
                        uv2 [index    ] = node.uv2 [0]
                        uv2 [index + 1] = node.uv2 [1]
                        uv2 [index + 2] = node.uv2 [2]
                        uv2 [index + 3] = node.uv2 [3]
        
                # vertex uv3
                if   export_uv3 == 16:
                    if uv3layer:
                        index = node.index * 2
                        uv3 [index    ] = max (- 32767, min (32767, int (math.floor (node.uv3 [0] * 32767.0 * 0.1))))
                        uv3 [index + 1] = max (- 32767, min (32767, int (math.floor (node.uv3 [1] * 32767.0 * 0.1))))
                    else:
                        index = node.index * 4
                        uv3 [index    ] = max (- 32768, min (32767, int (math.floor (node.uv3 [0] * 32768.0 * 0.1))))
                        uv3 [index + 1] = max (- 32768, min (32767, int (math.floor (node.uv3 [1] * 32768.0 * 0.1))))
                        uv3 [index + 2] = max (- 32768, min (32767, int (math.floor (node.uv3 [2] * 32768.0 * 0.1))))
                        uv3 [index + 3] = max (- 32768, min (32767, int (math.floor (node.uv3 [3] * 32768.0 * 0.1))))
                        
                elif export_uv3 == 32:
                    if uv3layer:
                        index = node.index * 2
                        uv3 [index    ] = node.uv3 [0]
                        uv3 [index + 1] = node.uv3 [1]
                    else:
                        index = node.index * 4
                        uv3 [index    ] = node.uv3 [0]
                        uv3 [index + 1] = node.uv3 [1]
                        uv3 [index + 2] = node.uv3 [2]
                        uv3 [index + 3] = node.uv3 [3]
                        
            # next node
            node = node.next

    #------------------------------------------------------------------------------------
    # BOUNDARY
    #------------------------------------------------------------------------------------
    
    # bounding box
    minco = [ 999999999.0, 999999999.0, 999999999.0]
    maxco = [-999999999.0,-999999999.0,-999999999.0]

    # collecting min/max values
    for v in mesh.vertices:
        if (v.co [0] < minco [0]): minco [0] = v.co [0]
        if (v.co [1] < minco [1]): minco [1] = v.co [1]
        if (v.co [2] < minco [2]): minco [2] = v.co [2]
        if (v.co [0] > maxco [0]): maxco [0] = v.co [0]
        if (v.co [1] > maxco [1]): maxco [1] = v.co [1]
        if (v.co [2] > maxco [2]): maxco [2] = v.co [2]

    # center of bounding box
    center = [0,0,0]
    
    center [0] = (minco [0] + maxco [0]) * 0.5
    center [1] = (minco [1] + maxco [1]) * 0.5
    center [2] = (minco [2] + maxco [2]) * 0.5

    # bounding sphere radius
    
    radius = mathutils.Vector ((maxco [0] - center [0], maxco [1] - center [1], maxco [2] - center [2])).length

    #------------------------------------------------------------------------------------
    # WRITE DATA INTO FILE
    #------------------------------------------------------------------------------------

    # write mode

    if   mode == 'TRIANGLE_STRIP':  stream.write (struct.pack ('i', 0))
    elif mode == 'TRIANGLES':       stream.write (struct.pack ('i', 1))
    elif mode == 'QUADS':           stream.write (struct.pack ('i', 2))

    stream.write (struct.pack ('i', cindices))
    stream.write (struct.pack ('i', cvertices))
    stream.write (struct.pack ('i', len (mesh.faces)))

    stream.write (struct.pack ('f', minco [0]))
    stream.write (struct.pack ('f', minco [1]))
    stream.write (struct.pack ('f', minco [2]))

    stream.write (struct.pack ('f', maxco [0]))
    stream.write (struct.pack ('f', maxco [1]))
    stream.write (struct.pack ('f', maxco [2]))
    
    for v in vertices:
        stream.write (struct.pack ('f', v))

    if export_tangents:
        stream.write (struct.pack ('i', 1))
        for t in tangents:
            stream.write (struct.pack ('B', t))
    else:   stream.write (struct.pack ('i', 0))            

    if export_normals:
        stream.write (struct.pack ('i', 1))
        for n in normals:
            stream.write (struct.pack ('h', n))
    else:   stream.write (struct.pack ('i', 0))

    if export_colors:
        stream.write (struct.pack ('i', 1))
        for c in colors:
            stream.write (struct.pack ('B', c))
    else:   stream.write (struct.pack ('i', 0))

    if   export_uv1 == 16:
        stream.write (struct.pack ('i', 16))
        if uv1layer:    stream.write (struct.pack ('i', 2))
        else:           stream.write (struct.pack ('i', 4))
        for u in uv1:
            stream.write (struct.pack ('h', u))
    elif export_uv1 == 32:
        stream.write (struct.pack ('i', 32))
        if uv1layer:    stream.write (struct.pack ('i', 2))
        else:           stream.write (struct.pack ('i', 4))
        for u in uv1:
            stream.write (struct.pack ('f', u))
    else:   stream.write (struct.pack ('i', 0))

    if   export_uv2 == 16:
        stream.write (struct.pack ('i', 16))
        if uv2layer:    stream.write (struct.pack ('i', 2))
        else:           stream.write (struct.pack ('i', 4))
        for u in uv2:
            stream.write (struct.pack ('h', u))
    elif export_uv2 == 32:
        stream.write (struct.pack ('i', 32))
        if uv2layer:    stream.write (struct.pack ('i', 2))
        else:           stream.write (struct.pack ('i', 4))
        for u in uv2:
            stream.write (struct.pack ('f', u))
    else:   stream.write (struct.pack ('i', 0))
    
    if   export_uv3 == 16:
        stream.write (struct.pack ('i', 16))
        if uv3layer:    stream.write (struct.pack ('i', 2))
        else:           stream.write (struct.pack ('i', 4))
        for u in uv3:
            stream.write (struct.pack ('h', u))
    elif export_uv3 == 32:
        stream.write (struct.pack ('i', 32))
        if uv3layer:    stream.write (struct.pack ('i', 2))
        else:           stream.write (struct.pack ('i', 4))
        for u in uv3:
            stream.write (struct.pack ('f', u))
    else:   stream.write (struct.pack ('i', 0))

    if   mode == 'TRIANGLE_STRIP':    
        for s in indices:
            stream.write (struct.pack ('H', s))
            
    elif mode == 'TRIANGLES':        
        for f in faces:
            stream.write (struct.pack ('H', f [0].index))
            stream.write (struct.pack ('H', f [1].index))
            stream.write (struct.pack ('H', f [2].index))
        
    elif mode == 'QUADS':
        for f in faces:
            stream.write (struct.pack ('H', f [0].index))
            stream.write (struct.pack ('H', f [1].index))
            stream.write (struct.pack ('H', f [2].index))
            stream.write (struct.pack ('H', f [3].index))
                    
    #------------------------------------------------------------------------------------
    # REMOVING TEMPORARY BLENDER DATA
    #------------------------------------------------------------------------------------
    
    # removing data link
    tmpo.data.user_clear ()

    # unlink temporary object from scene
    scene.objects.unlink (tmpo)
                
    # removing mesh
    bpy.data.meshes.remove (mesh)
        
    # removing temporary object
    bpy.data.objects.remove (tmpo)

    # clean up
    faces       [:] = []
    sharedl     [:] = []
    vertices    [:] = []
    
    if export_tangents: tangents [:] = []
    if export_normals:  normals  [:] = []        
    if export_colors:   colors   [:] = []
    if export_uv1:      uv1      [:] = []
    if export_uv2:      uv2      [:] = []
    if export_uv3:      uv3      [:] = []
            
    # closing file
    stream.close ()
    
    return True

####------------------------------------------------------------------------------------------------------------------------------------------------------
#### EXPORT INSTANCES
####------------------------------------------------------------------------------------------------------------------------------------------------------

def processExportInstances (psystem, filepath):
    
    # check object
    if (not psystem.settings.dupli_object) :
        
        print ('ERROR | Missing mesh.')
        return -1

    obj = psystem.settings.dupli_object

    # check object type
    if (not obj.type == 'MESH') :

        print ('ERROR | Missing mesh.')
        return -1

    # check source
    if (not psystem.settings.type == 'HAIR'):

        print ('ERROR | Only hair particle systems supported.')
        return -1

    mesh = obj.data
    
    # active scene
    scene = bpy.context.scene

    minco = [ 99999999,  99999999,  99999999]
    maxco = [-99999999, -99999999, -99999999]

    for v in mesh.vertices :
        
        if v.co [0] < minco [0]:    minco [0] = v.co [0]
        if v.co [1] < minco [1]:    minco [1] = v.co [1]
        if v.co [2] < minco [2]:    minco [2] = v.co [2]
        if v.co [0] > maxco [0]:    maxco [0] = v.co [0]
        if v.co [1] > maxco [1]:    maxco [1] = v.co [1]
        if v.co [2] > maxco [2]:    maxco [2] = v.co [2]

    # data
    pointstype = ctypes.c_float * 24;   points = pointstype ()

    points [ 0] = minco [0]; points [ 1] = minco [1]; points [ 2] = minco [2];
    points [ 3] = maxco [0]; points [ 4] = minco [1]; points [ 5] = minco [2];
    points [ 6] = minco [0]; points [ 7] = maxco [1]; points [ 8] = minco [2];
    points [ 9] = maxco [0]; points [10] = maxco [1]; points [11] = minco [2];    
    points [12] = minco [0]; points [13] = minco [1]; points [14] = maxco [2];
    points [15] = maxco [0]; points [16] = minco [1]; points [17] = maxco [2];
    points [18] = minco [0]; points [19] = maxco [1]; points [20] = maxco [2];
    points [21] = maxco [0]; points [22] = maxco [1]; points [23] = maxco [2];
    
    minco [:] = []
    maxco [:] = []    

    # toolkit dll
    toolkit = windll.LoadLibrary (sys.path [0] + '\shift_toolkit.dll')

    # copy of current setting
    settings = psystem.settings;    psystem.settings = psystem.settings.copy ()

    # set new count
    try:    psystem.settings.count      = settings ["count"]
    except: pass
    
    # set 100% of particles
    psystem.settings.draw_percentage    = 100.0

    # edit mode
    bpy.ops.particle.particle_edit_toggle ()
    bpy.ops.particle.particle_edit_toggle ()

    # data
    datatype = ctypes.c_float * (len (psystem.particles) * 7);  data  = datatype ()

    # data
    datatype = ctypes.c_uint  *  len (psystem.particles);       datak = datatype ()

    i = 0;  j = 0
    for p in psystem.particles:

        data [i] = ctypes.c_float (p.prev_location [0]);    i += 1;
        data [i] = ctypes.c_float (p.prev_location [1]);    i += 1;
        data [i] = ctypes.c_float (p.prev_location [2]);    i += 1;
        data [i] = ctypes.c_float (p.location [0]);         i += 1;
        data [i] = ctypes.c_float (p.location [1]);         i += 1;
        data [i] = ctypes.c_float (p.location [2]);         i += 1;
        data [i] = ctypes.c_float (p.size);                 i += 1;
        
        datak [j] = ctypes.c_uint (len (p.hair_keys)); j += 1

    # restore settings
    psystem.settings = settings;
    
    if (scene.shift_ex_instances_over):

        subx = ctypes.c_uint (scene.shift_ex_subdivisionx)
        suby = ctypes.c_uint (scene.shift_ex_subdivisiony)
        subz = ctypes.c_uint (scene.shift_ex_subdivisionz)

        rotx = ctypes.c_float (scene.shift_ex_instances_randomx)
        roty = ctypes.c_float (scene.shift_ex_instances_randomy)
        rotz = ctypes.c_float (scene.shift_ex_instances_randomz)

        rad = ctypes.c_float (scene.shift_ex_randomrad)

        cancell = ctypes.c_float (scene.shift_ex_cancellation)
        
        rotorder = scene.shift_ex_rotorder
        
    else:
        
        try:    subx = ctypes.c_uint (psystem.settings ['subdivision_x'])
        except: subx = ctypes.c_uint (scene.shift_ex_subdivisionx)
        try:    suby = ctypes.c_uint (psystem.settings ['subdivision_y'])
        except: suby = ctypes.c_uint (scene.shift_ex_subdivisiony)
        try:    subz = ctypes.c_uint (psystem.settings ['subdivision_z'])
        except: subz = ctypes.c_uint (scene.shift_ex_subdivisionz)

        try:    rotx = ctypes.c_float (psystem.settings ['rand_rot_x'] * cmath.pi / 180.0)
        except: rotx = ctypes.c_float (scene.shift_ex_instances_randomx)
        try:    roty = ctypes.c_float (psystem.settings ['rand_rot_y'] * cmath.pi / 180.0)
        except: roty = ctypes.c_float (scene.shift_ex_instances_randomy)
        try:    rotz = ctypes.c_float (psystem.settings ['rand_rot_z'] * cmath.pi / 180.0)
        except: rotz = ctypes.c_float (scene.shift_ex_instances_randomz)
    
        try:    rad = ctypes.c_float (psystem.settings ['rand_radius'])
        except: rad = ctypes.c_float (scene.shift_ex_randomrad)

        try:    rotorder = str (psystem.settings ['rand_rot_order']).upper ()
        except: rotorder = scene.shift_ex_rotorder

        try:    cancell = ctypes.c_float (psystem.settings ['cancell'])
        except: cancell = ctypes.c_float (scene.shift_ex_cancellation)


    if   (rotorder == "XYZ"):  order = ctypes.c_uint (1)
    elif (rotorder == "YXZ"):  order = ctypes.c_uint (2)
    elif (rotorder == "ZYX"):  order = ctypes.c_uint (3)
    elif (rotorder == "XZY"):  order = ctypes.c_uint (4)
    elif (rotorder == "YZX"):  order = ctypes.c_uint (5)
    elif (rotorder == "ZXY"):  order = ctypes.c_uint (6)
    
    else : order = ctypes.c_uint (1)
    
    # PROCESS
    toolkit.processSaveInstances (ctypes.c_char_p (filepath.encode ('ascii')), len (psystem.particles), data, datak, points, subx, suby, subz, rotx, roty, rotz, rad, order, cancell)

    del data
    del datak
    
    return True

####------------------------------------------------------------------------------------------------------------------------------------------------------
#### EXPORT SCENE
####------------------------------------------------------------------------------------------------------------------------------------------------------

def processExportScene (filepath):
        
    print ('\nSHIFT scene export starting... \n\t', filepath)
    print ('')

    start_time = time.clock ()

    # opening new binary file
    stream = io.open (filepath, mode = 'wb')

    #------------------------------------------------------------------------------------
    # COLLECTING OBJECTS, MESHES, MATERIALS AND TEXTURES
    #------------------------------------------------------------------------------------

    # final lists
    instanced  = []
    materials  = []
    textures   = []
    objects    = []
    meshes     = []

    # active scene
    scene = bpy.context.scene
    
    # take all objects
    object_list = bpy.data.objects

    # special case objects
    enviroment = None
    sun = None

    # collecting objects and its meshes
    for obj in object_list:

        # skip objects that are not in first layer
        if not obj.layers [0]: continue
        
        # skip hidden objects
        if obj.hide: continue

        # skip objects with 'no_export' custom property
        try:
            if obj      ['no_export'] == 1: continue
            if obj.data ['no_export'] == 1: continue
        except: pass        
        
        if   obj.type == 'LAMP':

            if obj.data.name == 'Sun' and obj.data.type == 'SUN' :   sun = obj;
            
        elif obj.type == 'MESH':

            mesh = obj.data;

            # if mesh have material ..
            if mesh.materials [0]:
                
                # append mesh
                meshes.append ((mesh.name, mesh))

                # enviroment is treated specially
                if 'Enviroment' in mesh.materials [0].name:

                    if enviroment:
                        print ('ERROR | Too many enviroment objects.')
                                    
                    enviroment = obj
                    continue;

                # adding lod1 mesh
                try:    meshes.append ((mesh ['lod1'], bpy.data.meshes [mesh ['lod1']]))
                except: pass

                # adding lod2 mesh
                try:    meshes.append ((mesh ['lod2'], bpy.data.meshes [mesh ['lod2']]))
                except: pass
                
                # adding lod3 mesh
                try:    meshes.append ((mesh ['lod3'], bpy.data.meshes [mesh ['lod3']]))
                except: pass
                
                # adding occlusion mesh
                try:    meshes.append (((obj.data ['occlusion'], bpy.data.meshes [obj.data ['occlusion']])))
                except: pass            

                # append object
                objects.append ((obj.name, obj, None))

            # collect all particle systems meshes
            for p in obj.particle_systems:
                
                if p.settings.dupli_object:
                    if (p.settings.dupli_object.type == 'MESH'):

                        # mesh
                        mesh = p.settings.dupli_object.data

                        # if mesh have material ..
                        if mesh.materials [0]:
                        
                            # append instances
                            instanced.append ((obj.name + '_' + p.name, p))
                            
                            # append object
                            objects.append ((obj.name + '_' + p.name, obj, len (instanced) - 1))
                            
                            # adding particle mesh
                            meshes.append ((mesh.name, mesh))

                            # adding lod1 mesh
                            try:    meshes.append ((mesh ['lod1'], bpy.data.meshes [mesh ['lod1']]))
                            except: pass

                            # adding lod2 mesh
                            try:    meshes.append ((mesh ['lod2'], bpy.data.meshes [mesh ['lod2']]))
                            except: pass
                            
                            # adding lod3 mesh
                            try:    meshes.append ((mesh ['lod3'], bpy.data.meshes [mesh ['lod3']]))
                            except: pass
                            
                            # adding occlusion mesh
                            try:    meshes.append (((p ['occlusion'], bpy.data.meshes [p ['occlusion']])))
                            except: pass
                
    # removing duplicate meshes
    meshes.sort (key = lambda meshes: meshes [0])
    
    prevname = None;    l = len (meshes)
    
    i = 0;
    while (i < l):
        name = meshes [i][0]
        if (name == prevname):
            del meshes [i]; l -= 1
            continue
        prevname = name;    i += 1
        
    # collecting materials
    for name, mesh in meshes:

        try:    mat = mesh.materials [0]
        except: mat = None

        if mat:                
            materials.append ((mat.name, mat))            

    # removing duplicate materials
    materials.sort (key = lambda materials: materials [0])
    
    prevname = None;    l = len (materials)
    
    i = 0
    while (i < l):
        name = materials [i][0]
        if (name == prevname):
            del materials [i];  l -= 1
            continue
        prevname = name;        i += 1

    # collecting textures
    for name, mat in materials:

        try:    tex = bpy.data.textures [mat ['tex_diffuse']];      textures.append ((tex.name, tex, True))
        except: pass;
        try:    tex = bpy.data.textures [mat ['tex_composite']];    textures.append ((tex.name, tex, False))
        except: pass;
        try:    tex = bpy.data.textures [mat ['tex_weights1']];     textures.append ((tex.name, tex, False))
        except: pass;
        try:    tex = bpy.data.textures [mat ['tex_weights2']];     textures.append ((tex.name, tex, False))
        except: pass;
        try:    tex = bpy.data.textures [mat ['tex_weights3']];     textures.append ((tex.name, tex, False))
        except: pass;
        try:    tex = bpy.data.textures [mat ['tex_weights4']];     textures.append ((tex.name, tex, False))
        except: pass;

    # removing duplicate textures
    textures.sort (key = lambda textures: textures [0])

    prevname = None;    l = len (textures)
    
    i = 0
    while (i < l):
        name = textures [i][0]
        if (name == prevname):
            del textures [i];   l -= 1
            continue
        prevname = name;        i += 1

    # writing header
    stream.write (struct.pack ('i', len (objects)))
    stream.write (struct.pack ('i', len (meshes)))
    stream.write (struct.pack ('i', len (materials)))
    stream.write (struct.pack ('i', len (textures)))
    stream.write (struct.pack ('i', len (instanced)))

    # world params
    
    try:    stream.write (struct.pack ('f', scene.world ["plane_near"])) 
    except:
        print ('ERROR | Missing world custom property \'plane_near\' ');    return;
        
    try:    stream.write (struct.pack ('f', scene.world ["plane_far"])) 
    except:
        print ('ERROR | Missing world custom property \'plane_far\' ');     return;

    #------------------------------------------------------------------------------------
    # WRITING TEXTURES
    #------------------------------------------------------------------------------------

    print ('Textures :')
    print ('')        
    
    for i, (name, tex, gamma) in enumerate (textures):

        # saving index
        tex.id_data ["tmp_index"] = i

        image = tex.image;

        filename = os.path.basename (bpy.path.abspath (image.filepath))

        # file name
        stream.write (struct.pack ('i',     len (filename) + 1))
        stream.write (struct.pack ('%isB' % len (filename), filename.encode ('ascii'), 0))

        # gamma
        if gamma:   stream.write (struct.pack ('i', 1))
        else:       stream.write (struct.pack ('i', 0))
        
        if   tex.extension == 'REPEAT':

            stream.write (struct.pack ('i', 0))     # wraps
            stream.write (struct.pack ('i', 0))     # wrapt
            
        elif tex.extension == 'EXTEND':

            stream.write (struct.pack ('i', 1))     # wraps
            stream.write (struct.pack ('i', 1))     # wrapt
            
        elif tex.extension == 'CLIP':

            stream.write (struct.pack ('i', 2))     # wraps
            stream.write (struct.pack ('i', 2))     # wrapt
            
        else :
            
            stream.write (struct.pack ('i', 0))     # wraps
            stream.write (struct.pack ('i', 0))     # wrapt

        print (image.filepath)
    print ('')        

    #------------------------------------------------------------------------------------
    # WRITING MATERIALS
    #------------------------------------------------------------------------------------
    
    print ('Materials :')
    print ('')
    
    for i, (name, mat) in enumerate (materials):

        # saving index
        mat.id_data ["tmp_index"] = i

        # material name
        stream.write (struct.pack ('i',     len (name) + 1))
        stream.write (struct.pack ('%isB' % len (name), name.encode ('ascii'), 0))
        
        # material type 
        if   ("Solid"         in name):
            stream.write (struct.pack ('i', 1))       # type
                
            diffuse     = - 1
            composite   = - 1

            try:    diffuse     = bpy.data.textures [mat ['tex_diffuse'  ]].id_data ['tmp_index']
            except: print ('ERROR | Material : ', name, ' custom property \'tex_diffuse\' not set or invalid')
            try:    composite   = bpy.data.textures [mat ['tex_composite']].id_data ['tmp_index']
            except: print ('ERROR | Material : ', name, ' custom property \'tex_composite\' not set or invalid')

            stream.write (struct.pack ('i', diffuse))
            stream.write (struct.pack ('i', composite))
            
        elif ("Terrain"       in name):
            stream.write (struct.pack ('i', 2))       # type

            diffuse     = - 1
            composite   = - 1
            
            weights1    = - 1
            weights2    = - 1
            weights3    = - 1
            weights4    = - 1

            try:    diffuse     = bpy.data.textures [mat ['tex_diffuse'    ]].id_data ['tmp_index']
            except: print ('ERROR | Material : ', name, ' custom property \'tex_diffuse\' not set or invalid')
            try:    composite   = bpy.data.textures [mat ['tex_composite'  ]].id_data ['tmp_index']
            except: print ('ERROR | Material : ', name, ' custom property \'tex_composite\' not set or invalid')
            try:    weights1    = bpy.data.textures [mat ['tex_weights1'   ]].id_data ['tmp_index']
            except: print ('ERROR | Material : ', name, ' custom property \'tex_weights1\' not set or invalid')
            try:    weights2    = bpy.data.textures [mat ['tex_weights2'   ]].id_data ['tmp_index']
            except: print ('ERROR | Material : ', name, ' custom property \'tex_weights2\' not set or invalid')
            try:    weights3    = bpy.data.textures [mat ['tex_weights3'   ]].id_data ['tmp_index']
            except: print ('ERROR | Material : ', name, ' custom property \'tex_weights3\' not set or invalid')
            try:    weights4    = bpy.data.textures [mat ['tex_weights4'   ]].id_data ['tmp_index']
            except: print ('ERROR | Material : ', name, ' custom property \'tex_weights4\' not set or invalid')

            stream.write (struct.pack ('i', diffuse))
            stream.write (struct.pack ('i', composite))
            
            stream.write (struct.pack ('i', weights1))
            stream.write (struct.pack ('i', weights2))
            stream.write (struct.pack ('i', weights3))
            stream.write (struct.pack ('i', weights4))
            
        elif ("Enviroment"    in name):
            stream.write (struct.pack ('i', 3))       # type

            diffuse     = - 1;

            try:    diffuse     = bpy.data.textures [mat ['tex_diffuse']].id_data ['tmp_index']
            except: print ('ERROR | Material : ', name, ' custom property \'tex_diffuse\' not set or invalid')
            
            stream.write (struct.pack ('i', diffuse))
            
        elif ("Foliage"          in name):
            stream.write (struct.pack ('i', 4))       # type
                        
            diffuse     = - 1
            composite   = - 1

            try:    diffuse     = bpy.data.textures [mat ['tex_diffuse'  ]].id_data ['tmp_index']
            except: print ('ERROR | Material : ', name, ' custom property \'tex_diffuse\' not set or invalid')
            try:    composite   = bpy.data.textures [mat ['tex_composite']].id_data ['tmp_index']
            except: print ('ERROR | Material : ', name, ' custom property \'tex_composite\' not set or invalid')

            stream.write (struct.pack ('i', diffuse))
            stream.write (struct.pack ('i', composite))
            
        elif ("Grass"         in name):
            stream.write (struct.pack ('i', 5))       # type
                
            diffuse     = - 1

            try:    diffuse     = bpy.data.textures [mat ['tex_diffuse']].id_data ['tmp_index']
            except: print ('ERROR | Material : ', name, ' custom property \'tex_diffuse\' not set or invalid')

            threshold   = 0.1
            
            try:    threshold   = mat ['threshold']
            except: print ('ERROR | Material : ', name, ' custom property \'threshold\' not set or invalid')

            damping     = 0.9
            
            try:    damping     = mat ['damping']
            except: print ('ERROR | Material : ', name, ' custom property \'damping\' not set or invalid')
            
            stream.write (struct.pack ('i', diffuse))            
            stream.write (struct.pack ('f', damping))
            stream.write (struct.pack ('f', threshold))
            
        print ('Material : %-20s' % name)
    print ('')

    #------------------------------------------------------------------------------------
    # WRITING MODELS
    #------------------------------------------------------------------------------------

    print ('Models :')
    print ('')
    
    # pre indexing
    for mi, (name, mesh) in enumerate (meshes):
        
        # saving index
        mesh.id_data ["tmp_index"] = mi
    
    # writing meshes
    for name, mesh in meshes:

        # writing datablock name
        stream.write (struct.pack ('i',     len (name) + 1))
        stream.write (struct.pack ('%isB' % len (name), name.encode ('ascii'), 0))

        occlusion = False

        # occlusion mesh index
        try:    stream.write (struct.pack ('H', bpy.data.meshes [mesh ['occlusion']]['tmp_index']));   occlusion = True
        except: stream.write (struct.pack ('H', 0xffff))

        shader = 0

        # params
        try:        stream.write (struct.pack ('f', mesh ['morph_position']));  shader = 2
        except:     stream.write (struct.pack ('f', 0.0))
        try:        stream.write (struct.pack ('f', mesh ['morph_normal']));  shader = 2
        except:     stream.write (struct.pack ('f', 0.0))
        try:        stream.write (struct.pack ('f', mesh ['morph_uv']));  shader = 2
        except:     stream.write (struct.pack ('f', 0.0))
        
        dlist = False

        try:
            if   mesh ['display_list'] == 1:  dlist = True
            elif mesh ['display_list'] == 0:  dlist = False
        except:     pass

        # shader
        try:
            if   mesh ['shader'] == 'normal':   shader = 0
            elif mesh ['shader'] == 'grow':     shader = 1
            elif mesh ['shader'] == 'morph':    shader = 2
            elif mesh ['shader'] == 'shrink':   shader = 3
        except:     pass

        stream.write (struct.pack ('B', shader))
        
        # flags
        flags = 0
        
        if dlist:       flags = flags | 0x0001
        if occlusion:   flags = flags | 0x0002

        stream.write (struct.pack ('H', flags))

        print ('Mesh : %-20s' % name)
    print ('')
                            
    #------------------------------------------------------------------------------------
    # WRITING INSTANCED OBJECTS
    #------------------------------------------------------------------------------------

    for name, psys in instanced:

        # object name
        stream.write (struct.pack ('i',     len (name) + 1))
        stream.write (struct.pack ('%isB' % len (name), name.encode ('ascii'), 0))
    
    #------------------------------------------------------------------------------------
    # WRITING OBJECTS
    #------------------------------------------------------------------------------------

    scenematrix = mathutils.Matrix.Scale (bpy.context.scene.shift_ex_scene_scale, 4) * mathutils.Matrix.Rotation (radians (- 90), 4, 'X');
    
    for name, obj, pindex in objects:

        # instances object
        if pindex != None:

            stream.write (struct.pack ('i', pindex))

            settings = instanced [pindex][1].settings
            
            mesh = settings.dupli_object.data

            # object name
            stream.write (struct.pack ('i',     len (name) + 1))
            stream.write (struct.pack ('%isB' % len (name), name.encode ('ascii'), 0))
            
            # mesh index
            stream.write (struct.pack ('H', mesh ['tmp_index']))

            # material index
            stream.write (struct.pack ('H', mesh.materials [0]['tmp_index']))

            # disappear distance
            try:    disappear = settings ['disappear']
            except:
                try:    disappear = mesh ['disappear']
                except: disappear = 1000000.0

            stream.write (struct.pack ('f', disappear))
            
            # disappear start
            try:    disappear_start = settings ['disappear_start']
            except:
                try:    disappear_start = mesh ['disappear_start']
                except: disappear_start = 0.0

            stream.write (struct.pack ('f', disappear_start))
            
            # shadow disappear distance
            try:    stream.write (struct.pack ('f', settings ['disappear_shadow']))
            except:
                try:    stream.write (struct.pack ('f', mesh ['disappear_shadow']))
                except: stream.write (struct.pack ('f', disappear))

        # single object
        else:

            stream.write (struct.pack ('i', -1))
            
            mesh = obj.data
        
            # object name
            stream.write (struct.pack ('i',     len (name) + 1))
            stream.write (struct.pack ('%isB' % len (name), name.encode ('ascii'), 0))
            
            # mesh index
            stream.write (struct.pack ('H', mesh ['tmp_index']))

            # material index
            stream.write (struct.pack ('H', mesh.materials [0]['tmp_index']))

            # disappear distance
            try:    disappear = obj ['disappear']
            except:
                try:    disappear = mesh ['disappear']
                except: disappear = 1000000.0

            stream.write (struct.pack ('f', disappear))
            
            # disappear start
            try:    disappear_start = obj ['disappear_start']
            except:
                try:    disappear_start = mesh ['disappear_start']
                except: disappear_start = 0.0

            stream.write (struct.pack ('f', disappear_start))
            
            # shadow disappear distance
            try:    stream.write (struct.pack ('f', obj ['disappear_shadow']))
            except:
                try:    stream.write (struct.pack ('f', mesh ['disappear_shadow']))
                except: stream.write (struct.pack ('f', disappear))
            
        # flags

        occluder = False

        try:
            if   obj ['occluder'] == 1:  occluder = True
            elif obj ['occluder'] == 0:  occluder = False
        except:
            try:
                if   obj.data ['occluder'] == 1:  occluder = True
                elif obj.data ['occluder'] == 0:  occluder = False
            except:     pass
                        
        flags = 0
        
        if occluder:    flags = flags | 1

        stream.write (struct.pack ('H', flags))
            
        # detail
                
        lod1 = 0xffff;  lod1_distance = disappear
        lod2 = 0xffff;  lod2_distance = disappear
        lod3 = 0xffff;  lod3_distance = disappear
        
        try:
            lod1 = mesh ['lod1']
            try:
                lod2 = mesh ['lod2']
                try:
                    lod3 = mesh ['lod3']                    
                except: pass
            except: pass
        except: pass

        # instances object
        if pindex != None:
            
            if (lod1 != 0xffff):
                try:    lod1 = bpy.data.meshes [lod1]['tmp_index']
                except: lod1 = 0xffff;  print ('ERROR | Mesh : ', name, ' has invalid \'lod1\' custom property value.')
                try:    lod1_distance = mesh ['lod1_distance']
                except:
                    try:    lod1_distance = settings ['lod1_distance']
                    except: lod1 = 0xffff;  print ('ERROR | Mesh / Particle Settings : ', name, '/', settings.name, ' is missing \'lod1_distance\' custom property.')
            if (lod2 != 0xffff):
                try:    lod2 = bpy.data.meshes [lod2]['tmp_index']
                except: lod2 = 0xffff;  print ('ERROR | Mesh : ', name, ' has invalid \'lod2\' custom property value.')
                try:    lod2_distance = mesh ['lod2_distance']
                except:
                    try:    lod2_distance = settings ['lod2_distance']
                    except: lod2 = 0xffff;  print ('ERROR | Mesh / Particle Settings : ', name, '/', settings.name, ' is missing \'lod2_distance\' custom property.')
            if (lod3 != 0xffff):
                try:    lod3 = bpy.data.meshes [lod3]['tmp_index']
                except: lod3 = 0xffff;  print ('ERROR | Mesh : ', name, ' has invalid \'lod3\' custom property value.')
                try:    lod3_distance = mesh ['lod3_distance']
                except:
                    try:    lod3_distance = settings ['lod3_distance']
                    except: lod3 = 0xffff;  print ('ERROR | Mesh / Particle Settings : ', name, '/', settings.name, ' is missing \'lod3_distance\' custom property.')
                    
        # single object
        else:

            if (lod1 != 0xffff):
                try:    lod1 = bpy.data.meshes [lod1]['tmp_index']
                except: lod1 = 0xffff;  print ('ERROR | Mesh : ', name, ' has invalid \'lod1\' custom property value.')
                try:    lod1_distance = mesh ['lod1_distance']
                except: lod1 = 0xffff;  print ('ERROR | Mesh : ', name, ' is missing \'lod1_distance\' custom property.')
            if (lod2 != 0xffff):
                try:    lod2 = bpy.data.meshes [lod2]['tmp_index']
                except: lod2 = 0xffff;  print ('ERROR | Mesh : ', name, ' has invalid \'lod2\' custom property value.')
                try:    lod2_distance = mesh ['lod2_distance']
                except: lod2 = 0xffff;  print ('ERROR | Mesh : ', name, ' is missing \'lod2_distance\' custom property.')
            if (lod3 != 0xffff):
                try:    lod3 = bpy.data.meshes [lod3]['tmp_index']
                except: lod3 = 0xffff;  print ('ERROR | Mesh : ', name, ' has invalid \'lod3\' custom property value.')
                try:    lod3_distance = mesh ['lod3_distance']
                except: lod3 = 0xffff;  print ('ERROR | Mesh : ', name, ' is missing \'lod3_distance\' custom property.')
                
        if lod1 != 0xffff:

            index = mesh ['tmp_index']

            # check for corrupted LOD

            if lod1 == index or lod2 == index or lod3 == index:

                stream.write (struct.pack ('I', 0))                
                print ('ERROR | Mesh : ', name, ' has invalid LOD definition (reference to itself).')
                
            else:
            
                stream.write (struct.pack ('I', 1))
                stream.write (struct.pack ('H', lod1))
                stream.write (struct.pack ('f', lod1_distance))
                stream.write (struct.pack ('H', lod2))
                stream.write (struct.pack ('f', lod2_distance))
                stream.write (struct.pack ('H', lod3))
                stream.write (struct.pack ('f', lod3_distance))
            
        else:   stream.write (struct.pack ('I', 0))

        # single transform
        if pindex == None:
                        
            # rotate world matrix to match up vector with y axis
            matrix = scenematrix * obj.matrix_world

            # world matrix
            for i in range (4):
                v = matrix [i]
                for j in range (4):
                    stream.write (struct.pack ('f', v [j]))
                                            
    #------------------------------------------------------------------------------------
    # WRITING ENVIROMENT    
    #------------------------------------------------------------------------------------

    if enviroment:
        
        # mesh index
        stream.write (struct.pack ('i', enviroment.data ['tmp_index']))

        # material index
        stream.write (struct.pack ('i', enviroment.data.materials [0]['tmp_index']))
        
    else:   print ('ERROR | Missing enviroment object.')

    #------------------------------------------------------------------------------------
    # WRITING SUN    
    #------------------------------------------------------------------------------------

    if sun:

        lamp = sun.data

        # direction
        direction = mathutils.Vector ((0.0, 0.0, -1.0, 0.0)) * (scenematrix * sun.matrix_world);    direction.normalize ()

        stream.write (struct.pack ('f', direction [0]))
        stream.write (struct.pack ('f', direction [1]))
        stream.write (struct.pack ('f', direction [2]))

        # distance
        distance = mathutils.Vector (sun.location).length
        
        stream.write (struct.pack ('f', distance))

        # color
        stream.write (struct.pack ('f', lamp.color [0]))
        stream.write (struct.pack ('f', lamp.color [1]))
        stream.write (struct.pack ('f', lamp.color [2]))

        # intensity
        stream.write (struct.pack ('f', lamp.energy))
        
        # ambient intensity
        stream.write (struct.pack ('f', lamp ['ambient']))

        # far plane
        stream.write (struct.pack ('f', lamp ['planefar']))
        
    else:   print ('ERROR | Missing sun object.')

    #------------------------------------------------------------------------------------
    # WRITING FOG
    #------------------------------------------------------------------------------------

    # fog color
    stream.write (struct.pack ('f', scene.world.horizon_color [0]))
    stream.write (struct.pack ('f', scene.world.horizon_color [1]))
    stream.write (struct.pack ('f', scene.world.horizon_color [2]))

    # closing file
    stream.close ()

    # removing temporary properties
    for tex in textures:
        try:    del tex.texture.id_data     ["tmp_index"]
        except: pass
    for name, mat in materials:
        try:    del mat.id_data             ["tmp_index"]
        except: pass
    for name, msh in meshes:
        try:    del msh.id_data             ["tmp_index"]
        except: pass

    # cleanup
    instanced [:] = []
    materials [:] = []
    textures  [:] = []
    meshes    [:] = []

    # ELAPSED TIME
            
    print ('\nexport finished in %.4f sec.' % (time.clock () - start_time))
    
    return True
        

####------------------------------------------------------------------------------------------------------------------------------------------------------
#### INTEGRATION AND GUI
####------------------------------------------------------------------------------------------------------------------------------------------------------

class ExporterOpScene (bpy.types.Operator):
     
    bl_idname       = "object.shift_export_scene_operator"
    bl_label        = "SHIFT - Export"
    bl_description  = "Export unhidden objects from all scenes first layer into SHIFT .shift scene file"
    
    def execute (self, context):
            
        filepath = bpy.context.scene.shift_ex_scene_filepath

        if filepath != '':

            filepath = os.path.abspath (filepath)
            
            # append extension if needed
            if not filepath.lower ().endswith (".shift"): filepath += ".shift"
                
            processExportScene (filepath)
        
            return {"FINISHED"}
        
        else:
            return {"CANCELLED"}

class ExporterOpMesh (bpy.types.Operator):
     
    bl_idname       = "object.shift_export_mesh_operator"
    bl_label        = "SHIFT - Export"
    bl_description  = "Export all selected objects with mesh datablock as SHIFT .mesh files"
    
    def execute (self, context):

        path = bpy.context.scene.shift_ex_mesh_filepath

        scene = bpy.context.scene 

        if path != '':

            if scene.shift_ex_mesh_auto:

                # cut off file name
                if path [len (path) - 1] != '\\':
                
                    path = os.path.abspath (bpy.context.scene.shift_ex_mesh_filepath).rpartition ('\\')[0] + '\\'
                
                if len (bpy.context.selected_objects) == 1:

                    obj = bpy.context.object

                    if not obj: obj = bpy.context.selected_objects [0]
                        
                    if (obj.type == 'MESH'):

                        if not (scene.shift_ex_mesh_skip and (len (obj.particle_systems) > 0)):
                            
                            # generated name stitched with full path
                            filepath = path + obj.data.name + '.mesh'
                            
                            # export
                            processExportMesh (obj.data, filepath)
                        
                    else:   print ('ERROR | Invalid object type.'); return {"CANCELLED"}
                    
                else:

                    # log
                    print ('\nSHIFT mesh export starting... \n\t', path)
                    print ('')

                    start_time = time.clock ()

                    for obj in bpy.context.selected_objects:
                        
                        if (obj.type == 'MESH'):

                            if not (scene.shift_ex_mesh_skip and (len (obj.particle_systems) > 0)):
                            
                                # generated name stitched with full path
                                filepath = path +  obj.data.name + '.mesh'

                                # export
                                processExportMesh (obj.data, filepath)

                    # elapsed time
                    print ('')
                    print ('\nexport finished in %.4f sec.' % (time.clock () - start_time))
                                                                
            else:
                
                if len (bpy.context.selected_objects) == 1:

                    obj = bpy.context.selected_objects [0]

                    if not obj: obj = bpy.context.selected_objects [0]

                    if (obj.type == 'MESH'):

                        if not (scene.shift_ex_mesh_skip and (len (obj.particle_systems) > 0)):
                            
                            filepath = scene.shift_ex_mesh_filepath

                            # check for invalid file name
                            if filepath.endswith ("\\"):
                                
                                print ('ERROR | Missing file name.')
                                return {"CANCELLED"}

                            filepath = os.path.abspath (filepath)
                            
                            # append extension if needed
                            if not filepath.endswith (".mesh"): filepath += ".mesh"

                            # export
                            processExportMesh (obj.data, filepath)
                        
                    else:   print ('ERROR | Invalid object type.'); return {"CANCELLED"}
                    
                else: print ('ERROR | Cannot export multiple meshes without autonaming turned on.')
            
            return {"FINISHED"}
        
        else:
            return {"CANCELLED"}

class ExporterOpMeshClear (bpy.types.Operator):

    bl_idname       = "object.shift_export_mesh_clear_operator"
    bl_label        = "SHIFT - Export"
    bl_description  = "Removes all *.mesh files from destination directory"
    
    def execute (self, context):

        path = bpy.context.scene.shift_ex_mesh_filepath

        # cut off file name
        if path [len (path) - 1] != '\\':
        
            path = os.path.abspath (bpy.context.scene.shift_ex_mesh_filepath).rpartition ('\\')[0] + '\\'

        print ('\nDELETING FILES : ')
        print ('\t' + path)
        
        for root, dirs, files in os.walk (path):

            for f in files:
                if f.rpartition ('.')[2] == 'mesh' :
                    
                    try:        print (f);   os.remove (path + f)
                    except:     print ('ERROR | Unable to remove file : ', f)
            break

        return {"FINISHED"}
    
class ExporterOpInstances (bpy.types.Operator):
     
    bl_idname       = "object.shift_export_instances_operator"
    bl_label        = "SHIFT - Export"
    bl_description  = "Export all selected objects hair particles datablocks as SHIFT .inst files"
    
    def execute (self, context):

        path = bpy.context.scene.shift_ex_instances_filepath

        if path != '':

            if bpy.context.scene.shift_ex_instances_auto:

                # cut off file name
                if path [len (path) - 1] != '\\':
                
                    path = os.path.abspath (bpy.context.scene.shift_ex_instances_filepath).rpartition ('\\')[0] + '\\'
                
                if len (bpy.context.selected_objects) == 1:

                    obj = bpy.context.selected_objects [0]

                    if (obj.type == 'MESH'):

                        # skip objects with 'no_export' custom property
                        try:
                            if obj      ['no_export'] == 1: return {"CANCELLED"}
                            if obj.data ['no_export'] == 1: return {"CANCELLED"}
                        except: pass

                        for i, p in enumerate (obj.particle_systems):

                            obj.particle_systems.active_index = i

                            # generated name stitched with full path
                            filename = obj.name + '_' + p.name + '.inst';  filepath = path + filename

                            start_time = time.clock ()
                            
                            # export
                            processExportInstances (p, filepath)
                            
                            # log
                            print ('\t' + filename + ' (%.4f sec.)' % (time.clock () - start_time))
                        
                    else:   print ('ERROR | Invalid object type.'); return {"CANCELLED"}
                    
                else:

                    # log
                    print ('\nSHIFT instances export starting... \n\t', path)
                    print ('')

                    start_time = time.clock ()

                    for obj in bpy.context.selected_objects:

                        # skip objects with 'no_export' custom property
                        try:
                            if obj      ['no_export'] == 1: continue
                            if obj.data ['no_export'] == 1: continue
                        except: pass

                        if (obj.type == 'MESH'):
                            
                            for i, p in enumerate (obj.particle_systems):

                                obj.particle_systems.active_index = i

                                # generated name stitched with full path
                                filename = obj.name + '_' + p.name + '.inst';  filepath = path + filename
                                
                                start_time = time.clock ()
                            
                                # export
                                processExportInstances (p, filepath)

                                # log
                                print ('\t' + filename + ' (%.4f sec.)' % (time.clock () - start_time))
                                
                    # elapsed time
                    print ('')
                    print ('\nexport finished in %.4f sec.' % (time.clock () - start_time))
                                                                
            else:
                
                if len (bpy.context.selected_objects) == 1:

                    obj = bpy.context.object

                    if (obj.type == 'MESH'):

                        # skip objects with 'no_export' custom property
                        try:
                            if obj      ['no_export'] == 1: return {"CANCELLED"}
                            if obj.data ['no_export'] == 1: return {"CANCELLED"}
                        except: pass

                        filepath = bpy.context.scene.shift_ex_instances_filepath

                        # check for invalid file name
                        if filepath.endswith ("\\"):
                            
                            print ('ERROR | Missing file name.')
                            return {"CANCELLED"}

                        filepath = os.path.abspath (filepath)
                        
                        # append extension if needed
                        if not filepath.endswith (".inst"): filepath += ".inst"

                        if obj.particle_systems.active :
                        
                            start_time = time.clock ()
                            
                            # export
                            processExportInstances (obj.particle_systems.active, filepath)

                            # log
                            print ('\t' + filename + ' (%.4f sec.)' % (time.clock () - start_time))

                        else:   print ('ERROR | Missing particle system.'); return {"CANCELLED"}
                        
                    else:   print ('ERROR | Invalid object type.'); return {"CANCELLED"}
                    
                else: print ('ERROR | Cannot export multiple meshes without autonaming turned on.')
            
            return {"FINISHED"}
        
        else:
            return {"CANCELLED"}

class ExporterOpInstancesClear (bpy.types.Operator):

    bl_idname       = "object.shift_export_instances_clear_operator"
    bl_label        = "SHIFT - Export"
    bl_description  = "Removes all *.inst files from destination directory"
    
    def execute (self, context):

        path = bpy.context.scene.shift_ex_instances_filepath

        # cut off file name
        if path [len (path) - 1] != '\\':
        
            path = os.path.abspath (bpy.context.scene.shift_ex_instances_filepath).rpartition ('\\')[0] + '\\'

        print ('\nDELETING FILES : ')
        print ('\t' + path)
        
        for root, dirs, files in os.walk (path):

            for f in files:
                if f.rpartition ('.')[2] == 'inst' :

                    try:        print (f);   os.remove (path + f)
                    except:     print ('ERROR | Unable to remove file : ', f)
            break

        return {"FINISHED"}
    
class ExporterOpTextures (bpy.types.Operator):
     
    bl_idname       = "object.shift_export_textures_operator"
    bl_label        = "SHIFT - Export"
    bl_description  = "Export all textures from unhidden objects from all scenes first layer (like scene exporter)"
    
    def execute (self, context):

        path = bpy.context.scene.shift_ex_textures_filepath

        if path != '':

            # cut off file name
            if path [len (path) - 1] != '\\':
            
                path = os.path.abspath (bpy.context.scene.shift_ex_instances_filepath).rpartition ('\\')[0] + '\\'
            
            # log
            print ('\nSHIFT textures export starting... \n\t', path)
            print ('')

            start_time = time.clock ()

            # list of materials
            materials = []
                            
            # take all objects
            object_list = bpy.data.objects

            # collecting objects and its meshes
            for obj in object_list:

                # skip objects that are not in first layer
                if not obj.layers [0]: continue
                
                # skip hidden objects
                if obj.hide: continue

                # skip objects with 'no_export' custom property
                try:
                    if obj      ['no_export'] == 1: continue
                    if obj.data ['no_export'] == 1: continue
                except: pass        
                            
                if obj.type == 'MESH':

                    mesh = obj.data;

                    # if mesh have material ..
                    if mesh.materials [0]:

                        mat = mesh.materials [0];   materials.append ((mat.name, mat));

                        # adding lod1
                        try:
                            m = bpy.data.meshes [mesh ['lod1']];
                            if (m.materials [0]):
                                mat = m.materials [0];   materials.append ((mat.name, mat));
                        except: pass

                        # adding lod2
                        try:
                            m = bpy.data.meshes [mesh ['lod2']];
                            if (m.materials [0]):
                                mat = m.materials [0];   materials.append ((mat.name, mat));
                        except: pass
                        
                        # adding lod3
                        try:
                            m = bpy.data.meshes [mesh ['lod3']];
                            if (m.materials [0]):
                                mat = m.materials [0];   materials.append ((mat.name, mat));
                        except: pass
                        
                        # adding occlusion
                        try:
                            m = bpy.data.meshes [obj.data ['occlusion']];
                            if (m.materials [0]):
                                mat = m.materials [0];   materials.append ((mat.name, mat));
                        except: pass            

                    # collect all particle systems meshes
                    for p in obj.particle_systems:
                        
                        if p.settings.dupli_object:
                            if (p.settings.dupli_object.type == 'MESH'):

                                # mesh
                                mesh = p.settings.dupli_object.data

                                # if mesh have material ..
                                if mesh.materials [0]:

                                    mat = mesh.materials [0];   materials.append ((mat.name, mat))                                        
            
            # removing duplicate materials
            materials.sort (key = lambda materials: materials [0])
            
            prevname = None;    l = len (materials)
            
            i = 0
            while (i < l):
                name = materials [i][0]
                if (name == prevname):
                    del materials [i];  l -= 1
                    continue
                prevname = name;        i += 1

            # copy textures to destination path
            for name, mat in materials:

                try:    tex = bpy.data.textures [mat ['tex_diffuse']]
                except: tex = None;
                if (tex):
                    shutil.copy2 (bpy.path.abspath (tex.image.filepath), path + tex.image.filepath.rpartition ('\\')[1]);    print (tex.image.filepath)
                try:    tex = bpy.data.textures [mat ['tex_composite']]
                except: tex = None;
                if (tex):
                    shutil.copy2 (bpy.path.abspath (tex.image.filepath), path + tex.image.filepath.rpartition ('\\')[1]);    print (tex.image.filepath)
                try:    tex = bpy.data.textures [mat ['tex_weights1']]
                except: tex = None;
                if (tex):
                    shutil.copy2 (bpy.path.abspath (tex.image.filepath), path + tex.image.filepath.rpartition ('\\')[1]);    print (tex.image.filepath)
                try:    tex = bpy.data.textures [mat ['tex_weights2']]
                except: tex = None;
                if (tex):
                    shutil.copy2 (bpy.path.abspath (tex.image.filepath), path + tex.image.filepath.rpartition ('\\')[1]);    print (tex.image.filepath)
                try:    tex = bpy.data.textures [mat ['tex_weights3']]
                except: tex = None;
                if (tex):
                    shutil.copy2 (bpy.path.abspath (tex.image.filepath), path + tex.image.filepath.rpartition ('\\')[1]);    print (tex.image.filepath)
                try:    tex = bpy.data.textures [mat ['tex_weights4']]
                except: tex = None;
                if (tex):
                    shutil.copy2 (bpy.path.abspath (tex.image.filepath), path + tex.image.filepath.rpartition ('\\')[1]);    print (tex.image.filepath)
                            
            # elapsed time
            print ('')
            print ('\nexport finished in %.4f sec.' % (time.clock () - start_time))
                        
            return {"FINISHED"}
        
        else:
            return {"CANCELLED"}

class ExporterOpTexturesClear (bpy.types.Operator):

    bl_idname       = "object.shift_export_textures_clear_operator"
    bl_label        = "SHIFT - Export"
    bl_description  = "Removes all *.tga and *.dds files from destination directory"
    
    def execute (self, context):

        path = bpy.context.scene.shift_ex_textures_filepath

        # cut off file name
        if path [len (path) - 1] != '\\':
        
            path = os.path.abspath (bpy.context.scene.shift_ex_textures_filepath).rpartition ('\\')[0] + '\\'

        print ('\nDELETING FILES : ')
        print ('\t' + path)
        
        for root, dirs, files in os.walk (path):

            for f in files:
                if f.rpartition ('.')[2] == 'tga' :

                    try:        print (f);   os.remove (path + f)
                    except:     print ('ERROR | Unable to remove file : ', f)
                    
                elif f.rpartition ('.')[2] == 'dds' :

                    try:        print (f);   os.remove (path + f)
                    except:     print ('ERROR | Unable to remove file : ', f)
            break

        return {"FINISHED"}
    
class ExporterPanel (bpy.types.Panel):
     
    bl_idname   = "object.shift_export_panel"
    bl_label    = "SHIFT - Export"
    bl_context  = "objectmode"
    bl_register = True
    bl_undo     = True

    bl_space_type   = 'VIEW_3D'
    bl_region_type  = 'TOOLS'

    def draw (self, context):
            
        layout = self.layout
        
        box = layout.box ()
        box.label           ('File :   ' + bpy.context.scene.shift_ex_scene_filepath.rpartition ('\\')[2])
        box = box.box ()
        split = box.split   (align = True, percentage = 0.4)
        split.operator      ('object.shift_export_scene_operator', 'Export')
        row = split.row ()
        row.alignment = 'LEFT'
        row.label           ('Scene')
        box.prop            (context.scene, 'shift_ex_scene_filepath')
        box.prop            (context.scene, 'shift_ex_scene_scale')
        
        layout.separator ()

        box = layout.box ()

        if bpy.context.scene.shift_ex_mesh_auto:               
            if len (bpy.context.selected_objects) == 1:
                box.label           ('File :   ' +  bpy.context.selected_objects [0].data.name + '.mesh')
            else:
                box.label           ('File :   *.mesh')
        else:
            box.label           ('File :   ' + bpy.context.scene.shift_ex_mesh_filepath.rpartition ('\\')[2])
            
        box = box.box ()
        split = box.split   (align = True, percentage = 0.4)
        split.operator      ('object.shift_export_mesh_operator', 'Export')
        row = split.row ()
        row.alignment = 'LEFT'
        row.label           ('Mesh')
        
        split = box.split   (align = True, percentage = 0.9)
        split.operator      ('object.shift_export_mesh_clear_operator', 'Clear')
        
        box.prop            (context.scene, 'shift_ex_mesh_filepath')
        box = box.box ()
        box.prop            (context.scene, 'shift_ex_mesh_skip')
        box.prop            (context.scene, 'shift_ex_mesh_auto')

        layout.separator ()
        
        box = layout.box ()

        if bpy.context.scene.shift_ex_instances_auto:
            box.label           ('File :   *.inst')
        else:
            box.label           ('File :   ' + bpy.context.scene.shift_ex_mesh_filepath.rpartition ('\\')[2])
            
        box = box.box ()
        split = box.split   (align = True, percentage = 0.4)
        split.operator      ('object.shift_export_instances_operator', 'Export')
        row = split.row ()
        row.alignment = 'LEFT'
        row.label           ('Instances')
        
        split = box.split   (align = True, percentage = 0.9)
        split.operator      ('object.shift_export_instances_clear_operator', 'Clear')
        
        box.prop            (context.scene, 'shift_ex_instances_filepath')

        _box = box.box ()
        (_box.box ()).prop  (context.scene, 'shift_ex_rotorder')
        split = (_box).split   (align = True, percentage = 0.6)
        col = split.column  (align = False)
        col.label           ('Random Rotation X :')
        col.label           ('Random Rotation Y :')
        col.label           ('Random Rotation Z :')
        col = split.column  (align = True)
        col.prop            (context.scene, 'shift_ex_instances_randomx')
        col.prop            (context.scene, 'shift_ex_instances_randomy')
        col.prop            (context.scene, 'shift_ex_instances_randomz')

        _box = box.box ()
        split = _box.split  (align = True, percentage = 0.5)
        col = split.column  (align = False)
        col.label           ('Subdivision X :')
        col.label           ('Subdivision Y :')
        col.label           ('Subdivision Z :')
        col = split.column  (align = True)
        col.prop            (context.scene, 'shift_ex_subdivisionx')
        col.prop            (context.scene, 'shift_ex_subdivisiony')
        col.prop            (context.scene, 'shift_ex_subdivisionz')

        _box = _box.box ()

        split = _box.split  (align = True, percentage = 0.5)
        split.label         ('Random Radius : ')
        split.prop          (context.scene, 'shift_ex_randomrad')
        split = _box.split  (align = True, percentage = 0.5)
        split.label         ('Cancellation Radius : ')
        split.prop          (context.scene, 'shift_ex_cancellation')
        
        box = box.box ()
        box.prop            (context.scene, 'shift_ex_instances_over')
        box.prop            (context.scene, 'shift_ex_instances_auto')

        layout.separator ()
        
        box = layout.box ()
            
        box = box.box ()
        split = box.split   (align = True, percentage = 0.4)
        split.operator      ('object.shift_export_textures_operator', 'Export')
        row = split.row ()
        row.alignment = 'LEFT'
        row.label           ('Textures')
        
        split = box.split   (align = True, percentage = 0.9)
        split.operator      ('object.shift_export_textures_clear_operator', 'Clear')
        
        box.prop            (context.scene, 'shift_ex_textures_filepath')

        
def register ():

    bpy.utils.register_module (__name__)

    # ----------------------------------------------------------
    bpy.types.Scene.shift_ex_scene_scale = FloatProperty (
        name        = "Scene scale",
        description = "Scale all geometry by constant factor",
        min         = 0.01,
        max         = 100.0,
        step        = 1.0,
        default     = 1.0)

    bpy.types.Scene.shift_ex_scene_filepath = StringProperty (
        name        = "",
        description = "File name with full path",
        default     = "",
        subtype     = 'FILE_PATH')
    
    # ----------------------------------------------------------
    bpy.types.Scene.shift_ex_mesh_filepath = StringProperty (
        name        = "",
        description = "File name with full path or destination directory",
        default     = "",
        subtype     = 'FILE_PATH')

    bpy.types.Scene.shift_ex_mesh_skip = BoolProperty (
        name        = "Skip Particle Emitters",
        description = "Meshes with particles systems attached are not exported",
        default     = True)
    
    bpy.types.Scene.shift_ex_mesh_auto = BoolProperty (
        name        = "Auto Name",
        description = "Generate file name from name of datablock",
        default     = True)    
    
    # ----------------------------------------------------------
    bpy.types.Scene.shift_ex_instances_filepath = StringProperty (
        name        = "",
        description = "File name with full path or destination directory",
        default     = "",
        subtype     = 'FILE_PATH')

    bpy.types.Scene.shift_ex_instances_auto = BoolProperty (
        name        = "Auto Name",
        description = "Generate file name from name of datablock",
        default     = True)

    bpy.types.Scene.shift_ex_instances_over = BoolProperty (
        name        = "Override settings",
        description = "Override specific generating options in particle system settings (custom properties)",
        default     = False)
    
    bpy.types.Scene.shift_ex_instances_randomx = FloatProperty (
        name        = "",
        description = "Random rotation of dupli object in X axis.",
        subtype     = 'ANGLE',
        min         = 0.0,
        max         = math.pi,
        step        = 1.0,
        default     = 0.0)

    bpy.types.Scene.shift_ex_instances_randomy = FloatProperty (
        name        = "",
        description = "Random rotation of dupli object in Y axis.",
        subtype     = 'ANGLE',
        min         = 0.0,
        max         = math.pi,
        step        = 1.0,
        default     = 0.0)
     
    bpy.types.Scene.shift_ex_instances_randomz = FloatProperty (
        name        = "",
        description = "Random rotation of dupli object in Z axis.",
        subtype     = 'ANGLE',
        min         = 0.0,
        max         = math.pi,
        step        = 1.0,
        default     = 0.0)

    bpy.types.Scene.shift_ex_subdivisionx = IntProperty (
        name        = "",
        description = "Subdivision level",
        min         = 1,
        max         = 16,
        step        = 1,
        default     = 1)
    bpy.types.Scene.shift_ex_subdivisiony = IntProperty (
        name        = "",
        description = "Subdivision level",
        min         = 1,
        max         = 16,
        step        = 1,
        default     = 1)    
    bpy.types.Scene.shift_ex_subdivisionz = IntProperty (
        name        = "",
        description = "Subdivision level",
        min         = 1,
        max         = 16,
        step        = 1,
        default     = 1)

    bpy.types.Scene.shift_ex_randomrad = FloatProperty (
        name        = "",
        description = "Random radius of jittering used in subdivision process.",
        subtype     = 'DISTANCE',
        min         = 0.0,
        max         = 1000.0,
        step        = 0.1,
        default     = 0.0)
    
    bpy.types.Scene.shift_ex_cancellation = FloatProperty (
        name        = "",
        description = "Cancellation radius, all other instances in this radius are removed.",
        subtype     = 'DISTANCE',
        min         = 0.0,
        max         = 10.0,
        step        = 0.001,
        default     = 0.0)
    
    bpy.types.Scene.shift_ex_rotorder = EnumProperty(
        name="Rot. Order",
        description="Random rotations order by axis",
        items=[("XYZ","XYZ","Random rotations order by axis"),
               ("YXZ","YXZ","Random rotations order by axis"),
               ("ZYX","ZYX","Random rotations order by axis"),
               ("XZY","XZY","Random rotations order by axis"),
               ("YZX","YZX","Random rotations order by axis"),
               ("ZXY","ZXY","Random rotations order by axis"),
              ],
        default='XYZ')
    
    # ----------------------------------------------------------
    bpy.types.Scene.shift_ex_textures_filepath = StringProperty (
        name        = "",
        description = "Destination directory",
        default     = "",
        subtype     = 'FILE_PATH')
    
def unregister ():

    bpy.utils.unregister_module (__name__)

if __name__ == "__main__":
    
    register ()
