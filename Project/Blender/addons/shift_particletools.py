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
    "name"              : "SHIFT - Particle Tools",
    "author"            : "BBC",
    "version"           : (1,0),
    "blender"           : (2, 5, 3),
    "api"               : 31236,
    "category"          : "Object",
    "location"          : "Tool Shelf",
    "warning"           : '',
    "wiki_url"          : "",
    "tracker_url"       : "",
    "description"       : "Various particle operations"}

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
#### PROCESS CONVERT
####------------------------------------------------------------------------------------------------------------------------------------------------------

def processConvert ():

    start_time = time.clock ()

    # shortcut
    scene = bpy.context.scene

    # shortcut
    selected = list (bpy.context.selected_objects)

    # deselect all
    bpy.ops.object.select_all (action = 'DESELECT')

    # log
    print ('\nConversion starting... \n\n\tObjects (', len (selected), ') :')
    print ('')

    for i, obj in enumerate (selected):

        # does it have particles ?
        if len (obj.particle_systems) > 0:

            passed = False

            # set active object
            scene.objects.active = obj

            # object mode
            bpy.ops.object.mode_set (mode = 'OBJECT')

            for s in obj.particle_systems:

                # does it have dupli object ?
                if s.settings.dupli_object:

                    passed = True

                    # dupli_object
                    objd = s.settings.dupli_object
                    
                    # set active object
                    scene.objects.active = objd;    objd.select = True
                        
                    # list of new objects
                    newobjects = []
                        
                    # hair ?
                    if s.settings.type == 'HAIR':

                        for j, p in enumerate (s.particles):

                            # create new object instance
                            objn = bpy.data.objects.new ('new_%s%i' % (objd.name, j),  objd.data);
                            
                            scale       = (p.hair [0].co - p.hair [1].co).length * (len (p.hair) - 1) * 0.5
                            translation =  p.hair [0].co
                            
                            # build matrix for particle
                            matrix = mathutils.Matrix ();   matrix.identity ()
                            
                            # scale
                            matrix = mathutils.Matrix.Scale (scale, 4) * matrix
                            
                            # hack ..
                            matrix = mathutils.Matrix.Rotation (radians (90), 4, 'Y') * matrix
                            
                            # rotation
                            rotmatrix = p.rotation.to_matrix ();    rotmatrix.resize_4x4 ();    matrix = rotmatrix * matrix
                            
                            # translation
                            matrix = mathutils.Matrix.Translation (translation) * matrix

                            # set new matrix
                            objn.matrix_world = matrix
                            
                            # link new object
                            scene.objects.link (objn)

                            # add to list
                            newobjects.append (objn)

                    else:

                        for j, p in enumerate (s.particles):
                            
                            # create new object instance
                            objn = bpy.data.objects.new ('new_%s%i' % (objd.name, j),  objd.data);
                            
                            # copy matrix
                            objn.matrix_world = objd.matrix_world.copy ()

                            # set new translation
                            objn.matrix_world [3][0] = p.location [0]
                            objn.matrix_world [3][1] = p.location [1]
                            objn.matrix_world [3][2] = p.location [2]
                            
                            # link new object
                            scene.objects.link (objn)
                            
                            # add to list
                            newobjects.append (objn)

                    if scene.shift_pat_merge:

                        if (len (newobjects) > 0):
                            
                            # deselect all
                            bpy.ops.object.select_all (action = 'DESELECT')

                            # set active object
                            scene.objects.active = newobjects [0];  newobjects [0].select = True
                            
                            # make copy of mesh
                            bpy.ops.object.make_single_user (object = True, obdata = True)

                            # select all new objects
                            for o in newobjects:    o.select = True

                            # join them
                            bpy.ops.object.join ()

                    # cleanup
                    newobjects [:] = []
                    
            if passed:
                
                # log
                print (i + 1, "Object : '", obj.name, "'")
                
            else:
            
                # log
                print (i + 1, "Object : '", obj.name, "' nothing converted (missing dupli_object)")
        else:
            
            # log
            print (i + 1, "Object : '", obj.name, "' does not have particle systems")

    # clean up
    selected [:] = []
    
    # log            
    print ('')
    print ('Conversion finished in %.4f sec.' % (time.clock () - start_time))
    
####------------------------------------------------------------------------------------------------------------------------------------------------------
#### PROCESS SET OBJECT
####------------------------------------------------------------------------------------------------------------------------------------------------------

def processSetObject ():

    # shortcut
    scene = bpy.context.scene

    # check
    try:    scene.objects [scene.shift_pat_object]    
    except:
        
        print ('Object : "' + scene.shift_pat_object + '" does not exist.'); return

    # shortcut
    selected = bpy.context.selected_objects

    for i, obj in enumerate (selected):

        # does it have particles ?
        if len (obj.particle_systems) > 0:

            for s in obj.particle_systems:
                
                s.settings.dupli_object = bpy.data.objects [scene.shift_pat_object];   s.settings.render_type = 'OBJECT'
                                        
####------------------------------------------------------------------------------------------------------------------------------------------------------
#### PROCESS SET DISPLAY
####------------------------------------------------------------------------------------------------------------------------------------------------------

def processSetDisplay ():

    # shortcut
    scene = bpy.context.scene

    # shortcut
    selected = bpy.context.selected_objects

    if scene.shift_pat_display == '1':

        for i, obj in enumerate (selected):

            # does it have particles ?
            if len (obj.particle_systems) > 0:

                for s in obj.particle_systems:
                    
                    s.settings.draw_method = 'NONE'
                    
    elif scene.shift_pat_display == '2':
        
        for i, obj in enumerate (selected):

            # does it have particles ?
            if len (obj.particle_systems) > 0:

                for s in obj.particle_systems:
                    
                    s.settings.draw_method = 'RENDER'
                    
    elif scene.shift_pat_display == '3':
    
        for i, obj in enumerate (selected):

            # does it have particles ?
            if len (obj.particle_systems) > 0:

                for s in obj.particle_systems:
                    
                    s.settings.draw_method = 'PATH';    s.settings.draw_size = scene.shift_pat_pathsize
                    
####------------------------------------------------------------------------------------------------------------------------------------------------------
#### PROCESS COPY SYSTEM
####------------------------------------------------------------------------------------------------------------------------------------------------------

def processCopySystem ():

    # shortcut
    scene = bpy.context.scene

    # shortcut
    selected = bpy.context.selected_objects

    # source object
    source = bpy.context.object

    # source particle system
    system = source.particle_systems.active
    
    # source particle system settings
    settings = system.settings

    for obj in selected:

        if obj != source and obj.type == 'MESH':

            scene.objects.active = obj

            bpy.ops.object.particle_system_add ()
            
            obj.particle_systems.active.name = system.name

            obj.particle_systems.active.settings = settings
                    
####------------------------------------------------------------------------------------------------------------------------------------------------------
#### PROCESS REMOVE ALL
####------------------------------------------------------------------------------------------------------------------------------------------------------

def processRemoveAll ():

    # shortcut
    scene = bpy.context.scene

    # shortcut
    selected = bpy.context.selected_objects
    
    for obj in selected:

        if obj.type == 'MESH':

            scene.objects.active = obj
            
            for p in obj.particle_systems:

                bpy.ops.object.particle_system_remove ()
                            
####------------------------------------------------------------------------------------------------------------------------------------------------------
#### INTEGRATION AND GUI
####------------------------------------------------------------------------------------------------------------------------------------------------------
    
class ParticleToolsConvertOp (bpy.types.Operator):

    bl_idname       = "object.particletools_convert_operator"
    bl_label        = "SHIFT - Particle Tools"
    bl_description  = "Converts visible particle systems with dupli_object into object instances"
    bl_register     = True
    bl_undo         = True
    
    def execute (self, context):

        processConvert ()

        return {'FINISHED'}
    
class ParticleToolsSetObjectOp (bpy.types.Operator):

    bl_idname       = "object.particletools_set_object_operator"
    bl_label        = "SHIFT - Particle Tools"
    bl_description  = "Set dupli object for particle system"
    bl_register     = True
    bl_undo         = True
    
    def execute (self, context):

        processSetObject ()

        return {'FINISHED'}
    
class ParticleToolsSetDisplayOp (bpy.types.Operator):

    bl_idname       = "object.particletools_set_display_operator"
    bl_label        = "SHIFT - Particle Tools"
    bl_description  = "Set display mode for particle system"
    bl_register     = True
    bl_undo         = True
    
    def execute (self, context):

        processSetDisplay ()

        return {'FINISHED'}
    
class ParticleToolsCopyToSelected (bpy.types.Operator):

    bl_idname       = "object.particletools_copy_to_selected_operator"
    bl_label        = "SHIFT - Particle Tools"
    bl_description  = "Copy active particle system from active object to all other selected objects"
    bl_register     = True
    bl_undo         = True
    
    def execute (self, context):

        processCopySystem ()

        return {'FINISHED'}
    
class ParticleToolsRemoveAll (bpy.types.Operator):

    bl_idname       = "object.particletools_remove_all_operator"
    bl_label        = "SHIFT - Particle Tools"
    bl_description  = "Remove all particle systems from selected objects"
    bl_register     = True
    bl_undo         = True
    
    def execute (self, context):

        processRemoveAll ()

        return {'FINISHED'}
    
class ParticleToolsPanel (bpy.types.Panel):
     
    bl_idname   = "object.particletools_panel"
    bl_label    = "SHIFT - Particle Tools"
    bl_context  = "objectmode"
    bl_register = True
    bl_undo     = True

    bl_space_type   = 'VIEW_3D'
    bl_region_type  = 'TOOLS'

    def draw (self, context):
            
        layout = self.layout
        
        box = layout.box    ()
        box.operator        ('object.particletools_remove_all_operator', 'Remove All Particle Sytems')
        box = layout.box    ()
        box.operator        ('object.particletools_copy_to_selected_operator', 'Copy To Selected')
        box = layout.box    ()
        split = box.split   (align = True, percentage = 0.7)
        split.operator      ('object.particletools_convert_operator', 'Convert To Mesh')
        split.prop          (context.scene, 'shift_pat_merge')
        box = layout.box    ()
        split = box.split   (align = True, percentage = 0.35)
        split.operator      ('object.particletools_set_object_operator', 'Set Object')
        split.prop          (context.scene, 'shift_pat_object')
        box = layout.box    ()
        col = box.column    (align = False)
        col.operator        ('object.particletools_set_display_operator', 'Set Display')
        box = col.box       ()
        col = box.column    (align = True)
        col.prop_enum       (context.scene, 'shift_pat_display', '1')
        col.prop_enum       (context.scene, 'shift_pat_display', '2')
        col.prop_enum       (context.scene, 'shift_pat_display', '3')
        col.prop            (context.scene, 'shift_pat_pathsize')
        
def register ():
    
    bpy.utils.register_module (__name__)

    # options

    # ----------------------------------------------------------
    bpy.types.Scene.shift_pat_merge = BoolProperty (
        name        = "Merge",
        description = "Merge all instances into one mesh",
        default     = False)

    # ----------------------------------------------------------
    bpy.types.Scene.shift_pat_object = StringProperty (
        name        = "",
        description = "Name of object for use with particles",
        default     = "",
        subtype     = 'NONE')
    
    # ----------------------------------------------------------
    bpy.types.Scene.shift_pat_display = EnumProperty (
        items       = (("1", "None", ""), ("2", "Rendered", ""), ("3", "Path", "")),
        name        = "Display mode",
        description = "How particles are rendered",
        default     = "1")

    # ----------------------------------------------------------
    bpy.types.Scene.shift_pat_pathsize = IntProperty (
        name        = "Path size",
        description = "Path size",
        min         = 1,
        max         = 10,
        step        = 1,
        subtype     = 'NONE',
        default     = 1)
     
def unregister ():

    bpy.utils.unregister_module (__name__)

    del bpy.types.Scene.shift_pat_pathsize
    del bpy.types.Scene.shift_pat_display
    del bpy.types.Scene.shift_pat_object
    del bpy.types.Scene.shift_pat_merge
     
if __name__ == "__main__":
    
    register ()
