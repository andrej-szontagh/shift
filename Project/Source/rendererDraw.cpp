
////////////////////////////////////////////////////////////////////////////////////////////////////////////
// MACRO
////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define M_DR_SET_SHADER_CULLON(shader) {    \
                                            \
    if (shader != dr_state_shader) {        \
        glUseProgram (dr_state_shader = shader);    \
                                                    \
        M_STATE_CULLFACE_SET;   \
    }   \
}

#define M_DR_SET_SHADER_CULLOFF(shader) {   \
                                            \
    if (shader != dr_state_shader) {        \
        glUseProgram (dr_state_shader = shader);    \
                                                    \
        M_STATE_CULLFACE_CLEAR;   \
    }   \
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// dr_Draw
////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if 1

VOID dr_Draw (

    )
{

    dr_state_shader = 0;

    // states
    M_STATE_TEX0_RECT_CLEAR;
    M_STATE_TEX1_RECT_CLEAR;
    M_STATE_TEX2_RECT_CLEAR;

    UINT_32 materialc = dr_context.count;

    for (UINT_32 i = 0; i < materialc; i ++) {

        TContextMaterial * conmat = &dr_context.list [i];    UINT_32 modelc = conmat->count;

        // shortcut
        TMaterial * material = &dr_materials [conmat->material];

        // per instance setup
        switch (material->type) {

            // ---------------------------------------------------------------------
            // SOLID
            // ---------------------------------------------------------------------
            case M_MATERIAL_TYPE_SOLID: {

                dr_SetMaterialSolid (material);

                for (UINT_32 j = 0; j < modelc; j ++) {
                    
                    TContextModel * conmod = &conmat->list [j];

                    UINT_32 mi = conmod->model; TModel * model = &dr_models [mi];

                    register UINT_32P p = conmod->list;
                    register UINT_32  c = conmod->count;

                    register UINT_32 model_mode  = model->mode;
                    register UINT_32 model_count = model->count;

                    switch (dr_model_shaders [mi]) {

                        case M_MODEL_SHADER_NORMAL:

                            // shader
                            M_DR_SET_SHADER_CULLON (dr_program_solid_gbuffers_normal);

                            // model
                            dr_SetModelSolidNormal (model);

                            // draw
                            for (UINT_32 k = 0; k < c; k ++)    dr_DrawSolidNormal (p [k], model_mode, model_count);

                            break;

                        case M_MODEL_SHADER_MORPH:

                            // shader
                            M_DR_SET_SHADER_CULLON (dr_program_solid_gbuffers_morph);

                            /// shader attributes MOVE
                            glMultiTexCoord3f (GL_TEXTURE2, model->param1, model->param2, model->param3);
                            glMultiTexCoord3f (GL_TEXTURE3, 0.0f, 0.0f, model->param1);

                            // model
                            dr_SetModelSolidMorph (model);

                            // draw
                            for (UINT_32 k = 0; k < c; k ++)    dr_DrawSolidMorph (p [k], model_mode, model_count);

                            break;

                        case M_MODEL_SHADER_SHRINK:

                            // shader
                            M_DR_SET_SHADER_CULLON (dr_program_solid_gbuffers_shrink);

                            // model
                            dr_SetModelSolidNormal (model);

                            // draw
                            for (UINT_32 k = 0; k < c; k ++)    dr_DrawSolidShrink (p [k], model_mode, model_count);

                            break;
                    }
                }

                break;
            }

            // ---------------------------------------------------------------------
            // TERRAIN
            // ---------------------------------------------------------------------
            case M_MATERIAL_TYPE_TERRAIN: {

                dr_SetMaterialTerrain (material);

                for (UINT_32 j = 0; j < modelc; j ++) {

                    TContextModel * conmod = &conmat->list [j];

                    UINT_32 mi = conmod->model; TModel * model = &dr_models [mi];

                    register UINT_32P p = conmod->list;
                    register UINT_32  c = conmod->count;

                    register UINT_32 model_mode  = model->mode;
                    register UINT_32 model_count = model->count;

                    switch (dr_model_shaders [mi]) {

                        case M_MODEL_SHADER_NORMAL:

                            // shader
                            if (dr_program_terrain_gbuffers_normal != dr_state_shader) {
                                                                      dr_state_shader = dr_program_terrain_gbuffers_normal;

                                glUseProgram (dr_state_shader);
                                M_STATE_CULLFACE_SET;
                            }

                            // model
                            dr_SetModelTerrainNormal (model);

                            // draw
                            for (UINT_32 k = 0; k < c; k ++)    dr_DrawTerrainNormal (p [k], model_mode, model_count);

                            break;

                        case M_MODEL_SHADER_MORPH:

                            // shader
                            if (dr_program_terrain_gbuffers_morph != dr_state_shader) {
                                                                     dr_state_shader = dr_program_terrain_gbuffers_morph;

                                glUseProgram (dr_state_shader);
                                M_STATE_CULLFACE_SET;
                            }

                            // model
                            dr_SetModelTerrainMorph (model);

                            // draw
                            for (UINT_32 k = 0; k < c; k ++)    dr_DrawTerrainMorph (p [k], model_mode, model_count);

                            break;
                    }
                }

                break;
            }

            // ---------------------------------------------------------------------
            // FOLIAGE
            // ---------------------------------------------------------------------
            case M_MATERIAL_TYPE_FOLIAGE: {

                ///
                ///continue;

                dr_SetMaterialFoliage (material);
                
                for (UINT_32 j = 0; j < modelc; j ++) {

                    TContextModel * conmod = &conmat->list [j];

                    UINT_32 mi = conmod->model; TModel * model = &dr_models [mi];

                    register UINT_32P p = conmod->list;
                    register UINT_32  c = conmod->count;

                    register UINT_32 model_list  = model->list;

                    switch (dr_model_shaders [mi]) {

                        case M_MODEL_SHADER_GROW:

                            // shader
                            M_DR_SET_SHADER_CULLOFF (dr_program_foliage_gbuffers_grow);

                            for (UINT_32 k = 0; k < c; k ++)    dr_DrawFoliageGrow (p [k], model_list);

                            break;

                        case M_MODEL_SHADER_SHRINK:

                            // shader
                            M_DR_SET_SHADER_CULLOFF (dr_program_foliage_gbuffers_shrink);

                            for (UINT_32 k = 0; k < c; k ++)    dr_DrawFoliageShrink (p [k], model_list);

                            break;
                    }
                }

                break;
            }

            // ---------------------------------------------------------------------
            // GRASS
            // ---------------------------------------------------------------------
            case M_MATERIAL_TYPE_GRASS: {

                ///
                ///continue;

                dr_SetMaterialGrass (material);

                for (UINT_32 j = 0; j < modelc; j ++) {

                    TContextModel * conmod = &conmat->list [j];

                    UINT_32 mi = conmod->model; TModel * model = &dr_models [mi];

                    // shader program
                    M_DR_SET_SHADER_CULLOFF (dr_program_grass_gbuffers_normal);

                    register UINT_32P p = conmod->list;
                    register UINT_32  c = conmod->count;

                    register UINT_32 model_list = model->list;

                    for (UINT_32 k = 0; k < c; k ++)    dr_DrawGrassNormal (p [k], model_list);
                }

                break;
            }
        }
    }
}

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// dr_DrawDepth
////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if 1

VOID dr_DrawDepth (
                    
    )
{

    dr_state_shader = 0;

    // states
    M_STATE_TEX0_RECT_CLEAR;
    M_STATE_TEX1_RECT_CLEAR;
    M_STATE_TEX2_RECT_CLEAR;

    //M_STATE_CLEAR_TEX0;
    M_STATE_TEX1_CLEAR;
    M_STATE_TEX2_CLEAR;
    M_STATE_TEX3_CLEAR;
    M_STATE_TEX4_CLEAR;
    M_STATE_TEX5_CLEAR;
    M_STATE_TEX6_CLEAR;
    M_STATE_TEX7_CLEAR;
    M_STATE_TEX8_CLEAR;
    M_STATE_TEX9_CLEAR;

    M_STATE_ARRAY_COLOR_CLEAR;
    M_STATE_ARRAY_SECONDARY_COLOR_CLEAR;
    M_STATE_ARRAY_NORMAL_CLEAR;
    //M_STATE_ARRAY_VERTEX_CLEAR;
    //M_STATE_ARRAY_TEX0_COORDS_CLEAR;
    //M_STATE_ARRAY_TEX1_COORDS_CLEAR;
    M_STATE_ARRAY_TEX2_COORDS_CLEAR;

    UINT_32 materialc = dr_context.count;

    for (UINT_32 i = 0; i < materialc; i ++) {

        TContextMaterial * conmat = &dr_context.list [i];    UINT_32 modelc = conmat->count;

        // shortcut
        TMaterial * material = &dr_materials [conmat->material];

        // per instance setup
        switch (material->type) {

            // ---------------------------------------------------------------------
            // SOLID
            // ---------------------------------------------------------------------
            case M_MATERIAL_TYPE_SOLID: {

                M_STATE_TEX0_CLEAR;

                for (UINT_32 j = 0; j < modelc; j ++) {

                    TContextModel * conmod = &conmat->list [j];

                    UINT_32 mi = conmod->model; TModel * model = &dr_models [mi];

                    register UINT_32P p = conmod->list;
                    register UINT_32  c = conmod->count;

                    register UINT_32 model_mode  = model->mode;
                    register UINT_32 model_count = model->count;

                    switch (dr_model_shaders [mi]) {

                        case M_MODEL_SHADER_NORMAL:

                            // shader
                            M_DR_SET_SHADER_CULLON (dr_program_solid_depth_normal);

                            // model
                            dr_SetModelDepthSolidNormal (model);

                            // draw
                            for (UINT_32 k = 0; k < c; k ++)    dr_DrawSolidNormal (p [k], model_mode, model_count);

                            break;

                        case M_MODEL_SHADER_MORPH:

                            // shader
                            M_DR_SET_SHADER_CULLON (dr_program_solid_depth_morph);

                            // shader attributes
                            glMultiTexCoord3f   (GL_TEXTURE2, model->param1, model->param2, model->param3);

                            // model
                            dr_SetModelDepthSolidMorph (model);

                            // draw
                            for (UINT_32 k = 0; k < c; k ++)    dr_DrawSolidMorph (p [k], model_mode, model_count);

                            break;

                        case M_MODEL_SHADER_SHRINK:

                            // shader
                            M_DR_SET_SHADER_CULLON (dr_program_solid_depth_shrink);

                            // model
                            dr_SetModelDepthSolidNormal (model);

                            // draw
                            for (UINT_32 k = 0; k < c; k ++)    dr_DrawSolidShrink (p [k], model_mode, model_count);

                            break;
                    }
                }

                break;
            }

            // ---------------------------------------------------------------------
            // TERRAIN
            // ---------------------------------------------------------------------
            case M_MATERIAL_TYPE_TERRAIN: {

                M_STATE_TEX0_CLEAR;

                for (UINT_32 j = 0; j < modelc; j ++) {

                    TContextModel * conmod = &conmat->list [j];

                    UINT_32 mi = conmod->model; TModel * model = &dr_models [mi];

                    register UINT_32P p = conmod->list;
                    register UINT_32  c = conmod->count;

                    register UINT_32 model_mode  = model->mode;
                    register UINT_32 model_count = model->count;

                    switch (dr_model_shaders [mi]) {

                        case M_MODEL_SHADER_NORMAL:

                            // shader
                            M_DR_SET_SHADER_CULLON (dr_program_terrain_depth_normal);

                            // model
                            dr_SetModelDepthTerrainNormal (model);

                            // draw
                            for (UINT_32 k = 0; k < c; k ++)    dr_DrawTerrainNormal (p [k], model_mode, model_count);

                            break;

                        case M_MODEL_SHADER_MORPH:

                            // shader
                            M_DR_SET_SHADER_CULLON (dr_program_terrain_depth_morph);

                            // model
                            dr_SetModelDepthTerrainMorph (model);

                            // draw
                            for (UINT_32 k = 0; k < c; k ++)    dr_DrawTerrainMorph (p [k], model_mode, model_count);

                            break;
                    }
                }

                break;
            }

            // ---------------------------------------------------------------------
            // FOLIAGE
            // ---------------------------------------------------------------------
            case M_MATERIAL_TYPE_FOLIAGE: {

                ///
                ///continue;

                M_STATE_TEX0_SET (material->diffuse);

                for (UINT_32 j = 0; j < modelc; j ++) {

                    TContextModel * conmod = &conmat->list [j];

                    UINT_32 mi = conmod->model; TModel * model = &dr_models [mi];

                    register UINT_32P p = conmod->list;
                    register UINT_32  c = conmod->count;

                    register UINT_32 model_list  = model->list;

                    switch (dr_model_shaders [mi]) {

                        case M_MODEL_SHADER_GROW:

                            // shader
                            M_DR_SET_SHADER_CULLOFF (dr_program_foliage_depth_grow);

                            for (UINT_32 k = 0; k < c; k ++)    dr_DrawFoliageGrow (p [k], model_list);

                            break;

                        case M_MODEL_SHADER_SHRINK:

                            // shader
                            M_DR_SET_SHADER_CULLOFF (dr_program_foliage_depth_shrink);

                            for (UINT_32 k = 0; k < c; k ++)    dr_DrawFoliageShrink (p [k], model_list);

                            break;
                    }
                }

                break;
            }

            // ---------------------------------------------------------------------
            // GRASS
            // ---------------------------------------------------------------------
            case M_MATERIAL_TYPE_GRASS: {

                ///
                ///continue;

                //  M_STATE_CLEAR_TEX0;

                M_STATE_TEX0_SET (material->diffuse);

                for (UINT_32 j = 0; j < modelc; j ++) {

                    TContextModel * conmod = &conmat->list [j];

                    UINT_32 mi = conmod->model; TModel * model = &dr_models [mi];

                    // shader
                    M_DR_SET_SHADER_CULLOFF (dr_program_grass_depth_normal);

                    register UINT_32P p = conmod->list;
                    register UINT_32  c = conmod->count;

                    register UINT_32 model_list  = model->list;

                    for (UINT_32 k = 0; k < c; k ++)    dr_DrawGrassNormal (p [k], model_list);
                }

                break;
            }
        }
    }
}

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// dr_DrawShadows
////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if 1

VOID dr_DrawShadows (

         UINT_32 split
    )
{
    register UINT_32P listp = dr_list_objects_shadow_split  [split];
    register UINT_32  listc = dr_list_objects_shadow_splitc [split];

    if (listc == 0) return;
    
    UINT_32 search1 = 0;
    UINT_32 search2 = 0;

    BOOL nextma;
    BOOL nextmo;

    // time stamp
    do dr_stamp ++; while (!dr_stamp);  // zero reserved

    // pick first material
    register UINT_32 material = dr_object_materials [listp [0]];  dr_material_stamps [material] = dr_stamp;

    /// WE NEED TO GRAB ALL MATERIAL COUNTERS FOR EACH SPLIT HERE !!

    // main loop
    while (true) {

        register UINT_32 materialn;

        UINT_32 count = 0;

        // grab objects with the same material
        dr_GrabSame (materialn, material, dr_material_stamps, dr_object_materials, dr_list_objects1, listp, listc, search1, count, nextma);

        /// CONSIDER USING dr_GrabSameStop

        // grab material
        TMaterial * ma = &dr_materials [material];

        // init
        search2 = 0;

        // type
        switch (ma->type) {

            // ---------------------------------------------------------------------
            // SOLID
            // ---------------------------------------------------------------------
            case M_MATERIAL_TYPE_SOLID: {

                ///
                ///break;

                M_STATE_TEX0_CLEAR;

                // pick first model
                register UINT_32 model = dr_object_models [dr_list_objects1 [0]];  dr_model_stamps [model] = dr_stamp;

                // until all done
                while (true) {

                    nextmo = false;

                    TModel * m = &dr_models [model];

                    register UINT_32 modeln;

                    register UINT_32 model_mode  = m->mode;
                    register UINT_32 model_count = m->count;

                    switch (dr_model_shaders [model]) {

                        case M_MODEL_SHADER_NORMAL:

                            // shader
                            M_DR_SET_SHADER_CULLON (dr_program_solid_depth_normal);

                            // model
                            dr_SetModelDepthSolidNormal (m);

                            // grab objects with the same model
                            dr_GrabSameCall (dr_DrawSolidNormal, model_mode, model_count, \
                                modeln, model, dr_model_stamps, dr_object_models, dr_list_objects1, count, search2, nextmo);

                            break;

                        case M_MODEL_SHADER_MORPH:

                            // shader
                            M_DR_SET_SHADER_CULLON (dr_program_solid_depth_morph);

                            // model
                            dr_SetModelDepthSolidMorph (m);

                            // grab objects with the same model
                            dr_GrabSameCall (dr_DrawSolidMorph, model_mode, model_count, \
                                modeln, model, dr_model_stamps, dr_object_models, dr_list_objects1, count, search2, nextmo);

                            break;

                        case M_MODEL_SHADER_SHRINK:

                            // shader
                            M_DR_SET_SHADER_CULLON (dr_program_solid_depth_shrink);

                            // model
                            dr_SetModelDepthSolidNormal (m);

                            // grab objects with the same model
                            dr_GrabSameCall (dr_DrawSolidShrink, model_mode, model_count, \
                                modeln, model, dr_model_stamps, dr_object_models, dr_list_objects1, count, search2, nextmo);

                            break;
                    }

                    // finished
                    if (!nextmo) break;

                    // set new model
                    model = modeln;
                }

                break;
            }

            // ---------------------------------------------------------------------
            // TERRAIN
            // ---------------------------------------------------------------------
            case M_MATERIAL_TYPE_TERRAIN: {

                M_STATE_TEX0_CLEAR;

                // pick first model
                register UINT_32 model = dr_object_models [dr_list_objects1 [0]];  dr_model_stamps [model] = dr_stamp;

                // until all done
                while (true) {

                    nextmo = false;

                    TModel * m = &dr_models [model];

                    register UINT_32 modeln;

                    register UINT_32 model_mode  = m->mode;
                    register UINT_32 model_count = m->count;

                    switch (dr_model_shaders [model]) {

                        case M_MODEL_SHADER_NORMAL:

                            // shader
                            M_DR_SET_SHADER_CULLON (dr_program_terrain_depth_normal);

                            // model
                            dr_SetModelDepthTerrainNormal (m);

                            // grab objects with the same model
                            dr_GrabSameCall (dr_DrawTerrainNormal, model_mode, model_count, \
                                modeln, model, dr_model_stamps, dr_object_models, dr_list_objects1, count, search2, nextmo);

                            break;

                        case M_MODEL_SHADER_MORPH:

                            // shader
                            M_DR_SET_SHADER_CULLON (dr_program_terrain_depth_morph);

                            // model
                            dr_SetModelDepthTerrainMorph (m);

                            // grab objects with the same model
                            dr_GrabSameCall (dr_DrawTerrainMorph, model_mode, model_count, \
                                modeln, model, dr_model_stamps, dr_object_models, dr_list_objects1, count, search2, nextmo);

                            break;
                    }

                    // finished
                    if (!nextmo) break;

                    // set new model
                    model = modeln;
                }

                break;
            }

            // ---------------------------------------------------------------------
            // FOLIAGE
            // ---------------------------------------------------------------------
            case M_MATERIAL_TYPE_FOLIAGE: {

                ///
                ///break;

                M_STATE_TEX0_SET (ma->diffuse);

                // pick first model
                register UINT_32 model = dr_object_models [dr_list_objects1 [0]];  dr_model_stamps [model] = dr_stamp;

                // until all done
                while (true) {

                    nextmo = false;

                    TModel * m = &dr_models [model];

                    register UINT_32 modeln;

                    // shortcut
                    register UINT_32 model_list = m->list;

                    switch (dr_model_shaders [model]) {

                        case M_MODEL_SHADER_GROW:

                            // shader
                            M_DR_SET_SHADER_CULLOFF (dr_program_foliage_depth_grow);

                            // grab objects with the same model
                            dr_GrabSameCall (dr_DrawFoliageGrow, model_list, \
                                modeln, model, dr_model_stamps, dr_object_models, dr_list_objects1, count, search2, nextmo);

                            break;

                        case M_MODEL_SHADER_SHRINK:

                            // shader
                            M_DR_SET_SHADER_CULLOFF (dr_program_foliage_depth_shrink);

                            // grab objects with the same model
                            dr_GrabSameCall (dr_DrawFoliageShrink, model_list, \
                                modeln, model, dr_model_stamps, dr_object_models, dr_list_objects1, count, search2, nextmo);

                            break;
                    }

                    // finished
                    if (!nextmo) break;

                    // set new model
                    model = modeln;
                }

                break;
            }

            // ---------------------------------------------------------------------
            // GRASS
            // ---------------------------------------------------------------------
            case M_MATERIAL_TYPE_GRASS: {

                ///
                ///break;

                M_STATE_TEX0_SET (ma->diffuse);

                // pick first model
                register UINT_32 model = dr_object_models [dr_list_objects1 [0]];  dr_model_stamps [model] = dr_stamp;

                // until all done
                while (true) {

                    nextmo = false;

                    TModel * m = &dr_models [model];

                    register UINT_32 modeln;

                    // shader
                    M_DR_SET_SHADER_CULLOFF (dr_program_grass_depth_normal);

                    // shortcut
                    register UINT_32 model_list = m->list;

                    // grab objects with the same model
                    dr_GrabSameCall (dr_DrawGrassNormal, model_list, \
                        modeln, model, dr_model_stamps, dr_object_models, dr_list_objects1, count, search2, nextmo);

                    // finished
                    if (!nextmo) break;

                    // set new model
                    model = modeln;
                }

                break;
            }
        }

        // finished
        if (!nextma) break;

        // set new material
        material = materialn;
    }
}

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// dr_DrawOcclusions
////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if 1

VOID dr_DrawOcclusions (
                    
    )
{
    // STATES

    M_STATE_TEX0_CLEAR;
    M_STATE_TEX1_CLEAR;
    M_STATE_TEX2_CLEAR;
    M_STATE_TEX3_CLEAR;
    M_STATE_TEX4_CLEAR;
    M_STATE_TEX5_CLEAR;
    M_STATE_TEX6_CLEAR;
    M_STATE_TEX7_CLEAR;
    M_STATE_TEX8_CLEAR;
    M_STATE_TEX9_CLEAR;

    M_STATE_TEX0_RECT_CLEAR;
    M_STATE_TEX1_RECT_CLEAR;
    M_STATE_TEX2_RECT_CLEAR;

    // states
    M_STATE_ARRAY_COLOR_CLEAR;
    M_STATE_ARRAY_SECONDARY_COLOR_CLEAR;
    M_STATE_ARRAY_NORMAL_CLEAR;
    //M_STATE_ARRAY_VERTEX_CLEAR;
    M_STATE_ARRAY_TEX0_COORDS_CLEAR;
    M_STATE_ARRAY_TEX1_COORDS_CLEAR;
    M_STATE_ARRAY_TEX2_COORDS_CLEAR;

    // depth test
    M_STATE_DEPTHTEST_SET;  glDepthFunc (GL_LESS);

    // cull faces
    M_STATE_CULLFACE_SET;

    // shader
    dr_state_shader = 0;

    // time stamp
    do dr_stamp ++; while (!dr_stamp);  // zero reserved

    // -----------------------------------
    // GRAB OCCLUDERS AND OCCLUSIONS
    // -----------------------------------

    register UINT_32 coccluders  = 0;
    register UINT_32 cocclusions = 0;

    for (UINT_32 i = 0; i < dr_list_objects_viewc; i ++) {

        // this array is sorted above so we just pick the right objects
        register UINT_32 ID = dr_list_objects_view [i];

        // is occluder ?
        if (dr_object_flags [ID] & M_FLAG_OBJECT_OCCLUDER) {

            register UINT_32 tags = dr_object_tags [ID];

            // mark this object
            tags |= M_TAG_OCCLUDER;

            // decide to use occluder (if is close enough)
            if ((dr_object_boundaries [ID].sphere / dr_object_distances [ID]) > M_DR_OCCLUDER) {

                // mark this object
                dr_object_tags [ID] = tags | M_TAG_OCCLUDERSEL;

                // add into list
                dr_list_objects1 [coccluders ++] = ID;
                continue;
            }

            // set tags
            dr_object_tags [ID] = tags;
        }

        // has occlusion model ?
        if (dr_model_occlusions [dr_object_models [ID]] != 0xffff) {

            dr_list_objects2 [cocclusions ++] = ID;
        }
    }

    // -----------------------------------
    // OCCLUDERS
    // -----------------------------------

    if (coccluders) {

        // writing depth on
        M_STATE_DEPTHMASK_SET;

        UINT_32 count = 0;
        
        UINT_32 search1 = 0;
        UINT_32 search2 = 0;

        BOOL nextma;
        BOOL nextmo;

        // time stamp
        do dr_stamp ++; while (!dr_stamp);  // zero reserved

        // pick first material
        register UINT_32 material = dr_object_materials [dr_list_objects1 [0]];  dr_material_stamps [material] = dr_stamp;

        // main loop
        while (true) {

            register UINT_32 materialn;

            // grab objects with the same material
            dr_GrabSame (materialn, material, dr_material_stamps, dr_object_materials, dr_list_objects3, dr_list_objects1, coccluders, search1, count, nextma);

            /// CONSIDER USING dr_GrabSameStop

            // ---------------------------------------------------------------------
            // SET MATERIAL STATES
            // ---------------------------------------------------------------------

            // grab material
            TMaterial * ma = &dr_materials [material];

            // init
            search2 = 0;

            // type
            switch (ma->type) {

                // ---------------------------------------------------------------------
                // SOLID
                // ---------------------------------------------------------------------
                case M_MATERIAL_TYPE_SOLID: {

                    // pick first model
                    register UINT_32 model = dr_object_models [dr_list_objects3 [0]];  dr_model_stamps [model] = dr_stamp;

                    // until all done
                    while (true) {

                        nextmo = false;

                        TModel * m = &dr_models [model];

                        register UINT_32 modeln, c;

                        // grab objects with the same model
                        dr_GrabSame (modeln, model, dr_model_stamps, dr_object_models, dr_list_objects4, dr_list_objects3, count, search2, c, nextmo);

                        // sort them by distance
                        dr_SortDistanceInsert (dr_list_objects4, c);

                        register UINT_32 model_mode  = m->mode;
                        register UINT_32 model_count = m->count;

                        switch (dr_model_shaders [model]) {

                            case M_MODEL_SHADER_NORMAL:

                                // shader
                                M_DR_SET_SHADER_CULLON (dr_program_solid_depth_normal);

                                // model
                                dr_SetModelDepthSolidNormal (m);

                                // draw
                                for (UINT_32 n = 0; n < c; n ++)    dr_DrawSolidNormal (dr_list_objects4 [n], model_mode, model_count);

                                break;

                            case M_MODEL_SHADER_MORPH:

                                // shader
                                M_DR_SET_SHADER_CULLON (dr_program_solid_depth_morph);

                                // model
                                dr_SetModelDepthSolidMorph (m);

                                // draw
                                for (UINT_32 n = 0; n < c; n ++)    dr_DrawSolidMorph (dr_list_objects4 [n], model_mode, model_count);

                                break;

                            case M_MODEL_SHADER_SHRINK:

                                // shader
                                M_DR_SET_SHADER_CULLON (dr_program_solid_depth_shrink);

                                // model
                                dr_SetModelDepthSolidNormal (m);

                                // draw
                                for (UINT_32 n = 0; n < c; n ++)    dr_DrawSolidShrink (dr_list_objects4 [n], model_mode, model_count);

                                break;
                        }

                        // finished
                        if (!nextmo) break;

                        // set new model
                        model = modeln;
                    }

                    break;
                }

                // ---------------------------------------------------------------------
                // TERRAIN
                // ---------------------------------------------------------------------
                case M_MATERIAL_TYPE_TERRAIN: {
                    
                    // pick first model
                    register UINT_32 model = dr_object_models [dr_list_objects3 [0]];  dr_model_stamps [model] = dr_stamp;

                    // until all done
                    while (true) {

                        nextmo = false;

                        TModel * m = &dr_models [model];

                        register UINT_32 modeln, c;

                        // grab objects with the same model
                        dr_GrabSame (modeln, model, dr_model_stamps, dr_object_models, dr_list_objects4, dr_list_objects3, count, search2, c, nextmo);

                        // sort them by distance
                        dr_SortDistanceInsert (dr_list_objects4, c);

                        register UINT_32 model_mode  = m->mode;
                        register UINT_32 model_count = m->count;

                        switch (dr_model_shaders [model]) {

                            case M_MODEL_SHADER_NORMAL:

                                // shader
                                M_DR_SET_SHADER_CULLON (dr_program_terrain_depth_normal);

                                // model
                                dr_SetModelDepthTerrainNormal (m);

                                for (UINT_32 n = 0; n < c; n ++)    dr_DrawTerrainNormal (dr_list_objects4 [n], model_mode, model_count);

                                break;

                            case M_MODEL_SHADER_MORPH:

                                // shader
                                M_DR_SET_SHADER_CULLON (dr_program_terrain_depth_morph);

                                // model
                                dr_SetModelDepthTerrainMorph (m);

                                for (UINT_32 n = 0; n < c; n ++)    dr_DrawTerrainMorph (dr_list_objects4 [n], model_mode, model_count);

                                break;
                        }

                        // finished
                        if (!nextmo) break;

                        // set new model
                        model = modeln;
                    }

                    break;
                }
            }

            // finished
            if (!nextma) break;

            // set new material
            material = materialn;
        }
    }

    // -----------------------------------
    // OCCLUSIONS
    // -----------------------------------

    // NOTE : we dont need to short occlusions by distance cause they are not written into depth buffer

    if (cocclusions) {

        // writing depth off
        M_STATE_DEPTHMASK_CLEAR;

        for (UINT_32 n = 0; n < cocclusions; n ++) {

            register UINT_32 ID = dr_list_objects2 [n];

            register FLOAT_32P transform = &dr_object_transforms [ID << 4];

            // matrix (3x3 + translation)
            glMultiTexCoord3fv (GL_TEXTURE4, &(transform [ 0]));
            glMultiTexCoord3fv (GL_TEXTURE5, &(transform [ 4]));
            glMultiTexCoord3fv (GL_TEXTURE6, &(transform [ 8]));
            glMultiTexCoord3fv (GL_TEXTURE7, &(transform [12]));

            // start query
            glBeginQueryARB (GL_SAMPLES_PASSED_ARB, dr_object_queries [ID]);

            // draw ..
            glCallList (dr_models [dr_model_occlusions [dr_object_models [ID]]].list);

            // end query
            glEndQueryARB (GL_SAMPLES_PASSED_ARB);
        }
    }
}

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// dr_DrawBlend
////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if 1

VOID dr_DrawBlend (
                    
    )
{
}

#endif
