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
    "name"              : "SHIFT - Transform",
    "author"            : "BBC",
    "version"           : (1,0),
    "blender"           : (2, 5, 3),
    "api"               : 31236,
    "category"          : "Object",
    "location"          : "Tool Shelf",
    "warning"           : '',
    "wiki_url"          : "",
    "tracker_url"       : "",
    "description"       : "Typed transforms for multiple objects"}

import bpy
import time
import math
import random
import operator
import mathutils

from math       import *
from bpy.props  import *

####------------------------------------------------------------------------------------------------------------------------------------------------------
#### PROCESS LOCATION
####------------------------------------------------------------------------------------------------------------------------------------------------------

def processLocationX ():

    # shortcut
    scene = bpy.context.scene
    
    # shortcut
    selected = bpy.context.selected_objects

    # use random ?
    if (scene.shift_tr_locrandom):

        # adding index for later identification
        for i, obj in enumerate (selected):
            r = random.random ()
            obj.location [0] = r * scene.shift_tr_locaxisx + (1.0 - r) * scene.shift_tr_locmin
    else:
        # adding index for later identification
        for i, obj in enumerate (selected):

            obj.location [0] = scene.shift_tr_locaxisx

def processLocationY ():

    # shortcut
    scene = bpy.context.scene
    
    # shortcut
    selected = bpy.context.selected_objects

    # use random ?
    if (scene.shift_tr_locrandom):
        
        # adding index for later identification
        for i, obj in enumerate (selected):
            r = random.random ()
            obj.location [1] = r * scene.shift_tr_locaxisy + (1.0 - r) * scene.shift_tr_locmin
    else:
        # adding index for later identification
        for i, obj in enumerate (selected):

            obj.location [1] = scene.shift_tr_locaxisy

def processLocationZ ():

    # shortcut
    scene = bpy.context.scene
    
    # shortcut
    selected = bpy.context.selected_objects

    # use random ?
    if (scene.shift_tr_locrandom):
    
        # adding index for later identification
        for i, obj in enumerate (selected):
            r = random.random ()
            obj.location [2] = r * scene.shift_tr_locaxisz + (1.0 - r) * scene.shift_tr_locmin
    else:
        # adding index for later identification
        for i, obj in enumerate (selected):

            obj.location [2] = scene.shift_tr_locaxisz
        
####------------------------------------------------------------------------------------------------------------------------------------------------------
#### PROCESS ROTATION
####------------------------------------------------------------------------------------------------------------------------------------------------------

def processRotationX ():

    # shortcut
    scene = bpy.context.scene
    
    # shortcut
    selected = bpy.context.selected_objects

    # use random ?
    if (scene.shift_tr_rotrandom):
        
        # adding index for later identification
        for i, obj in enumerate (selected):
            r = random.random ()
            obj.rotation_euler [0] = r * scene.shift_tr_rotaxisx + (1.0 - r) * scene.shift_tr_rotmin
    else:
        # adding index for later identification
        for i, obj in enumerate (selected):

            obj.rotation_euler [0] = scene.shift_tr_rotaxisx

def processRotationY ():

    # shortcut
    scene = bpy.context.scene
    
    # shortcut
    selected = bpy.context.selected_objects

    # use random ?
    if (scene.shift_tr_rotrandom):
        
        # adding index for later identification
        for i, obj in enumerate (selected):
            r = random.random ()
            obj.rotation_euler [1] = r * scene.shift_tr_rotaxisy + (1.0 - r) * scene.shift_tr_rotmin
    else:
        # adding index for later identification
        for i, obj in enumerate (selected):

            obj.rotation_euler [1] = scene.shift_tr_rotaxisy

def processRotationZ ():

    # shortcut
    scene = bpy.context.scene
    
    # shortcut
    selected = bpy.context.selected_objects

    # use random ?
    if (scene.shift_tr_rotrandom):
        
        # adding index for later identification
        for i, obj in enumerate (selected):
            r = random.random ()
            obj.rotation_euler [2] = r * scene.shift_tr_rotaxisz + (1.0 - r) * scene.shift_tr_rotmin
    else:
        # adding index for later identification
        for i, obj in enumerate (selected):

            obj.rotation_euler [2] = scene.shift_tr_rotaxisz
        
####------------------------------------------------------------------------------------------------------------------------------------------------------
#### PROCESS SCALE
####------------------------------------------------------------------------------------------------------------------------------------------------------

def processScaleX ():

    # shortcut
    scene = bpy.context.scene
    
    # shortcut
    selected = bpy.context.selected_objects

    # use uniform ?
    if (scene.shift_tr_scaleuniform):
        
        # use random ?
        if (scene.shift_tr_scalerandom):
            
            # adding index for later identification
            for i, obj in enumerate (selected):
                r = random.random ()
                obj.scale [0] = r * scene.shift_tr_scaleaxisx + (1.0 - r) * scene.shift_tr_scalemin
                obj.scale [1] = obj.scale [0]
                obj.scale [2] = obj.scale [0]                
        else:
            # adding index for later identification
            for i, obj in enumerate (selected):

                obj.scale [0] = scene.shift_tr_scaleaxisx
                obj.scale [1] = obj.scale [0]
                obj.scale [2] = obj.scale [0]                
    else:
        # use random ?
        if (scene.shift_tr_scalerandom):
            
            # adding index for later identification
            for i, obj in enumerate (selected):
                r = random.random ()
                obj.scale [0] = r * scene.shift_tr_scaleaxisx + (1.0 - r) * scene.shift_tr_scalemin
        else:
            # adding index for later identification
            for i, obj in enumerate (selected):

                obj.scale [0] = scene.shift_tr_scaleaxisx

def processScaleY ():

    # shortcut
    scene = bpy.context.scene
    
    # shortcut
    selected = bpy.context.selected_objects

    # use uniform ?
    if (scene.shift_tr_scaleuniform):
    
        # use random ?
        if (scene.shift_tr_scalerandom):
            
            # adding index for later identification
            for i, obj in enumerate (selected):
                r = random.random ()
                obj.scale [1] = r * scene.shift_tr_scaleaxisy + (1.0 - r) * scene.shift_tr_scalemin
                obj.scale [0] = obj.scale [1]
                obj.scale [2] = obj.scale [1]
        else:
            # adding index for later identification
            for i, obj in enumerate (selected):

                obj.scale [1] = scene.shift_tr_scaleaxisy
                obj.scale [0] = obj.scale [1]
                obj.scale [2] = obj.scale [1]
    else:
        # use random ?
        if (scene.shift_tr_scalerandom):
            
            # adding index for later identification
            for i, obj in enumerate (selected):
                r = random.random ()
                obj.scale [1] = r * scene.shift_tr_scaleaxisy + (1.0 - r) * scene.shift_tr_scalemin
        else:
            # adding index for later identification
            for i, obj in enumerate (selected):

                obj.scale [1] = scene.shift_tr_scaleaxisy

def processScaleZ ():

    # shortcut
    scene = bpy.context.scene
    
    # shortcut
    selected = bpy.context.selected_objects

    # use uniform ?
    if (scene.shift_tr_scaleuniform):
        
        # use random ?
        if (scene.shift_tr_scalerandom):
            
            # adding index for later identification
            for i, obj in enumerate (selected):
                r = random.random ()
                obj.scale [2] = r * scene.shift_tr_scaleaxisz + (1.0 - r) * scene.shift_tr_scalemin
                obj.scale [0] = obj.scale [2]
                obj.scale [1] = obj.scale [2]
        else:
            # adding index for later identification
            for i, obj in enumerate (selected):

                obj.scale [2] = scene.shift_tr_scaleaxisz
                obj.scale [0] = obj.scale [2]
                obj.scale [1] = obj.scale [2]
    else:
        # use random ?
        if (scene.shift_tr_scalerandom):
            
            # adding index for later identification
            for i, obj in enumerate (selected):
                r = random.random ()
                obj.scale [2] = r * scene.shift_tr_scaleaxisz + (1.0 - r) * scene.shift_tr_scalemin
        else:
            # adding index for later identification
            for i, obj in enumerate (selected):

                obj.scale [2] = scene.shift_tr_scaleaxisz
        
####------------------------------------------------------------------------------------------------------------------------------------------------------
#### INTEGRATION AND GUI
####------------------------------------------------------------------------------------------------------------------------------------------------------

class TransformLocationXOp (bpy.types.Operator):

    bl_idname       = "object.transform_locationx_operator"
    bl_label        = "SHIFT - Transform"
    bl_description  = "Applies location to all selected objects"
    bl_register     = True
    bl_undo         = True
    
    def execute (self, context):

        processLocationX ()

        return {'FINISHED'}

class TransformLocationYOp (bpy.types.Operator):

    bl_idname       = "object.transform_locationy_operator"
    bl_label        = "SHIFT - Transform"
    bl_description  = "Applies location to all selected objects"
    bl_register     = True
    bl_undo         = True
    
    def execute (self, context):

        processLocationY ()

        return {'FINISHED'}
    
class TransformLocationZOp (bpy.types.Operator):

    bl_idname       = "object.transform_locationz_operator"
    bl_label        = "SHIFT - Transform"
    bl_description  = "Apply location to all selected objects"
    bl_register     = True
    bl_undo         = True
    
    def execute (self, context):

        processLocationZ ()

        return {'FINISHED'}
    
class TransformRotateXOp (bpy.types.Operator):

    bl_idname       = "object.transform_rotationx_operator"
    bl_label        = "SHIFT - Rotate"
    bl_description  = "Apply rotation to all selected objects"
    bl_register     = True
    bl_undo         = True
    
    def execute (self, context):

        processRotationX ()

        return {'FINISHED'}
    
class TransformRotateYOp (bpy.types.Operator):

    bl_idname       = "object.transform_rotationy_operator"
    bl_label        = "SHIFT - Rotate"
    bl_description  = "Apply rotation to all selected objects"
    bl_register     = True
    bl_undo         = True
    
    def execute (self, context):

        processRotationY ()

        return {'FINISHED'}
    
class TransformRotateZOp (bpy.types.Operator):

    bl_idname       = "object.transform_rotationz_operator"
    bl_label        = "SHIFT - Rotate"
    bl_description  = "Apply rotation to all selected objects"
    bl_register     = True
    bl_undo         = True
    
    def execute (self, context):

        processRotationZ ()

        return {'FINISHED'}

class TransformScaleXOp (bpy.types.Operator):

    bl_idname       = "object.transform_scalex_operator"
    bl_label        = "SHIFT - Scale"
    bl_description  = "Apply scale to all selected objects"
    bl_register     = True
    bl_undo         = True
    
    def execute (self, context):

        processScaleX ()

        return {'FINISHED'}

class TransformScaleYOp (bpy.types.Operator):

    bl_idname       = "object.transform_scaley_operator"
    bl_label        = "SHIFT - Scale"
    bl_description  = "Apply scale to all selected objects"
    bl_register     = True
    bl_undo         = True
    
    def execute (self, context):

        processScaleY ()

        return {'FINISHED'}

class TransformScaleZOp (bpy.types.Operator):

    bl_idname       = "object.transform_scalez_operator"
    bl_label        = "SHIFT - Scale"
    bl_description  = "Apply scale to all selected objects"
    bl_register     = True
    bl_undo         = True
    
    def execute (self, context):

        processScaleZ ()

        return {'FINISHED'}

class TransformPanel (bpy.types.Panel):
     
    bl_idname   = "object.transform_panel"
    bl_label    = "SHIFT - Transform"
    bl_context  = "objectmode"
    bl_register = True
    bl_undo     = True

    bl_space_type   = 'VIEW_3D'
    bl_region_type  = 'TOOLS'

    def draw (self, context):
            
        layout = self.layout
        
        box = layout.box ()
        box.label          ('Location')
        _box = box.box ()
        split = _box.split (percentage = 0.35, align = False)

        split1 = split.split (percentage = 0.8, align = False)
        col = split1.column (align = True)
        col.operator       ('object.transform_locationx_operator', 'Apply')
        col.operator       ('object.transform_locationy_operator', 'Apply')
        col.operator       ('object.transform_locationz_operator', 'Apply')
        col = split1.column (align = True)
        col.label          ('X :')
        col.label          ('Y :')
        col.label          ('Z :')
        
        col = split.column (align = True)
        col.prop           (context.scene, 'shift_tr_locaxisx')
        col.prop           (context.scene, 'shift_tr_locaxisy')
        col.prop           (context.scene, 'shift_tr_locaxisz')

        _box = box.box ()
        col = _box.column (align = False)
        split = col.split (percentage = 0.5, align = False)
        split.prop       (context.scene, 'shift_tr_locrandom')
        split.prop       (context.scene, 'shift_tr_locmin')

        box = layout.box ()
        box.label          ('Rotation')
        _box = box.box ()
        split = _box.split (percentage = 0.35, align = False)

        split1 = split.split (percentage = 0.8, align = False)
        col = split1.column (align = True)
        col.operator       ('object.transform_rotationx_operator', 'Apply')
        col.operator       ('object.transform_rotationy_operator', 'Apply')
        col.operator       ('object.transform_rotationz_operator', 'Apply')
        col = split1.column (align = True)
        col.label          ('X :')
        col.label          ('Y :')
        col.label          ('Z :')
        
        col = split.column (align = True)
        col.prop           (context.scene, 'shift_tr_rotaxisx')
        col.prop           (context.scene, 'shift_tr_rotaxisy')
        col.prop           (context.scene, 'shift_tr_rotaxisz')

        _box = box.box ()
        col = _box.column (align = False)
        split = col.split (percentage = 0.5, align = False)
        split.prop       (context.scene, 'shift_tr_rotrandom')
        split.prop       (context.scene, 'shift_tr_rotmin')

        box = layout.box ()
        box.label          ('Scale')        
        _box = box.box ()
        split = _box.split (percentage = 0.35, align = False)

        split1 = split.split (percentage = 0.8, align = False)
        col = split1.column (align = True)
        col.operator       ('object.transform_scalex_operator', 'Apply')
        col.operator       ('object.transform_scaley_operator', 'Apply')
        col.operator       ('object.transform_scalez_operator', 'Apply')
        col = split1.column (align = True)
        col.label          ('X :')
        col.label          ('Y :')
        col.label          ('Z :')
        
        col = split.column (align = True)
        col.prop           (context.scene, 'shift_tr_scaleaxisx')
        col.prop           (context.scene, 'shift_tr_scaleaxisy')
        col.prop           (context.scene, 'shift_tr_scaleaxisz')

        _box = box.box ()
        col = _box.column (align = False)
        split = col.split (percentage = 0.5, align = False)
        split.prop       (context.scene, 'shift_tr_scalerandom')
        split.prop       (context.scene, 'shift_tr_scalemin')
        col.prop         (context.scene, 'shift_tr_scaleuniform')
        
        
def register ():

    bpy.utils.register_module (__name__)

    # options
    
    # ----------------------------------------------------------
    bpy.types.Scene.shift_tr_locaxisx = FloatProperty (
        description = "X axis",
        min         = -10000.0,
        max         =  10000.0,
        precision   = 3,
        step        = 1,
        subtype     = 'DISTANCE',
        default     = 0.0)
    bpy.types.Scene.shift_tr_locaxisy = FloatProperty (
        description = "Y axis",
        min         = -10000.0,
        max         =  10000.0,
        precision   = 3,
        step        = 1,
        subtype     = 'DISTANCE',
        default     = 0.0)
    bpy.types.Scene.shift_tr_locaxisz = FloatProperty (
        description = "Z axis",
        min         = -10000.0,
        max         =  10000.0,
        precision   = 3,
        step        = 1,
        subtype     = 'DISTANCE',
        default     = 0.0)
    bpy.types.Scene.shift_tr_locmin = FloatProperty (
        description = "Minimum value",
        min         = -10000.0,
        max         =  10000.0,
        precision   = 3,
        step        = 1,
        subtype     = 'DISTANCE',
        default     = 0.0)
    bpy.types.Scene.shift_tr_locrandom = BoolProperty (
        name        = "random",
        description = "Use random generated values in selected range",
        default     = False)
         
    # ----------------------------------------------------------
    bpy.types.Scene.shift_tr_rotaxisx = FloatProperty (
        description = "X axis",
        min         = - math.pi,
        max         =   math.pi,
        precision   = 1,
        step        = 1,
        subtype     = 'ANGLE',
        default     = 0.0)
    bpy.types.Scene.shift_tr_rotaxisy = FloatProperty (
        description = "Y axis",
        min         = - math.pi,
        max         =   math.pi,
        precision   = 1,
        step        = 1,
        subtype     = 'ANGLE',
        default     = 0.0)
    bpy.types.Scene.shift_tr_rotaxisz = FloatProperty (
        description = "Z axis",
        min         = - math.pi,
        max         =   math.pi,
        precision   = 1,
        step        = 1,
        subtype     = 'ANGLE',
        default     = 0.0)
    bpy.types.Scene.shift_tr_rotmin = FloatProperty (
        description = "Minimum value",
        min         = - math.pi,
        max         =   math.pi,
        precision   = 1,
        step        = 1,
        subtype     = 'ANGLE',
        default     = 0.0)
    bpy.types.Scene.shift_tr_rotrandom = BoolProperty (
        name        = "random",
        description = "Use random generated values in selected range",
        default     = False)
    
    # ----------------------------------------------------------
    bpy.types.Scene.shift_tr_scaleaxisx = FloatProperty (
        description = "X axis",
        min         = 0.0,
        max         = 10.0,
        precision   = 3,
        step        = 1,
        subtype     = 'NONE',
        default     = 1.0)
    bpy.types.Scene.shift_tr_scaleaxisy = FloatProperty (
        description = "Y axis",
        min         = 0.0,
        max         = 10.0,
        precision   = 3,
        step        = 1,
        subtype     = 'NONE',
        default     = 1.0)
    bpy.types.Scene.shift_tr_scaleaxisz = FloatProperty (
        description = "Z axis",
        min         = 0.0,
        max         = 10.0,
        precision   = 3,
        step        = 1,
        subtype     = 'NONE',
        default     = 1.0)
    bpy.types.Scene.shift_tr_scalemin = FloatProperty (
        description = "Minimum value",
        min         = 0.0,
        max         = 10.0,
        precision   = 3,
        step        = 1,
        subtype     = 'NONE',
        default     = 1.0)
    bpy.types.Scene.shift_tr_scalerandom = BoolProperty (
        name        = "random",
        description = "Use random generated values in selected range",
        default     = False)
    bpy.types.Scene.shift_tr_scaleuniform = BoolProperty (
        name        = "uniform",
        description = "Use uniform scaleing",
        default     = False)

    
def unregister ():

    bpy.utils.unregister_module (__name__)

    del bpy.types.Scene.shift_tr_locaxisx
    del bpy.types.Scene.shift_tr_locaxisy
    del bpy.types.Scene.shift_tr_locaxisz
    del bpy.types.Scene.shift_tr_locrandom
    del bpy.types.Scene.shift_tr_locmin
    
    del bpy.types.Scene.shift_tr_rotaxisx
    del bpy.types.Scene.shift_tr_rotaxisy
    del bpy.types.Scene.shift_tr_rotaxisz
    del bpy.types.Scene.shift_tr_rotrandom
    del bpy.types.Scene.shift_tr_rotmin
    
    del bpy.types.Scene.shift_tr_scaleaxisx
    del bpy.types.Scene.shift_tr_scaleaxisy
    del bpy.types.Scene.shift_tr_scaleaxisz
    del bpy.types.Scene.shift_tr_scalerandom
    del bpy.types.Scene.shift_tr_scalemin
             
if __name__ == "__main__":
    
    register ()
