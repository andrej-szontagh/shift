
////////////////////////////////////////////////////////////////////////////////////////////////////////////
// dr_Matrices
////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if 1

INLINE VOID dr_Matrices ()
{

    // VIEW MATRIX
   
    dr_matrixv [0] = dr_cambi [0]; dr_matrixv [1] = dr_camup [0]; dr_matrixv [ 2] = -dr_camdir [0]; dr_matrixv [ 3] = 0.0;
    dr_matrixv [4] = dr_cambi [1]; dr_matrixv [5] = dr_camup [1]; dr_matrixv [ 6] = -dr_camdir [1]; dr_matrixv [ 7] = 0.0;
    dr_matrixv [8] = dr_cambi [2]; dr_matrixv [9] = dr_camup [2]; dr_matrixv [10] = -dr_camdir [2]; dr_matrixv [11] = 0.0;

    FLOAT_32 point [3];

    vINVERT3 (dr_campos); vTRANSFORM_COL4x4v3 (point, dr_campos, dr_matrixv); vINVERT3 (dr_campos);

    dr_matrixv [12] = point [0];
    dr_matrixv [13] = point [1]; 
    dr_matrixv [14] = point [2];
    dr_matrixv [15] = 1.0;

    // PROJECTION MATRIX

    FLOAT_32 depth = dr_planefar - dr_planenear;

    dr_matrixp [ 0] = dr_planenear / (FLOAT_32) dr_nearw;
    dr_matrixp [ 1] = 0;
    dr_matrixp [ 2] = 0;
    dr_matrixp [ 3] = 0;

    dr_matrixp [ 4] = 0;
    dr_matrixp [ 5] = dr_planenear / (FLOAT_32) dr_nearh;
    dr_matrixp [ 6] = 0;
    dr_matrixp [ 7] = 0;

    dr_matrixp [ 8] = 0;
    dr_matrixp [ 9] = 0;
    dr_matrixp [10] = - (dr_planefar + dr_planenear) / depth;
    dr_matrixp [11] = -1;

    dr_matrixp [12] = 0;
    dr_matrixp [13] = 0;
    dr_matrixp [14] = - (dr_planefar * dr_planenear * 2) / depth;
    dr_matrixp [15] = 0;
}

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// dr_Render
////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if 1

VOID    dr_Render (FLOAT_32 delta)
{

    // timing ..
    dr_delta  = delta;
    dr_swing += delta;

    GLuint target1 = dr_auxhdr1;
    GLuint target2 = dr_auxhdr2;

    #ifdef M_DEBUG
        debug_StartTimer (255);
    #endif

    //////////////////////////////////////////////
    // CLEAR TAGS
    //////////////////////////////////////////////

    memset (dr_object_tags, 0, SIZE_UINT_16 * (dr_objectsc + M_RESERVED_OBJECTS));

    //////////////////////////////////////////////
    // UPDATE MATRICES
    //////////////////////////////////////////////

    dr_Matrices ();

    M_STATE_MATRIXMODE_MODELVIEW;   glLoadMatrixf (dr_matrixv);
    M_STATE_MATRIXMODE_PROJECTION;  glLoadMatrixf (dr_matrixp);

    //////////////////////////////////////////////
    // UPDATING CLIPPING FRUSTUM
    //////////////////////////////////////////////

    dr_ClipUpdate ();

    //////////////////////////////////////////////
    // UPDATING SUN PARAMETERS AND CLIPPING PLANES
    //////////////////////////////////////////////

    dr_SunPrepare ();

    FLOAT_32 dir [4] = { (FLOAT_32) -dr_sun_directionz [0], 
                         (FLOAT_32) -dr_sun_directionz [1], 
                         (FLOAT_32) -dr_sun_directionz [2], 0.0 };

    glLightfv (GL_LIGHT0, GL_DIFFUSE,  dr_sun_color);
    glLightfv (GL_LIGHT0, GL_POSITION, dir);

    //////////////////////////////////////////////
    // CLIPPING
    //////////////////////////////////////////////

    #ifdef M_DEBUG
        debug_StartTimerQuery (0);
    #endif

    // init counters
    for (UINT_32 i = 0; i < dr_control_sun_splits; i ++)

            dr_list_objects_shadow_splitc [i] = 0;

    // init counters
    dr_list_objects_viewc = 0;

    // clip all tree nodes
    dr_ClipCombinedRecursive (&dr_tree.root, M_CLIP_COLLIDING, M_CLIP_COLLIDING);

    #ifdef M_DEBUG
        debug_EndTimerQuery (0, "Clipper        : ", dr_debug_profile_clip, dr_debug_profile_clipg);
    #endif

    //////////////////////////////////////////////
    // LOD AND DISTANCE
    //////////////////////////////////////////////

    #ifdef M_DEBUG
        debug_StartTimerQuery (0);
    #endif

    // time stamp
    do dr_stamp ++; while (!dr_stamp);  // zero reserved

    // view objects
    for (UINT_32 i = 0; i < dr_list_objects_viewc; i ++) {

        // shortcut
        register UINT_32 ID = dr_list_objects_view [i];

        // we calculated linear distances for occluders
        if (!(dr_object_tags [ID] &  M_TAG_DISTANCE)) {
              dr_object_tags [ID] |= M_TAG_DISTANCE;

            /// CALCULATE LINEAR DISTANCE ONLY FOR OBJECTS THAT USE IT

            // set detail and distance
            dr_SetDetail    (ID, dr_object_distances [ID] = sqrt (dr_object_distancesq [ID]));
        }
    }

    // shadow objects
    for (UINT_32 split = 0; split < dr_control_sun_splits; split ++) {

        register UINT_32P listp = dr_list_objects_shadow_split  [split];
        register UINT_32  listc = dr_list_objects_shadow_splitc [split];

        for (UINT_32 i = 0; i < listc; i ++) {

            // shortcut
            register UINT_32 ID = listp [i];

            // we calculated linear distances for occluders
            if (!(dr_object_tags [ID] &  M_TAG_DISTANCE)) {
                  dr_object_tags [ID] |= M_TAG_DISTANCE;

                /// CALCULATE LINEAR DISTANCE ONLY FOR OBJECTS THAT USE IT

                // set detail and distance
                dr_SetDetail    (ID, dr_object_distances [ID] = sqrt (dr_object_distancesq [ID]));
            }
        }
    }

    #ifdef M_DEBUG
        debug_EndTimerQuery (0, "Detail         : ", dr_debug_profile_detail, dr_debug_profile_detailg);
    #endif

    //////////////////////////////////////////////
    // FBO DEPTH ONLY
    //////////////////////////////////////////////

    glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, 0);

    glDrawBuffer (GL_NONE); glColorMask (GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

    //////////////////////////////////////////////
    // OCCLUSION CULLING
    //////////////////////////////////////////////

    #ifdef M_DEBUG
        debug_StartTimerQuery (0);
    #endif

    // using only part
    glViewport (0, 0, dr_w [dr_control_culling], dr_h [dr_control_culling]);

    // clearing depth buffer
    glScissor  (0, 0, dr_w [dr_control_culling], dr_h [dr_control_culling]);
    
    // we enable writing before clean up
    M_STATE_DEPTHMASK_SET

    // clear only part of buffer
    glEnable  (GL_SCISSOR_TEST);    glClear (GL_DEPTH_BUFFER_BIT);
    glDisable (GL_SCISSOR_TEST);

    // draw
    dr_DrawOcclusions ();
    
    #ifdef M_DEBUG
        debug_EndTimerQuery (0, "Occlusion Cull.: ", dr_debug_profile_occlusion, dr_debug_profile_occlusiong);
    #endif

    //////////////////////////////////////////////
    // SET MATERIAL COUNTERS FOR VIEW
    //////////////////////////////////////////////

    // init counters
    memset ((VOIDP) dr_material_counters,  0, SIZE_UINT_32 * dr_materialsc);

    // we save counts of objects per material to stop searching earlyier
    for (UINT_32 i = 0; i < dr_list_objects_viewc; i ++) {

        dr_material_counters [dr_object_materials [dr_list_objects_view [i]]] ++;
    }

    //////////////////////////////////////////////
    // FBO DEPTH ONLY
    //////////////////////////////////////////////

    glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, dr_framebuffer_sun);

    glDrawBuffer (GL_NONE); glColorMask (GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

    //////////////////////////////////////////////
    // SUN SHADOWMAPS
    //////////////////////////////////////////////

    #ifdef M_DEBUG
        debug_StartTimerQuery (0);
    #endif

    glViewport (0, 0, dr_control_sun_res, dr_control_sun_res);

    dr_SunShadows ();

    #ifdef M_DEBUG
        debug_EndTimerQuery (0, "Shadows        : ", dr_debug_profile_shadows, dr_debug_profile_shadowsg);
    #endif

    // DEBUG

    #ifdef M_DEBUG

        if ((debug >= 5) && (debug <= 9)) {

            glUseProgram  (dr_program_debug_shadow);

            glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, 0);

            glDrawBuffer (GL_BACK); glColorMask (GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

            glViewport (0, 0, dr_width, dr_height);

            M_STATE_TEX0_RECT_CLEAR;
            M_STATE_TEX1_RECT_CLEAR;
            M_STATE_TEX2_RECT_CLEAR;

            //  M_STATE_TEX0_CLEAR;
			M_STATE_TEX1_CLEAR;
			M_STATE_TEX2_CLEAR;
			M_STATE_TEX3_CLEAR;
			M_STATE_TEX4_CLEAR;
			M_STATE_TEX5_CLEAR;
			M_STATE_TEX6_CLEAR;
			M_STATE_TEX7_CLEAR;
			M_STATE_TEX8_CLEAR;
			M_STATE_TEX9_CLEAR;
            M_STATE_TEX10_CLEAR;

            M_STATE_CULLFACE_CLEAR;
            M_STATE_DEPTHTEST_CLEAR;

            M_STATE_MATRIXMODE_PROJECTION;  glLoadIdentity ();
            M_STATE_MATRIXMODE_MODELVIEW;   glLoadIdentity ();

            glActiveTexture (GL_TEXTURE0);

            switch (debug) {
                case 5: M_STATE_TEX0_SET (dr_sunshadows [0]);   glBindTexture (GL_TEXTURE_2D, dr_sunshadows [0]);   break;
                case 6: M_STATE_TEX0_SET (dr_sunshadows [1]);   glBindTexture (GL_TEXTURE_2D, dr_sunshadows [1]);   break;
                case 7: M_STATE_TEX0_SET (dr_sunshadows [2]);   glBindTexture (GL_TEXTURE_2D, dr_sunshadows [2]);   break;
                case 8: M_STATE_TEX0_SET (dr_sunshadows [3]);   glBindTexture (GL_TEXTURE_2D, dr_sunshadows [3]);   break;
                case 9: M_STATE_TEX0_SET (dr_sunshadows [4]);   glBindTexture (GL_TEXTURE_2D, dr_sunshadows [4]);   break;
            }

            glTexParameteri  (GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);

            glCallList (dr_quadpot);

            glTexParameteri  (GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

            glUseProgram  (0);
            
            return;
        }

    #endif

    //////////////////////////////////////////////
    // SORTING AND CULLING
    //////////////////////////////////////////////

    #ifdef M_DEBUG
        debug_StartTimerQuery (0);
    #endif

    dr_Sort ();

    #ifdef M_DEBUG
        debug_EndTimerQuery (0, "Sorting        : ", dr_debug_profile_sort, dr_debug_profile_sortg);
    #endif

    //////////////////////////////////////////////
    // GEOMETRY PASSES
    //////////////////////////////////////////////
   
    M_STATE_MATRIXMODE_MODELVIEW;   glLoadMatrixf (dr_matrixv);
    M_STATE_MATRIXMODE_PROJECTION;  glLoadMatrixf (dr_matrixp);

    M_STATE_DEPTHTEST_SET;
    M_STATE_CULLFACE_SET;

    glViewport (0, 0, dr_width, dr_height);

    //////////////////////////////////////////////
    // FBO DEPTH ONLY
    //////////////////////////////////////////////

    glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, dr_framebuffer);

    glDrawBuffer (GL_NONE); glColorMask (GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

    glFramebufferTexture2DEXT (GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,  GL_TEXTURE_RECTANGLE_ARB, dr_depth,  0);
    glFramebufferTexture2DEXT (GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_RECTANGLE_ARB, 0,         0);
    glFramebufferTexture2DEXT (GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT1_EXT, GL_TEXTURE_RECTANGLE_ARB, 0,         0);

    //////////////////////////////////////////////
    // DEPTH PASS
    //////////////////////////////////////////////

    #ifdef M_DEBUG
        debug_StartTimerQuery (0);
    #endif

    // depth mode
    M_STATE_DEPTHMASK_SET;  glDepthFunc (GL_LESS);

    // clear
    glClear (GL_DEPTH_BUFFER_BIT);

    /// WE CAN USE QUADRATIC DISTANCES TILL THIS POINT !!!!!!!!!!!!!!!!!!!!!!!!!1   
        /// CALCULATE SQRT ONLY FOR VISIBLE OBJECTS

    // draw
    dr_DrawDepth ();

    #ifdef M_DEBUG
        debug_EndTimerQuery (0, "DepthPass      : ", dr_debug_profile_depth, dr_debug_profile_depthg);
    #endif

    //////////////////////////////////////////////
    // RENDERING COLOR BACK
    //////////////////////////////////////////////

    glDrawBuffer (GL_BACK); glColorMask (GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

    //////////////////////////////////////////////
    // G-BUFFERS PASS
    //////////////////////////////////////////////

    #ifdef M_DEBUG
        debug_StartTimerQuery (0);
    #endif

    glFramebufferTexture2DEXT (GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,  GL_TEXTURE_RECTANGLE_ARB, dr_G1, 0);
    glFramebufferTexture2DEXT (GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT1_EXT,  GL_TEXTURE_RECTANGLE_ARB, dr_G2, 0);

    // render targets
    GLenum buffers [2] = {  GL_COLOR_ATTACHMENT0_EXT, GL_COLOR_ATTACHMENT1_EXT };   glDrawBuffers  (2, buffers);

    // early-z culling
    M_STATE_DEPTHMASK_CLEAR;    glDepthFunc (GL_EQUAL);

    //  glEnable  (GL_POLYGON_OFFSET_FILL); glPolygonOffset (-1.0f, -1.0f);

    dr_Draw ();

    // BACK

    //  glDisable (GL_POLYGON_OFFSET_FILL);

    M_STATE_DEPTHMASK_SET;      glDepthFunc (GL_LEQUAL);

    glDrawBuffers (1, buffers);

    #ifdef M_DEBUG
        debug_EndTimerQuery (0, "GBuffers       : ", dr_debug_profile_gbuffers, dr_debug_profile_gbuffersg);
    #endif

    // DEBUG

    #ifdef M_DEBUG

        if ((debug >= 1) && (debug <= 2)) {

            glUseProgram  (0);

            glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, 0);

            glViewport (0, 0, dr_width, dr_height);

            //  M_STATE_TEX0_RECT_CLEAR;
            M_STATE_TEX1_RECT_CLEAR;
            M_STATE_TEX2_RECT_CLEAR;

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
			M_STATE_TEX10_CLEAR;

            M_STATE_CULLFACE_CLEAR;
            M_STATE_DEPTHTEST_CLEAR;

            M_STATE_MATRIXMODE_MODELVIEW;	glLoadIdentity ();
            M_STATE_MATRIXMODE_PROJECTION;	glLoadIdentity ();

            switch (debug) {

                case 1:     glColor4f (1.0, 1.0, 1.0, 1.0);
                    M_STATE_TEX0_RECT_SET (dr_G1);      break;
                case 2:     glColor4f (1.0, 1.0, 1.0, 1.0);
                    M_STATE_TEX0_RECT_SET (dr_G2);      break;
            }

            glCallList (dr_quads [0]);
            
            return;
        }

    #endif

    //////////////////////////////////////////////
    // SUN
    //////////////////////////////////////////////

    #ifdef M_DEBUG
        debug_StartTimerQuery (0);
    #endif

    // states

    glFramebufferTexture2DEXT (GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,  GL_TEXTURE_RECTANGLE_ARB, target1,    0);
    glFramebufferTexture2DEXT (GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT1_EXT,  GL_TEXTURE_RECTANGLE_ARB, 0,          0);

    // early z cull

    M_STATE_CULLFACE_CLEAR;
    M_STATE_DEPTHMASK_CLEAR;    M_STATE_DEPTHTEST_SET;

    glDepthRange (1.0, 1.0);    glDepthFunc (GL_GREATER);

    // draw

    dr_SunDraw ();

    #ifdef M_DEBUG
        debug_EndTimerQuery (0, "Sun            : ", dr_debug_profile_sun, dr_debug_profile_sung);
    #endif

    //////////////////////////////////////////////
    // ENVIROMENT OBJECT
    //////////////////////////////////////////////

    #ifdef M_DEBUG
        debug_StartTimerQuery (0);
    #endif

    // early z cull

    M_STATE_CULLFACE_CLEAR;
    M_STATE_DEPTHMASK_CLEAR;    M_STATE_DEPTHTEST_SET;

    glDepthRange (1.0, 1.0);    glDepthFunc (GL_EQUAL);

    // setup buffers

    M_STATE_ARRAY_COLOR_CLEAR;
    M_STATE_ARRAY_SECONDARY_COLOR_CLEAR;
    M_STATE_ARRAY_NORMAL_CLEAR;
    M_STATE_ARRAY_VERTEX_SET            (dr_enviroment_model->vbo_vertices, 3, GL_FLOAT);
    M_STATE_ARRAY_TEX0_COORDS_SET       (dr_enviroment_model->vbo_uv1,      2, GL_SHORT);
    M_STATE_ARRAY_TEX1_COORDS_CLEAR;
    M_STATE_ARRAY_TEX2_COORDS_CLEAR;
    M_STATE_INDICES_SET                 (dr_enviroment_model->ebo);

    // texture

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
    M_STATE_TEX10_CLEAR;

    M_STATE_TEX0_RECT_SET   (dr_enviroment_material->diffuse);
    M_STATE_TEX1_RECT_CLEAR;
    M_STATE_TEX2_RECT_CLEAR;

    // special projection

    M_STATE_MATRIXMODE_PROJECTION;  glPushMatrix ();

    glLoadIdentity ();  gluPerspective  (dr_fovy, dr_aspect, dr_planenear, dr_planefar);

    gluLookAt (	dr_origin [0], 
                dr_origin [1], 
                dr_origin [2], 
                dr_origin [0] + dr_camdir [0], 
                dr_origin [1] + dr_camdir [1], 
                dr_origin [2] + dr_camdir [2], 
                dr_worldup [0], 
                dr_worldup [1], 
                dr_worldup [2]);

    // program

    glUseProgram (dr_program_enviroment);

	// image adjustment parameters

	glMultiTexCoord3f (GL_TEXTURE1, dr_control_sky_desaturation, dr_control_sky_brightness, dr_control_sky_contrast);

    // draw ..

    glDrawElements (GL_TRIANGLE_STRIP, dr_enviroment_model->count, GL_UNSIGNED_SHORT, NULL);

    // restore

    M_STATE_MATRIXMODE_PROJECTION;  glPopMatrix ();

    glBindBufferARB (GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
    glBindBufferARB (GL_ARRAY_BUFFER_ARB, 0);

    glDepthRange (0.0, 1.0);    glDepthFunc (GL_LESS);

    #ifdef M_DEBUG
        debug_EndTimerQuery (0, "Enviroment     : ", dr_debug_profile_enviroment, dr_debug_profile_enviromentg);
    #endif

    //////////////////////////////////////////////
    // SSAO
    //////////////////////////////////////////////

    if (dr_control_ssao_enable) {

        #ifdef M_DEBUG
			debug_StartTimerQuery (0);
        #endif

        // early z cull

        M_STATE_CULLFACE_CLEAR;
        M_STATE_DEPTHMASK_CLEAR;    M_STATE_DEPTHTEST_SET;

        glDepthRange (1.0, 1.0);    glDepthFunc (GL_GREATER);

        // SSAO

        glFramebufferTexture2DEXT (GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_RECTANGLE_ARB, dr_auxA [dr_control_ssao_res], 0);

        glViewport (0, 0, dr_w [dr_control_ssao_res], dr_h [dr_control_ssao_res]);

        // clear

        glClear (GL_COLOR_BUFFER_BIT);  glClearColor (1.0, 1.0, 1.0, 1.0);

        // program

        glUseProgram  (dr_program_ssao);

        // textures

        M_STATE_TEX1_SET (dr_rand);

        M_STATE_TEX0_RECT_SET   (dr_G2);
        M_STATE_TEX1_RECT_CLEAR;
        M_STATE_TEX2_RECT_SET   (target1);

        glCallList (dr_quads [0]);

        // depth test off

        M_STATE_DEPTHTEST_CLEAR;

        glDepthRange (0.0, 1.0);    glDepthFunc (GL_LESS);

        // VERTICAL BILATERAL BLUR

        glFramebufferTexture2DEXT (GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_RECTANGLE_ARB, dr_auxB [dr_control_ssao_res], 0);
        glUseProgram  (dr_program_ssao_blurvert);

        M_STATE_TEX1_CLEAR;

        M_STATE_TEX0_RECT_SET   (dr_auxA [dr_control_ssao_res]);
        M_STATE_TEX1_RECT_SET   (dr_G2);
        M_STATE_TEX2_RECT_CLEAR;

        glCallList (dr_quads [dr_control_ssao_res]);

        // DEBUG

        #ifdef M_DEBUG
                  
            if ((debug >= 11) && (debug <= 11)) {

                glFramebufferTexture2DEXT (GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_RECTANGLE_ARB, target1, 0);
                glViewport (0, 0, dr_width, dr_height);

                glClearColor (1.0, 1.0, 1.0, 1.0);  glClear (GL_COLOR_BUFFER_BIT);
                glClearColor (0.0, 0.0, 0.0, 0.0);
            }

        #endif

        // HORIZONTAL BILATERAL BLUR & BLEND

        glFramebufferTexture2DEXT (GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_RECTANGLE_ARB, target2, 0);
        glUseProgram  (dr_program_ssao_blurhblend);

        glViewport (0, 0, dr_width, dr_height);

        M_STATE_TEX0_RECT_SET   (dr_auxB [dr_control_ssao_res]);
        M_STATE_TEX1_RECT_SET   (target1);
        M_STATE_TEX2_RECT_SET   (dr_G2);

        glCallList (dr_quads [0]);

        #ifdef M_DEBUG
	        debug_EndTimerQuery (0, "SSAO           : ", dr_debug_profile_ssao, dr_debug_profile_ssaog);
        #endif

        // DEBUG

        #ifdef M_DEBUG
                  
            if ((debug >= 10) && (debug <= 11)) {

                glUseProgram  (dr_program_ssao_debug);

                glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, 0);

                glViewport (0, 0, dr_width, dr_height);

                glActiveTexture (GL_TEXTURE0); glEnable (GL_TEXTURE_RECTANGLE_ARB);

				switch (debug) {
					case 10:	glBindTexture (GL_TEXTURE_RECTANGLE_ARB, dr_auxA [dr_control_ssao_res]);	glCallList (dr_quads [dr_control_ssao_res]);	break;
					case 11:	glBindTexture (GL_TEXTURE_RECTANGLE_ARB, target2);	                        glCallList (dr_quads [0]);	                    break;
				}

                glDisable (GL_TEXTURE_RECTANGLE_ARB);

                M_STATE_DEPTHMASK_SET;

                M_STATE_MATRIXMODE_PROJECTION;	glPopMatrix ();
                M_STATE_MATRIXMODE_MODELVIEW;	glPopMatrix (); 
                
                return;
            }

        #endif

        GLuint tmp;

        tmp = target1;
              target1 = target2;
                        target2 = tmp;
    }

    //////////////////////////////////////////////
    // FOG
    //////////////////////////////////////////////

    if (dr_control_fog) {

        #ifdef M_DEBUG
			debug_StartTimerQuery (0);
        #endif

        glFramebufferTexture2DEXT (GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,  GL_TEXTURE_RECTANGLE_ARB, dr_depth,    0);
        glFramebufferTexture2DEXT (GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_RECTANGLE_ARB, target1,     0);

        M_STATE_DEPTHTEST_SET;

        // early z cull

        M_STATE_DEPTHMASK_CLEAR;    glDepthFunc (GL_NOTEQUAL);

        // all depths 1.0

        glDepthRange (1.0, 1.0);

        glUseProgram (dr_program_fog);
        
        // textures

        M_STATE_TEX0_RECT_CLEAR;
        M_STATE_TEX1_RECT_CLEAR;
        M_STATE_TEX2_RECT_SET   (dr_G2);

        glColor3fv (dr_fog_color);

		glEnable (GL_BLEND);	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glCallList (dr_quads [0]);

		glDisable (GL_BLEND);

		// restore

        glColor3f (1.0, 1.0, 1.0);

        glDepthRange (0.0, 1.0);

        #ifdef M_DEBUG
            debug_EndTimerQuery (0, "Fog            : ", dr_debug_profile_fog, dr_debug_profile_fogg);
        #endif
    }

    //////////////////////////////////////////////
    // BLENDED OBJECTS
    //////////////////////////////////////////////
/*
    #ifdef M_DEBUG
        debug_StartTimer (0);
    #endif

    // states

    glFramebufferTexture2DEXT (GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,  GL_TEXTURE_RECTANGLE_ARB, dr_depth, 0);

    M_STATE_DEPTHTEST_SET;

    // early z cull

    M_STATE_DEPTHMASK_CLEAR; glDepthFunc (GL_LESS);

    M_STATE_MATRIXMODE_PROJECTION;  glPushMatrix ();    glLoadMatrixf (dr_matrixp);
    M_STATE_MATRIXMODE_MODELVIEW;   glPushMatrix ();    glLoadMatrixf (dr_matrixv);

    // shadow matrix

    glActiveTexture  (GL_TEXTURE0);
    M_STATE_MATRIXMODE_TEXTURE;     glPushMatrix ();    glLoadMatrixd (&dr_sun.matrices [0]);

    // lighting params

    glMultiTexCoord3f (GL_TEXTURE1, dr_sun.intensity, dr_sun.ambient, (FLOAT_32) dr_sun.offsets  [0]);

    // fog params

    glSecondaryColor3fv (dr_fog.color);

    // blending on
    glEnable (GL_BLEND);

    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    dr_DrawBegin ();    dr_DrawBlend ();    dr_DrawEnd ();

    glActiveTexture (GL_TEXTURE0);

    M_STATE_MATRIXMODE_TEXTURE;      glPopMatrix ();
    M_STATE_MATRIXMODE_MODELVIEW;    glPopMatrix ();
    M_STATE_MATRIXMODE_PROJECTION;   glPopMatrix ();

    #ifdef M_DEBUG
        debug_EndTimer (0, "Blended        : ");
    #endif
*/
    //////////////////////////////////////////////
    // AA
    //////////////////////////////////////////////

    if (dr_control_aa) {

        #ifdef M_DEBUG
			debug_StartTimerQuery (0);
        #endif

        M_STATE_DEPTHTEST_CLEAR;

        glFramebufferTexture2DEXT (GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,   GL_TEXTURE_RECTANGLE_ARB, 0,          0);
        glFramebufferTexture2DEXT (GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,  GL_TEXTURE_RECTANGLE_ARB, target2,    0);

        // textures

        M_STATE_TEX0_RECT_SET   (target1);
        M_STATE_TEX1_RECT_SET   (dr_depth);
        M_STATE_TEX2_RECT_CLEAR;

        glUseProgram  (dr_program_aa_blurhoriz);

        glCallList (dr_quads [0]);

        glFramebufferTexture2DEXT (GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,  GL_TEXTURE_RECTANGLE_ARB, target1,	0);

        glUseProgram  (dr_program_aa_blurvert);

        glCallList (dr_quads [0]);

        #ifdef M_DEBUG
            debug_EndTimerQuery (0, "AA             : ", dr_debug_profile_aa, dr_debug_profile_aag);
        #endif
    }

    //////////////////////////////////////////////
    // MATRICES FOR FIXED PIPELINE PROCESSING
    //////////////////////////////////////////////

    M_STATE_MATRIXMODE_PROJECTION;  glLoadIdentity ();
    M_STATE_MATRIXMODE_MODELVIEW;   glLoadIdentity ();

    //////////////////////////////////////////////
    // TONEMAPPING
    //////////////////////////////////////////////

    #ifdef M_DEBUG
		debug_StartTimerQuery (0);
    #endif

	FLOAT_32 newscale = 1.0;

	if (dr_control_autoexposure || dr_control_bloom_enable) {

		// HI-PASS

		glFramebufferTexture2DEXT (GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,  GL_TEXTURE_RECTANGLE_ARB, 0, 0);
		glFramebufferTexture2DEXT (GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_RECTANGLE_ARB, dr_auxA [0], 0);

        M_STATE_DEPTHTEST_CLEAR;
		M_STATE_CULLFACE_CLEAR;

		glViewport (0, 0, dr_w [0], dr_h [0]);

		glUseProgram  (dr_program_hdr_hipass);

		glFogCoordf (dr_intensityscale);

		M_STATE_TEX0_RECT_SET (target1);
        M_STATE_TEX1_RECT_CLEAR;
        M_STATE_TEX2_RECT_CLEAR;

		glCallList (dr_quads [0]);

		// DOWN SCALE

		glUseProgram (0);

		for (UINT_32 i = 1; i < dr_control_mip; i ++) {

			glFramebufferTexture2DEXT (GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_RECTANGLE_ARB, dr_auxA [i], 0);
			glViewport (0, 0, dr_w [i], dr_h [i]);	M_STATE_TEX0_RECT_SET (dr_auxA [i - 1]);
			glCallList (dr_quads [i - 1]);
		}

		if (dr_control_autoexposure) {

			/// REMOVE THIS !! .. USE BLENDING 

			// RETIEVEING DATA FOR AUTOEXPOSURE

            M_STATE_TEX0_RECT_SET (dr_auxA [dr_control_mip - 1]);
			glGetTexImage	(GL_TEXTURE_RECTANGLE_ARB, 0, GL_ALPHA, GL_UNSIGNED_BYTE, (VOIDP) dr_intensitydata);

			// EVALUATING INTENSITY

			FLOAT_32 intensity = 0.0;

			for (UINT_32 i = 0; i < dr_intensitysize; i ++)	{

				intensity +=  dr_intensitydata [i];

			}	intensity /= (dr_intensitysize * 255);

			// TONE MAPPING FACTOR

			newscale = dr_intensityscale + (dr_control_hdr_exposure - intensity) * MIN (dr_control_hdr_exposure_speed * delta, 1.0f);

			newscale = MIN (MAX (newscale, dr_control_hdr_exposure_scale_min), dr_control_hdr_exposure_scale_max);

		}
	}

	if (dr_control_bloom_enable) {

        M_STATE_TEX1_RECT_CLEAR;
        M_STATE_TEX2_RECT_CLEAR;

        UINT_32 targetset = 0;

		// VERTICAL BLUR

        if (dr_control_bloom_enable_vblur) {

		    glUseProgram  (dr_program_hdr_blurvert);

		    for (UINT_32 i = dr_control_bloom_res; i < dr_control_mip; i ++) {

			    glFramebufferTexture2DEXT (GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_RECTANGLE_ARB, dr_auxB [i], 0);
			    glViewport (0, 0, dr_w [i], dr_h [i]); M_STATE_TEX0_RECT_SET (dr_auxA [i]);
			    glCallList (dr_quads [i]);                  
		    }

            targetset = 1;
        }

		// HORIZONTAL BLUR

        if (dr_control_bloom_enable_hblur) {

            if (dr_control_bloom_enable_vblur) {

		        glUseProgram  (dr_program_hdr_blurhoriz);

		        for (UINT_32 i = dr_control_bloom_res; i < dr_control_mip; i ++) {

			        glFramebufferTexture2DEXT (GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_RECTANGLE_ARB, dr_auxA [i], 0);
			        glViewport (0, 0, dr_w [i], dr_h [i]); M_STATE_TEX0_RECT_SET (dr_auxB [i]);
			        glCallList (dr_quads [i]);
		        }

                targetset = 0;

            } else {

		        glUseProgram  (dr_program_hdr_blurhoriz);

		        for (UINT_32 i = dr_control_bloom_res; i < dr_control_mip; i ++) {

			        glFramebufferTexture2DEXT (GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_RECTANGLE_ARB, dr_auxB [i], 0);
			        glViewport (0, 0, dr_w [i], dr_h [i]); M_STATE_TEX0_RECT_SET (dr_auxA [i]);
			        glCallList (dr_quads [i]);
		        }

                targetset = 1;
            }
        }

		// IMAGE RECONSTRUCTION

		glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, 0);

		glDrawBuffer (GL_BACK); glColorMask (GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

		glViewport (0, 0, dr_width, dr_height);

		glUseProgram  (dr_program_hdr);		glFogCoordf (dr_control_bloom_strength);

		M_STATE_TEX0_RECT_SET (target1);

        if (targetset == 0) {

		    for (UINT_32 i = dr_control_bloom_res; i < dr_control_mip; i ++) {

			    glActiveTexture	(GL_TEXTURE1 + i);
			    glEnable		(GL_TEXTURE_RECTANGLE_ARB);
			    glBindTexture	(GL_TEXTURE_RECTANGLE_ARB, dr_auxA [i]);
		    }

        } else {

		    for (UINT_32 i = dr_control_bloom_res; i < dr_control_mip; i ++) {

			    glActiveTexture	(GL_TEXTURE1 + i);
			    glEnable		(GL_TEXTURE_RECTANGLE_ARB);
			    glBindTexture	(GL_TEXTURE_RECTANGLE_ARB, dr_auxB [i]);
		    }
        }

		if (dr_control_gammacorrection && GLEE_EXT_texture_sRGB && GLEE_EXT_framebuffer_sRGB) {

			glEnable  (GL_FRAMEBUFFER_SRGB_EXT);	glCallList (dr_quads [0]);
			glDisable (GL_FRAMEBUFFER_SRGB_EXT);

		} else {

			glCallList (dr_quads [0]);
		}

		for (UINT_32 i = dr_control_bloom_res; i < dr_control_mip; i ++) {

			glActiveTexture (GL_TEXTURE1 + i);
			glDisable (GL_TEXTURE_RECTANGLE_ARB);
		}

	} else {

		// IMAGE RECONSTRUCTION

		glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, 0);

		glDrawBuffer (GL_BACK); glColorMask (GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

		glViewport (0, 0, dr_width, dr_height);

		glUseProgram (0);

		M_STATE_TEX0_RECT_SET (target1);
        M_STATE_TEX1_RECT_CLEAR;
        M_STATE_TEX2_RECT_CLEAR;

		glColor3f (dr_intensityscale, dr_intensityscale, dr_intensityscale);

		if (dr_control_gammacorrection && GLEE_EXT_texture_sRGB && GLEE_EXT_framebuffer_sRGB) {

			glEnable  (GL_FRAMEBUFFER_SRGB_EXT);	glCallList (dr_quads [0]);
			glDisable (GL_FRAMEBUFFER_SRGB_EXT);

		} else {

			glCallList (dr_quads [0]);
		}

		glColor3f (1.0f, 1.0f, 1.0f);
	}

	dr_intensityscale = newscale;

    #ifdef M_DEBUG
        debug_EndTimerQuery (0, "Tonemapping    : ", dr_debug_profile_tonemap, dr_debug_profile_tonemapg);
    #endif

    //////////////////////////////////////////////
    // GUI
    //////////////////////////////////////////////

    // DEBUG INFO

    #ifdef M_DEBUG

        FLOAT_64 elapsed;

        debug_EndTimer (255, "FRAME END ------------------------------------ : ", elapsed);

        M_STATE_TEX0_RECT_CLEAR;
        M_STATE_TEX1_RECT_CLEAR;
        M_STATE_TEX2_RECT_CLEAR;

        CHAR text [4096];
        
        sprintf (text, "DEBUG : \n\n");

        sprintf (text, "%s - view      : %i \n",   text, dr_list_objects_viewc);

        for (UINT_32 n = 0; n < dr_control_sun_splits; n ++) {

            sprintf (text, "%s - shadow%i   : %i \n",   text, n + 1, dr_list_objects_shadow_splitc [n]);
        }
        
        sprintf (text, "%s\n", text);

        sprintf (text, "%s - nodes     : %i \n",   text, dr_nodesc);
        sprintf (text, "%s - chains    : %i \n\n", text, dr_chainsc);

        sprintf (text, "%s - culled    : %i \n",   text, dr_debug_culled);
        sprintf (text, "%s - missed    : %i \n\n", text, dr_debug_missed);

        sprintf (text, "%s - PROFILER  : Sorting        : %Lf ms / %Lf ms \n", text, dr_debug_profile_sort,       dr_debug_profile_sortg);
        sprintf (text, "%s - PROFILER  : Clipping       : %Lf ms / %Lf ms \n", text, dr_debug_profile_clip,       dr_debug_profile_clipg);
        sprintf (text, "%s - PROFILER  : Occlusion      : %Lf ms / %Lf ms \n", text, dr_debug_profile_occlusion,  dr_debug_profile_occlusiong);
        sprintf (text, "%s - PROFILER  : Details        : %Lf ms / %Lf ms \n", text, dr_debug_profile_detail,     dr_debug_profile_detailg);
        sprintf (text, "%s - PROFILER  : Shadows        : %Lf ms / %Lf ms \n", text, dr_debug_profile_shadows,    dr_debug_profile_shadowsg);
        sprintf (text, "%s - PROFILER  : G-Buffers      : %Lf ms / %Lf ms \n", text, dr_debug_profile_gbuffers,   dr_debug_profile_gbuffersg);
        sprintf (text, "%s - PROFILER  : Depth          : %Lf ms / %Lf ms \n", text, dr_debug_profile_depth,      dr_debug_profile_depthg);
        sprintf (text, "%s - PROFILER  : Sun            : %Lf ms / %Lf ms \n", text, dr_debug_profile_sun,        dr_debug_profile_sung);
        sprintf (text, "%s - PROFILER  : Enviroment     : %Lf ms / %Lf ms \n", text, dr_debug_profile_enviroment, dr_debug_profile_enviromentg);

        sprintf (text, "%s\n\n", text);

        sprintf (text, "%s - dr_control_mip                     : %i\n", text, dr_control_mip);
        sprintf (text, "%s - dr_control_anisotrophy             : %i\n", text, dr_control_anisotrophy);
        sprintf (text, "%s - dr_control_gammacorrection         : %i\n", text, dr_control_gammacorrection);
        sprintf (text, "%s - dr_control_autoexposure            : %i\n", text, dr_control_autoexposure);
        sprintf (text, "%s - dr_control_fog                     : %i\n", text, dr_control_fog);
        sprintf (text, "%s - dr_control_aa                      : %i\n", text, dr_control_aa);

        sprintf (text, "%s\n", text);

        sprintf (text, "%s - dr_control_image_desaturation      : %f\n", text, dr_control_image_desaturation);
        sprintf (text, "%s - dr_control_image_brightness        : %f\n", text, dr_control_image_brightness);
        sprintf (text, "%s - dr_control_image_contrast          : %f\n", text, dr_control_image_contrast);

        sprintf (text, "%s\n", text);

        sprintf (text, "%s - dr_control_sky_desaturation        : %f\n", text, dr_control_sky_desaturation);
        sprintf (text, "%s - dr_control_sky_brightness          : %f\n", text, dr_control_sky_brightness);
        sprintf (text, "%s - dr_control_sky_contrast            : %f\n", text, dr_control_sky_contrast);

        sprintf (text, "%s\n", text);

        sprintf (text, "%s - dr_control_hdr_exposure            : %f\n", text, dr_control_hdr_exposure);
        sprintf (text, "%s - dr_control_hdr_exposure_scale_min  : %f\n", text, dr_control_hdr_exposure_scale_min);
        sprintf (text, "%s - dr_control_hdr_exposure_scale_max  : %f\n", text, dr_control_hdr_exposure_scale_max);
        sprintf (text, "%s - dr_control_hdr_exposure_speed      : %f\n", text, dr_control_hdr_exposure_speed);

        sprintf (text, "%s\n", text);

        sprintf (text, "%s - dr_control_sun_res                 : %i\n", text, dr_control_sun_res);
        sprintf (text, "%s - dr_control_sun_splits              : %i\n", text, dr_control_sun_splits);
        sprintf (text, "%s - dr_control_sun_offset              : %f\n", text, dr_control_sun_offset);
        sprintf (text, "%s - dr_control_sun_transition          : %f\n", text, dr_control_sun_transition);
        sprintf (text, "%s - dr_control_sun_distribution        : %f\n", text, dr_control_sun_distribution);
        sprintf (text, "%s - dr_control_sun_scheme              : %f\n", text, dr_control_sun_scheme);
        sprintf (text, "%s - dr_control_sun_zoom                : %f\n", text, dr_control_sun_zoom);
        sprintf (text, "%s - dr_control_sun_debug               : %i\n", text, dr_control_sun_debug);

        sprintf (text, "%s\n", text);

        sprintf (text, "%s - dr_control_bloom_enable            : %i\n", text, dr_control_bloom_enable);
        sprintf (text, "%s - dr_control_bloom_enable_vblur      : %i\n", text, dr_control_bloom_enable_vblur);
        sprintf (text, "%s - dr_control_bloom_enable_hblur      : %i\n", text, dr_control_bloom_enable_hblur);
        sprintf (text, "%s - dr_control_bloom_res               : %i\n", text, dr_control_bloom_res);
        sprintf (text, "%s - dr_control_bloom_strength          : %f\n", text, dr_control_bloom_strength);

        sprintf (text, "%s\n", text);

        sprintf (text, "%s - dr_control_ssao_enable             : %i\n", text, dr_control_ssao_enable);
        sprintf (text, "%s - dr_control_ssao_res                : %i\n", text, dr_control_ssao_res);

        dr_DrawText (- 0.95f, 0.8f, 0.015f, 0.025f, dr_text, true, text);

    #endif

    // frame advance
    dr_frame ++;
}

#endif
