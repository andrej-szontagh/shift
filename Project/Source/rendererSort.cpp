
////////////////////////////////////////////////////////////////////////////////////////////////////////////
// dr_CheckOcclusionQuery
////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if 1

INLINEF BOOL dr_CheckOcclusionQuery (UINT_32 &ID)
{
    register UINT_32 query = dr_object_queries [ID];

    // occluder has invalid query
    // we assume occluders all always visible so skip them
    if (query && (!(dr_object_tags [ID] & M_TAG_OCCLUDERSEL))) {

        GLuint avail = GL_FALSE;
        glGetQueryObjectuivARB (query, GL_QUERY_RESULT_AVAILABLE, &avail);

        if (avail == GL_TRUE) {

            GLuint samples = 0;
            glGetQueryObjectuivARB (query, GL_QUERY_RESULT_ARB, &samples);

            if (samples <= dr_detailculling) return (TRUE);

        } else dr_debug_missed ++;
    }

    return (FALSE);
}

#endif 

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// dr_Sort
////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if 1

VOID    dr_Sort ()
{
    // safe check
    if (dr_list_objects_viewc == 0) return;

    // indices
    UINT_32 countma = 0;  UINT_32 searchma = 0;
    UINT_32 countmo = 0;  UINT_32 searchmo = 0;

    UINT_32 countf = 0;

    BOOL nextma = false;
    BOOL nextmo = false;

    // debug info
    dr_debug_missed = 0;
    dr_debug_culled = 0;

    // context nodes
    TContextMaterial * conmat;
    TContextModel    * conmod;

    // preallocated model nodes
    TContextModel * pmodels = dr_context_models;    dr_context.count = 0;

    // time stamp
    do dr_stamp ++; while (!dr_stamp);  // zero reserved

    // pick first material
    register UINT_32 material = dr_object_materials [dr_list_objects_view [0]];  dr_material_stamps [material] = dr_stamp;

    // traverse thru all visible objects
    while (true) {

        register UINT_32 materialn;
        register UINT_32 countmax  = dr_material_counters [material];

        // grab objects with the same material and stop when grabbed them all
        dr_GrabSameStop (materialn, material, dr_material_stamps, dr_object_materials, dr_list_objects1, dr_list_objects_view, dr_list_objects_viewc, searchma, countmax, countma, nextma);

        // we set new material context node
        conmat = &dr_context.list [dr_context.count ++];  
        conmat->material = material; conmat->count = 0; conmat->list = pmodels;

        // pick first model
        register UINT_32 model = dr_object_models [dr_list_objects1 [0]];  dr_model_stamps [model] = dr_stamp;

        // set start
        searchmo = 0;

        // all fields in sublist of our material
        while (true) {

            register UINT_32 modeln;

            // grab objects with the same model
            dr_GrabSame (modeln, model, dr_model_stamps, dr_object_models, dr_list_objects2, dr_list_objects1, countma, searchmo, countmo, nextmo);

            // we set new model context node
            conmod = &conmat->list [conmat->count ++];
            conmod->model = model; conmod->list = &dr_list_objects3 [countf];

            // ---------------------------------------
            // SORTING BY DISTANCE
            // 
            //
            UINT_32 pcountf = countf;

            // if there is too many objects of model sublist
            if (countmo > 32) {

                // RADIX SORT

                // build temporary values array
                for (UINT_32 n = 0; n < countmo; n ++) {

                    dr_object_distances_sort [n] = (UINT_32) (dr_object_distances [dr_list_objects2 [n]] * M_INT_SCALE);
                }

                // preform radix
                UINT_32P indices = dr_RadixSort (dr_object_distances_sort, countmo);

                //// we skip per object testing occlusion testing if not applicable
                if (dr_model_flags [model] & M_FLAG_MODEL_OCCLUSION) {

                    // + QUERY TEST

                    // build final object list in sublist range
                    for (UINT_32 n = 0; n < countmo; n ++) {

                        register UINT_32 ID = dr_list_objects2 [indices [n]];

                        // check occlusion
                        if (dr_CheckOcclusionQuery (ID)) continue;

                        // add object
                        dr_list_objects3 [countf ++] = ID;
                    }

                } else {

                    // build final object list in sublist range
                    for (UINT_32 n = 0; n < countmo; n ++) {

                        // add object
                        dr_list_objects3 [countf ++] = dr_list_objects2 [indices [n]];
                    }
                }

            // small array we use simple sorting algorithm
            } else {

                // we skip per object testing occlusion testing if not applicable
                if (dr_model_flags [model] & M_FLAG_MODEL_OCCLUSION) {

                    // INSERT SORT + QUERY TEST

                    UINT_32P p = &dr_list_objects3 [countf];

                    dr_SortDistanceInsertMoveCall (dr_CheckOcclusionQuery, dr_list_objects2, p, countmo);  countf += countmo;

                } else {

                    // INSERT SORT

                    UINT_32P p = &dr_list_objects3 [countf];

                    dr_SortDistanceInsertMove (dr_list_objects2, p, countmo);  countf += countmo;
                }
            }

            // object count
            conmod->count = countf - pcountf;

            // we are done
            if (!nextmo) break;

            // set new model
            model = modeln;
        }

        // we are done
        if (!nextma) break;

        // set new material
        material = materialn;

        // now we push the data index
        // we use this to save memory avoiding to allocate model list for each 
        // material node and rather using one big preallocated list with known size assuming 
        // there is no objects having same model but different matrial. 
        // if that occur there is some more reserved space, but of course is limited (look for dr_context_models)
        pmodels += conmat->count;
    }

    dr_debug_culled = dr_list_objects_viewc - countf;
}

#endif

