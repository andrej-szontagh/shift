
VOID dr_LoadShaderGrass ()
{
    // g-buffers
    lo_LoadShaders ("Shaders\\dr_grass_gbuffers_normal.vert",
                    "Shaders\\dr_grass_gbuffers_normal.frag", "\n", 
                    &dr_program_grass_gbuffers_normal,
                    &dr_program_grass_gbuffers_normalv,
                    &dr_program_grass_gbuffers_normalf);

    glUseProgram    (dr_program_grass_gbuffers_normal);
    glUniform1i     (glGetUniformLocation (dr_program_grass_gbuffers_normal, "tex_diffuse"),       0);
    glUniform2f     (glGetUniformLocation (dr_program_grass_gbuffers_normal, "uvscale"),           10.0f/32767.0f, 10.0f/32767.0f);
    glUniform1f     (glGetUniformLocation (dr_program_grass_gbuffers_normal, "planefar"),          dr_planefar);

    // depth
    lo_LoadShaders ("Shaders\\dr_grass_depth_normal.vert",
                    "Shaders\\dr_grass_depth_normal.frag", "\n", 
                    &dr_program_grass_depth_normal,
                    &dr_program_grass_depth_normalv,
                    &dr_program_grass_depth_normalf);

    glUseProgram    (dr_program_grass_depth_normal);
    glUniform1i     (glGetUniformLocation (dr_program_grass_depth_normal, "tex_diffuse"),          0);
    glUniform2f     (glGetUniformLocation (dr_program_grass_depth_normal, "uvscale"),              10.0f/32767.0f, 10.0f/32767.0f);
}

VOID dr_SetMaterialGrass (TMaterial *material)
{
    M_STATE_TEX0_SET (material->diffuse);
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

    // set params
    glMultiTexCoord2f (GL_TEXTURE3, material->damping, material->threshold);
}

VOID dr_SetMaterialGrassDepth (TMaterial *material)
{
    M_STATE_TEX0_SET (material->diffuse);
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

    // set params
    glMultiTexCoord2f (GL_TEXTURE3, material->damping, material->threshold);
}

VOID dr_SetModelGrassNormal (TModel *model)
{
    M_STATE_ARRAY_COLOR_CLEAR;
    M_STATE_ARRAY_SECONDARY_COLOR_CLEAR;
    M_STATE_ARRAY_NORMAL_CLEAR;
    M_STATE_ARRAY_VERTEX_SET            (model->vbo_vertices, 3, GL_FLOAT);
    M_STATE_ARRAY_TEX0_COORDS_SET       (model->vbo_uv1, model->size_uv1, model->type_uv1);
    M_STATE_ARRAY_TEX1_COORDS_CLEAR;
    M_STATE_ARRAY_TEX2_COORDS_CLEAR;
    M_STATE_INDICES_SET                 (model->ebo)
}

VOID dr_SetModelDepthGrassNormal (TModel *model)
{
    M_STATE_ARRAY_VERTEX_SET            (model->vbo_vertices, 3, GL_FLOAT);
    M_STATE_ARRAY_TEX0_COORDS_SET       (model->vbo_uv1, model->size_uv1, model->type_uv1);
    M_STATE_ARRAY_TEX1_COORDS_CLEAR
    M_STATE_INDICES_SET                 (model->ebo)
}

VOID dr_DrawGrassNormal (

            UINT_32     &ID,
            UINT_32     &list
    )
{

    // shader LOD parameters
    register FLOAT_32 factor = (dr_object_distances [ID] - dr_object_morph1 [ID]) * dr_object_morph2 [ID];  factor = MIN (1.0f, MAX (0.0f, factor));

    /// DO NOT SQUARE FACTOR, SQUARE DISTANCE !!!

	/// IF THIS IS LAST LOD USE EXPONENTIAL FALLOFF

    // set params
    glFogCoordf (factor);

    // shader animation
    FLOAT_32 fswing = 0.03f * sin (0.005f * dr_swing + ID);

    // set params
    glMultiTexCoord2f (GL_TEXTURE2, fswing, 0.0f);

    // shortcut
    register FLOAT_32P transform = dr_object_instances_transforms [ID];

	UINT_32 c = dr_object_instances [ID];

	///
	UINT_32 distances [65535];

    for (UINT_32 i = 0; i < c; i ++) {

		FLOAT_32 v [3] = {	transform [0] - dr_campos [0], 
							transform [1] - dr_campos [1],
							transform [2] - dr_campos [2]	};	transform += 7;

		// quadratic distance
		distances [i] = (UINT_32) (100.0 * vDOT3 (v, v));
	}

	// radix sorting
    UINT_32P indices = so_RadixSort (&dr_radix_instanced, distances, c);

	// shortcut
	transform = dr_object_instances_transforms [ID];

    // instances loop
    for (UINT_32 i = 0; i < c; i ++) {

		UINT_32 offset = indices [i] * 7;

        /// ADD CLIPPING !!!

        // transform ..
        glMultiTexCoord3fv (GL_TEXTURE4, &(transform [offset    ]));
        glMultiTexCoord4fv (GL_TEXTURE5, &(transform [offset + 3]));

        // draw ..
        glCallList (list);
    }
/*
    // instances loop
    for (INT_32 i = dr_object_instances [ID] - 1; i >= 0; i --) {

        // transform ..
        glMultiTexCoord3fv (GL_TEXTURE4, &(transform [ 0]));
        glMultiTexCoord4fv (GL_TEXTURE5, &(transform [ 3]));   transform += 7;

        // draw ..
        glCallList (list);
    }
*/
}

VOID dr_DrawGrassShadow (

            UINT_32     &ID,
            UINT_32     &list
    )
{

    // shader LOD parameters
    register FLOAT_32 factor = (dr_object_distances [ID] - dr_object_morph1 [ID]) * dr_object_morph2 [ID];  factor = MIN (1.0f, MAX (0.0f, factor));

    /// DO NOT SQUARE FACTOR, SQUARE DISTANCE !!!

	/// IF THIS IS LAST LOD USE EXPONENTIAL FALLOFF

	/// WE USE THE LOWEST LOD !!
	//register TDetail * detail = &dr_object_details [ID];

	//FLOAT_32 maxdist = 0.0f;

	//if (detail->lod3 != 0xffff) { list = dr_models [detail->lod3].list; maxdist = detail->lod3_distance;	} else {
	//if (detail->lod2 != 0xffff) { list = dr_models [detail->lod2].list; maxdist = detail->lod2_distance;	} else {
	//if (detail->lod1 != 0xffff) { list = dr_models [detail->lod1].list; maxdist = detail->lod1_distance;	}}}

	//if (dr_object_distances [ID] < maxdist) factor = 0.0f;
	///

    // set params
    glFogCoordf (factor);

    // shader animation
    FLOAT_32 fswing = 0.03f * sin (0.005f * dr_swing + ID);

    /// ADD MATERIAL OR MODEL PROPERTY FOR CONTROLLING SWING SPEED !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    // set params
    glMultiTexCoord2f (GL_TEXTURE2, fswing, 0.25f);

    // shortcut
    register FLOAT_32P transform = dr_object_instances_transforms [ID];

    // instances loop
    for (INT_32 i = dr_object_instances [ID] - 1; i >= 0; i --) {

        /// ADD MATERIAL OR MODEL PROPERTY FOR CONTROLLING EXCLUDING FROM SHADOW RENDERING BY INSTANCE SCALE !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

        /// ADD CLIPPING !!!

		if (transform [6] > 6.0) {

			// transform ..
			glMultiTexCoord3fv (GL_TEXTURE4, &(transform [ 0]));
			glMultiTexCoord4fv (GL_TEXTURE5, &(transform [ 3]));

			// draw ..
			glCallList (list);
		}

        transform += 7;
    }
}
