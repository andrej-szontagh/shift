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
import math
import time
import struct
import ctypes
import random
import mathutils

from math       import radians
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
        
        if   'Basic'        in mat.name:
            
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
            
        elif 'Layered'      in mat.name:
            
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
    
    # opening new binary file
    stream = io.open (filepath, mode = 'wb')

    # write count of instances
    stream.write (struct.pack ('H', len (psystem.particles)))

    # global matrix
    scenematrix = mathutils.Matrix.Rotation (radians (- 90), 4, 'X')

    minco = [ 99999999,  99999999,  99999999]
    maxco = [-99999999, -99999999, -99999999]

    for v in mesh.vertices :
        
        if v.co [0] < minco [0]:    minco [0] = v.co [0]
        if v.co [1] < minco [1]:    minco [1] = v.co [1]
        if v.co [2] < minco [2]:    minco [2] = v.co [2]
        if v.co [0] > maxco [0]:    maxco [0] = v.co [0]
        if v.co [1] > maxco [1]:    maxco [1] = v.co [1]
        if v.co [2] > maxco [2]:    maxco [2] = v.co [2]

    points = [mathutils.Vector ((minco [0], minco [1], minco [2])),
              mathutils.Vector ((maxco [0], minco [1], minco [2])),
              mathutils.Vector ((minco [0], maxco [1], minco [2])),
              mathutils.Vector ((maxco [0], maxco [1], minco [2])),
              mathutils.Vector ((minco [0], minco [1], maxco [2])),
              mathutils.Vector ((maxco [0], minco [1], maxco [2])),
              mathutils.Vector ((minco [0], maxco [1], maxco [2])),
              mathutils.Vector ((maxco [0], maxco [1], maxco [2]))]
    
    minco [:] = []
    maxco [:] = []

    minco = [ 99999999,  99999999,  99999999]
    maxco = [-99999999, -99999999, -99999999]

    for p in psystem.particles:
        
        # transformation values ..
                
        scale       = (p.hair [0].co - p.hair [1].co).length * (len (p.hair) - 1) * 0.5
        translation =  p.hair [0].co

        # build matrix for instance
        matrix = mathutils.Matrix ();   matrix.identity ()
        
        # random rotations
        matrix = mathutils.Matrix.Rotation (random.random () * scene.shift_ex_instances_randomx, 4, 'X') * matrix
        matrix = mathutils.Matrix.Rotation (random.random () * scene.shift_ex_instances_randomy, 4, 'Y') * matrix
        matrix = mathutils.Matrix.Rotation (random.random () * scene.shift_ex_instances_randomz, 4, 'Z') * matrix
        
        # scale
        matrix = mathutils.Matrix.Scale (scale, 4) * matrix
        
        # hack ..
        matrix = mathutils.Matrix.Rotation (radians (90), 4, 'Y') * matrix

        # rotation
        rotmatrix = p.rotation.to_matrix ();    rotmatrix.resize_4x4 ();    matrix = rotmatrix * matrix
        
        # translation
        matrix = mathutils.Matrix.Translation (translation) * matrix
        
        # rotate world matrix to match up vector with y axis
        matrix = scenematrix * matrix

        # transformation values ..
                
        rotation    =  matrix.to_euler ('XYZ')
        
        # collect whole system boundary
        for point in points :

            pointn = point * matrix
            
            if pointn [0] < minco [0]:  minco [0] = pointn [0]
            if pointn [1] < minco [1]:  minco [1] = pointn [1]
            if pointn [2] < minco [2]:  minco [2] = pointn [2]
            if pointn [0] > maxco [0]:  maxco [0] = pointn [0]
            if pointn [1] > maxco [1]:  maxco [1] = pointn [1]
            if pointn [2] > maxco [2]:  maxco [2] = pointn [2]

        # position  
        stream.write (struct.pack ('f',   translation.x))
        stream.write (struct.pack ('f',   translation.z))
        stream.write (struct.pack ('f', - translation.y))
        stream.write (struct.pack ('f', - rotation.x))
        stream.write (struct.pack ('f', - rotation.y))
        stream.write (struct.pack ('f', - rotation.z))
        stream.write (struct.pack ('f',   scale))            

    # boundary
    stream.write (struct.pack ('f', minco [0]))
    stream.write (struct.pack ('f', minco [1]))
    stream.write (struct.pack ('f', minco [2]))

    stream.write (struct.pack ('f', maxco [0]))
    stream.write (struct.pack ('f', maxco [1]))
    stream.write (struct.pack ('f', maxco [2]))
    
    # closing file
    stream.close ()

    # clean up
    points [:] = []
    
    minco [:] = []
    maxco [:] = []
    
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
                try:    meshes.append (((obj ['occlusion'], bpy.data.meshes [obj ['occlusion']])))
                except:
                    try:    meshes.append (((obj.data ['occlusion'], bpy.data.meshes [obj.data ['occlusion']])))
                    except: pass            

                # append object
                objects.append ((obj.name, obj))

            # collect all particle systems meshes
            for p in obj.particle_systems:
                
                if p.settings.dupli_object:
                    if (p.settings.dupli_object.type == 'MESH'):

                        # mesh
                        mesh = p.settings.dupli_object.data

                        # if mesh have material ..
                        if mesh.materials [0]:
                        
                            # append object
                            objects.append ((obj.name + '_' + p.name, p))
                            
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
    
    i = 0;
    while (i < l):
        name = materials [i][0]
        if (name == prevname):
            del materials [i];  l -= 1;
            continue
        prevname = name;        i += 1

    # collecting textures
    for name, mat in materials:
        for tex in mat.texture_slots:
            if tex:
                textures.append (tex)

    # writing header
    stream.write (struct.pack ('i', len (objects)))
    stream.write (struct.pack ('i', len (meshes)))
    stream.write (struct.pack ('i', len (materials)))
    stream.write (struct.pack ('i', len (textures)))

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
    
    for i, tex in enumerate (textures):

        # saving index
        tex.texture.id_data ["tmp_index"] = i

        image = tex.texture.image;

        filename = os.path.basename (image.filepath)

        # file name
        stream.write (struct.pack ('i',     len (filename) + 1))
        stream.write (struct.pack ('%isB' % len (filename), filename.encode ('ascii'), 0))

        if   tex.texture.extension == 'REPEAT':

            stream.write (struct.pack ('i', 0))     # wraps
            stream.write (struct.pack ('i', 0))     # wrapt
            
        elif tex.texture.extension == 'EXTEND':

            stream.write (struct.pack ('i', 1))     # wraps
            stream.write (struct.pack ('i', 1))     # wrapt
            
        elif tex.texture.extension == 'CLIP':

            stream.write (struct.pack ('i', 2))     # wraps
            stream.write (struct.pack ('i', 2))     # wrapt
            
        else :
            
            stream.write (struct.pack ('i', 0))     # wraps
            stream.write (struct.pack ('i', 0))     # wrapt

        # anisotrophy        
        try:    stream.write (struct.pack ('i', tex.texture.id_data ["anisotrophy"]))
        except: stream.write (struct.pack ('i', 1))

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
        if   ("Basic"         in name):
            stream.write (struct.pack ('i', 1))       # type

            try:    stream.write (struct.pack ('f', mat ['gloss']))
            except:
                stream.write (struct.pack ('f', 1.0))
                print ('ERROR | Material : ', name, ' custom property \'gloss\' not set')

            try:    stream.write (struct.pack ('f', mat ['shininess']))
            except:
                stream.write (struct.pack ('f', 10.0))
                print ('ERROR | Material : ', name, ' custom property \'shininess\' not set')
                
            specular = - 1
            diffuse = - 1
            normal = - 1

            for tex in mat.texture_slots:
                if tex:
                    if (tex.use_map_specular):          specular = tex.texture.id_data ["tmp_index"];
                    if (tex.use_map_color_diffuse):     diffuse = tex.texture.id_data ["tmp_index"];
                    if (tex.use_map_normal):            normal = tex.texture.id_data ["tmp_index"];

            stream.write (struct.pack ('i', normal))
            stream.write (struct.pack ('i', diffuse))
            stream.write (struct.pack ('i', specular))
            
        elif ("Layered"       in name):
            stream.write (struct.pack ('i', 2))       # type

            try:    stream.write (struct.pack ('f', mat ['gloss1']))
            except:
                stream.write (struct.pack ('f', 1.0))
                print ('ERROR | Material : ', name, ' custom property \'gloss1\' not set')
            try:    stream.write (struct.pack ('f', mat ['gloss2']))
            except:
                stream.write (struct.pack ('f', 1.0))
                print ('ERROR | Material : ', name, ' custom property \'gloss2\' not set')
            try:    stream.write (struct.pack ('f', mat ['gloss3']))
            except:
                stream.write (struct.pack ('f', 1.0))
                print ('ERROR | Material : ', name, ' custom property \'gloss3\' not set')
            try:    stream.write (struct.pack ('f', mat ['shininess1']))
            except:
                stream.write (struct.pack ('f', 10.0))
                print ('ERROR | Material : ', name, ' custom property \'shininess1\' not set')
            try:    stream.write (struct.pack ('f', mat ['shininess2']))
            except:
                stream.write (struct.pack ('f', 10.0))
                print ('ERROR | Material : ', name, ' custom property \'shininess2\' not set')
            try:    stream.write (struct.pack ('f', mat ['shininess3']))
            except:
                stream.write (struct.pack ('f', 10.0))
                print ('ERROR | Material : ', name, ' custom property \'shininess3\' not set')

            specular1 = - 1;    specular2 = - 1;    specular3 = - 1;
            diffuse1 = - 1;     diffuse2 = - 1;     diffuse3 = - 1;
            normal1 = - 1;      normal2 = - 1;      normal3 = - 1;

            for tex in mat.texture_slots:
                if tex:
                    if (tex.use_map_specular):
                        if   (specular1 < 0):       specular1 = tex.texture.id_data ["tmp_index"];
                        elif (specular2 < 0):       specular2 = tex.texture.id_data ["tmp_index"];
                        elif (specular3 < 0):       specular3 = tex.texture.id_data ["tmp_index"];
                    if (tex.use_map_color_diffuse):
                        if   (diffuse1 < 0):        diffuse1 = tex.texture.id_data ["tmp_index"];
                        elif (diffuse2 < 0):        diffuse2 = tex.texture.id_data ["tmp_index"];
                        elif (diffuse3 < 0):        diffuse3 = tex.texture.id_data ["tmp_index"];                    
                    if (tex.use_map_normal):
                        if   (normal1 < 0):         normal1 = tex.texture.id_data ["tmp_index"];
                        elif (normal2 < 0):         normal2 = tex.texture.id_data ["tmp_index"];
                        elif (normal3 < 0):         normal3 = tex.texture.id_data ["tmp_index"];
                        
                    if (tex.use_map_translucency):  mask = tex.texture.id_data ["tmp_index"];

            stream.write (struct.pack ('i', mask))
            stream.write (struct.pack ('i', normal1))
            stream.write (struct.pack ('i', normal2))
            stream.write (struct.pack ('i', normal3))
            stream.write (struct.pack ('i', diffuse1))
            stream.write (struct.pack ('i', diffuse2))
            stream.write (struct.pack ('i', diffuse3))
            stream.write (struct.pack ('i', specular1))
            stream.write (struct.pack ('i', specular2))
            stream.write (struct.pack ('i', specular3))
            
        elif ("Enviroment"    in name):
            stream.write (struct.pack ('i', 3))       # type

            diffuse = - 1;

            for tex in mat.texture_slots:
                if tex:
                    if (tex.use_map_color_diffuse):
                        diffuse = tex.texture.id_data ["tmp_index"];

            stream.write (struct.pack ('i', diffuse))
            
        elif ("Foliage"          in name):
            stream.write (struct.pack ('i', 4))       # type
            
            try:    stream.write (struct.pack ('f', mat ['gloss']))
            except:
                stream.write (struct.pack ('f', 1.0))
                print ('ERROR | Material : ', name, ' custom property \'gloss\' not set')

            try:    stream.write (struct.pack ('f', mat ['shininess']))
            except:
                stream.write (struct.pack ('f', 10.0))
                print ('ERROR | Material : ', name, ' custom property \'shininess\' not set')
            
            specular = - 1
            diffuse = - 1
            normal = - 1

            for tex in mat.texture_slots:
                if tex:
                    if (tex.use_map_specular):          specular = tex.texture.id_data ["tmp_index"];
                    if (tex.use_map_color_diffuse):     diffuse = tex.texture.id_data ["tmp_index"];
                    if (tex.use_map_normal):            normal = tex.texture.id_data ["tmp_index"];

            stream.write (struct.pack ('i', normal))
            stream.write (struct.pack ('i', diffuse))
            stream.write (struct.pack ('i', specular))
            
        elif ("Grass"         in name):
            stream.write (struct.pack ('i', 5))       # type

            # gloss
            try:    stream.write (struct.pack ('f', mat ['gloss']))
            except:
                stream.write (struct.pack ('f', 1.0))
                print ('ERROR | Material : ', name, ' custom property \'gloss\' not set')

            # shininess
            try:    stream.write (struct.pack ('f', mat ['shininess']))
            except:
                stream.write (struct.pack ('f', 10.0))
                print ('ERROR | Material : ', name, ' custom property \'shininess\' not set')

            try: mat ['range1']
            except: print ('ERROR | Material : ', name, ' custom property \'range1\' not set')

            try: mat ['range2']
            except: print ('ERROR | Material : ', name, ' custom property \'range2\' not set')

            try: mat ['range3']
            except: print ('ERROR | Material : ', name, ' custom property \'range3\' not set')

            try:
                if not ((mat ['range1'] < mat ['range2']) and (mat ['range2'] < mat ['range3'])):   raise
                
            except: print ('ERROR | Material : ', name, ' custom property rule (range1 < range2 < range3) broken')
            
            # range1
            stream.write (struct.pack ('f', mat ['range1']))

            # range2
            stream.write (struct.pack ('f', mat ['range2']))
            
            # range3
            stream.write (struct.pack ('f', mat ['range3']))
                
            diffuse = - 1

            for tex in mat.texture_slots:
                if tex:
                    if (tex.use_map_color_diffuse):     diffuse = tex.texture.id_data ["tmp_index"];

            stream.write (struct.pack ('i', diffuse))
            
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

        morphing = False

        # params
        try:        stream.write (struct.pack ('f', mesh ['morph_position']));  morphing = True
        except:     stream.write (struct.pack ('f', 0.0))
        try:        stream.write (struct.pack ('f', mesh ['morph_normal']));  morphing = True
        except:     stream.write (struct.pack ('f', 0.0))
        try:        stream.write (struct.pack ('f', mesh ['morph_uv']));  morphing = True
        except:     stream.write (struct.pack ('f', 0.0))

        occlusion = False

        try:    bpy.data.meshes [mesh ['occlusion']];  occlusion = True
        except:     pass
        
        occluder = False

        try:
            if   mesh ['occluder'] == 1:  occluder = True
            elif mesh ['occluder'] == 0:  occluder = False
        except:     pass
        
        display_list = False

        try:
            if   mesh ['display_list'] == 1:  display_list = True
            elif mesh ['display_list'] == 0:  display_list = False
        except:     pass
        
        # flags
        flags = 0
        
        if display_list:    flags = flags | 1
        if morphing:        flags = flags | 2
        if occluder:        flags = flags | 4
        if occlusion:       flags = flags | 8

        stream.write (struct.pack ('H', flags))

        print ('Mesh : %-20s' % name)
    print ('')
                            
    #------------------------------------------------------------------------------------
    # WRITING OBJECTS
    #------------------------------------------------------------------------------------

    scenematrix = mathutils.Matrix.Scale (bpy.context.scene.shift_ex_scene_scale, 4) * mathutils.Matrix.Rotation (radians (- 90), 4, 'X');
    
    for name, obj in objects:

        if   type (obj) == bpy.types.Object:                mesh = obj.data
        elif type (obj) == bpy.types.ParticleSystem:        mesh = obj.settings.dupli_object.data

        # object name
        stream.write (struct.pack ('i',     len (name) + 1))
        stream.write (struct.pack ('%isB' % len (name), name.encode ('ascii'), 0))
        
        # mesh index
        stream.write (struct.pack ('H', mesh ['tmp_index']))

        # material index
        stream.write (struct.pack ('H', mesh.materials [0]['tmp_index']))

        occlusion = False

        # occlusion mesh index
        try:    stream.write (struct.pack ('H', bpy.data.meshes [obj ['occlusion']]['tmp_index']));     occlusion = True
        except:
            try:    stream.write (struct.pack ('H', bpy.data.meshes [mesh ['occlusion']]['tmp_index']));   occlusion = True
            except: stream.write (struct.pack ('H', 0xffff))

        # disappear distance
        try:    disappear = obj ['disappear']
        except:
            try:    disappear = mesh ['disappear']
            except: disappear = 1000000.0

        stream.write (struct.pack ('f', disappear))
        
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
        except:     pass
                
        flags = 0
        
        if occlusion:   flags = flags | 8
        if occluder:    flags = flags | 4

        stream.write (struct.pack ('H', flags))
            
        # detail
        lod1 = 0xffff;  lod1_distance = disappear
        lod2 = 0xffff;  lod2_distance = disappear
        lod3 = 0xffff;  lod3_distance = disappear

        try:    lod_start = mesh ['lod_start']
        except: lod_start = 0.0
        
        try:
            lod1 = mesh ['lod1']
            try:
                lod2 = mesh ['lod2']
                try:
                    lod3 = mesh ['lod3']                    
                except: pass
            except: pass
        except: pass

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
                stream.write (struct.pack ('f', lod_start))
                stream.write (struct.pack ('H', lod1))
                stream.write (struct.pack ('f', lod1_distance))
                stream.write (struct.pack ('H', lod2))
                stream.write (struct.pack ('f', lod2_distance))
                stream.write (struct.pack ('H', lod3))
                stream.write (struct.pack ('f', lod3_distance))
            
        else:   stream.write (struct.pack ('I', 0))

        if type (obj) == bpy.types.Object:
                
            # is not instances object
            stream.write (struct.pack ('I', 0))
            
            # rotate world matrix to match up vector with y axis
            matrix = scenematrix * obj.matrix_world

            # world matrix
            for i in range (4):
                v = matrix [i]
                for j in range (4):
                    stream.write (struct.pack ('f', v [j]))
        
        elif type (obj) == bpy.types.ParticleSystem:

            # this is instances object
            stream.write (struct.pack ('I', 1))
                                    
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
        del tex.texture.id_data ["tmp_index"]
    for name, mat in materials:
        del mat.id_data         ["tmp_index"]
    for name, msh in meshes:
        del msh.id_data         ["tmp_index"]

    # cleanup
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

                    obj = bpy.context.object

                    if (obj.type == 'MESH'):

                        for p in obj.particle_systems:

                            # generated name stitched with full path
                            filename = obj.name + '_' + p.name + '.inst';  filepath = path + filename
                            
                            # export
                            processExportInstances (p, filepath)
                            
                            # log
                            print ('\t' + filename)
                        
                    else:   print ('ERROR | Invalid object type.'); return {"CANCELLED"}
                    
                else:

                    # log
                    print ('\nSHIFT instances export starting... \n\t', path)
                    print ('')

                    start_time = time.clock ()

                    for obj in bpy.context.selected_objects:

                        if (obj.type == 'MESH'):
                            
                            for p in obj.particle_systems:

                                # generated name stitched with full path
                                filename = obj.name + '_' + p.name + '.inst';  filepath = path + filename
                                
                                # export
                                processExportInstances (p, filepath)

                                # log
                                print ('\t' + filename)
                                
                    # elapsed time
                    print ('')
                    print ('\nexport finished in %.4f sec.' % (time.clock () - start_time))
                                                                
            else:
                
                if len (bpy.context.selected_objects) == 1:

                    obj = bpy.context.object

                    if (obj.type == 'MESH'):

                        filepath = bpy.context.scene.shift_ex_instances_filepath

                        # check for invalid file name
                        if filepath.endswith ("\\"):
                            
                            print ('ERROR | Missing file name.')
                            return {"CANCELLED"}

                        filepath = os.path.abspath (filepath)
                        
                        # append extension if needed
                        if not filepath.endswith (".inst"): filepath += ".inst"

                        if obj.particle_systems.active :
                        
                            # export
                            processExportInstances (obj.particle_systems.active, filepath)

                            # log
                            print ('\t' + filepath)

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
            if len (bpy.context.selected_objects) == 1:
                box.label           ('File :   ' +  bpy.context.selected_objects [0].data.name + '.inst')
            else:
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
        box_ = box.box      ()
        col  = box_.column  (align = True)
        split = col.split   (align = False, percentage = 0.6)
        split.label         ('Random Rotation X :')
        split.prop          (context.scene, 'shift_ex_instances_randomx')
        split = col.split   (align = False, percentage = 0.6)
        split.label         ('Random Rotation Y :')
        split.prop          (context.scene, 'shift_ex_instances_randomy')
        split = col.split   (align = False, percentage = 0.6)
        split.label         ('Random Rotation Z :')
        split.prop          (context.scene, 'shift_ex_instances_randomz')
        box = box.box ()
        box.prop            (context.scene, 'shift_ex_instances_auto')
        
def register ():

    bpy.utils.register_module (__name__)

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
    
    bpy.types.Scene.shift_ex_instances_filepath = StringProperty (
        name        = "",
        description = "File name with full path or destination directory",
        default     = "",
        subtype     = 'FILE_PATH')

    bpy.types.Scene.shift_ex_instances_auto = BoolProperty (
        name        = "Auto Name",
        description = "Generate file name from name of datablock",
        default     = True)

    bpy.types.Scene.shift_ex_instances_randomx = FloatProperty (
        name        = "",
        description = "Random rotation of dupli object in X axis.",
        subtype     = 'ANGLE',
        min         = 0.0,
        max         = math.pi,
        step        = 1.0,
        default     = 1.0)

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
    
def unregister ():

    bpy.utils.unregister_module (__name__)

    del bpy.types.Scene.shift_ex_scene_filepath
    del bpy.types.Scene.shift_ex_scene_scale
    
    del bpy.types.Scene.shift_ex_mesh_filepath
    del bpy.types.Scene.shift_ex_mesh_skip
    del bpy.types.Scene.shift_ex_mesh_auto
    
    del bpy.types.Scene.shift_ex_instances_filepath
    del bpy.types.Scene.shift_ex_instances_auto
    del bpy.types.Scene.shift_ex_instances_randomx
    del bpy.types.Scene.shift_ex_instances_randomy
    del bpy.types.Scene.shift_ex_instances_randomz
             
if __name__ == "__main__":
    
    register ()
