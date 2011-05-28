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
    "name"              : "SHIFT - Normals Tools",
    "author"            : "BBC",
    "version"           : (1,0),
    "blender"           : (2, 5, 3),
    "api"               : 31236,
    "category"          : "Object",
    "location"          : "Tool Shelf",
    "warning"           : '',
    "wiki_url"          : "",
    "tracker_url"       : "",
    "description"       : "Tools for advanced normals manipulation"}

import bpy
import time
import math
import operator
import mathutils

from math       import *
from bpy.props  import *

####------------------------------------------------------------------------------------------------------------------------------------------------------
#### PROCESS STITCH
####------------------------------------------------------------------------------------------------------------------------------------------------------

def processStitch ():

    start_time = time.clock ()

    # shortcut
    selected = bpy.context.selected_objects

    # shortcut
    scene = bpy.context.scene

    # anything selected ?
    if len (selected) == 0: return

    # log
    print ('\nNormals stitch starting... \n\n\tObjects (', len (selected), ') :')
    print ('')

    # original mesh data
    datao = []

    # removing unwanted objects
    for obj in selected:

        # is it mesh ?
        if obj.type != 'MESH':  obj.select = False

    # only meshes
    selected = list (bpy.context.selected_objects)

    # saving positions and normals
    for obj in selected:

        # saving normals
        datao.append ([(v.co [0], v.co [1], v.co [2], v.normal [0], v.normal [1], v.normal [2]) for v in obj.data.vertices])
            
    # transform
    for obj in selected :

        obj.data.transform (obj.matrix_world)

    # restore original normals
    for i, obj in enumerate (selected) :

        # deselect 
        bpy.ops.object.select_all (action = 'DESELECT')
        
        # set active object
        scene.objects.active = obj;     obj.select = True

        ## LITTLE TRICK TO DISABLE AUTO NORMAL-RECALCULATION

        bpy.ops.object.mode_set (mode = 'EDIT')
        bpy.ops.object.mode_set (mode = 'OBJECT')

        ## END

        # restore original normals
        for v in obj.data.vertices:

            v.normal [0] = datao [i][v.index][3]
            v.normal [1] = datao [i][v.index][4]
            v.normal [2] = datao [i][v.index][5]

    # lists of vertices

    listv = [[] for obj in selected]

    for i, obj in enumerate (selected):

        edge_faces = {e.key: 0 for e in obj.data.edges}

        # counting faces of each edge
        for f in obj.data.faces:
            edge_faces [f.edge_keys [0]] += 1
            edge_faces [f.edge_keys [1]] += 1
            edge_faces [f.edge_keys [2]] += 1
            
            try:    edge_faces [f.edge_keys [3]] += 1
            except: pass

        listvi = listv [i]

        # edges that are 'open' ..
        for e in obj.data.edges:
            if edge_faces [e.key] == 1:
                listvi.append (e.vertices [0])
                listvi.append (e.vertices [1])

        # removing double vertices
        listvi.sort ()
        
        prevv = None
        for j, v in enumerate (listvi):

            if (v == prevv):
                try:    listvi.pop (j)
                except: pass
                continue
            prevv = v

        # cleanup
        edge_faces.clear ()

    # shortcut
    limit = bpy.context.scene.shift_nt_mlimit

    # shortcut
    angle = bpy.context.scene.shift_nt_alimit

    # spherical boundaries list
    boundaries = []
    
    # evalueate boundaries
    for i, obj in enumerate (selected):

        center  = mathutils.Vector ((0.0, 0.0, 0.0))
        
        minv    = mathutils.Vector (( 999999999.0,  999999999.0,  999999999.0))
        maxv    = mathutils.Vector ((-999999999.0, -999999999.0, -999999999.0))

        for v in obj.data.vertices:

            if v.co [0] < minv [0]: minv [0] = v.co [0]
            if v.co [1] < minv [1]: minv [1] = v.co [1]
            if v.co [2] < minv [2]: minv [2] = v.co [2]
            
            if v.co [0] > maxv [0]: maxv [0] = v.co [0]
            if v.co [1] > maxv [1]: maxv [1] = v.co [1]
            if v.co [2] > maxv [2]: maxv [2] = v.co [2]
            
            center += v.co

        center /= len (obj.data.vertices);  radius = (center - minv).length + limit
        
        boundaries.append ((center, radius))

    # accumulating normal values
    for i, obj in enumerate (selected):
              
        for j, objr in enumerate (selected):
            if (objr != obj) and (objr.data != obj.data):

                # boundary test
                if (boundaries [i][0] - boundaries [j][0]).length < (boundaries [i][1] + boundaries [j][1]):
                
                    for vi in listv [i]:
                        v = obj.data.vertices [vi]
                        
                        for vj in listv [j]:
                            vv = objr.data.vertices [vj]
                            
                            if (v.co - vv.co).length < limit:
                                if v.normal.angle (vv.normal) < angle:
                                
                                    v.normal += vv.normal;  vv.normal += v.normal

        # log
        print (i + 1, "Object : '", obj.name, "'")

    # normalizing normals
    for obj in selected:

        for v in obj.data.vertices:

            v.normal = v.normal.normalize ()

    # deselect 
    bpy.ops.object.select_all (action = 'DESELECT')

    # restoring positions
    for i, obj in enumerate (selected):

        for v in obj.data.vertices:

            v.co [0] = datao [i][v.index][0]
            v.co [1] = datao [i][v.index][1]
            v.co [2] = datao [i][v.index][2]

        obj.select = True

    # log            
    print ('')
    print ('Stitching finished in %.4f sec.' % (time.clock () - start_time))

    boundaries [:] = []
    selected   [:] = []
    

####------------------------------------------------------------------------------------------------------------------------------------------------------
#### SMOOTH
####------------------------------------------------------------------------------------------------------------------------------------------------------

def smooth (obj, iterations):

    # mesh
    mesh = obj.data

    # shortcut
    scene = bpy.context.scene

    # saving vertex normals      
    normals = [mathutils.Vector ((v.normal [0], v.normal [1], v.normal [2])) for v in obj.data.vertices]

    # set active object
    scene.objects.active = obj;     obj.select = True

    # edit mode
    bpy.ops.object.mode_set (mode = 'OBJECT')

    # deselect all
    bpy.ops.object.select_all (action = 'DESELECT')

    # set active object
    scene.objects.active = obj;     obj.select = True

    ## LITTLE TRICK TO DISABLE AUTO NORMAL-RECALCULATION

    bpy.ops.object.mode_set (mode = 'EDIT')
    bpy.ops.object.mode_set (mode = 'OBJECT')

    ## END

    # restoring normals
    for v in obj.data.vertices:    v.normal = normals [v.index]

    # clean up
    normals [:] = []

    # neighbour vertices
    neighbours = [[] for v in mesh.vertices]

    for e in mesh.edges:
        
        v1 = mesh.vertices [e.vertices [0]]
        v2 = mesh.vertices [e.vertices [1]]
        
        neighbours [e.vertices [0]].append (v2)
        neighbours [e.vertices [1]].append (v1)
    
    # iterating ..
    for n in range (iterations):

        # new normals
        newnormals = [mathutils.Vector ((v.normal [0], v.normal [1], v.normal [2])) for v in mesh.vertices]

        def accum (v, plist):

            normal = mathutils.Vector ((v.normal [0], v.normal [1], v.normal [2]))
    
            for vv in neighbours [v.index]:
                if vv not in plist:
                    plist.append (vv)
                    
                    if vv.co != v.co :  normal += vv.normal
                    else :              normal += accum (vv, plist)
                    
            return (normal)

        # accumulation
        for v in mesh.vertices:

            plist = []; newnormals [v.index] = accum (v, plist);    plist [:] = []
            
        # set normals and normalize
        for v in mesh.vertices:
            v.normal = newnormals [v.index];    v.normal.normalize ()

        # clean up
        newnormals [:] = []
        
####------------------------------------------------------------------------------------------------------------------------------------------------------
#### PROCESS SMOOTH
####------------------------------------------------------------------------------------------------------------------------------------------------------

def processSmooth ():

    start_time = time.clock ()

    # shortcut
    scene = bpy.context.scene
    
    # shortcut
    selected = bpy.context.selected_objects

    # anything selected ?
    if len (selected) == 0: return

    # log
    print ('\nSmoothing starting... \n\n\tObjects (', len (selected), ') :')
    print ('')

    # adding index for later identification
    for i, obj in enumerate (selected):

        # is it mesh ?
        if obj and obj.type == 'MESH':

            smooth (obj, scene.shift_nt_iterations)

            # log
            print (i + 1, "Object : '", obj.name, "'")
            
        else:
            
            # log
            print (i + 1, "Object : '", obj.name, "' is not a mesh")

    # deselect all
    bpy.ops.object.select_all (action = 'DESELECT')

    # restoring selections
    for obj in selected:    obj.select = True
    
    # log
    print ('')
    print ('Smoothing finished in %.4f sec.' % (time.clock () - start_time))

####------------------------------------------------------------------------------------------------------------------------------------------------------
#### INTEGRATION AND GUI
####------------------------------------------------------------------------------------------------------------------------------------------------------

class NormalsToolsStitchOp (bpy.types.Operator):

    bl_idname       = "object.normalstools_stitch_operator"
    bl_label        = "SHIFT - Normals Tools"
    bl_description  = "Stitch normals of separate objects with respect of their positions"
    bl_register     = True
    bl_undo         = True
    
    def execute (self, context):

        processStitch ()

        return {'FINISHED'}

class NormalsToolsSmoothOp (bpy.types.Operator):

    bl_idname       = "object.normalstools_smooth_operator"
    bl_label        = "SHIFT - Normals Tools"
    bl_description  = "Stitch normals of separate objects with respect of their positions"
    bl_register     = True
    bl_undo         = True
    
    def execute (self, context):

        processSmooth ()

        return {'FINISHED'}
    
class NormalsToolsPanel (bpy.types.Panel):
     
    bl_idname   = "object.normalstools_stitch_panel"
    bl_label    = "SHIFT - Normals Tools"
    bl_context  = "objectmode"
    bl_register = True
    bl_undo     = True

    bl_space_type   = 'VIEW_3D'
    bl_region_type  = 'TOOLS'

    def draw (self, context):
            
        layout = self.layout
        
        box = layout.box()
        box.operator       ('object.normalstools_stitch_operator', 'Stitch')
        
        split = (box.box ()).split (align = True, percentage = 0.35)
        
        col = split.column (align = False)
        col.label          ('Min. Distance :')
        col.label          ('Min. Angle : ')
        col = split.column (align = True)
        col.prop           (context.scene, 'shift_nt_mlimit')
        col.prop           (context.scene, 'shift_nt_alimit')
        
        box = layout.box()
        box.operator       ('object.normalstools_smooth_operator', 'Smooth')
        box.prop           (context.scene, 'shift_nt_iterations')
        
def register ():

    bpy.utils.register_module (__name__)

    # options
    
    # ----------------------------------------------------------
    bpy.types.Scene.shift_nt_mlimit = FloatProperty (
        description = "Vertices with distances under this limit are considered to be the same vertex.",
        min         = 0.001,
        max         = 0.1,
        precision   = 4,
        step        = 0.001,
        subtype     = 'DISTANCE',
        default     = 0.001)        
    bpy.types.Scene.shift_nt_alimit = FloatProperty (
        description = "Vertices which angle of their normals are under this limit are considered to be the same vertex.",
        min         = math.pi / 180.0,
        max         = math.pi,
        precision   = 2,
        step        = 1,
        subtype     = 'ANGLE',
        default     = (30.0 * math.pi) / 180.0)

    # ----------------------------------------------------------
    bpy.types.Scene.shift_nt_iterations = IntProperty (
        name        = "Iterations",
        description = "Number of iterations, the more iterations the more smoothed normals.",
        min         = 1,
        max         = 32,
        step        = 1,
        default     = 1)
     
def unregister ():

    bpy.utils.unregister_module (__name__)
    
    del bpy.types.Scene.shift_nt_iterations
    del bpy.types.Scene.shift_nt_mlimit
    del bpy.types.Scene.shift_nt_alimit
     
if __name__ == "__main__":
    
    register ()
