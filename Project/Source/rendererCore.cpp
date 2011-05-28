
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
    for (UINT_32 i = 0; i < M_DR_SUN_SPLITS; i ++)

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
    for (UINT_32 split = 0; split < dr_sun_count; split ++) {

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
    glViewport (0, 0, dr_w [dr_levelculling], dr_h [dr_levelculling]);

    // clearing depth buffer
    glScissor  (0, 0, dr_w [dr_levelculling], dr_h [dr_levelculling]);
    
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

    glViewport (0, 0, M_DR_SUN_SHADOW, M_DR_SUN_SHADOW);

    dr_SunShadows ();

    #ifdef M_DEBUG
        debug_EndTimerQuery (0, "Shadows        : ", dr_debug_profile_shadows, dr_debug_profile_shadowsg);
    #endif

    // DEBUG

    #ifdef M_DEBUG

        if ((debug >= 5) && (debug <= 9)) {

            glUseProgram  (0);

            glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, 0);

            glDrawBuffer (GL_BACK); glColorMask (GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

            glViewport (0, 0, dr_width, dr_height);

            M_STATE_CLEAR_TEX0_RECT;
            M_STATE_CLEAR_TEX1_RECT;
            M_STATE_CLEAR_TEX2_RECT;

            //  M_STATE_CLEAR_TEX0;
            M_STATE_CLEAR_TEX1;
            M_STATE_CLEAR_TEX2;
            M_STATE_CLEAR_TEX3;
            M_STATE_CLEAR_TEX4;
            M_STATE_CLEAR_TEX5;
            M_STATE_CLEAR_TEX6;
            M_STATE_CLEAR_TEX7;
            M_STATE_CLEAR_TEX8;
            M_STATE_CLEAR_TEX9;

            M_STATE_CULLFACE_CLEAR;
            M_STATE_DEPTHTEST_CLEAR;

            M_STATE_MATRIXMODE_PROJECTION;  glLoadIdentity ();
            M_STATE_MATRIXMODE_MODELVIEW;   glLoadIdentity ();

            M_STATE_TEX0_SET;

            switch (debug) {
                case 5: glBindTexture (GL_TEXTURE_2D, dr_sunshadows [0]);   break;
                case 6: glBindTexture (GL_TEXTURE_2D, dr_sunshadows [1]);   break;
                case 7: glBindTexture (GL_TEXTURE_2D, dr_sunshadows [2]);   break;
                case 8: glBindTexture (GL_TEXTURE_2D, dr_sunshadows [3]);   break;
                case 9: glBindTexture (GL_TEXTURE_2D, dr_sunshadows [4]);   break;
            }

            glTexParameteri  (GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);

            glCallList (dr_quadpot);

            glTexParameteri  (GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
            
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
    // FBO DEPTH ONLY
    //////////////////////////////////////////////

    glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, dr_framebuffer);

    glDrawBuffer (GL_NONE); glColorMask (GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

    glFramebufferTexture2DEXT (GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,  GL_TEXTURE_RECTANGLE_ARB, dr_depth,  0);
    glFramebufferTexture2DEXT (GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_RECTANGLE_ARB, 0,         0);
    glFramebufferTexture2DEXT (GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT1_EXT, GL_TEXTURE_RECTANGLE_ARB, 0,         0);

    //////////////////////////////////////////////
    // GEOMETRY PASSES
    //////////////////////////////////////////////
   
    M_STATE_MATRIXMODE_MODELVIEW;   glLoadMatrixf (dr_matrixv);
    M_STATE_MATRIXMODE_PROJECTION;  glLoadMatrixf (dr_matrixp);

    M_STATE_DEPTHTEST_SET;
    M_STATE_CULLFACE_SET;

    glViewport (0, 0, dr_width, dr_height);

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

            //  M_STATE_CLEAR_TEX0_RECT;
            M_STATE_CLEAR_TEX1_RECT;
            M_STATE_CLEAR_TEX2_RECT;

            M_STATE_CLEAR_TEX0;
            M_STATE_CLEAR_TEX1;
            M_STATE_CLEAR_TEX2;
            M_STATE_CLEAR_TEX3;
            M_STATE_CLEAR_TEX4;
            M_STATE_CLEAR_TEX5;
            M_STATE_CLEAR_TEX6;
            M_STATE_CLEAR_TEX7;
            M_STATE_CLEAR_TEX8;
            M_STATE_CLEAR_TEX9;

            M_STATE_CULLFACE_CLEAR;
            M_STATE_DEPTHTEST_CLEAR;

            M_STATE_TEX0_RECT_SET;

            M_STATE_MATRIXMODE_MODELVIEW;	glLoadIdentity ();
            M_STATE_MATRIXMODE_PROJECTION;	glLoadIdentity ();

            switch (debug) {

                case 1:     glColor4f (1.0, 1.0, 1.0, 1.0);
                    glBindTexture   (GL_TEXTURE_RECTANGLE_ARB, dr_G1);      break;
                case 2:     glColor4f (1.0, 1.0, 1.0, 1.0);
                    glBindTexture   (GL_TEXTURE_RECTANGLE_ARB, dr_G2);      break;
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

    M_STATE_DEPTHTEST_SET;  M_STATE_DEPTHMASK_CLEAR;

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

    M_STATE_DEPTHMASK_CLEAR;    glDepthFunc (GL_EQUAL);

    // all depths 1.0

    glDepthRange (1.0, 1.0);

    // culling off

    M_STATE_CULLFACE_CLEAR;

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

    M_STATE_TEX0_RECT_SET   (dr_enviroment_material->diffuse1);
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

    // draw ..

    glDrawElements (GL_TRIANGLE_STRIP, dr_enviroment_model->count, GL_UNSIGNED_SHORT, NULL);

    // restore

    M_STATE_MATRIXMODE_PROJECTION;  glPopMatrix ();

    glBindBufferARB (GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
    glBindBufferARB (GL_ARRAY_BUFFER_ARB, 0);

    glDepthRange (0.0, 1.0);

    #ifdef M_DEBUG
        debug_EndTimerQuery (0, "Enviroment     : ", dr_debug_profile_enviroment, dr_debug_profile_enviromentg);
    #endif
/*
    //////////////////////////////////////////////
    // SSAO
    //////////////////////////////////////////////

    if (dr_enablessao) {

        #ifdef M_DEBUG
            debug_StartTimer (0);
        #endif

        // SSAO

        glFramebufferTexture2DEXT (GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,  GL_TEXTURE_RECTANGLE_ARB, 0, 0);
        glFramebufferTexture2DEXT (GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_RECTANGLE_ARB, dr_auxA [dr_levelssao], 0);

        glViewport (0, 0, dr_w [dr_levelssao], dr_h [dr_levelssao]);

        glUseProgram  (dr_program_ssao);

        M_STATE_MATRIXMODE_MODELVIEW;  glLoadMatrixf (dr_matrixv);

        glActiveTexture (GL_TEXTURE0);  glEnable (GL_TEXTURE_RECTANGLE_ARB);
        glBindTexture   (GL_TEXTURE_RECTANGLE_ARB, dr_G2);

        glActiveTexture (GL_TEXTURE1);  glEnable (GL_TEXTURE_2D);
        glBindTexture   (GL_TEXTURE_2D, dr_rand);

        glActiveTexture (GL_TEXTURE2);  glEnable (GL_TEXTURE_RECTANGLE_ARB);
        glBindTexture   (GL_TEXTURE_RECTANGLE_ARB, target1);

        glCallList (dr_quads [0]);

        glActiveTexture (GL_TEXTURE2);	glDisable (GL_TEXTURE_RECTANGLE_ARB);
        glActiveTexture (GL_TEXTURE1);	glDisable (GL_TEXTURE_2D);
        glActiveTexture (GL_TEXTURE0);	glDisable (GL_TEXTURE_RECTANGLE_ARB);

        // VERTICAL BILATERAL BLUR

        glEnable  (GL_TEXTURE_RECTANGLE_ARB);

        glFramebufferTexture2DEXT (GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_RECTANGLE_ARB, dr_auxB [dr_levelssao], 0);
        glUseProgram  (dr_program_ssao_blurvert);

        glActiveTexture (GL_TEXTURE0);	glEnable  (GL_TEXTURE_RECTANGLE_ARB);
        glBindTexture	(GL_TEXTURE_RECTANGLE_ARB, dr_auxA [dr_levelssao]);
        
        glActiveTexture (GL_TEXTURE1);	glEnable  (GL_TEXTURE_RECTANGLE_ARB);
        glBindTexture	(GL_TEXTURE_RECTANGLE_ARB, dr_G2);

        glCallList (dr_quads [dr_levelssao]);

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
        
        glActiveTexture (GL_TEXTURE0);  glEnable (GL_TEXTURE_RECTANGLE_ARB);
        glBindTexture   (GL_TEXTURE_RECTANGLE_ARB, dr_auxB [dr_levelssao]);

        glActiveTexture (GL_TEXTURE1);  glEnable (GL_TEXTURE_RECTANGLE_ARB);
        glBindTexture   (GL_TEXTURE_RECTANGLE_ARB, target1);

        glActiveTexture (GL_TEXTURE2);  glEnable (GL_TEXTURE_RECTANGLE_ARB);
        glBindTexture   (GL_TEXTURE_RECTANGLE_ARB, dr_G2);

        glCallList (dr_quads [0]);

        glActiveTexture (GL_TEXTURE2);	glDisable (GL_TEXTURE_RECTANGLE_ARB);
        glActiveTexture (GL_TEXTURE1);	glDisable (GL_TEXTURE_RECTANGLE_ARB);
        glActiveTexture (GL_TEXTURE0);	glDisable (GL_TEXTURE_RECTANGLE_ARB);

        #ifdef M_DEBUG
            debug_EndTimer (0, "SSAO           : ");
        #endif

        // DEBUG

        #ifdef M_DEBUG
                  
            if ((debug >= 10) && (debug <= 11)) {

                glUseProgram  (dr_program_ssao_debug);

                glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, 0);

                glViewport (0, 0, dr_width, dr_height);

                glActiveTexture (GL_TEXTURE0); glEnable (GL_TEXTURE_RECTANGLE_ARB);

				switch (debug) {
					case 10:	glBindTexture (GL_TEXTURE_RECTANGLE_ARB, dr_auxA [dr_levelssao]);	glCallList (dr_quads [dr_levelssao]);	break;
					case 11:	glBindTexture (GL_TEXTURE_RECTANGLE_ARB, target2);	                glCallList (dr_quads [0]);	            break;
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

    if (dr_enablefog) {

        #ifdef M_DEBUG
            debug_StartTimer (0);
        #endif

        glFramebufferTexture2DEXT (GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,  GL_TEXTURE_RECTANGLE_ARB, dr_depth,    0);
        glFramebufferTexture2DEXT (GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_RECTANGLE_ARB, target2,     0);

        M_STATE_DEPTHTEST_SET;

        // early z cull

        M_STATE_DEPTHMASK_CLEAR;    glDepthFunc (GL_NOTEQUAL);

        // all depths 1.0

        glDepthRange (1.0, 1.0);

        glUseProgram  (dr_program_fog);
        
        glActiveTexture (GL_TEXTURE0);  glEnable (GL_TEXTURE_RECTANGLE_ARB);
        glBindTexture   (GL_TEXTURE_RECTANGLE_ARB, dr_G2);

        glActiveTexture (GL_TEXTURE1);  glEnable (GL_TEXTURE_RECTANGLE_ARB);
        glBindTexture   (GL_TEXTURE_RECTANGLE_ARB, target1);

        M_STATE_MATRIXMODE_MODELVIEW;   glPushMatrix (); glLoadMatrixf (dr_matrixv);

        glColor3fv (dr_fog.color);

        glCallList (dr_quads [0]);

        glColor3f (1.0, 1.0, 1.0);

        M_STATE_MATRIXMODE_MODELVIEW;   glPopMatrix (); 

        glActiveTexture (GL_TEXTURE1);	glDisable (GL_TEXTURE_RECTANGLE_ARB);
        glActiveTexture (GL_TEXTURE0);	glDisable (GL_TEXTURE_RECTANGLE_ARB);

        glDepthRange (0.0, 1.0);

        #ifdef M_DEBUG
            debug_EndTimer (0, "Fog            : ");
        #endif

        tmp = target1;
              target1 = target2;
                        target2 = tmp;
    }
*/
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
/*
    if (dr_enableaa) {

        #ifdef M_DEBUG
            debug_StartTimer (0);
        #endif

        M_STATE_DEPTHTEST_CLEAR;

        glFramebufferTexture2DEXT (GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,   GL_TEXTURE_RECTANGLE_ARB, 0,          0);
        glFramebufferTexture2DEXT (GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,  GL_TEXTURE_RECTANGLE_ARB, target2,    0);

        glActiveTexture (GL_TEXTURE0);  glEnable (GL_TEXTURE_RECTANGLE_ARB);    glBindTexture (GL_TEXTURE_RECTANGLE_ARB, target1);
        glActiveTexture (GL_TEXTURE1);  glEnable (GL_TEXTURE_RECTANGLE_ARB);    glBindTexture (GL_TEXTURE_RECTANGLE_ARB, dr_depth);

        glUseProgram  (dr_program_aa_blurhoriz);

        glCallList (dr_quads [0]);

        glFramebufferTexture2DEXT (GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,  GL_TEXTURE_RECTANGLE_ARB, target1, 0);

        glActiveTexture (GL_TEXTURE0);  glEnable (GL_TEXTURE_RECTANGLE_ARB);    glBindTexture (GL_TEXTURE_RECTANGLE_ARB, target2);
        glActiveTexture (GL_TEXTURE1);  glEnable (GL_TEXTURE_RECTANGLE_ARB);    glBindTexture (GL_TEXTURE_RECTANGLE_ARB, dr_depth);

        glUseProgram  (dr_program_aa_blurvert);

        glCallList (dr_quads [0]);

        glActiveTexture (GL_TEXTURE0);  glDisable (GL_TEXTURE_RECTANGLE_ARB);

        #ifdef M_DEBUG
            debug_EndTimer (0, "AA             : ");
        #endif
    }
*/
    //////////////////////////////////////////////
    // MATRICES FOR NON SHADER PROCESSING
    //////////////////////////////////////////////

    M_STATE_MATRIXMODE_PROJECTION;  glLoadIdentity ();
    M_STATE_MATRIXMODE_MODELVIEW;   glLoadIdentity ();

    //////////////////////////////////////////////
    // BLOOM
    //////////////////////////////////////////////
/*
    #ifdef M_DEBUG
        debug_StartTimer (0);
    #endif

    if (dr_enableautoexposure || dr_enablebloom) {

        // HI-PASS

        glFramebufferTexture2DEXT (GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,  GL_TEXTURE_RECTANGLE_ARB, 0, 0);
        glFramebufferTexture2DEXT (GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_RECTANGLE_ARB, dr_auxA [0], 0);

        glViewport (0, 0, dr_w [0], dr_h [0]);

        glUseProgram  (dr_program_hdr_hipass);

        /// REMOVE UNIFORM USE TEX COORD !!!!!!!!!!!!!!
        glUniform1f (glGetUniformLocation (dr_program_hdr_hipass, "scale"), dr_enableautoexposure ? dr_intensityscale : 1.0f);

        glActiveTexture (GL_TEXTURE0);  glEnable (GL_TEXTURE_RECTANGLE_ARB);

        glBindTexture	(GL_TEXTURE_RECTANGLE_ARB, target1);

        glCallList (dr_quads [0]);

        // DOWN SCALE

        glUseProgram (0);

        for (UINT_32 i = 1; i < M_DR_MIPLEVELS; i ++) {

            glFramebufferTexture2DEXT (GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_RECTANGLE_ARB, dr_auxA [i], 0);
            glViewport (0, 0, dr_w [i], dr_h [i]);	glBindTexture (GL_TEXTURE_RECTANGLE_ARB, dr_auxA [i - 1]);
            glCallList (dr_quads [i - 1]);
        }
    }

    if (dr_enableautoexposure) {

        // RETIEVEING DATA FOR AUTOEXPOSURE

        glBindTexture	(GL_TEXTURE_RECTANGLE_ARB, dr_auxA [M_DR_MIPLEVELS - 1]);
        glGetTexImage	(GL_TEXTURE_RECTANGLE_ARB, 0, GL_ALPHA, GL_UNSIGNED_BYTE, (VOIDP) dr_intensitydata);
    }

    if (dr_enablebloom) {

        // VERTICAL BLUR

        glUseProgram  (dr_program_hdr_blurvert);

        for (UINT_32 i = dr_levelbloom; i < M_DR_MIPLEVELS; i ++) {

            glFramebufferTexture2DEXT (GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_RECTANGLE_ARB, dr_auxB [i], 0);
            glViewport (0, 0, dr_w [i], dr_h [i]); glBindTexture (GL_TEXTURE_RECTANGLE_ARB, dr_auxA [i]);
            glCallList (dr_quads [i]);
        }

        // HORIZONTAL BLUR

        glUseProgram  (dr_program_hdr_blurhoriz);

        for (UINT_32 i = dr_levelbloom; i < M_DR_MIPLEVELS; i ++) {

            glFramebufferTexture2DEXT (GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_RECTANGLE_ARB, dr_auxA [i], 0);
            glViewport (0, 0, dr_w [i], dr_h [i]); glBindTexture (GL_TEXTURE_RECTANGLE_ARB, dr_auxB [i]);
            glCallList (dr_quads [i]);
        }

        // IMAGE RECONSTRUCTION

        glFramebufferTexture2DEXT (GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_RECTANGLE_ARB, dr_auxB [0], 0);
        glViewport (0, 0, dr_width, dr_height);

        M_STATE_CULLFACE_CLEAR;
        M_STATE_DEPTHTEST_CLEAR;

        glUseProgram  (dr_program_hdr);
        
        /// REMOVE UNIFORM USE TEX COORD !!!!!!!!!!!!!!
        glUniform1f (glGetUniformLocation (dr_program_hdr, "scale"), dr_enableautoexposure ? dr_intensityscale : 1.0f);

        glActiveTexture	(GL_TEXTURE0);  glEnable (GL_TEXTURE_RECTANGLE_ARB); 

        glBindTexture	(GL_TEXTURE_RECTANGLE_ARB, target1);

        for (UINT_32 i = dr_levelbloom; i < M_DR_MIPLEVELS; i ++) {

            glActiveTexture	(GL_TEXTURE0 + (i + 1));
            glEnable		(GL_TEXTURE_RECTANGLE_ARB);
            glBindTexture	(GL_TEXTURE_RECTANGLE_ARB, dr_auxA [i]);
        }

        glCallList (dr_quads [0]);

        for (UINT_32 i = 0; i <= M_DR_MIPLEVELS; i ++) {

            glActiveTexture (GL_TEXTURE0 + i);
            glDisable (GL_TEXTURE_RECTANGLE_ARB);
        }

        glActiveTexture (GL_TEXTURE0);
        glDisable (GL_TEXTURE_RECTANGLE_ARB);
    }

    #ifdef M_DEBUG
        debug_EndTimer (0, "Bloom          : ");
    #endif
*/
    //////////////////////////////////////////////
    // AUTO-EXPOSURE
    //////////////////////////////////////////////
/*
    if (dr_enableautoexposure) {

        #ifdef M_DEBUG
            debug_StartTimer (0);
        #endif

        if (!dr_enablebloom) {

            // IMAGE RECONSTRUCTION

            glFramebufferTexture2DEXT (GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_RECTANGLE_ARB, dr_auxB [0], 0);
            glViewport (0, 0, dr_width, dr_height);

            M_STATE_CULLFACE_CLEAR;
            M_STATE_DEPTHTEST_CLEAR;

            glUseProgram (0);

            glEnable	 (GL_TEXTURE_RECTANGLE_ARB);

            glBindTexture	(GL_TEXTURE_RECTANGLE_ARB, target1);

            glColor3f (dr_intensityscale, dr_intensityscale, dr_intensityscale);

            glCallList (dr_quads [0]);

            glColor3f (1.0f, 1.0f, 1.0f);

            glDisable (GL_TEXTURE_RECTANGLE_ARB);
        }

        FLOAT_32 intensity		= 0.0;
        FLOAT_32 intensitymin	= 1.0;
        FLOAT_32 intensitymax	= 0.0;

        for (UINT_32 i = 0; i < dr_intensitysize; i ++)	{

            if (dr_intensitydata [i] < intensitymin) intensitymin = dr_intensitydata [i];
            if (dr_intensitydata [i] > intensitymax) intensitymax = dr_intensitydata [i];

            intensity +=  dr_intensitydata [i];

        }	intensity /= (dr_intensitysize * 255);

        intensitymin /= 255;
        intensitymax /= 255;

        // TONE MAPPING

        dr_intensityscale = dr_intensityscale + (M_DR_HDR_EXPOSURE - intensity) * MIN (M_DR_HDR_EXPOSURE_SPEED * delta, 1.0f);

        #ifdef M_DEBUG
            debug_EndTimer (0, "Autoexposure   : ");
        #endif
    }
*/
    //////////////////////////////////////////////
    // BACK FRAMEBUFFER
    //////////////////////////////////////////////

    glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, 0);

    glDrawBuffer (GL_BACK); glColorMask (GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

    //////////////////////////////////////////////
    // FINAL IMAGE
    //////////////////////////////////////////////

    glUseProgram  (0);

    M_STATE_DEPTHTEST_CLEAR;
    M_STATE_CULLFACE_CLEAR;

    glViewport (0, 0, dr_width, dr_height);

    //  M_STATE_CLEAR_TEX0_RECT;
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

    if (dr_enableautoexposure || dr_enablebloom) {

        M_STATE_TEX0_RECT_SET (dr_auxB [0]);    } else {
        M_STATE_TEX0_RECT_SET (target1);
    }

    glCallList (dr_quads [0]);

    //////////////////////////////////////////////
    // GUI
    //////////////////////////////////////////////

    // DEBUG INFO

    #ifdef M_DEBUG

        FLOAT_64 elapsed;

        debug_EndTimer (255, "FRAME END ------------------------------------ : ", elapsed);
    #endif

    #ifdef M_DEBUG

        M_STATE_CLEAR_TEX0_RECT;
        M_STATE_CLEAR_TEX1_RECT;
        M_STATE_CLEAR_TEX2_RECT;

        CHAR text [2048];
        
        sprintf (text, "DEBUG : \n\n");

        sprintf (text, "%s - view    : %i \n",   text, dr_list_objects_viewc);
        sprintf (text, "%s - shadow1 : %i \n",   text, dr_list_objects_shadow_splitc [0]);
        sprintf (text, "%s - shadow2 : %i \n",   text, dr_list_objects_shadow_splitc [1]);
        sprintf (text, "%s - shadow3 : %i \n",   text, dr_list_objects_shadow_splitc [2]);
        sprintf (text, "%s - shadow4 : %i \n",   text, dr_list_objects_shadow_splitc [3]);
        sprintf (text, "%s - shadow5 : %i \n\n", text, dr_list_objects_shadow_splitc [4]);

        sprintf (text, "%s - nodes   : %i \n",   text, dr_nodesc);
        sprintf (text, "%s - chains  : %i \n\n", text, dr_chainsc);

        sprintf (text, "%s - culled  : %i \n",   text, dr_debug_culled);
        sprintf (text, "%s - missed  : %i \n\n", text, dr_debug_missed);

        sprintf (text, "%s - profiler | Sorting       : %Lf ms / %Lf ms \n", text, dr_debug_profile_sort,       dr_debug_profile_sortg);
        sprintf (text, "%s - profiler | Clipping      : %Lf ms / %Lf ms \n", text, dr_debug_profile_clip,       dr_debug_profile_clipg);
        sprintf (text, "%s - profiler | Occlusion     : %Lf ms / %Lf ms \n", text, dr_debug_profile_occlusion,  dr_debug_profile_occlusiong);
        sprintf (text, "%s - profiler | Details       : %Lf ms / %Lf ms \n", text, dr_debug_profile_detail,     dr_debug_profile_detailg);
        sprintf (text, "%s - profiler | Shadows       : %Lf ms / %Lf ms \n", text, dr_debug_profile_shadows,    dr_debug_profile_shadowsg);
        sprintf (text, "%s - profiler | G-Buffers     : %Lf ms / %Lf ms \n", text, dr_debug_profile_gbuffers,   dr_debug_profile_gbuffersg);
        sprintf (text, "%s - profiler | Depth         : %Lf ms / %Lf ms \n", text, dr_debug_profile_depth,      dr_debug_profile_depthg);
        sprintf (text, "%s - profiler | Sun           : %Lf ms / %Lf ms \n", text, dr_debug_profile_sun,        dr_debug_profile_sung);
        sprintf (text, "%s - profiler | Enviroment    : %Lf ms / %Lf ms \n", text, dr_debug_profile_enviroment, dr_debug_profile_enviromentg);

        dr_DrawText (- 0.95f, 0.9f, 0.03f, 0.05f, dr_text, true, text);

    #endif

    // frame advance
    dr_frame ++;
}

#endif
