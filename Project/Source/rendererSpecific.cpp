
////////////////////////////////////////////////////////////////////////////////////////////////////////////
// STATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////////////////////

// --------------------------------------------------------------------------------
// SOLID
// --------------------------------------------------------------------------------

INLINEF VOID dr_LoadShaderSolid ()
{
    /// SHARE FRAGMENT PROGRAM

    // NORMAL

    // g-buffers
    lo_LoadShaders ("Shaders\\dr_solid_gbuffers_normal.vert",
                    "Shaders\\dr_solid_gbuffers_normal.frag", "\n", 
                    &dr_program_solid_gbuffers_normal,
                    &dr_program_solid_gbuffers_normalv,
                    &dr_program_solid_gbuffers_normalf);

    glUseProgram    (dr_program_solid_gbuffers_normal);
    glUniform1i     (glGetUniformLocation (dr_program_solid_gbuffers_normal, "tex_normal"),     0);
    glUniform1i     (glGetUniformLocation (dr_program_solid_gbuffers_normal, "tex_diffuse"),    1);
    glUniform2f     (glGetUniformLocation (dr_program_solid_gbuffers_normal, "uvscale"),        10.0f/32767.0f, 10.0f/32767.0f);
    glUniform1f     (glGetUniformLocation (dr_program_solid_gbuffers_normal, "planefar"),       dr_planefar);

    // depth
    lo_LoadShaders ("Shaders\\dr_solid_depth_normal.vert",
                    "Shaders\\dr_solid_depth_normal.frag", "\n", 
                    &dr_program_solid_depth_normal,
                    &dr_program_solid_depth_normalv,
                    &dr_program_solid_depth_normalf);

    // MORPH

    // g-buffers
    lo_LoadShaders ("Shaders\\dr_solid_gbuffers_morph.vert",
                    "Shaders\\dr_solid_gbuffers_normal.frag", "\n", 
                    &dr_program_solid_gbuffers_morph,
                    &dr_program_solid_gbuffers_morphv,
                    &dr_program_solid_gbuffers_morphf);

    glUseProgram    (dr_program_solid_gbuffers_morph);
    glUniform1i     (glGetUniformLocation (dr_program_solid_gbuffers_morph, "tex_normal"),      0);
    glUniform1i     (glGetUniformLocation (dr_program_solid_gbuffers_morph, "tex_diffuse"),     1);
    glUniform2f     (glGetUniformLocation (dr_program_solid_gbuffers_morph, "uvscale"),         10.0f/32767.0f, 10.0f/32767.0f);
    glUniform1f     (glGetUniformLocation (dr_program_solid_gbuffers_morph, "planefar"),        dr_planefar);

    // depth
    lo_LoadShaders ("Shaders\\dr_solid_depth_morph.vert",
                    "Shaders\\dr_solid_depth_normal.frag", "\n", 
                    &dr_program_solid_depth_morph,
                    &dr_program_solid_depth_morphv,
                    &dr_program_solid_depth_morphf);

    // SHRINK

    // g-buffers
    lo_LoadShaders ("Shaders\\dr_solid_gbuffers_shrink.vert",
                    "Shaders\\dr_solid_gbuffers_normal.frag", "\n", 
                    &dr_program_solid_gbuffers_shrink,
                    &dr_program_solid_gbuffers_shrinkv,
                    &dr_program_solid_gbuffers_shrinkf);

    glUseProgram    (dr_program_solid_gbuffers_shrink);
    glUniform1i     (glGetUniformLocation (dr_program_solid_gbuffers_shrink, "tex_normal"),     0);
    glUniform1i     (glGetUniformLocation (dr_program_solid_gbuffers_shrink, "tex_diffuse"),    1);
    glUniform2f     (glGetUniformLocation (dr_program_solid_gbuffers_shrink, "uvscale"),        10.0f/32767.0f, 10.0f/32767.0f);
    glUniform1f     (glGetUniformLocation (dr_program_solid_gbuffers_shrink, "planefar"),       dr_planefar);

    // depth
    lo_LoadShaders ("Shaders\\dr_solid_depth_shrink.vert",
                    "Shaders\\dr_solid_depth_normal.frag", "\n", 
                    &dr_program_solid_depth_shrink,
                    &dr_program_solid_depth_shrinkv,
                    &dr_program_solid_depth_shrinkf);
}

INLINEF VOID dr_SetMaterialSolid (TMaterial *material)
{
    M_STATE_TEX0_SET (material->normal1);
    M_STATE_TEX1_SET (material->diffuse1);
    M_STATE_TEX2_CLEAR;
    M_STATE_TEX3_CLEAR;
    M_STATE_TEX4_CLEAR;
    M_STATE_TEX5_CLEAR;
    M_STATE_TEX6_CLEAR;
    M_STATE_TEX7_CLEAR;
    M_STATE_TEX8_CLEAR;
    M_STATE_TEX9_CLEAR;
}

INLINEF VOID dr_SetModelSolidNormal (TModel *model)
{
    M_STATE_ARRAY_COLOR_CLEAR;
    M_STATE_ARRAY_VERTEX_SET            (model->vbo_vertices, 3, GL_FLOAT);
    M_STATE_ARRAY_NORMAL_SET            (model->vbo_normals,     GL_SHORT);
    M_STATE_ARRAY_SECONDARY_COLOR_SET   (model->vbo_tangents, 3, GL_UNSIGNED_BYTE);
    M_STATE_ARRAY_TEX0_COORDS_SET       (model->vbo_uv1, model->size_uv1, model->type_uv1);
    M_STATE_ARRAY_TEX1_COORDS_CLEAR;
    M_STATE_ARRAY_TEX2_COORDS_CLEAR;
    M_STATE_INDICES_SET                 (model->ebo)
}

INLINEF VOID dr_SetModelSolidMorph (TModel *model)
{
    M_STATE_ARRAY_COLOR_CLEAR;
    M_STATE_ARRAY_VERTEX_SET            (model->vbo_vertices, 3, GL_FLOAT);
    M_STATE_ARRAY_NORMAL_SET            (model->vbo_normals,     GL_SHORT);
    M_STATE_ARRAY_SECONDARY_COLOR_SET   (model->vbo_tangents, 3, GL_UNSIGNED_BYTE);
    M_STATE_ARRAY_TEX0_COORDS_SET       (model->vbo_uv1, model->size_uv1, model->type_uv1);
    M_STATE_ARRAY_TEX1_COORDS_SET       (model->vbo_uv2, model->size_uv2, model->type_uv2);
    M_STATE_ARRAY_TEX2_COORDS_CLEAR;
    M_STATE_INDICES_SET                 (model->ebo)

    // model specific shader attributes
    glMultiTexCoord3f       (GL_TEXTURE2, model->param1, model->param2, model->param3);
}

INLINEF VOID dr_SetModelDepthSolidNormal (TModel *model)
{
    M_STATE_ARRAY_VERTEX_SET            (model->vbo_vertices, 3, GL_FLOAT);
    M_STATE_ARRAY_TEX0_COORDS_CLEAR;
    M_STATE_ARRAY_TEX1_COORDS_CLEAR;
    M_STATE_INDICES_SET                 (model->ebo)
}

INLINEF VOID dr_SetModelDepthSolidMorph (TModel *model)
{
    M_STATE_ARRAY_VERTEX_SET            (model->vbo_vertices, 3, GL_FLOAT);
    M_STATE_ARRAY_TEX0_COORDS_CLEAR;
    M_STATE_ARRAY_TEX1_COORDS_SET       (model->vbo_uv2, model->size_uv2, model->type_uv2);
    M_STATE_INDICES_SET                 (model->ebo)

    // model specific shader attributes
    glMultiTexCoord3f   (GL_TEXTURE2, model->param1, model->param2, model->param3);
}

INLINEF VOID dr_DrawSolidNormal (

            UINT_32 &ID,
            UINT_32 &model_mode,
            UINT_32 &model_count
    )
{
    // multiple instances ?
    if (dr_object_instances [ID] > 1) {

        register FLOAT_32P transform = dr_object_instances_transforms [ID];

        for (INT_32 i = dr_object_instances [ID] - 1; i >= 0; i --) {

            // transform ..
            glMultiTexCoord3fv (GL_TEXTURE4, &(transform [ 0]));
            glMultiTexCoord4fv (GL_TEXTURE5, &(transform [ 3]));   transform += 7;

            // draw ..
            glDrawElements (model_mode, model_count, GL_UNSIGNED_SHORT, NULL);            
        }

    } else {

        register FLOAT_32P transform = &dr_object_transforms [ID << 4];

        // matrix (3x3 + translation)
        glMultiTexCoord3fv (GL_TEXTURE4, &(transform [ 0]));
        glMultiTexCoord3fv (GL_TEXTURE5, &(transform [ 4]));
        glMultiTexCoord3fv (GL_TEXTURE6, &(transform [ 8]));
        glMultiTexCoord3fv (GL_TEXTURE7, &(transform [12]));

        // draw ..
        glDrawElements (model_mode, model_count, GL_UNSIGNED_SHORT, NULL);
    }
}

INLINEF VOID dr_DrawSolidMorph (

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
    dr_DrawSolidNormal (ID, model_mode, model_count);
}

INLINEF VOID dr_DrawSolidShrink (

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
    dr_DrawSolidNormal (ID, model_mode, model_count);
}

// --------------------------------------------------------------------------------
// TERRAIN
// --------------------------------------------------------------------------------

INLINEF VOID dr_LoadShaderTerrain ()
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
    glUniform1i     (glGetUniformLocation (dr_program_terrain_gbuffers_normal, "tex_normal1"),     0);
    glUniform1i     (glGetUniformLocation (dr_program_terrain_gbuffers_normal, "tex_normal2"),     1);
    glUniform1i     (glGetUniformLocation (dr_program_terrain_gbuffers_normal, "tex_normal3"),     2);
    glUniform1i     (glGetUniformLocation (dr_program_terrain_gbuffers_normal, "tex_diffuse1"),    3);
    glUniform1i     (glGetUniformLocation (dr_program_terrain_gbuffers_normal, "tex_diffuse2"),    4);
    glUniform1i     (glGetUniformLocation (dr_program_terrain_gbuffers_normal, "tex_diffuse3"),    5);
    glUniform1i     (glGetUniformLocation (dr_program_terrain_gbuffers_normal, "tex_mask"),        6);
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
    glUniform1i     (glGetUniformLocation (dr_program_terrain_gbuffers_morph, "tex_normal1"),     0);
    glUniform1i     (glGetUniformLocation (dr_program_terrain_gbuffers_morph, "tex_normal2"),     1);
    glUniform1i     (glGetUniformLocation (dr_program_terrain_gbuffers_morph, "tex_normal3"),     2);
    glUniform1i     (glGetUniformLocation (dr_program_terrain_gbuffers_morph, "tex_diffuse1"),    3);
    glUniform1i     (glGetUniformLocation (dr_program_terrain_gbuffers_morph, "tex_diffuse2"),    4);
    glUniform1i     (glGetUniformLocation (dr_program_terrain_gbuffers_morph, "tex_diffuse3"),    5);
    glUniform1i     (glGetUniformLocation (dr_program_terrain_gbuffers_morph, "tex_mask"),        6);
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

INLINEF VOID dr_SetMaterialTerrain (TMaterial *material)
{
    M_STATE_TEX0_SET (material->normal1);
    M_STATE_TEX1_SET (material->normal2);
    M_STATE_TEX2_SET (material->normal3);
    M_STATE_TEX3_SET (material->diffuse1);
    M_STATE_TEX4_SET (material->diffuse2);
    M_STATE_TEX5_SET (material->diffuse3);
    M_STATE_TEX6_SET (material->mask);
}

INLINEF VOID dr_SetModelTerrainNormal (TModel *model)
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

INLINEF VOID dr_SetModelTerrainMorph (TModel *model)
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

INLINEF VOID dr_SetModelDepthTerrainNormal (TModel *model)
{
    M_STATE_ARRAY_VERTEX_SET            (model->vbo_vertices, 3, GL_FLOAT);
    M_STATE_ARRAY_TEX0_COORDS_CLEAR;
    M_STATE_ARRAY_TEX1_COORDS_CLEAR;
    M_STATE_INDICES_SET                 (model->ebo)
}

INLINEF VOID dr_SetModelDepthTerrainMorph (TModel *model)
{
    M_STATE_ARRAY_VERTEX_SET            (model->vbo_vertices, 3, GL_FLOAT);
    M_STATE_ARRAY_TEX0_COORDS_CLEAR;
    M_STATE_ARRAY_TEX1_COORDS_SET       (model->vbo_uv3, model->size_uv3, model->type_uv3);
    M_STATE_INDICES_SET                 (model->ebo)

    // model specific shader attributes
    glMultiTexCoord3f   (GL_TEXTURE2, model->param1, model->param2, model->param3);
}

INLINEF VOID dr_DrawTerrainNormal (

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

INLINEF VOID dr_DrawTerrainMorph (

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

// --------------------------------------------------------------------------------
// FOLIAGE
// --------------------------------------------------------------------------------

INLINEF VOID dr_LoadShaderFoliage ()
{
    // GROW

    // g-buffers
    lo_LoadShaders ("Shaders\\dr_foliage_gbuffers_grow.vert",
                    "Shaders\\dr_foliage_gbuffers_normal.frag", "\n", 
                    &dr_program_foliage_gbuffers_grow,
                    &dr_program_foliage_gbuffers_growv,
                    &dr_program_foliage_gbuffers_growf);

    glUseProgram    (dr_program_foliage_gbuffers_grow);
    glUniform1i     (glGetUniformLocation (dr_program_foliage_gbuffers_grow, "tex_normal"),         0);
    glUniform1i     (glGetUniformLocation (dr_program_foliage_gbuffers_grow, "tex_diffuse"),        1);
    glUniform2f     (glGetUniformLocation (dr_program_foliage_gbuffers_grow, "uvscale"),            10.0f/32767.0f, 10.0f/32767.0f);
    glUniform1f     (glGetUniformLocation (dr_program_foliage_gbuffers_grow, "planefar"),           dr_planefar);

    // depth
    lo_LoadShaders ("Shaders\\dr_foliage_depth_grow.vert",
                    "Shaders\\dr_foliage_depth_normal.frag", "\n", 
                    &dr_program_foliage_depth_grow,
                    &dr_program_foliage_depth_growv,
                    &dr_program_foliage_depth_growf);

    glUseProgram    (dr_program_foliage_depth_grow);
    glUniform1i     (glGetUniformLocation (dr_program_foliage_depth_grow, "tex_diffuse"),           0);
    glUniform2f     (glGetUniformLocation (dr_program_foliage_depth_grow, "uvscale"),               10.0f/32767.0f, 10.0f/32767.0f);

    // SHRINK

    // g-buffers
    lo_LoadShaders ("Shaders\\dr_foliage_gbuffers_shrink.vert",
                    "Shaders\\dr_foliage_gbuffers_normal.frag", "\n", 
                    &dr_program_foliage_gbuffers_shrink,
                    &dr_program_foliage_gbuffers_shrinkv,
                    &dr_program_foliage_gbuffers_shrinkf);

    glUseProgram    (dr_program_foliage_gbuffers_shrink);
    glUniform1i     (glGetUniformLocation (dr_program_foliage_gbuffers_shrink, "tex_normal"),       0);
    glUniform1i     (glGetUniformLocation (dr_program_foliage_gbuffers_shrink, "tex_diffuse"),      1);
    glUniform2f     (glGetUniformLocation (dr_program_foliage_gbuffers_shrink, "uvscale"),          10.0f/32767.0f, 10.0f/32767.0f);
    glUniform1f     (glGetUniformLocation (dr_program_foliage_gbuffers_shrink, "planefar"),         dr_planefar);

    // depth
    lo_LoadShaders ("Shaders\\dr_foliage_depth_shrink.vert",
                    "Shaders\\dr_foliage_depth_normal.frag", "\n", 
                    &dr_program_foliage_depth_shrink,
                    &dr_program_foliage_depth_shrinkv,
                    &dr_program_foliage_depth_shrinkf);

    glUseProgram    (dr_program_foliage_depth_shrink);
    glUniform1i     (glGetUniformLocation (dr_program_foliage_depth_shrink, "tex_diffuse"),         0);
    glUniform2f     (glGetUniformLocation (dr_program_foliage_depth_shrink, "uvscale"),             10.0f/32767.0f, 10.0f/32767.0f);
}

INLINEF VOID dr_SetMaterialFoliage (TMaterial *material)
{
    M_STATE_TEX0_SET (material->normal1);
    M_STATE_TEX1_SET (material->diffuse1);
    M_STATE_TEX2_CLEAR;
    M_STATE_TEX3_CLEAR;
    M_STATE_TEX4_CLEAR;
    M_STATE_TEX5_CLEAR;
    M_STATE_TEX6_CLEAR;
    M_STATE_TEX7_CLEAR;
    M_STATE_TEX8_CLEAR;
    M_STATE_TEX9_CLEAR;

    glMultiTexCoord2f (GL_TEXTURE2, material->gloss1, material->shininess1);
}

INLINEF VOID dr_SetModelFoliageNormal (TModel *model)
{
    M_STATE_ARRAY_COLOR_CLEAR;
    M_STATE_ARRAY_VERTEX_SET            (model->vbo_vertices, 3, GL_FLOAT);
    M_STATE_ARRAY_NORMAL_SET            (model->vbo_normals,     GL_SHORT);
    M_STATE_ARRAY_SECONDARY_COLOR_SET   (model->vbo_tangents, 3, GL_UNSIGNED_BYTE);
    M_STATE_ARRAY_TEX0_COORDS_SET       (model->vbo_uv1, model->size_uv1, model->type_uv1);
    M_STATE_ARRAY_TEX1_COORDS_CLEAR;
    M_STATE_ARRAY_TEX2_COORDS_CLEAR;
    M_STATE_INDICES_SET                 (model->ebo)
}

INLINEF VOID dr_SetModelDepthFoliageNormal (TModel *model)
{
    M_STATE_ARRAY_VERTEX_SET            (model->vbo_vertices, 3, GL_FLOAT);
    M_STATE_ARRAY_TEX0_COORDS_SET       (model->vbo_uv1, model->size_uv1, model->type_uv1);
    M_STATE_ARRAY_TEX1_COORDS_CLEAR;
    M_STATE_INDICES_SET                 (model->ebo)
}

INLINEF VOID dr_DrawFoliageGrow (

            UINT_32 &ID,
            UINT_32 &list
    )
{
    // shader LOD parameters
    register FLOAT_32 factor = (dr_object_distances [ID] - dr_object_morph1 [ID]) * dr_object_morph2 [ID];  factor = MIN (1.0f, MAX (0.0f, factor));

    /// DO NOT SQUARE FACTOR, SQUARE DISTANCE !!!

    // set params
    glFogCoordf (factor);

    // shader animation
    FLOAT_32 fswing = (0.005f * (((ID % 4) + 1) / 5.0f)) * sin (0.005f * dr_swing + ID);

    // set params
    glMultiTexCoord1f (GL_TEXTURE1, fswing);

    glMultiTexCoord4f (GL_TEXTURE3, 0.0, 0.0, 2.0f, - 1.0f);

    // shortcut
    register FLOAT_32P transform = &dr_object_transforms [ID << 4];

    // matrix (3x3 + translation)
    glMultiTexCoord3fv (GL_TEXTURE4, &(transform [ 0]));
    glMultiTexCoord3fv (GL_TEXTURE5, &(transform [ 4]));
    glMultiTexCoord3fv (GL_TEXTURE6, &(transform [ 8]));
    glMultiTexCoord3fv (GL_TEXTURE7, &(transform [12]));

    // draw ..
    glCallList (list);
}

INLINEF VOID dr_DrawFoliageShrink (

            UINT_32 &ID,
            UINT_32 &list
    )
{
    // shader LOD parameters
    register FLOAT_32 factor = (dr_object_distances [ID] - dr_object_morph1 [ID]) * dr_object_morph2 [ID];  factor = MIN (1.0f, MAX (0.0f, factor));

    /// DO NOT SQUARE FACTOR, SQUARE DISTANCE !!!

    // set params
    glFogCoordf (factor);

    // set params
    glMultiTexCoord1f (GL_TEXTURE1, dr_swing);

    // shortcut
    register FLOAT_32P transform = &dr_object_transforms [ID << 4];

    // matrix (3x3 + translation)
    glMultiTexCoord3fv (GL_TEXTURE4, &(transform [ 0]));
    glMultiTexCoord3fv (GL_TEXTURE5, &(transform [ 4]));
    glMultiTexCoord3fv (GL_TEXTURE6, &(transform [ 8]));
    glMultiTexCoord3fv (GL_TEXTURE7, &(transform [12]));

    // draw ..
    glCallList (list);
}

// --------------------------------------------------------------------------------
// GRASS
// --------------------------------------------------------------------------------

INLINEF VOID dr_LoadShaderGrass ()
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

INLINEF VOID dr_SetMaterialGrass (TMaterial *material)
{
    M_STATE_TEX0_SET (material->diffuse1);
    M_STATE_TEX1_CLEAR;
    M_STATE_TEX2_CLEAR;
    M_STATE_TEX3_CLEAR;
    M_STATE_TEX4_CLEAR;
    M_STATE_TEX5_CLEAR;
    M_STATE_TEX6_CLEAR;
    M_STATE_TEX7_CLEAR;
    M_STATE_TEX8_CLEAR;
    M_STATE_TEX9_CLEAR;

    glMultiTexCoord2f (GL_TEXTURE3, material->gloss1, material->shininess1);
}

INLINEF VOID dr_SetModelGrassNormal (TModel *model)
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

INLINEF VOID dr_SetModelDepthGrassNormal (TModel *model)
{
    M_STATE_ARRAY_VERTEX_SET            (model->vbo_vertices, 3, GL_FLOAT);
    M_STATE_ARRAY_TEX0_COORDS_SET       (model->vbo_uv1, model->size_uv1, model->type_uv1);
    M_STATE_ARRAY_TEX1_COORDS_CLEAR
    M_STATE_INDICES_SET                 (model->ebo)
}

INLINEF VOID dr_DrawGrassNormal (

            UINT_32     &ID,
            UINT_32     &list
    )
{

    // shader LOD parameters
    register FLOAT_32 factor = (dr_object_distances [ID] - dr_object_morph1 [ID]) * dr_object_morph2 [ID];  factor = MIN (1.0f, MAX (0.0f, factor));

    /// DO NOT SQUARE FACTOR, SQUARE DISTANCE !!!

    // set params
    glFogCoordf (factor);

    // shader animation
    FLOAT_32 fswing = 0.03f * sin (0.005f * dr_swing + ID);

    // set params
    glMultiTexCoord1f (GL_TEXTURE2, fswing);

    // shortcut
    register FLOAT_32P transform = dr_object_instances_transforms [ID];

    // instances loop
    for (INT_32 i = dr_object_instances [ID] - 1; i >= 0; i --) {

        // transform ..
        glMultiTexCoord3fv (GL_TEXTURE4, &(transform [ 0]));
        glMultiTexCoord4fv (GL_TEXTURE5, &(transform [ 3]));   transform += 7;

        // draw ..
        glCallList (list);
    }
}
