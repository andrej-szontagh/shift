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
    "name"              : "SHIFT - Rename",
    "author"            : "BBC",
    "version"           : (1,0),
    "blender"           : (2, 5, 3),
    "api"               : 31236,
    "category"          : "Object",
    "location"          : "Tool Shelf",
    "warning"           : '',
    "wiki_url"          : "",
    "tracker_url"       : "",
    "description"       : "Rename multiple objects"}


import bpy
import time
import operator
import mathutils

from math       import *
from bpy.props  import *

####------------------------------------------------------------------------------------------------------------------------------------------------------
#### FORMATTING NAMES - PARSE
####------------------------------------------------------------------------------------------------------------------------------------------------------

def formatNameParse (format_string, keyword):

    literals = []

    literals.append (('l0', '%>'    + keyword))
    literals.append (('l1', '%>>'   + keyword))
    literals.append (('l2', '%>>>'  + keyword))
    literals.append (('l3', '%>>>>' + keyword))
    literals.append (('r3', '%'     + keyword + '<<<<'))
    literals.append (('r2', '%'     + keyword + '<<<'))
    literals.append (('r1', '%'     + keyword + '<<'))
    literals.append (('r0', '%'     + keyword + '<'))

    fstr = str (format_string)

    replaces = []

    fl = len (fstr)

    for lt in literals :

        ltok  = lt [0]
        ltstr = lt [1];     ll = len (ltstr)

        index = fstr.find (ltstr)

        while index >= 0:
            
            delimiter = ''; index += ll
            
            while ((index < fl) and (fstr [index] != '%')):
                
                delimiter += fstr [index]; index += 1

            if delimiter != '':

                replaces.append ((ltok, ltstr + delimiter + '%', delimiter))

                fstr = fstr.replace (ltstr + delimiter + '%', '')
                
                index = fstr.find (ltstr)
                
            else: break

    if (fstr.find ('%' + keyword + '%') >= 0):
        
        replaces.append ((None, '%' + keyword + '%', None))

    literals [:] = []

    return replaces

####------------------------------------------------------------------------------------------------------------------------------------------------------
#### FORMATTING NAMES - SUBSTITUTE
####------------------------------------------------------------------------------------------------------------------------------------------------------

def formatName (format_string, substition_string, replaces):

    result = format_string
    for r in replaces:
        if not  r [0]:  result = format_string.replace (r [1], substition_string)
        elif    r [0][0] == 'l':
            try:    i = int (r[0][1]);  result = format_string.replace (r [1], substition_string.split  (r [2], i + 1)[i + 1])
            except: pass
        elif    r [0][0] == 'r':
            try:    i = int (r[0][1]);  result = format_string.replace (r [1], substition_string.rsplit (r [2], i + 1)[0])
            except: pass

    return (result)

####------------------------------------------------------------------------------------------------------------------------------------------------------
#### PROCESS RENAME
####------------------------------------------------------------------------------------------------------------------------------------------------------

def processRename ():

    start_time = time.clock ()

    # save list
    selected = list (bpy.context.selected_objects)

    # anything selected ?
    if len (selected) == 0: return

    # log
    print ('\nRename starting... \n\n\tObjects (', len (selected), ') :')
    print ('')

    # naming ..
    nameo = bpy.context.scene.shift_rn_nameo
    named = bpy.context.scene.shift_rn_named

    # parsing formatting string
    replo = formatNameParse (nameo, 'name')
    repld = formatNameParse (named, 'name')

    # process all objects
    for i, obj in enumerate (selected):

        # is it mesh ?
        if obj and obj.type == 'MESH':

            obj.name        = formatName (nameo, obj.name, replo)
            obj.data.name   = formatName (named, obj.data.name, repld)
              
            # log
            print ("Object : '", obj.name, " \tData : ", obj.data.name)

        else:
            
            print ('ERROR | Object \'', obj.name, '\' is not mesh !')

    # log            
    print ('')
    print ('\nRename finished in %.4f sec.' % (time.clock () - start_time))
    
####------------------------------------------------------------------------------------------------------------------------------------------------------
#### CHECK NAMES
####------------------------------------------------------------------------------------------------------------------------------------------------------

def processCheckNames ():

    print ('Check names : ')
    print ('')

    # naming ..
    nameo = bpy.context.scene.shift_rn_nameo
    named = bpy.context.scene.shift_rn_named

    # parsing formatting string
    replo = formatNameParse (nameo, 'name')
    repld = formatNameParse (named, 'name')

    for obj in bpy.context.selected_objects:

        # is it mesh ?
        if obj and obj.type == 'MESH':

            nameoo = formatName (nameo, obj.name, replo)
            namedd = formatName (named, obj.data.name, repld)

            print ("Object : '", nameoo, " \tData : ", namedd)
    print ('')
            
    return

####------------------------------------------------------------------------------------------------------------------------------------------------------
#### INTEGRATION AND GUI
####------------------------------------------------------------------------------------------------------------------------------------------------------

class RenameOp (bpy.types.Operator):

    bl_idname   = "object.rename_operator"
    bl_label    = "SHIFT - Rename"
    bl_register = True
    bl_undo     = True
    
    def execute (self, context):

        if bpy.context.scene.shift_rn_nameo != '' and \
           bpy.context.scene.shift_rn_named != '' :

            processRename ()

        return {'FINISHED'}

class RenameOpCheck (bpy.types.Operator):

    bl_idname       = "object.rename_operator_check"
    bl_label        = "SHIFT - Rename"
    bl_description  = "Prints formatted name(s)"
    bl_register     = True
    bl_undo         = False
    
    def execute (self, context):

        if bpy.context.scene.shift_rn_nameo != '' and \
           bpy.context.scene.shift_rn_named != '' :

            processCheckNames ()

        return {'FINISHED'}
        
class ConvexHullPanel (bpy.types.Panel):
     
    bl_idname   = "object.rename_panel"
    bl_label    = "SHIFT - Rename"
    bl_context  = "objectmode"
    bl_register = True
    bl_undo     = True

    bl_space_type   = 'VIEW_3D'
    bl_region_type  = 'TOOLS'

    def draw (self, context):
            
        layout = self.layout
        
        box = layout.box()
        box.operator        ('object.rename_operator', 'Rename')

        box1 = box.box ()
        
        split = box1.split  (percentage = 0.95, align = True)
        split.prop          (context.scene, 'shift_rn_nameo')
        split.operator      ('object.rename_operator_check', 'C')
        
        split = box1.split  (percentage = 0.95, align = True)
        split.prop          (context.scene, 'shift_rn_named')
        split.operator      ('object.rename_operator_check', 'C')
                
def register ():

    bpy.utils.register_module (__name__)
    
    # options
        
    # ----------------------------------------------------------
    bpy.types.Scene.shift_rn_nameo = StringProperty (
        name        = "Object",
        description = "Formatting string determining name of object(s)",
        default     = "%name%",
        subtype     = 'NONE')
    bpy.types.Scene.shift_rn_named = StringProperty (
        name        = "Data",
        description = "Formatting string determining name of mesh datablock(s)",
        default     = "%name%",
        subtype     = 'NONE')
     
def unregister ():

    bpy.utils.unregister_module (__name__)

    del bpy.types.Scene.shift_rn_nameo
    del bpy.types.Scene.shift_rn_named
             
if __name__ == "__main__":
    
    register ()
