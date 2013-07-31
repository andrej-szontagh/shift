
VOID dr_LoadShaderTerrain ()
{
    /// SHARE FRAGMENT PROGRAM

    // NORMAL

    // g-buffers
    lo_LoadShaders ("Shaders\\dr_terrain_gbuffers_normal.vert",
                    "Shaders\\dr_terrain_gbuffers_normal.frag", "\n", 
                    &dr_program_terrain_gbuffers_normal,
                    &dr_program_terrain_gbuffers_normalv,
                    &dr_program_terrain_gbuffers_normalf);

    glUseProgram    (dr_program_terrain_gbuffers_normal);
    glUniform1i     (glGetUniformLocation (dr_program_terrain_gbuffers_normal, "tex_diffuse"),     0);
    glUniform1i     (glGetUniformLocation (dr_program_terrain_gbuffers_normal, "tex_composite"),   1);
    glUniform1i     (glGetUniformLocation (dr_program_terrain_gbuffers_normal, "tex_weights1"),    2);
    glUniform1i     (glGetUniformLocation (dr_program_terrain_gbuffers_normal, "tex_weights2"),    3);
    glUniform1i     (glGetUniformLocation (dr_program_terrain_gbuffers_normal, "tex_weights3"),    4);
    glUniform1i     (glGetUniformLocation (dr_program_terrain_gbuffers_normal, "tex_weights4"),    5);
    glUniform2f     (glGetUniformLocation (dr_program_terrain_gbuffers_normal, "uv1scale"),        1.0f,           1.0f);
    glUniform2f     (glGetUniformLocation (dr_program_terrain_gbuffers_normal, "uv2scale"),        10.0f/32767.0f, 10.0f/32767.0f);
    glUniform1f     (glGetUniformLocation (dr_program_terrain_gbuffers_normal, "planefar"),        dr_planefar);

    // depth
    lo_LoadShaders ("Shaders\\dr_terrain_depth_normal.vert",
                    "Shaders\\dr_terrain_depth_normal.frag", "\n", 
                    &dr_program_terrain_depth_normal,
                    &dr_program_terrain_depth_normalv,
                    &dr_program_terrain_depth_normalf);

    // MORPH

    // g-buffers
    lo_LoadShaders ("Shaders\\dr_terrain_gbuffers_morph.vert",
                    "Shaders\\dr_terrain_gbuffers_normal.frag", "\n", 
                    &dr_program_terrain_gbuffers_morph,
                    &dr_program_terrain_gbuffers_morphv,
                    &dr_program_terrain_gbuffers_morphf);

    glUseProgram    (dr_program_terrain_gbuffers_morph);
    glUniform1i     (glGetUniformLocation (dr_program_terrain_gbuffers_morph, "tex_diffuse"),     0);
    glUniform1i     (glGetUniformLocation (dr_program_terrain_gbuffers_morph, "tex_composite"),   1);
    glUniform1i     (glGetUniformLocation (dr_program_terrain_gbuffers_morph, "tex_weights1"),    2);
    glUniform1i     (glGetUniformLocation (dr_program_terrain_gbuffers_morph, "tex_weights2"),    3);
    glUniform1i     (glGetUniformLocation (dr_program_terrain_gbuffers_morph, "tex_weights3"),    4);
    glUniform1i     (glGetUniformLocation (dr_program_terrain_gbuffers_morph, "tex_weights4"),    5);
    glUniform2f     (glGetUniformLocation (dr_program_terrain_gbuffers_morph, "uv1scale"),        1.0f,           1.0f);
    glUniform2f     (glGetUniformLocation (dr_program_terrain_gbuffers_morph, "uv2scale"),        10.0f/32767.0f, 10.0f/32767.0f);
    glUniform1f     (glGetUniformLocation (dr_program_terrain_gbuffers_morph, "planefar"),        dr_planefar);

    dr_program_terrain_gbuffers_morph_gloss     = glGetUniformLocation (dr_program_terrain_gbuffers_morph, "gloss");
    dr_program_terrain_gbuffers_morph_shininess = glGetUniformLocation (dr_program_terrain_gbuffers_morph, "shininess");

    // depth
    lo_LoadShaders ("Shaders\\dr_terrain_depth_morph.vert",
                    "Shaders\\dr_terrain_depth_normal.frag", "\n", 
                    &dr_program_terrain_depth_morph,
                    &dr_program_terrain_depth_morphv,
                    &dr_program_terrain_depth_morphf);
}

VOID dr_SetMaterialTerrain (TMaterial *material)
{
    M_STATE_TEX0_SET (material->diffuse);
    M_STATE_TEX1_SET (material->composite);
    M_STATE_TEX2_SET (material->weights1);
    M_STATE_TEX3_SET (material->weights2);
    M_STATE_TEX4_SET (material->weights3);
    M_STATE_TEX5_SET (material->weights4);
}

VOID dr_SetMaterialTerrainDepth (TMaterial *material)
{
    M_STATE_TEX0_CLEAR;
    M_STATE_TEX1_CLEAR;
    M_STATE_TEX2_CLEAR;
    M_STATE_TEX3_CLEAR;
    M_STATE_TEX4_CLEAR;
    M_STATE_TEX5_CLEAR;
}

VOID dr_SetModelTerrainNormal (TModel *model)
{
    M_STATE_ARRAY_COLOR_CLEAR;
    M_STATE_ARRAY_VERTEX_SET            (model->vbo_vertices, 3,   GL_FLOAT);
    M_STATE_ARRAY_NORMAL_SET            (model->vbo_normals,       GL_SHORT);
    M_STATE_ARRAY_SECONDARY_COLOR_SET   (model->vbo_tangents, 3,   GL_UNSIGNED_BYTE);
    M_STATE_ARRAY_TEX0_COORDS_SET       (model->vbo_uv1, model->size_uv1, model->type_uv1);
    M_STATE_ARRAY_TEX1_COORDS_SET       (model->vbo_uv2, model->size_uv2, model->type_uv2);
    M_STATE_ARRAY_TEX2_COORDS_CLEAR;
    M_STATE_INDICES_SET                 (model->ebo)
}

VOID dr_SetModelTerrainMorph (TModel *model)
{
    M_STATE_ARRAY_COLOR_CLEAR;
    M_STATE_ARRAY_VERTEX_SET            (model->vbo_vertices, 3,   GL_FLOAT);
    M_STATE_ARRAY_NORMAL_SET            (model->vbo_normals,       GL_SHORT);
    M_STATE_ARRAY_SECONDARY_COLOR_SET   (model->vbo_tangents, 3,   GL_UNSIGNED_BYTE);
    M_STATE_ARRAY_TEX0_COORDS_SET       (model->vbo_uv1, model->size_uv1, model->type_uv1);
    M_STATE_ARRAY_TEX1_COORDS_SET       (model->vbo_uv2, model->size_uv2, model->type_uv2);
    M_STATE_ARRAY_TEX2_COORDS_SET       (model->vbo_uv3, model->size_uv3, model->type_uv3);
    M_STATE_INDICES_SET                 (model->ebo)

    // model specific shader attributes
    glMultiTexCoord3f   (GL_TEXTURE3, model->param1, model->param2, model->param3);
}

VOID dr_SetModelDepthTerrainNormal (TModel *model)
{
    M_STATE_ARRAY_VERTEX_SET            (model->vbo_vertices, 3, GL_FLOAT);
    M_STATE_ARRAY_TEX0_COORDS_CLEAR;
    M_STATE_ARRAY_TEX1_COORDS_CLEAR;
    M_STATE_INDICES_SET                 (model->ebo)
}

VOID dr_SetModelDepthTerrainMorph (TModel *model)
{
    M_STATE_ARRAY_VERTEX_SET            (model->vbo_vertices, 3, GL_FLOAT);
    M_STATE_ARRAY_TEX0_COORDS_CLEAR;
    M_STATE_ARRAY_TEX1_COORDS_SET       (model->vbo_uv3, model->size_uv3, model->type_uv3);
    M_STATE_INDICES_SET                 (model->ebo)

    // model specific shader attributes
    glMultiTexCoord3f   (GL_TEXTURE2, model->param1, model->param2, model->param3);
}

VOID dr_DrawTerrainNormal (

            UINT_32 &ID,
            UINT_32 &model_mode,
            UINT_32 &model_count
    )
{
    register FLOAT_32P transform = &dr_object_transforms [ID << 4];

    // matrix (3x3 + translation)
    glMultiTexCoord3fv (GL_TEXTURE4, &(transform [ 0]));
    glMultiTexCoord3fv (GL_TEXTURE5, &(transform [ 4]));
    glMultiTexCoord3fv (GL_TEXTURE6, &(transform [ 8]));
    glMultiTexCoord3fv (GL_TEXTURE7, &(transform [12]));

    // draw ..
    glDrawElements (model_mode, model_count, GL_UNSIGNED_SHORT, NULL);
}

VOID dr_DrawTerrainMorph (

            UINT_32 &ID,
            UINT_32 &model_mode,
            UINT_32 &model_count
    )
{
    // shader LOD parameters
    register FLOAT_32 factor = (dr_object_distances [ID] - dr_object_morph1 [ID]) * dr_object_morph2 [ID];  factor = MIN (1.0f, MAX (0.0f, factor));

    /// DO NOT SQUARE FACTOR, SQUARE DISTANCE !!!

    // set params
    glFogCoordf (factor * factor);

    // draw
    dr_DrawTerrainNormal (ID, model_mode, model_count);
}
