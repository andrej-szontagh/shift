
struct TShaderFiles
{
	UINT_8	shader;
	CHARP	shader_vertex;
	CHARP	shader_fragment;
};

VOID dr_LoadShaderFoliage ()
{
	TShaderFiles files [6];

	files [0].shader			= M_MODEL_SHADER_GROW;
	files [0].shader_vertex		= "Shaders\\dr_foliage_gbuffers_grow.vert";
	files [0].shader_fragment	= "Shaders\\dr_foliage_gbuffers_normal.frag";

	files [1].shader			= M_MODEL_SHADER_GROW | M_SHADER_DEPTH;
	files [1].shader_vertex		= "Shaders\\dr_foliage_depth_grow.vert";
	files [1].shader_fragment	= "Shaders\\dr_foliage_depth_normal.frag";

	files [2].shader			= M_MODEL_SHADER_GROW | M_SHADER_SHADOW;
	files [2].shader_vertex		= "Shaders\\dr_foliage_depth_grow.vert";
	files [2].shader_fragment	= "Shaders\\dr_foliage_depth_shadow.frag";

	files [3].shader			= M_MODEL_SHADER_SHRINK;
	files [3].shader_vertex		= "Shaders\\dr_foliage_gbuffers_shrink.vert";
	files [3].shader_fragment	= "Shaders\\dr_foliage_gbuffers_normal.frag";

	files [4].shader			= M_MODEL_SHADER_SHRINK | M_SHADER_DEPTH;
	files [4].shader_vertex		= "Shaders\\dr_foliage_depth_shrink.vert";
	files [4].shader_fragment	= "Shaders\\dr_foliage_depth_normal.frag";

	files [5].shader			= M_MODEL_SHADER_SHRINK | M_SHADER_SHADOW;
	files [5].shader_vertex		= "Shaders\\dr_foliage_depth_shrink.vert";
	files [5].shader_fragment	= "Shaders\\dr_foliage_depth_shadow.frag";

	UINT_8	shaderid;
	UINT_32 shader_vertex;
	UINT_32 shader_fragment;
	UINT_32 program;

	char* prefix;

	for (int i = 0; i < 6; i ++) {

		for (int j = 0; j < 2; j ++) {

			switch (j) {
				case 0: shaderid = files [i].shader;							prefix = "\n";					break;
				case 1: shaderid = files [i].shader | M_MODEL_SHADER_INSTANCED;	prefix = "#define INSTANCED\n";	break;
			}

			// g-buffers
			lo_LoadShaders (files [i].shader_vertex,
							files [i].shader_fragment, prefix, 
							&program,
							&shader_vertex,
							&shader_fragment);

			glUseProgram (program);

			INT_32 u;

			u = glGetUniformLocation (program, "tex_diffuse");		if (u >= 0)	glUniform1i	(u,	0);
			u = glGetUniformLocation (program, "tex_composite");	if (u >= 0)	glUniform1i (u, 1);
			u = glGetUniformLocation (program, "uvscale");			if (u >= 0)	glUniform2f (u, 10.0f/32767.0f, 10.0f/32767.0f);

			dr_program_foliage [shaderid] = program;
		}
	}
}

VOID dr_SetMaterialFoliage (TMaterial *material)
{
    M_STATE_TEX0_SET (material->diffuse);
    M_STATE_TEX1_SET (material->composite);
    M_STATE_TEX2_CLEAR;
    M_STATE_TEX3_CLEAR;
    M_STATE_TEX4_CLEAR;
    M_STATE_TEX5_CLEAR;
    M_STATE_TEX6_CLEAR;
    M_STATE_TEX7_CLEAR;
    M_STATE_TEX8_CLEAR;
    M_STATE_TEX9_CLEAR;
	M_STATE_TEX10_CLEAR;
}

VOID dr_SetMaterialFoliageDepth (TMaterial *material)
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
}

VOID dr_SetModelFoliageNormal (TModel *model)
{
    M_STATE_ARRAY_VERTEX_SET            (model->vbo_vertices, 3, GL_FLOAT);
    M_STATE_ARRAY_NORMAL_SET            (model->vbo_normals,     GL_SHORT);                         /// NORMALS OR NOT ??
    M_STATE_ARRAY_COLOR_SET             (model->vbo_colors,   3, GL_UNSIGNED_BYTE);
    M_STATE_ARRAY_SECONDARY_COLOR_SET   (model->vbo_tangents, 3, GL_UNSIGNED_BYTE);                 /// TANGENTS OR NOT ??
    M_STATE_ARRAY_TEX0_COORDS_SET       (model->vbo_uv1, model->size_uv1, model->type_uv1);
    M_STATE_ARRAY_TEX1_COORDS_CLEAR;
    M_STATE_ARRAY_TEX2_COORDS_CLEAR;
    M_STATE_INDICES_SET                 (model->ebo)
}

VOID dr_SetModelDepthFoliageNormal (TModel *model)
{
    M_STATE_ARRAY_VERTEX_SET            (model->vbo_vertices, 3, GL_FLOAT);
    M_STATE_ARRAY_TEX0_COORDS_SET       (model->vbo_uv1, model->size_uv1, model->type_uv1);
    M_STATE_ARRAY_TEX1_COORDS_CLEAR;
    M_STATE_INDICES_SET                 (model->ebo)
}

VOID dr_PrepareFoliageGrow (UINT_32 ID) {

    // shader LOD parameters
    register FLOAT_32 factor = (dr_object_distances [ID] - dr_object_morph1 [ID]) * dr_object_morph2 [ID];  factor = MIN (1.0f, MAX (0.0f, factor));

    /// DO NOT SQUARE FACTOR, SQUARE DISTANCE !!!

    // set params
    glFogCoordf (factor);

    // shader animation
    // FLOAT_32 fswing = (0.005f * (((ID % 4) + 1) / 5.0f)) * sin (0.005f * dr_swing + ID);

    FLOAT_32 fswing = (0.005f * (1.0f / 5.0f)) * sin (0.005f * dr_swing);

    // set params
    glMultiTexCoord1f (GL_TEXTURE1, fswing);

    /// ADD MATERIAL PARAMETER !! (OR MODEL ?)

    glMultiTexCoord4f (GL_TEXTURE3, 0.0, 0.0, 2.0f, - 1.0f);
}

VOID dr_PrepareFoliageShrink (UINT_32 ID) {

    // shader LOD parameters
    register FLOAT_32 factor = (dr_object_distances [ID] - dr_object_morph1 [ID]) * dr_object_morph2 [ID];  factor = MIN (1.0f, MAX (0.0f, factor));

    /// DO NOT SQUARE FACTOR, SQUARE DISTANCE !!!

    // set params
    glFogCoordf (factor);

    /// ADD MATERIAL OR MODEL PROPERTY FOR CONTROLLING SWING SPEED !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    // set params
    glMultiTexCoord1f (GL_TEXTURE1, dr_swing);
}

VOID dr_DrawFoliage (TModel *model, UINT_32 modelid, UINT_32P list, UINT_32 count, BOOL depth) {

	UINT_8 shader = dr_model_shaders [modelid];

	if (depth) shader |= M_SHADER_DEPTH;

	// shader
	M_DR_SET_SHADER_CULLOFF (dr_program_foliage [shader]);

	if (dr_model_flags [modelid] & M_FLAG_MODEL_LIST) {

	    UINT_32 model_list = model->list;

		if (shader & M_MODEL_SHADER_INSTANCED) {

			switch (shader & 0x07) {
				case M_MODEL_SHADER_GROW:	for (UINT_32 k = 0; k < count; k ++) M_DR_DRAW_LIST_INSTANCED   (dr_PrepareFoliageGrow,     list [k], model_list);	break;
				case M_MODEL_SHADER_SHRINK:	for (UINT_32 k = 0; k < count; k ++) M_DR_DRAW_LIST_INSTANCED   (dr_PrepareFoliageShrink,   list [k], model_list);	break;
			}

		} else {

			switch (shader & 0x07) {
				case M_MODEL_SHADER_GROW:	for (UINT_32 k = 0; k < count; k ++) M_DR_DRAW_LIST_SINGLE      (dr_PrepareFoliageGrow,     list [k], model_list);	break;
				case M_MODEL_SHADER_SHRINK:	for (UINT_32 k = 0; k < count; k ++) M_DR_DRAW_LIST_SINGLE      (dr_PrepareFoliageShrink,   list [k], model_list);	break;
			}
		}

	} else {

		// model
		if (depth)	dr_SetModelDepthFoliageNormal	(model);	else
					dr_SetModelFoliageNormal		(model);

        UINT_32 model_mode  = model->mode;
        UINT_32 model_count = model->count;

		if (shader & M_MODEL_SHADER_INSTANCED) {

			switch (shader & 0x07) {
				case M_MODEL_SHADER_GROW:	for (UINT_32 k = 0; k < count; k ++) M_DR_DRAW_VBO_INSTANCED    (dr_PrepareFoliageGrow,     list [k], model_mode, model_count);	break;
				case M_MODEL_SHADER_SHRINK:	for (UINT_32 k = 0; k < count; k ++) M_DR_DRAW_VBO_INSTANCED    (dr_PrepareFoliageShrink,   list [k], model_mode, model_count);	break;
			}

		} else {

			switch (shader & 0x07) {
				case M_MODEL_SHADER_GROW:	for (UINT_32 k = 0; k < count; k ++) M_DR_DRAW_VBO_SINGLE       (dr_PrepareFoliageGrow,     list [k], model_mode, model_count);	break;
				case M_MODEL_SHADER_SHRINK:	for (UINT_32 k = 0; k < count; k ++) M_DR_DRAW_VBO_SINGLE       (dr_PrepareFoliageShrink,   list [k], model_mode, model_count);	break;
			}
		}
	}
}

VOID dr_DrawFoliageShadow (UINT_32 &propn, UINT_32 &prop, UINT_32P &listin, UINT_32 &listinc, UINT_32 &search, BOOL &next)  {

    TModel* model = &dr_models [prop];

	UINT_8 shader = dr_model_shaders [prop] | M_SHADER_SHADOW;

	// shader
	M_DR_SET_SHADER_CULLOFF (dr_program_foliage [shader]);

	if (dr_model_flags [prop] & M_FLAG_MODEL_LIST) {

	    UINT_32 model_list = model->list;

		if (shader & M_MODEL_SHADER_INSTANCED) {

			switch (shader & 0x07) {
				case M_MODEL_SHADER_GROW:	M_DR_GRAB_SAME_DRAW1 (dr_PrepareFoliageGrow,    dr_DrawListInstanced,   model_list, propn, prop, dr_model_stamps, dr_object_models, listin, listinc, search, next);  break;
				case M_MODEL_SHADER_SHRINK:	M_DR_GRAB_SAME_DRAW1 (dr_PrepareFoliageShrink,  dr_DrawListInstanced,   model_list, propn, prop, dr_model_stamps, dr_object_models, listin, listinc, search, next);  break;
			}

		} else {

			switch (shader & 0x07) {
				case M_MODEL_SHADER_GROW:	M_DR_GRAB_SAME_DRAW1 (dr_PrepareFoliageGrow,    dr_DrawListSingle,      model_list, propn, prop, dr_model_stamps, dr_object_models, listin, listinc, search, next);  break;
				case M_MODEL_SHADER_SHRINK:	M_DR_GRAB_SAME_DRAW1 (dr_PrepareFoliageShrink,  dr_DrawListSingle,      model_list, propn, prop, dr_model_stamps, dr_object_models, listin, listinc, search, next);  break;
			}
		}

	} else {

		dr_SetModelDepthFoliageNormal (model);

        UINT_32 model_mode  = model->mode;
        UINT_32 model_count = model->count;

		if (shader & M_MODEL_SHADER_INSTANCED) {

			switch (shader & 0x07) {
				case M_MODEL_SHADER_GROW:	M_DR_GRAB_SAME_DRAW2 (dr_PrepareFoliageGrow,    dr_DrawVBOInstanced,    model_mode, model_count, propn, prop, dr_model_stamps, dr_object_models, listin, listinc, search, next);  break;
				case M_MODEL_SHADER_SHRINK:	M_DR_GRAB_SAME_DRAW2 (dr_PrepareFoliageShrink,  dr_DrawVBOInstanced,    model_mode, model_count, propn, prop, dr_model_stamps, dr_object_models, listin, listinc, search, next);  break;
			}

		} else {

			switch (shader & 0x07) {
				case M_MODEL_SHADER_GROW:	M_DR_GRAB_SAME_DRAW2 (dr_PrepareFoliageGrow,    dr_DrawVBOSingle,       model_mode, model_count, propn, prop, dr_model_stamps, dr_object_models, listin, listinc, search, next);  break;
				case M_MODEL_SHADER_SHRINK:	M_DR_GRAB_SAME_DRAW2 (dr_PrepareFoliageShrink,  dr_DrawVBOSingle,       model_mode, model_count, propn, prop, dr_model_stamps, dr_object_models, listin, listinc, search, next);  break;
			}
		}
	}
}

