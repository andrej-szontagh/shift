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
    "name"              : "SHIFT - Mesh Tools",
    "author"            : "BBC",
    "version"           : (1,0),
    "blender"           : (2, 5, 3),
    "api"               : 31236,
    "category"          : "Object",
    "location"          : "Tool Shelf",
    "warning"           : '',
    "wiki_url"          : "",
    "tracker_url"       : "",
    "description"       : "Various mesh operations"}

import os
import bpy
import sys
import time
import math
import ctypes
import operator
import mathutils

from math       import *
from ctypes     import *
from bpy.props  import *

####------------------------------------------------------------------------------------------------------------------------------------------------------
#### PROCESS TRIANGULATE
####------------------------------------------------------------------------------------------------------------------------------------------------------

def processTriangulate ():

    start_time = time.clock ()

    # shortcut
    scene = bpy.context.scene

    # shortcut
    selected = bpy.context.selected_objects

    # log
    print ('\nTriangulate starting... \n\n\tObjects (', len (selected), ') :')
    print ('')

    for i, obj in enumerate (selected):

        # is it mesh ?
        if obj and obj.type == 'MESH':

            # set active object
            scene.objects.active = obj

            # saving vertex normals
            if scene.shift_mt_preserve :
                
                normals = [mathutils.Vector ((v.normal [0], v.normal [1], v.normal [2])) for v in obj.data.vertices]
            
            # edit mode
            bpy.ops.object.mode_set (mode = 'EDIT')
        
            # face selection mode
            bpy.context.tool_settings.mesh_select_mode = (False, False, True)
        
            # select all
            bpy.ops.mesh.select_all (action = 'SELECT')
        
            # unhide all faces
            bpy.ops.mesh.reveal ()
        
            # conversion
            bpy.ops.mesh.quads_convert_to_tris ()
        
            # object mode
            bpy.ops.object.mode_set (mode = 'OBJECT')

            if scene.shift_mt_preserve :

                # restoring normals
                for v in obj.data.vertices:    v.normal = normals [v.index]

                # clean up                
                normals [:] = []

            # log
            print (i + 1, "Object : '", obj.name, "'")
            
        else:
            
            # log
            print (i + 1, "Object : '", obj.name, "' is not a mesh")
        
    # log            
    print ('')
    print ('Triangulate finished in %.4f sec.' % (time.clock () - start_time))

####------------------------------------------------------------------------------------------------------------------------------------------------------
#### PROCESS SPLIT GRID
####------------------------------------------------------------------------------------------------------------------------------------------------------

def processSplitGrid (obj):

    start_time = time.clock ()

    # shortcut
    mesh = obj.data
    
    # shortcut
    scene = bpy.context.scene

    # active object
    scene.objects.active = obj;     obj.select = True

    # is it mesh ?
    if not obj or obj.type != 'MESH':  return

    # log
    print ('\nSplit starting... \n\n\tsplits (', scene.shift_mt_subdivisionx * scene.shift_mt_subdivisiony * scene.shift_mt_subdivisionz, ') :')
    print ('')

    # check mesh
    for f in mesh.faces:
        
        try:    f.vertices [3]
        except: continue

        print ('ERROR | Mesh :', mesh.name, ' containts one or more quads.')
        return -1

    # log
    start_time_ = time.clock ();    print ('Prepare ..')

    # save group name
    gname = "tmp_group"

    # create temporary group
    bpy.ops.group.create (name = gname)

    # our group
    group = bpy.data.groups [gname]

    # saving vertex data
    if scene.shift_mt_preserve :

        layer = mesh.vertex_colors.new (name = gname)

        mesh.vertex_colors.active = layer

        for i, col in enumerate (layer.data) :

            f = mesh.faces [i]

            v = mesh.vertices [f.vertices [0]]
            col.color1 [0] = 0.5 + 0.5 * v.normal [0]
            col.color1 [1] = 0.5 + 0.5 * v.normal [1]
            col.color1 [2] = 0.5 + 0.5 * v.normal [2]
            
            v = mesh.vertices [f.vertices [1]]
            col.color2 [0] = 0.5 + 0.5 * v.normal [0]
            col.color2 [1] = 0.5 + 0.5 * v.normal [1]
            col.color2 [2] = 0.5 + 0.5 * v.normal [2]

            v = mesh.vertices [f.vertices [2]]
            col.color3 [0] = 0.5 + 0.5 * v.normal [0]
            col.color3 [1] = 0.5 + 0.5 * v.normal [1]
            col.color3 [2] = 0.5 + 0.5 * v.normal [2]
                        
    # edit mode
    bpy.ops.object.mode_set (mode = 'EDIT')
        
    # vertex selection mode
    bpy.context.tool_settings.mesh_select_mode = (True, False, False)

    # unhide all vertices
    bpy.ops.mesh.reveal ()

    # deselect
    bpy.ops.mesh.select_all (action = 'DESELECT')

    # object mode
    bpy.ops.object.mode_set (mode = 'OBJECT')

    # deselect
    bpy.ops.object.select_all (action = 'DESELECT')
    
    # evaluating bounding box
    minv    = mathutils.Vector (( 999999999.0,  999999999.0,  999999999.0))
    maxv    = mathutils.Vector ((-999999999.0, -999999999.0, -999999999.0))

    for v in obj.data.vertices:

        if v.co [0] < minv [0]: minv [0] = v.co [0]
        if v.co [1] < minv [1]: minv [1] = v.co [1]
        if v.co [2] < minv [2]: minv [2] = v.co [2]
        
        if v.co [0] > maxv [0]: maxv [0] = v.co [0]
        if v.co [1] > maxv [1]: maxv [1] = v.co [1]
        if v.co [2] > maxv [2]: maxv [2] = v.co [2]

    clusterx = abs (maxv [0] - minv [0]) / scene.shift_mt_subdivisionx
    clustery = abs (maxv [1] - minv [1]) / scene.shift_mt_subdivisiony
    clusterz = abs (maxv [2] - minv [2]) / scene.shift_mt_subdivisionz

    # particles
    if (scene.shift_mt_particles):

        # array of face vertices
        facestype = ctypes.c_float * (len (obj.data.faces) * 9);    faces = facestype ()

        # internal copies of particle systems
        particles  = [None for s in obj.particle_systems]
        particlesi = [None for s in obj.particle_systems]

        # particle data
        data_particles = [None for s in obj.particle_systems]

        # counters
        particlec = [0 for s in obj.particle_systems]

        for i, s in enumerate (obj.particle_systems):

            if (s.settings.type == 'HAIR'):

                # active system
                obj.particle_systems.active_index = i

                bpy.ops.particle.particle_edit_toggle ()
                bpy.ops.particle.particle_edit_toggle ()
                
                # shortcut
                obj_particles = s.particles
                
                # array of particle positions and their indices
                particlestype   = ctypes.c_float * (len (obj_particles) * 3);   particles  [i] = particlestype ()
                particlestype   = ctypes.c_uint  *  len (obj_particles);        particlesi [i] = particlestype ()
        
                # saving particle data
                data_particles [i] = [[] for p in obj_particles]

                # shortcuts
                data_particless = data_particles [i]
                particless      = particles [i]

                c = 0
                for j, p in enumerate (obj_particles):

                    particless [c    ] = ctypes.c_float (p.is_hair [0].co [0])
                    particless [c + 1] = ctypes.c_float (p.is_hair [0].co [1])
                    particless [c + 2] = ctypes.c_float (p.is_hair [0].co [2]);  c += 3
                    
                    data_particless [j] = [ p.alive_state, \
                                            p.location [0], \
                                            p.location [1], \
                                            p.location [2], \
                                            p.rotation [0], \
                                            p.rotation [1], \
                                            p.rotation [2], \
                                            p.rotation [3], p.size, \
                                            p.velocity [0], \
                                            p.velocity [1], \
                                            p.velocity [2], \
                                            p.angular_velocity [0], \
                                            p.angular_velocity [1], \
                                            p.angular_velocity [2], \
                                            p.prev_location [0], \
                                            p.prev_location [1], \
                                            p.prev_location [2], \
                                            p.prev_rotation [0], \
                                            p.prev_rotation [1], \
                                            p.prev_rotation [2], \
                                            p.prev_rotation [3], \
                                            p.prev_velocity [0], \
                                            p.prev_velocity [1], \
                                            p.prev_velocity [2], \
                                            p.prev_angular_velocity [0], \
                                            p.prev_angular_velocity [1], \
                                            p.prev_angular_velocity [2], \
                                            p.lifetime,   p.birth_time,   p.die_time, \
                                            len (p.is_hair),  [], \
                                            len (p.keys),     []]
                    
                    data = data_particless [j][32]
                    for pp in p.is_hair:
                        data.append (pp.co [0])
                        data.append (pp.co [1])
                        data.append (pp.co [2])
                        data.append (pp.co_hair_space [0])
                        data.append (pp.co_hair_space [1])
                        data.append (pp.co_hair_space [2])
                        data.append (pp.time)
                        data.append (pp.weight)
                
                    data = data_particless [j][34]
                    for k in p.keys:
                        data.append (angular_velocity [0])
                        data.append (angular_velocity [1])
                        data.append (angular_velocity [2])
                        data.append (location [0])
                        data.append (location [1])
                        data.append (location [2])
                        data.append (rotation [0])
                        data.append (rotation [1])
                        data.append (rotation [2])
                        data.append (rotation [3])
                        data.append (velocity [0])
                        data.append (velocity [1])
                        data.append (velocity [2])
                        data.append (time)
        
                # free memory
                s.settings.count = 0

                bpy.ops.particle.edited_clear ()
                
                bpy.ops.particle.particle_edit_toggle ()
                bpy.ops.particle.particle_edit_toggle ()

                # save count
                particlec [i] = c
        
    # log
    print ('Prepare done in %.4f sec' % (time.clock () - start_time_))

    print ('')        
    print ('Splitting ..')
    
    if (scene.shift_mt_subdivisionx > 1):
        
        for obj in group.objects:

            # active object
            scene.objects.active = obj; obj.select = True
                    
            # deselect vertices
            for v in obj.data.vertices: v.select = False
            
            position = minv [0]
            for x in range (scene.shift_mt_subdivisionx - 1):

                # end positiom
                end = position + clusterx
                    
                # taking whole faces
                for f in obj.data.faces:

                    for j in f.vertices: 
                        v = obj.data.vertices [j]
                        if  v.co [0] >= position and v.co [0] <= end :
                            
                            for k in f.vertices: 
                                obj.data.vertices [k].select = True
                                
                            break

                # edit mode
                bpy.ops.object.mode_set (mode = 'EDIT')

                # separate
                bpy.ops.mesh.separate ()
                
                # object mode
                bpy.ops.object.mode_set (mode = 'OBJECT')
                                                                
                # advance
                position += clusterx

                # log
                print ("Split ", len (group.objects))
                
            # unselect
            obj.select = False

    if (scene.shift_mt_subdivisiony > 1):
        
        for obj in group.objects:

            # active object
            scene.objects.active = obj; obj.select = True
        
            # deselect vertices
            for v in obj.data.vertices: v.select = False
            
            position = minv [1]
            for y in range (scene.shift_mt_subdivisiony - 1):

                # end positiom
                end = position + clustery
            
                # taking whole faces
                for f in obj.data.faces:

                    for j in f.vertices: 
                        v = obj.data.vertices [j]
                        if  v.co [1] >= position and v.co [1] <= end :
                            
                            for k in f.vertices: 
                                obj.data.vertices [k].select = True
                                
                            break
                
                # edit mode
                bpy.ops.object.mode_set (mode = 'EDIT')
                
                # separate
                bpy.ops.mesh.separate ()
                
                # object mode
                bpy.ops.object.mode_set (mode = 'OBJECT')
                                
                # advance
                position += clustery

                # log
                print ("Split ", len (group.objects))
                
            # unselect
            obj.select = False

    if (scene.shift_mt_subdivisionz > 1):

        for obj in group.objects:

            # active object
            scene.objects.active = obj; obj.select = True
        
            # deselect vertices
            for v in obj.data.vertices: v.select = False
            
            position = minv [2]
            for z in range (scene.shift_mt_subdivisionz - 1):

                # end positiom
                end = position + clusterz
            
                # taking whole faces
                for f in obj.data.faces:

                    for j in f.vertices: 
                        v = obj.data.vertices [j]
                        if  v.co [2] >= position and v.co [2] <= end :
                            
                            for k in f.vertices: 
                                obj.data.vertices [k].select = True
                                
                            break
                                    
                # edit mode
                bpy.ops.object.mode_set (mode = 'EDIT')

                # separate
                bpy.ops.mesh.separate ()

                # object mode
                bpy.ops.object.mode_set (mode = 'OBJECT')
                                    
                # advance
                position += clusterz

                # log
                print ("Split ", len (group.objects))
                
            # unselect
            obj.select = False

    # object mode
    bpy.ops.object.mode_set (mode = 'OBJECT')
    
    # deselect
    bpy.ops.object.select_all (action = 'DESELECT')

    # particles
    if (scene.shift_mt_particles):
        
        print ('')        
        print ('Splitting particles ..')

    # select new objects
    for obji, obj in enumerate (group.objects):

        # set active object
        scene.objects.active = obj;     obj.select = True

        # particles
        if (scene.shift_mt_particles):

            facec = 0

            # toolkit dll
            toolkit = windll.LoadLibrary (sys.path [0] + '\shift_toolkit.dll')

            for f in obj.data.faces:
                
                v1 = obj.data.vertices [f.vertices [0]]
                v2 = obj.data.vertices [f.vertices [1]]
                v3 = obj.data.vertices [f.vertices [2]]

                faces [facec    ] = v1.co [0]
                faces [facec + 1] = v1.co [1]
                faces [facec + 2] = v1.co [2]
                
                faces [facec + 3] = v2.co [0]
                faces [facec + 4] = v2.co [1]
                faces [facec + 5] = v2.co [2]
                
                faces [facec + 6] = v3.co [0]
                faces [facec + 7] = v3.co [1]
                faces [facec + 8] = v3.co [2]

                facec += 9

            for si, s in enumerate (obj.particle_systems):

                if (s.settings.type == 'HAIR'):

                    # active system
                    obj.particle_systems.active_index = si

                    # cut particles
                    count = toolkit.processParticlesCut (particlesi [si], particles [si], faces, particlec [si], facec)

                    # create new settings
                    s.settings = \
                    s.settings.copy ()
                    s.settings.count = count

                    # refresh
                    bpy.ops.object.mode_set (mode = 'EDIT')
                    bpy.ops.object.mode_set (mode = 'OBJECT')

                    # shortcut
                    obj_particles = s.particles

                    # shortcut
                    data = data_particles [si]

                    for i, p in enumerate (obj_particles):

                        index = particlesi [si][i]

                        p.alive_state               = data [index][ 0]
                        p.location [0]              = data [index][ 1]
                        p.location [1]              = data [index][ 2]
                        p.location [2]              = data [index][ 3]
                        p.rotation [0]              = data [index][ 4]
                        p.rotation [1]              = data [index][ 5]
                        p.rotation [2]              = data [index][ 6]
                        p.rotation [3]              = data [index][ 7]
                        p.size                      = data [index][ 8]
                        p.velocity [0]              = data [index][ 9]
                        p.velocity [1]              = data [index][10]
                        p.velocity [2]              = data [index][11]
                        p.angular_velocity [0]      = data [index][12]
                        p.angular_velocity [1]      = data [index][13]
                        p.angular_velocity [2]      = data [index][14]
                        p.prev_location [0]         = data [index][15]
                        p.prev_location [1]         = data [index][16]
                        p.prev_location [2]         = data [index][17]
                        p.prev_rotation [0]         = data [index][18]
                        p.prev_rotation [1]         = data [index][19]
                        p.prev_rotation [2]         = data [index][20]
                        p.prev_rotation [3]         = data [index][21]
                        p.prev_velocity [0]         = data [index][22]
                        p.prev_velocity [1]         = data [index][23]
                        p.prev_velocity [2]         = data [index][24]
                        p.prev_angular_velocity [0] = data [index][25]
                        p.prev_angular_velocity [1] = data [index][26]
                        p.prev_angular_velocity [2] = data [index][27]
                        p.lifetime                  = data [index][28]
                        p.birth_time                = data [index][29]
                        p.die_time                  = data [index][30]

                        c = min (data [index][31], len (p.is_hair))

                        keys = data [index][32];  k = 0

                        for j in range (c):

                            h = p.is_hair [j]
                        
                            h.co [0]            = keys [k    ]
                            h.co [1]            = keys [k + 1]
                            h.co [2]            = keys [k + 2]

                            # WRITING ON THESE CAUSE INVALID DATA
##                            h.co_hair_space [0] = keys [k + 3]
##                            h.co_hair_space [1] = keys [k + 4]
##                            h.co_hair_space [2] = keys [k + 5]
                            
                            h.time              = keys [k + 6]
                            h.weight            = keys [k + 7]
                            
                            k += 8

                        c = min (data [index][33], len (p.keys))

                        keys = data [index][34];  k = 0
                        for j in range (c):

                            key = p.keys [j]

                            key.angular_velocity [0]    = keys [k     ]
                            key.angular_velocity [1]    = keys [k +  1]
                            key.angular_velocity [2]    = keys [k +  2]
                            key.location [0]            = keys [k +  3]
                            key.location [1]            = keys [k +  4]
                            key.location [2]            = keys [k +  5]
                            key.rotation [0]            = keys [k +  6]
                            key.rotation [1]            = keys [k +  7]
                            key.rotation [2]            = keys [k +  8]
                            key.rotation [3]            = keys [k +  9]
                            key.velocity [0]            = keys [k + 10]
                            key.velocity [1]            = keys [k + 11]
                            key.velocity [2]            = keys [k + 12]
                            key.time                    = keys [k + 13]

                            k += 14

            # log
            print ("Split ", obji + 1)

    # particles clean up
    if (scene.shift_mt_particles):

        data_particles [:] = []
        
        particles  [:] = []
        particlesi [:] = []

        particlec [:] = []

        del faces
        
    # restoring normals
    if scene.shift_mt_preserve :
        
        for obj in group.objects:

            scene.objects.active = obj

            mesh = obj.data
            data = obj.data.vertex_colors.active.data

            tags = [True for v in mesh.vertices]

            for i, col in enumerate (data) :

                f = mesh.faces [i]

                i1 = f.vertices [0]
                i2 = f.vertices [1]
                i3 = f.vertices [2]
                
                if tags [i1] :
                    v = mesh.vertices [i1]
                    v.normal [0] = 2.0 * col.color1 [0] - 1.0
                    v.normal [1] = 2.0 * col.color1 [1] - 1.0
                    v.normal [2] = 2.0 * col.color1 [2] - 1.0
                    tags [i1] = False
                
                if tags [i2] :
                    v = mesh.vertices [i2]
                    v.normal [0] = 2.0 * col.color2 [0] - 1.0
                    v.normal [1] = 2.0 * col.color2 [1] - 1.0
                    v.normal [2] = 2.0 * col.color2 [2] - 1.0
                    tags [i2] = False
                
                if tags [i3] :
                    v = mesh.vertices [i3]
                    v.normal [0] = 2.0 * col.color3 [0] - 1.0
                    v.normal [1] = 2.0 * col.color3 [1] - 1.0
                    v.normal [2] = 2.0 * col.color3 [2] - 1.0
                    tags [i3] = False
                
            # clean up
            tags [:] = []

    # remove group
    bpy.data.groups.remove (group)

    # particles
    if (scene.shift_mt_particles):
        
        # unload dll
        del toolkit    
            
    # log            
    print ('')
    print ('Splitting finished in %.4f sec.' % (time.clock () - start_time))
        
    return

####------------------------------------------------------------------------------------------------------------------------------------------------------
#### PROCESS SPLIT TEXTURE
####------------------------------------------------------------------------------------------------------------------------------------------------------

def processSplitTex ():

    start_time = time.clock ()

    # shortcut
    obj = bpy.context.object

    # shortcut
    mesh = obj.data

    # shortcut
    scene = bpy.context.scene

    # is it mesh ?
    if not obj or obj.type != 'MESH':  return

    # log
    print ('\nSplit starting... ')
    print ('')

    # check mesh
    for f in mesh.faces:
        
        try:    f.vertices [3]
        except: continue

        print ('ERROR | Mesh :', mesh.name, ' containts one or more quads.')
        return -1

    names = (scene.shift_mt_tex1,
             scene.shift_mt_tex2,
             scene.shift_mt_tex3,
             scene.shift_mt_tex4,
             scene.shift_mt_tex5,
             scene.shift_mt_tex6,
             scene.shift_mt_tex7,
             scene.shift_mt_tex8)
    
    textures = []

    for n in names :
        
        if (n != '') :
            try: tex = bpy.data.textures [n]
            except:
                print ("ERROR | Cannot find texture : '" + n + "'")
                return -1

            try :   tex.image.filepath
            except :
                print ("ERROR | Texture : '" + n + "' is not image")
                return -1

            textures.append ([tex])

    if len (textures) == 0 : return

    # save area
    otype = bpy.context.area.type

    # image context
    bpy.context.area.type = 'IMAGE_EDITOR'

    # array of names
    narraytype = ctypes.c_char_p * len (textures);  narray = narraytype ()
    
    for i, t in enumerate (textures):

        filep = bpy.path.abspath ('//') + 'tmp_' + str (i) + '.tga'

        narray [i] = ctypes.c_char_p (filep);   t.append (filep)

        bpy.context.area.active_space.image = t [0].image

        bpy.ops.image.save_as (file_type = 'TARGA RAW', filepath = filep, relative_path = False, copy = True)

    # restore area
    bpy.context.area.type = otype

    # toolkit dll
    toolkit = windll.LoadLibrary (sys.path [0] + '\shift_toolkit.dll')

    # BEGIN
    r = toolkit.processTexSplitBegin (narray, ctypes.c_char_p (bpy.path.abspath ('//') + 'tmp_err.tga'), len (textures), scene.shift_mt_overlap)

    # result ..
    if (r < 0.0):
        
        # END
        toolkit.processTexSplitEnd ()

        # cleanup
        del toolkit

        # delete temporary files
        for i, t in enumerate (textures):
            
            os.remove (t [1])

        # error
        print ("ERROR | There are some texels that use more then maximum overlapping layers (" + str (scene.shift_mt_overlap) + ") \n")
        print ("   Check error image : " + bpy.path.abspath ('//') + 'tmp_err.tga')
        
        return -1

    # active UV texture layer
    layer = mesh.uv_textures.active

    # face selection mode
    bpy.context.tool_settings.mesh_select_mode = (False, False, True)
    
    # object mode
    bpy.ops.object.mode_set (mode = 'OBJECT')
            
    # error
    err = False

    # masks are saved into texture coords
    mlayer = mesh.uv_textures.new ('tmp_masks')

    # set active UVs
    mesh.uv_textures.active = mlayer

    # list of masks
    lmasks = []
    
    # PROCESS
    for i, f in enumerate (mesh.faces) :

        uv1 = layer.data [i].uv1
        uv2 = layer.data [i].uv2
        uv3 = layer.data [i].uv3
        
        r = toolkit.processTexSplitTriangle (ctypes.c_float (uv1 [0]),
                                             ctypes.c_float (uv1 [1]),
                                             ctypes.c_float (uv2 [0]),
                                             ctypes.c_float (uv2 [1]),
                                             ctypes.c_float (uv3 [0]),
                                             ctypes.c_float (uv3 [1]))

        mlayer.data [i].uv1 [0] = r

        found = False
        for mask in lmasks:
            if mask == r: found = True;   break
            
        if not found: lmasks.append (r)
        
        if (r == 0):    f.select = True;    err = True
        else:

            f.select = False

    if (err):

        # END
        toolkit.processTexSplitEnd ()

        # cleanup
        del toolkit

        # delete temporary files
        for i, t in enumerate (textures):
            
            os.remove (t [1])
        
        # error
        print ("ERROR | There are some faces that use more then maximum overlapping layers (" + str (scene.shift_mt_overlap) + ") \n")
        print ("   Problematic faces are selected.")
        
        return -1

    ## print ("lmasks : ", lmasks)

    # save group name
    gname = "tmp_" + obj.name

    # create temporary group
    bpy.ops.group.create (name = gname)

    # saving vertex data
    if scene.shift_mt_preserve :

        layer = mesh.vertex_colors.new (name = gname)

        mesh.vertex_colors.active = layer

        for i, col in enumerate (layer.data) :

            f = mesh.faces [i]

            v = mesh.vertices [f.vertices [0]]
            col.color1 [0] = 0.5 + 0.5 * v.normal [0]
            col.color1 [1] = 0.5 + 0.5 * v.normal [1]
            col.color1 [2] = 0.5 + 0.5 * v.normal [2]
            
            v = mesh.vertices [f.vertices [1]]
            col.color2 [0] = 0.5 + 0.5 * v.normal [0]
            col.color2 [1] = 0.5 + 0.5 * v.normal [1]
            col.color2 [2] = 0.5 + 0.5 * v.normal [2]

            v = mesh.vertices [f.vertices [2]]
            col.color3 [0] = 0.5 + 0.5 * v.normal [0]
            col.color3 [1] = 0.5 + 0.5 * v.normal [1]
            col.color3 [2] = 0.5 + 0.5 * v.normal [2]

    # separate parts
    for i, m in enumerate (lmasks):        
        for j, f in enumerate (mesh.faces):            
            if (mlayer.data [j].uv1 [0] == m):  f.select = True

        # saving mask
        obj ['texmask'] = m

        # edit mode
        bpy.ops.object.mode_set (mode = 'EDIT')

        # separate
        bpy.ops.mesh.separate ()

        # object mode
        bpy.ops.object.mode_set (mode = 'OBJECT')
            
    # clean up
    scene.objects.unlink (obj)

    # deselect
    bpy.ops.object.select_all (action = 'DESELECT')

    # our group
    group = bpy.data.groups [gname]

    # select new objects
    for obj in group.objects:

        scene.objects.active = obj; obj.select = True

        # copy mask into datablock
        m = obj ['texmask'];    obj.data ['texmask'] = m

        # delete original
        del obj ['texmask']

        for i in range (32):
            if (m & (1 << i)):
                obj.data ['texmask_tex' + str (i)] = 'tex' + str (i)
        
        # active layer
        obj.data.uv_textures.active = obj.data.uv_textures ['tmp_masks']

        # remove
        try: bpy.ops.mesh.uv_texture_remove ()        
        except: pass

    # restoring normals
    if scene.shift_mt_preserve :
        
        for obj in group.objects:

            scene.objects.active = obj

            obj.update ()

            mesh = obj.data
            data = obj.data.vertex_colors.active.data

            tags = [True for v in mesh.vertices]

            for i, col in enumerate (data) :

                f = mesh.faces [i]

                i1 = f.vertices [0]
                i2 = f.vertices [1]
                i3 = f.vertices [2]

                if tags [i1] :
                    v = mesh.vertices [i1]
                    v.normal [0] = 2.0 * col.color1 [0] - 1.0
                    v.normal [1] = 2.0 * col.color1 [1] - 1.0
                    v.normal [2] = 2.0 * col.color1 [2] - 1.0
                    tags [i1] = False
                
                if tags [i2] :
                    v = mesh.vertices [i2]
                    v.normal [0] = 2.0 * col.color2 [0] - 1.0
                    v.normal [1] = 2.0 * col.color2 [1] - 1.0
                    v.normal [2] = 2.0 * col.color2 [2] - 1.0
                    tags [i2] = False
                
                if tags [i3] :
                    v = mesh.vertices [i3]
                    v.normal [0] = 2.0 * col.color3 [0] - 1.0
                    v.normal [1] = 2.0 * col.color3 [1] - 1.0
                    v.normal [2] = 2.0 * col.color3 [2] - 1.0
                    tags [i3] = False
                
        # clean up
        tags [:] = []

    # remove group
    bpy.data.groups.remove (group)
        
    # END
    toolkit.processTexSplitEnd ()
        
    # unload dll    
    del toolkit

    # delete temporary files
    for i, t in enumerate (textures):
        
        os.remove (t [1])
        
    # log            
    print ('')
    print ('Splitting finished in %.4f sec.' % (time.clock () - start_time))
    
    return

####------------------------------------------------------------------------------------------------------------------------------------------------------
#### PROCESS SPLIT TEXTURE MERGE
####------------------------------------------------------------------------------------------------------------------------------------------------------

def processSplitTexMerge ():

    start_time = time.clock ()

    # shortcut
    scene = bpy.context.scene

    # shortcut
    selected = bpy.context.selected_objects

    # log
    print ('\nMergeing starting... \n\n\tObjects (', len (selected), ') :')
    print ('')

    # sorted list of objects
    sselected = []
    
    for obj in selected:

        # is it mesh ?
        if obj and obj.type == 'MESH':  sselected.append ((obj, len (obj.data.faces)))
        else:                           sselected.append ((obj, 999999999))

    # sorting objects by poly-count
    sselected.sort (key = lambda sselected: sselected [1])
    
    # object mode            
    bpy.ops.object.mode_set (mode = 'OBJECT')

    # deselect all
    bpy.ops.object.select_all (action = 'DESELECT')
    
    # init mask
    mask = 0

    for i, (obj, l) in enumerate (sselected):

        # is it mesh ?
        if obj and obj.type == 'MESH':

            # set active object
            scene.objects.active = obj

            # mask
            try:    maskn = obj.data ['texmask']
            except: maskn = 0

            # variable-precision SWAR algorithm
            bits = mask | maskn
            
            bits = bits - ((bits >> 1) & 0x55555555)
            bits = (bits & 0x33333333) + ((bits >> 2) & 0x33333333)
            bits = ((bits + (bits >> 4) & 0xf0f0f0f) * 0x1010101) >> 24

            if (bits <= scene.shift_mt_overlap):

                # select
                obj.select = True

                # add mask
                mask |= maskn

                if (i > 0):
                    
                    # join meshes
                    bpy.ops.object.join ()

                    # set mask to new object
                    bpy.context.object.data ['texmask'] = mask
            
                # log
                print (i + 1, "Object : '", obj.name, "'")
                
            else:

                # log
                print (i + 1, "Object : '", obj.name, "' exceed maximum overlapping")
            
        else:
            
            # log
            print (i + 1, "Object : '", obj.name, "' is not a mesh")

    # set new props
    for i in range (32):
        if (mask & (1 << i)):
            bpy.context.object.data ['texmask_tex' + str (i)] = 'tex' + str (i)
        
    # log            
    print ('')
    print ('Merge finished in %.4f sec.' % (time.clock () - start_time))

    # clean up
    sselected [:] = []
    
    return

####------------------------------------------------------------------------------------------------------------------------------------------------------
#### PROCESS NONMANIFOLD
####------------------------------------------------------------------------------------------------------------------------------------------------------

def processNonmanifold ():

    start_time = time.clock ()

    # shortcut
    scene = bpy.context.scene

    # shortcut
    selected = bpy.context.selected_objects

    # log
    print ('\nSelection starting... \n\n\tObjects (', len (selected), ') :')
    print ('')

    for i, obj in enumerate (selected):

        # is it mesh ?
        if obj and obj.type == 'MESH':

            # set active object
            scene.objects.active = obj

            # object mode
            bpy.ops.object.mode_set (mode = 'OBJECT')

            # saving vertex normals
            if scene.shift_mt_preserve :
                
                normals = [mathutils.Vector ((v.normal [0], v.normal [1], v.normal [2])) for v in obj.data.vertices]
            
            # edit mode
            bpy.ops.object.mode_set (mode = 'EDIT')
        
            # vertex selection mode
            bpy.context.tool_settings.mesh_select_mode = (True, False, False)

            # unhide all faces
            bpy.ops.mesh.reveal ()
        
            # deselect all
            bpy.ops.mesh.select_all (action = 'DESELECT')

            # select non-manifold
            bpy.ops.mesh.select_non_manifold ()

            # invert selection
            if scene.shift_mt_invert :

                bpy.ops.mesh.select_all (action = 'INVERT')
                   
            # object mode
            bpy.ops.object.mode_set (mode = 'OBJECT')

            if scene.shift_mt_preserve :

                # restoring normals
                for v in obj.data.vertices:    v.normal = normals [v.index]

                # clean up                
                normals [:] = []

            # log
            print (i + 1, "Object : '", obj.name, "'")
            
        else:
            
            # log
            print (i + 1, "Object : '", obj.name, "' is not a mesh")
        
    # log            
    print ('')
    print ('Selection finished in %.4f sec.' % (time.clock () - start_time))

####------------------------------------------------------------------------------------------------------------------------------------------------------
#### PROCESS MERGE
####------------------------------------------------------------------------------------------------------------------------------------------------------

def processMerge ():

    start_time = time.clock ()

    # shortcut
    scene = bpy.context.scene

    # shortcut
    selected = bpy.context.selected_objects

    # do nothing
    if len (selected) < 2:

        return

    # set active object
    if not bpy.context.object:

        scene.objects.active = selected [0]

    # object mode
    bpy.ops.object.mode_set (mode = 'OBJECT')

    # join objects
    bpy.ops.object.join ()

##    # save all hair particles
##    if scene.shift_mt_mhair :
##
##        # save hair here
        
    # remove doubles
    if scene.shift_mt_mdouble :
        
        # edit mode
        bpy.ops.object.mode_set (mode = 'EDIT')

        # vertex selection mode
        bpy.context.tool_settings.mesh_select_mode = (True, False, False)

        # unhide all faces
        bpy.ops.mesh.reveal ()

        # select all
        bpy.ops.mesh.select_all (action = 'SELECT')

        # remove doubles
        bpy.ops.mesh.remove_doubles ()

##    # restore particles
##    if scene.shift_mt_mhair :
##
##        # restore hair here

    # object mode
    bpy.ops.object.mode_set (mode = 'OBJECT')
        
    # log            
    print ('')
    print ('Merge finished in %.4f sec.' % (time.clock () - start_time))
    
####------------------------------------------------------------------------------------------------------------------------------------------------------
#### INTEGRATION AND GUI
####------------------------------------------------------------------------------------------------------------------------------------------------------

class MeshToolsTriangulateOp (bpy.types.Operator):

    bl_idname       = "object.meshtools_triangulate_operator"
    bl_label        = "SHIFT - Mesh Tools"
    bl_description  = "Convert mesh quads to triangles on selected objects"
    bl_register     = True
    bl_undo         = True
    
    def execute (self, context):

        processTriangulate ()

        return {'FINISHED'}

class MeshToolsGridSplitterOp (bpy.types.Operator):

    bl_idname       = "object.meshtools_gridsplitter_operator"
    bl_label        = "SHIFT - Mesh Tools"
    bl_description  = "Split selected mesh to multiple meshes by uniform grid"
    bl_register     = True
    bl_undo         = False
    
    def execute (self, context):

        result = []

        selected = list (bpy.context.selected_objects)

        if (len (selected) >= 1):

            # set some active object
            bpy.context.scene.objects.active = selected [0]

            # object mode
            bpy.ops.object.mode_set (mode = 'OBJECT')
                        
            for obj in selected:

                # deselect all
                bpy.ops.object.select_all (action = 'DESELECT')

                # split
                processSplitGrid (obj)

                # add created objects to the list ..
                sel = bpy.context.selected_objects
                
                for o in sel:   result.append (o)

            # select created objects
            for obj in result:  obj.select = True

            # clean up
            selected [:] = []
            result [:] = []

        return {'FINISHED'}

class MeshToolsTexSplitterOp (bpy.types.Operator):

    bl_idname       = "object.meshtools_texsplitter_operator"
    bl_label        = "SHIFT - Mesh Tools"
    bl_description  = "Split selected mesh to multiple meshes by texture overlapping areas"
    bl_register     = True
    bl_undo         = False
    
    def execute (self, context):

        processSplitTex ()

        return {'FINISHED'}
    
class MeshToolsTexSplitterMergeOp (bpy.types.Operator):

    bl_idname       = "object.meshtools_texsplitter_merge_operator"
    bl_label        = "SHIFT - Mesh Tools"
    bl_description  = "Merges multiple meshes with compatible tex. mask until max. ovelapping value is reached"
    bl_register     = True
    bl_undo         = False
    
    def execute (self, context):

        processSplitTexMerge ()

        return {'FINISHED'}
    
class MeshToolsNonmanifoldOp (bpy.types.Operator):

    bl_idname       = "object.meshtools_nonmanifold_operator"
    bl_label        = "SHIFT - Mesh Tools"
    bl_description  = "Selects nonmanifold vertices on multiple meshes"
    bl_register     = True
    bl_undo         = True
    
    def execute (self, context):

        processNonmanifold ()

        return {'FINISHED'}
    
class MeshToolsMergeOp (bpy.types.Operator):

    bl_idname       = "object.meshtools_merge_operator"
    bl_label        = "SHIFT - Mesh Tools"
    bl_description  = "Merges mesh objects performing some special operations"
    bl_register     = True
    bl_undo         = True
    
    def execute (self, context):

        processMerge ()

        return {'FINISHED'}
    
class MeshToolsPanel (bpy.types.Panel):
     
    bl_idname   = "object.meshtools__panel"
    bl_label    = "SHIFT - Mesh Tools"
    bl_context  = "objectmode"
    bl_register = True
    bl_undo     = True

    bl_space_type   = 'VIEW_3D'
    bl_region_type  = 'TOOLS'

    def draw (self, context):
            
        layout = self.layout
        
        box = layout.box    ()
        box.operator        ('object.meshtools_triangulate_operator', 'Triangulate')

        box = layout.box    ()
        box.operator        ('object.meshtools_merge_operator', 'Merge')
        box = box.box       ()
        split = box.split   (align = True, percentage = 0.3)
        split.prop          (context.scene, 'shift_mt_mhair')
        split.prop          (context.scene, 'shift_mt_mdouble')
        
        box = layout.box    ()
        box.operator        ('object.meshtools_gridsplitter_operator', 'Split')
        
        split = (box.box ()).split   (align = True, percentage = 0.5)
        col = split.column  (align = False)
        col.label           ('Subdivision X :')
        col.label           ('Subdivision Y :')
        col.label           ('Subdivision Z :')
        col = split.column  (align = True)
        col.prop            (context.scene, 'shift_mt_subdivisionx')
        col.prop            (context.scene, 'shift_mt_subdivisiony')
        col.prop            (context.scene, 'shift_mt_subdivisionz')
        
        box = box.box       ()
        box.prop            (context.scene, 'shift_mt_particles')

        box = layout.box    ()
        box.operator        ('object.meshtools_texsplitter_operator', 'Split')
        box = box.box       ()
        split = box.split   (align = True, percentage = 0.5)
        col = split.column  (align = False)
        col.label           ('max. Overlapping :')
        col = split.column  (align = True)
        col.prop            (context.scene, 'shift_mt_overlap')

        boxs = box; box = box.box       ()
        
        if (bpy.context.object) and (bpy.context.object.type == 'MESH'):

            if (bpy.context.object.data.uv_textures.active):
            
                (box.box ()).label  ('UV : ' + bpy.context.object.data.uv_textures.active.name)
        
        split = box.split   (align = True, percentage = 0.35)
        col = split.column  (align = True)
        col.label           ('Texture 1 :')
        col.label           ('Texture 2 :')
        col.label           ('Texture 3 :')
        col.label           ('Texture 4 :')
        col.label           ('Texture 5 :')
        col.label           ('Texture 6 :')
        col.label           ('Texture 7 :')
        col.label           ('Texture 8 :')
        col = split.column  (align = True)
        col.prop            (context.scene, 'shift_mt_tex1')
        col.prop            (context.scene, 'shift_mt_tex2')
        col.prop            (context.scene, 'shift_mt_tex3')
        col.prop            (context.scene, 'shift_mt_tex4')
        col.prop            (context.scene, 'shift_mt_tex5')
        col.prop            (context.scene, 'shift_mt_tex6')
        col.prop            (context.scene, 'shift_mt_tex7')
        col.prop            (context.scene, 'shift_mt_tex8')

        boxs.operator        ('object.meshtools_texsplitter_merge_operator', 'Merge')

        box = layout.box    ()
        split = box.split   (align = True, percentage = 0.7)
        split.operator      ('object.meshtools_nonmanifold_operator', 'Select Non - Manifold')
        split.prop          (context.scene, 'shift_mt_invert')

        box = layout.box    ()
        box.prop            (context.scene, 'shift_mt_preserve')
        
def register ():

    bpy.utils.register_module (__name__)

    # options

    # ----------------------------------------------------------
    bpy.types.Scene.shift_mt_subdivisionx = IntProperty (
        name        = "",
        description = "Subdivision level",
        min         = 1,
        max         = 16,
        step        = 1,
        default     = 1)
    bpy.types.Scene.shift_mt_subdivisiony = IntProperty (
        name        = "",
        description = "Subdivision level",
        min         = 1,
        max         = 16,
        step        = 1,
        default     = 1)    
    bpy.types.Scene.shift_mt_subdivisionz = IntProperty (
        name        = "",
        description = "Subdivision level",
        min         = 1,
        max         = 16,
        step        = 1,
        default     = 1)
    
    # ----------------------------------------------------------s
    bpy.types.Scene.shift_mt_overlap = IntProperty (
        name        = "",
        description = "Maximum overlaping textures",
        min         = 2,
        max         = 32,
        step        = 1,
        default     = 1)
    
    # ----------------------------------------------------------s
    bpy.types.Scene.shift_mt_tex1 = StringProperty (
#        name        = "Texture 1.",
        description = "Texture mask",
        default     = "")
    bpy.types.Scene.shift_mt_tex2 = StringProperty (
#        name        = "Texture 2.",
        description = "Texture mask",
        default     = "")
    bpy.types.Scene.shift_mt_tex3 = StringProperty (
#        name        = "Texture 3.",
        description = "Texture mask",
        default     = "")
    bpy.types.Scene.shift_mt_tex4 = StringProperty (
#        name        = "Texture 4.",
        description = "Texture mask",
        default     = "")
    bpy.types.Scene.shift_mt_tex5 = StringProperty (
#        name        = "Texture 5.",
        description = "Texture mask",
        default     = "")
    bpy.types.Scene.shift_mt_tex6 = StringProperty (
#        name        = "Texture 6.",
        description = "Texture mask",
        default     = "")
    bpy.types.Scene.shift_mt_tex7 = StringProperty (
#        name        = "Texture 7.",
        description = "Texture mask",
        default     = "")
    bpy.types.Scene.shift_mt_tex8 = StringProperty (
#        name        = "Texture 8.",
        description = "Texture mask",
        default     = "")

    # ----------------------------------------------------------
    bpy.types.Scene.shift_mt_invert = BoolProperty (
        name        = "Invert",
        description = "Invert Selection",
        default     = False)
    bpy.types.Scene.shift_mt_preserve = BoolProperty (
        name        = "Preserve Vertex Normals",
        description = "Preserves vertex normals",
        default     = False)
    bpy.types.Scene.shift_mt_particles = BoolProperty (
        name        = "Split Hair",
        description = "Creates new particles system settings and restore original particle positions",
        default     = False)
    
    # ----------------------------------------------------------
    bpy.types.Scene.shift_mt_mhair = BoolProperty (
        name        = "Hair",
        description = "Merge hair particles systems preserving particles",
        default     = False)
    bpy.types.Scene.shift_mt_mdouble = BoolProperty (
        name        = "Remove doubles",
        description = "Remove vertex duplicity after merge",
        default     = True)
    
def unregister ():

    bpy.utils.unregister_module (__name__)

    del bpy.types.Scene.shift_mt_subdivisionx
    del bpy.types.Scene.shift_mt_subdivisiony
    del bpy.types.Scene.shift_mt_subdivisionz
    del bpy.types.Scene.shift_mt_particles
    del bpy.types.Scene.shift_mt_preserve
    del bpy.types.Scene.shift_mt_overlap
    del bpy.types.Scene.shift_mt_invert
    del bpy.types.Scene.shift_mt_mdouble
    del bpy.types.Scene.shift_mt_mhair
    del bpy.types.Scene.shift_mt_tex1
    del bpy.types.Scene.shift_mt_tex2
    del bpy.types.Scene.shift_mt_tex3
    del bpy.types.Scene.shift_mt_tex4
    del bpy.types.Scene.shift_mt_tex5
    del bpy.types.Scene.shift_mt_tex6
    del bpy.types.Scene.shift_mt_tex7
    del bpy.types.Scene.shift_mt_tex8
     
if __name__ == "__main__":
    
    register ()
