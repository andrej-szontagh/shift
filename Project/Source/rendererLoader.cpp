
////////////////////////////////////////////////////////////////////////////////////////////////////////////
// dr_Init
////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if 1

VOID	dr_Init ()
{

    UINT_32 i;

    // INITIALIZATIONS

    /// MACRO HERE

    dr_stamp = 1;

    dr_frame = 0;
    dr_swing = 0.0;

    dr_origin [0] = 0.0;
    dr_origin [1] = 0.0;
    dr_origin [2] = 0.0;

    dr_detailculling = M_DR_CULLING_SAMPLES;

    // DIMENSIONS

    UINT_32 fraction = 1;

    dr_w = (UINT_32P) malloc (SIZE_UINT_32 * dr_control_mip);
    dr_h = (UINT_32P) malloc (SIZE_UINT_32 * dr_control_mip);

    for (i = 0; i < dr_control_mip; i ++) {

        dr_w [i] = dr_width  / fraction;
        dr_h [i] = dr_height / fraction;

        fraction *= 2;
    }

    //////////////////////////////////////////////////////////////////////////////
    // INIT. CLIPPING FRUSTUM
    //////////////////////////////////////////////////////////////////////////////

    dr_fovyr        = (dr_fovy = M_DR_FOVY) * PI / 180.0;

    dr_aspect       = dr_width / (FLOAT_64) dr_height;

    dr_avertical    = tan ((FLOAT_64) dr_fovyr * 0.5);

    dr_ClipUpdate ();

    //////////////////////////////////////////////////////////////////////////////
    // CREATING TEXTURES
    //////////////////////////////////////////////////////////////////////////////

    // RANDOM

    UINT_32 c = M_DR_SSAO_RAND * M_DR_SSAO_RAND * 4;

    UINT_8P texture = (UINT_8P) malloc (c * SIZE_UINT_8);

    for (i = 0; i < c; i += 4) {

        texture [i    ] = (UINT_8) (255 * rand () / RAND_MAX);
        texture [i + 1] = (UINT_8) (255 * rand () / RAND_MAX);
        texture [i + 2] = (UINT_8) (255 * rand () / RAND_MAX);
        texture [i + 3] = (UINT_8) (255 * rand () / RAND_MAX);
    }

    glGenTextures    (1, &dr_rand);
    glBindTexture    (GL_TEXTURE_2D, dr_rand);
    glTexParameteri  (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri  (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri  (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri  (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexImage2D     (GL_TEXTURE_2D, 0, GL_RGBA8, M_DR_SSAO_RAND, M_DR_SSAO_RAND, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture);

    free (texture);

    // TEXT

    TImage image;

    lo_LoadImage ("Data\\images\\system\\font.tga", &image);

    glGenTextures    (1, &dr_text);
    glBindTexture    (GL_TEXTURE_2D, dr_text);
    glTexParameteri  (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri  (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri  (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri  (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D     (GL_TEXTURE_2D, 0, GL_RGBA8, image.width, image.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.data);

    free (image.data);

    // DEPTH

    glGenTextures    (1, &dr_depth);
    glBindTexture    (GL_TEXTURE_RECTANGLE_ARB, dr_depth);
    glTexParameteri  (GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri  (GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri  (GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri  (GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D     (GL_TEXTURE_RECTANGLE_ARB, 0, GL_DEPTH_COMPONENT32, dr_width, dr_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

    // G1

    glGenTextures    (1, &dr_G1);
    glBindTexture    (GL_TEXTURE_RECTANGLE_ARB, dr_G1);
    glTexParameteri  (GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri  (GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri  (GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri  (GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D     (GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA8, dr_width, dr_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    // G2

    glGenTextures    (1, &dr_G2);
    glBindTexture    (GL_TEXTURE_RECTANGLE_ARB, dr_G2);
    glTexParameteri  (GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri  (GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri  (GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri  (GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D     (GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA8, dr_width, dr_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    // G3

    glGenTextures    (1, &dr_G3);
    glBindTexture    (GL_TEXTURE_RECTANGLE_ARB, dr_G3);
    glTexParameteri  (GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri  (GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri  (GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri  (GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D     (GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA8, dr_width, dr_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    // AUXILIARY HDR 1

    // NOTE : do not use linear interpolation it damages coded data

    glGenTextures    (1, &dr_auxhdr1);
    glBindTexture    (GL_TEXTURE_RECTANGLE_ARB, dr_auxhdr1);
    glTexParameteri  (GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri  (GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri  (GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri  (GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D     (GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA16F, dr_width, dr_height, 0, GL_RGBA, GL_FLOAT, NULL);

    // AUXILIARY HDR 2

    // NOTE : do not use linear interpolation it damages coded data

    glGenTextures    (1, &dr_auxhdr2);
    glBindTexture    (GL_TEXTURE_RECTANGLE_ARB, dr_auxhdr2);
    glTexParameteri  (GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri  (GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri  (GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri  (GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D     (GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA16F, dr_width, dr_height, 0, GL_RGBA, GL_FLOAT, NULL);

    // AUXILIARY MIPMAPS

    dr_auxA = (UINT_32P) malloc (SIZE_UINT_32 * dr_control_mip);
    dr_auxB = (UINT_32P) malloc (SIZE_UINT_32 * dr_control_mip);

    for (i = 0; i < dr_control_mip; i ++) {

        glGenTextures    (1, &dr_auxA [i]);
        glBindTexture    (GL_TEXTURE_RECTANGLE_ARB, dr_auxA [i]);
        glTexParameteri  (GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri  (GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri  (GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri  (GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D     (GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA8, dr_w [i],  dr_h [i],  0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

        glGenTextures    (1, &dr_auxB [i]);
        glBindTexture    (GL_TEXTURE_RECTANGLE_ARB, dr_auxB [i]);
        glTexParameteri  (GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri  (GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri  (GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri  (GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D     (GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA8, dr_w [i],  dr_h [i],  0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    }

    //////////////////////////////////////////////////////////////////////////////
    // CREATING FRAMEBUFFER OBJECTS
    //////////////////////////////////////////////////////////////////////////////

    // G-BUFFERS

    glGenFramebuffersEXT  (1, &dr_framebuffer);
    glGenRenderbuffersEXT (1, &dr_renderbuffer);

    glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, dr_framebuffer);

    glBindRenderbufferEXT		 (GL_RENDERBUFFER_EXT, dr_renderbuffer);
    glRenderbufferStorageEXT	 (GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT32, dr_width, dr_height);
    glFramebufferRenderbufferEXT (GL_FRAMEBUFFER_EXT,  GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, dr_renderbuffer);

    //////////////////////////////////////////////////////////////////////////////
    // GENERAL SHADERS
    //////////////////////////////////////////////////////////////////////////////

    /////////////////////////////
    // enviroment
    lo_LoadShaders ("Shaders\\dr_enviroment.vert",
        "Shaders\\dr_enviroment.frag", "\n", 
        &dr_program_enviroment,
        &dr_program_enviromentv,
        &dr_program_enviromentf);

    glUseProgram    (dr_program_enviroment);
    
    glUniform1i     (glGetUniformLocation (dr_program_enviroment, "tex_enviroment"), 0);
    glUniform2f     (glGetUniformLocation (dr_program_enviroment, "uvscale"),
        ((FLOAT_32) dr_enviroment_material->width  * 10.0f) / 32767.0f,
        ((FLOAT_32) dr_enviroment_material->height * 10.0f) / 32767.0f);

    /////////////////////////////
    // fog
    lo_LoadShaders ("Shaders\\dr_fog.vert",
        "Shaders\\dr_fog.frag", "\n", 
        &dr_program_fog,
        &dr_program_fogv,
        &dr_program_fogf);

    glUseProgram    (dr_program_fog);
    glUniform1i     (glGetUniformLocation (dr_program_fog, "tex_depth"),    0);
    glUniform1f     (glGetUniformLocation (dr_program_fog, "farw"),         (FLOAT_32) dr_farw);
    glUniform1f     (glGetUniformLocation (dr_program_fog, "farh"),         (FLOAT_32) dr_farh);
    glUniform1f     (glGetUniformLocation (dr_program_fog, "farplane"),     dr_planefar);
    glUniform1f     (glGetUniformLocation (dr_program_fog, "nearplane"),    dr_planenear);

    //////////////////////////////////////////////////////////////////////////////
    // DEBUG SHADERS
    //////////////////////////////////////////////////////////////////////////////

    /////////////////////////////
    // shadow
    lo_LoadShaders ("Shaders\\dr_debug_shadow.vert",
        "Shaders\\dr_debug_shadow.frag", "\n", 
        &dr_program_debug_shadow,
        &dr_program_debug_shadowv,
        &dr_program_debug_shadowf);

    glUseProgram    (dr_program_debug_shadow);
    glUniform1i     (glGetUniformLocation (dr_program_debug_shadow, "tex"), 0);

    //////////////////////////////////////////////////////////////////////////////
    // SPECIFIC SHADERS
    //////////////////////////////////////////////////////////////////////////////

    dr_LoadShaderSolid   ();
    dr_LoadShaderTerrain ();
    dr_LoadShaderFoliage ();
    dr_LoadShaderGrass   ();

    //////////////////////////////////////////////////////////////////////////////
    // AA SHADERS
    //////////////////////////////////////////////////////////////////////////////

    /////////////////////////////
    // vetrical blur
    lo_LoadShaders ("Shaders\\dr_aa_blurv.vert",
        "Shaders\\dr_aa_blurv.frag", "\n", 
        &dr_program_aa_blurvert,
        &dr_program_aa_blurvertv,
        &dr_program_aa_blurvertf);

    glUseProgram    (dr_program_aa_blurvert);
    glUniform1i     (glGetUniformLocation (dr_program_aa_blurvert,     "tex_depth"),    0);
    glUniform1i     (glGetUniformLocation (dr_program_aa_blurvert,     "tex"),          1);
    glUniform1f     (glGetUniformLocation (dr_program_aa_blurvert,     "nom"),          2.0f * dr_planenear);
    glUniform1f     (glGetUniformLocation (dr_program_aa_blurvert,     "denom1"),       dr_planefar + dr_planenear);
    glUniform1f     (glGetUniformLocation (dr_program_aa_blurvert,     "denom2"),       dr_planefar - dr_planenear);

    /////////////////////////////
    // horizontal blur
    lo_LoadShaders ("Shaders\\dr_aa_blurh.vert",
        "Shaders\\dr_aa_blurh.frag", "\n", 
        &dr_program_aa_blurhoriz,
        &dr_program_aa_blurhorizv,
        &dr_program_aa_blurhorizf);

    glUseProgram    (dr_program_aa_blurhoriz);
    glUniform1i     (glGetUniformLocation (dr_program_aa_blurhoriz,    "tex_depth"),    0);
    glUniform1i     (glGetUniformLocation (dr_program_aa_blurhoriz,    "tex"),          1);
    glUniform1f     (glGetUniformLocation (dr_program_aa_blurhoriz,    "nom"),          2.0f * dr_planenear);
    glUniform1f     (glGetUniformLocation (dr_program_aa_blurhoriz,    "denom1"),       dr_planefar + dr_planenear);
    glUniform1f     (glGetUniformLocation (dr_program_aa_blurhoriz,    "denom2"),       dr_planefar - dr_planenear);

    //////////////////////////////////////////////////////////////////////////////
    // HDR SHADERS
    //////////////////////////////////////////////////////////////////////////////

    /////////////////////////////
    // HDR
    CHAR prefix [64];   sprintf (prefix, "\n#define M_MINLEVEL %i\n#define M_MAXLEVEL %i\n", dr_control_bloom_res, dr_control_mip - 1);

    lo_LoadShaders ("Shaders\\dr_hdr.vert",
        "Shaders\\dr_hdr.frag", prefix, 
        &dr_program_hdr,
        &dr_program_hdrv,
        &dr_program_hdrf);

    glUseProgram  (dr_program_hdr);
    glUniform1i  (glGetUniformLocation (dr_program_hdr,         "base"),   0);

    if ((dr_control_bloom_res <= 0) && (dr_control_mip >= 1)) 
        glUniform1i  (glGetUniformLocation (dr_program_hdr,     "tex0"),   1);
    if ((dr_control_bloom_res <= 1) && (dr_control_mip >= 2)) 
        glUniform1i  (glGetUniformLocation (dr_program_hdr,     "tex1"),   2);
    if ((dr_control_bloom_res <= 2) && (dr_control_mip >= 3)) 
        glUniform1i  (glGetUniformLocation (dr_program_hdr,     "tex2"),   3);
    if ((dr_control_bloom_res <= 3) && (dr_control_mip >= 4)) 
        glUniform1i  (glGetUniformLocation (dr_program_hdr,     "tex3"),   4);
    if ((dr_control_bloom_res <= 4) && (dr_control_mip >= 5)) 
        glUniform1i  (glGetUniformLocation (dr_program_hdr,     "tex4"),   5);
    if ((dr_control_bloom_res <= 5) && (dr_control_mip >= 6)) 
        glUniform1i  (glGetUniformLocation (dr_program_hdr,     "tex5"),   6);
    if ((dr_control_bloom_res <= 6) && (dr_control_mip >= 7)) 
        glUniform1i  (glGetUniformLocation (dr_program_hdr,     "tex6"),   7);
    if ((dr_control_bloom_res <= 7) && (dr_control_mip >= 8)) 
        glUniform1i  (glGetUniformLocation (dr_program_hdr,     "tex7"),   8);
    if ((dr_control_bloom_res <= 8) && (dr_control_mip >= 9)) 
        glUniform1i  (glGetUniformLocation (dr_program_hdr,     "tex8"),   9);

    /////////////////////////////
    // vetrical blur
    lo_LoadShaders ("Shaders\\dr_hdr_blurv.vert",
        "Shaders\\dr_hdr_blurv.frag", "\n", 
        &dr_program_hdr_blurvert,
        &dr_program_hdr_blurvertv,
        &dr_program_hdr_blurvertf);

    glUseProgram    (dr_program_hdr_blurvert);
    glUniform1i     (glGetUniformLocation (dr_program_hdr_blurvert,     "tex"),     0);

    /////////////////////////////
    // horizontal blur
    lo_LoadShaders ("Shaders\\dr_hdr_blurh.vert",
        "Shaders\\dr_hdr_blurh.frag", "\n", 
        &dr_program_hdr_blurhoriz,
        &dr_program_hdr_blurhorizv,
        &dr_program_hdr_blurhorizf);

    glUseProgram    (dr_program_hdr_blurhoriz);
    glUniform1i     (glGetUniformLocation (dr_program_hdr_blurhoriz,    "tex"),     0);

    /////////////////////////////
    // hipass filter
    lo_LoadShaders ("Shaders\\dr_hdr_hipass.vert",
        "Shaders\\dr_hdr_hipass.frag", "\n", 
        &dr_program_hdr_hipass,
        &dr_program_hdr_hipassv,
        &dr_program_hdr_hipassf);

    glUseProgram    (dr_program_hdr_hipass);
    glUniform1i     (glGetUniformLocation (dr_program_hdr_hipass,       "tex"),     0);

    //////////////////////////////////////////////////////////////////////////////
    // SSAO SHADERS
    //////////////////////////////////////////////////////////////////////////////

    /////////////////////////////
    // SSAO
    lo_LoadShaders ("Shaders\\dr_ssao.vert",
        "Shaders\\dr_ssao.frag", "\n", 
        &dr_program_ssao,
        &dr_program_ssaov,
        &dr_program_ssaof);

    glUseProgram    (dr_program_ssao);
    glUniform1i     (glGetUniformLocation (dr_program_ssao,             "tex_depth"),   0);
    glUniform1i     (glGetUniformLocation (dr_program_ssao,             "tex_rand"),    1);
    glUniform1i     (glGetUniformLocation (dr_program_ssao,             "tex_G2"),      2);
    glUniform1f     (glGetUniformLocation (dr_program_ssao,             "randscale"),   M_DR_SSAO_RANDSCALE);
    glUniform1f     (glGetUniformLocation (dr_program_ssao,             "farw"),        (FLOAT_32) dr_farw);
    glUniform1f     (glGetUniformLocation (dr_program_ssao,             "farh"),        (FLOAT_32) dr_farh);
    glUniform1f     (glGetUniformLocation (dr_program_ssao,             "farplane"),    dr_planefar);
    glUniform1f     (glGetUniformLocation (dr_program_ssao,             "nearplane"),   dr_planenear);
    glUniform1f     (glGetUniformLocation (dr_program_ssao,             "hwidth"),      dr_width  * 0.5f);
    glUniform1f     (glGetUniformLocation (dr_program_ssao,             "hheight"),     dr_height * 0.5f);
    glUniform1f     (glGetUniformLocation (dr_program_ssao,             "scalex"),      (FLOAT_32) (dr_farw / (dr_width  * 0.5)));
    glUniform1f     (glGetUniformLocation (dr_program_ssao,             "scaley"),      (FLOAT_32) (dr_farh / (dr_height * 0.5)));
    glUniform1f     (glGetUniformLocation (dr_program_ssao,             "depth1"),      (FLOAT_32) (2.0 *  dr_planenear));
    glUniform1f     (glGetUniformLocation (dr_program_ssao,             "depth2"),      (FLOAT_32) (2.0 *  dr_planefar));
    glUniform1f     (glGetUniformLocation (dr_program_ssao,             "depth3"),      (FLOAT_32) (2.0 * (dr_planefar - dr_planenear)));

    /////////////////////////////
    // vertical blur and upscale
    lo_LoadShaders ("Shaders\\dr_ssao_blurv.vert",
        "Shaders\\dr_ssao_blurv.frag", "\n", 
        &dr_program_ssao_blurvert,
        &dr_program_ssao_blurvertv,
        &dr_program_ssao_blurvertf);

    glUseProgram    (dr_program_ssao_blurvert);
    glUniform1i     (glGetUniformLocation (dr_program_ssao_blurvert,    "tex_depth"),   0);
    glUniform1i     (glGetUniformLocation (dr_program_ssao_blurvert,    "tex"),         1);
    glUniform1f     (glGetUniformLocation (dr_program_ssao_blurvert,    "depth1"),      (FLOAT_32) (2.0 *  dr_planenear));
    glUniform1f     (glGetUniformLocation (dr_program_ssao_blurvert,    "depth2"),      (FLOAT_32) (2.0 *  dr_planefar));
    glUniform1f     (glGetUniformLocation (dr_program_ssao_blurvert,    "depth3"),      (FLOAT_32) (2.0 * (dr_planefar - dr_planenear)));
    glUniform1f     (glGetUniformLocation (dr_program_ssao_blurvert,    "scalex"),      (FLOAT_32) dr_w [dr_control_ssao_res] / dr_width);
    glUniform1f     (glGetUniformLocation (dr_program_ssao_blurvert,    "scaley"),      (FLOAT_32) dr_h [dr_control_ssao_res] / dr_height);

    /////////////////////////////
    // horizontal blur and blend
    lo_LoadShaders ("Shaders\\dr_ssao_blurhblend.vert",
        "Shaders\\dr_ssao_blurhblend.frag", "\n", 
        &dr_program_ssao_blurhblend,
        &dr_program_ssao_blurhblendv,
        &dr_program_ssao_blurhblendf);

    glUseProgram    (dr_program_ssao_blurhblend);
    glUniform1i     (glGetUniformLocation (dr_program_ssao_blurhblend,  "tex_depth"),   0);
    glUniform1i     (glGetUniformLocation (dr_program_ssao_blurhblend,  "tex_raw"),     1);
    glUniform1i     (glGetUniformLocation (dr_program_ssao_blurhblend,  "tex"),         2);
    glUniform1f     (glGetUniformLocation (dr_program_ssao_blurhblend,  "depth1"),      (FLOAT_32) (2.0 *  dr_planenear));
    glUniform1f     (glGetUniformLocation (dr_program_ssao_blurhblend,  "depth2"),      (FLOAT_32) (2.0 *  dr_planefar));
    glUniform1f     (glGetUniformLocation (dr_program_ssao_blurhblend,  "depth3"),      (FLOAT_32) (2.0 * (dr_planefar - dr_planenear)));

    //////////////////////////////////////////////////////////////////////////////
    // RENDER LISTS
    //////////////////////////////////////////////////////////////////////////////

    dr_quads = (UINT_32P) malloc (SIZE_UINT_32 * dr_control_mip);

    for (i = 0; i < dr_control_mip; i ++) {

        dr_quads [i] = glGenLists (1);

        glNewList (dr_quads [i], GL_COMPILE);
        glBegin (GL_QUADS);
        glMultiTexCoord2f (GL_TEXTURE0,  0.0f,		            0.0f);
        glVertex3f ( - 1.0f, - 1.0f, 0.0f);
        glMultiTexCoord2f (GL_TEXTURE0, (FLOAT_32) dr_w [i],	0.0f);
        glVertex3f (   1.0f, - 1.0f, 0.0f);
        glMultiTexCoord2f (GL_TEXTURE0, (FLOAT_32) dr_w [i],	(FLOAT_32) dr_h [i]);
        glVertex3f (   1.0f,   1.0f, 0.0f);
        glMultiTexCoord2f (GL_TEXTURE0,  0.0f,					(FLOAT_32) dr_h [i]);
        glVertex3f ( - 1.0f,   1.0f, 0.0f);
        glEnd ();
        glEndList ();
    }

    dr_quadpot = glGenLists (1);

    glNewList (dr_quadpot, GL_COMPILE);
    glBegin (GL_QUADS);
    glTexCoord2f (0.0f,		0.0f);
    glVertex3f ( - 1.0f, - 1.0f, 0.0f);
    glTexCoord2f (1.0f,		0.0f);
    glVertex3f (   1.0f, - 1.0f, 0.0f);
    glTexCoord2f (1.0f,		1.0f);
    glVertex3f (   1.0f,   1.0f, 0.0f);
    glTexCoord2f (0.0f,		1.0f);
    glVertex3f ( - 1.0f,   1.0f, 0.0f);
    glEnd ();
    glEndList ();

    dr_quad = glGenLists (1);

    glNewList (dr_quad, GL_COMPILE);
    glBegin (GL_QUADS);
    glVertex3f ( - 1.0f, - 1.0f, 0.0f);
    glVertex3f (   1.0f, - 1.0f, 0.0f);
    glVertex3f (   1.0f,   1.0f, 0.0f);
    glVertex3f ( - 1.0f,   1.0f, 0.0f);
    glEnd ();
    glEndList ();

    //////////////////////////////////////////////////////////////////////////////
    // INITIALIZING HDR
    //////////////////////////////////////////////////////////////////////////////

    // no change
    dr_intensityscale = 1.0;

    // storage for intensity sample
    dr_intensitydata = (UINT_8P) malloc (SIZE_UINT_8 * (dr_intensitysize = dr_w [dr_control_mip - 1] * dr_h [dr_control_mip - 1]) * 2);		/// * 2 (crashes sometimes)

    //////////////////////////////////////////////////////////////////////////////
    // INITIALIZING MATRICES
    //////////////////////////////////////////////////////////////////////////////

    dr_Matrices ();

    //////////////////////////////////////////////////////////////////////////////
    // MAXIMUM CLIP PLANES
    //////////////////////////////////////////////////////////////////////////////

    glGetIntegerv (GL_MAX_CLIP_PLANES, &dr_maxplanes);

    //////////////////////////////////////////////////////////////////////////////
    // INITIALIZING SUN
    //////////////////////////////////////////////////////////////////////////////

    dr_SunLoad ();
    dr_SunInit ();

    //////////////////////////////////////////////////////////////////////////////
    // ENVIROMENT
    //////////////////////////////////////////////////////////////////////////////

    glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);

    //////////////////////////////////////////////////////////////////////////////
    // INIT STATES
    //////////////////////////////////////////////////////////////////////////////

    glBindBufferARB (GL_ELEMENT_ARRAY_BUFFER_ARB, 0);     dr_state_indices    = 0;
    glDisableClientState (GL_COLOR_ARRAY);                dr_state_color      = 0;
    glDisableClientState (GL_SECONDARY_COLOR_ARRAY);      dr_state_colors     = 0;
    glDisableClientState (GL_VERTEX_ARRAY);               dr_state_vertex     = 0;
    glDisableClientState (GL_NORMAL_ARRAY);               dr_state_normal     = 0;

    glClientActiveTexture (GL_TEXTURE0);  glDisableClientState (GL_TEXTURE_COORD_ARRAY);   dr_state_tex0_coord = 0;
    glClientActiveTexture (GL_TEXTURE1);  glDisableClientState (GL_TEXTURE_COORD_ARRAY);   dr_state_tex1_coord = 0;
    glClientActiveTexture (GL_TEXTURE2);  glDisableClientState (GL_TEXTURE_COORD_ARRAY);   dr_state_tex2_coord = 0;

    glActiveTexture (GL_TEXTURE0);  glDisable (GL_TEXTURE_RECTANGLE_ARB);  dr_state_tex0_rect = 0;
    glActiveTexture (GL_TEXTURE1);  glDisable (GL_TEXTURE_RECTANGLE_ARB);  dr_state_tex1_rect = 0;
    glActiveTexture (GL_TEXTURE2);  glDisable (GL_TEXTURE_RECTANGLE_ARB);  dr_state_tex2_rect = 0;
    glActiveTexture (GL_TEXTURE3);  glDisable (GL_TEXTURE_RECTANGLE_ARB);  dr_state_tex3_rect = 0;

    glActiveTexture (GL_TEXTURE0);  glDisable (GL_TEXTURE_2D);  dr_state_tex0  = 0;
    glActiveTexture (GL_TEXTURE1);  glDisable (GL_TEXTURE_2D);  dr_state_tex1  = 0;
    glActiveTexture (GL_TEXTURE2);  glDisable (GL_TEXTURE_2D);  dr_state_tex2  = 0;
    glActiveTexture (GL_TEXTURE3);  glDisable (GL_TEXTURE_2D);  dr_state_tex3  = 0;
    glActiveTexture (GL_TEXTURE4);  glDisable (GL_TEXTURE_2D);  dr_state_tex4  = 0;
    glActiveTexture (GL_TEXTURE5);  glDisable (GL_TEXTURE_2D);  dr_state_tex5  = 0;
    glActiveTexture (GL_TEXTURE6);  glDisable (GL_TEXTURE_2D);  dr_state_tex6  = 0;
    glActiveTexture (GL_TEXTURE7);  glDisable (GL_TEXTURE_2D);  dr_state_tex7  = 0;
    glActiveTexture (GL_TEXTURE8);  glDisable (GL_TEXTURE_2D);  dr_state_tex8  = 0;
    glActiveTexture (GL_TEXTURE9);  glDisable (GL_TEXTURE_2D);  dr_state_tex9  = 0;
    glActiveTexture (GL_TEXTURE10); glDisable (GL_TEXTURE_2D);  dr_state_tex10 = 0;
    glActiveTexture (GL_TEXTURE11); glDisable (GL_TEXTURE_2D);  dr_state_tex11 = 0;
    glActiveTexture (GL_TEXTURE12); glDisable (GL_TEXTURE_2D);  dr_state_tex12 = 0;
    glActiveTexture (GL_TEXTURE13); glDisable (GL_TEXTURE_2D);  dr_state_tex13 = 0;
    glActiveTexture (GL_TEXTURE14); glDisable (GL_TEXTURE_2D);  dr_state_tex14 = 0;
    glActiveTexture (GL_TEXTURE15); glDisable (GL_TEXTURE_2D);  dr_state_tex15 = 0;
    glActiveTexture (GL_TEXTURE16); glDisable (GL_TEXTURE_2D);  dr_state_tex16 = 0;

    for (i = 0; i < M_DR_MAX_CLIP_PLANES; i ++) {

        glDisable (GL_CLIP_PLANE0 + i); dr_state_planes [i] = false;
    }

    glMatrixMode (GL_MODELVIEW);  dr_state_matrixmode   = GL_MODELVIEW;
    glDepthMask (GL_TRUE);        dr_state_depthmask    = true;
    glEnable  (GL_DEPTH_TEST);    dr_state_depthtest    = true;
    glEnable  (GL_CULL_FACE);     dr_state_cullface     = true;
}

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// dr_Free
////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if 1

void		dr_Free (
    )
{

    // unload sun data

    dr_SunUnload ();

    // unload HDR data

    free (dr_intensitydata);

    // dimension arrays

    free (dr_w);
    free (dr_h);

    // int. display lists

    free (dr_quads);

    // auxiliary mipmaps

    free (dr_auxA);
    free (dr_auxB);
}

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// dr_AdjustAnisotrophy
////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if 1

VOID	dr_AdjustAnisotrophy (UINT_32 anisotrophy)
{
    anisotrophy = MAX (1,  anisotrophy);
    anisotrophy = MIN (16, anisotrophy);

    dr_control_anisotrophy = anisotrophy;

    for (UINT_32 i = 0; i < dr_materialsc; i ++) {

        if (dr_materials [i].diffuse    > 0) { glBindTexture (GL_TEXTURE_2D, dr_materials [i].diffuse);     glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, dr_control_anisotrophy); }
        if (dr_materials [i].weights1   > 0) { glBindTexture (GL_TEXTURE_2D, dr_materials [i].weights1);    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, dr_control_anisotrophy); }
        if (dr_materials [i].weights2   > 0) { glBindTexture (GL_TEXTURE_2D, dr_materials [i].weights2);    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, dr_control_anisotrophy); }
        if (dr_materials [i].weights3   > 0) { glBindTexture (GL_TEXTURE_2D, dr_materials [i].weights3);    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, dr_control_anisotrophy); }
        if (dr_materials [i].weights4   > 0) { glBindTexture (GL_TEXTURE_2D, dr_materials [i].weights4);    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, dr_control_anisotrophy); }
        if (dr_materials [i].composite  > 0) { glBindTexture (GL_TEXTURE_2D, dr_materials [i].composite);   glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, dr_control_anisotrophy); }
    }
}

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// dr_AdjustGamma
////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if 1

VOID	dr_AdjustGamma (BOOL enable)
{
    if (enable == dr_control_gammacorrection)  return;
                  dr_control_gammacorrection = enable;

    if (dr_control_gammacorrection) {

        for (UINT_32 i = 0; i < dr_materialsc; i ++) {

            if (dr_materials [i].diffuse > 0) { 

                glBindTexture (GL_TEXTURE_2D, dr_materials [i].diffuse);

                INT_32 w;   glGetTexLevelParameteriv (GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH,           &w);
                INT_32 h;   glGetTexLevelParameteriv (GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT,          &h);
                INT_32 f;   glGetTexLevelParameteriv (GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &f);

                VOIDP data = NULL;

                switch (f) {
                    case GL_RGB8:                           data = malloc (w*h*SIZE_UINT_8*3);
                        glGetTexImage (GL_TEXTURE_2D, 0, GL_RGB,  GL_UNSIGNED_BYTE, (VOIDP) data);  glTexImage2D (GL_TEXTURE_2D, 0, GL_SRGB8_EXT,                           w, h, 0, GL_RGB,   GL_UNSIGNED_BYTE, data); break;
                    case GL_RGBA8:                          data = malloc (w*h*SIZE_UINT_8*4);
                        glGetTexImage (GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, (VOIDP) data);  glTexImage2D (GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8_EXT,                    w, h, 0, GL_RGBA,  GL_UNSIGNED_BYTE, data); break;
                    case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:   data = malloc (w*h*SIZE_UINT_8*3);
                        glGetTexImage (GL_TEXTURE_2D, 0, GL_RGB,  GL_UNSIGNED_BYTE, (VOIDP) data);  glTexImage2D (GL_TEXTURE_2D, 0, GL_COMPRESSED_SRGB_S3TC_DXT1_EXT,       w, h, 0, GL_RGB,   GL_UNSIGNED_BYTE, data); break;
                    case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:  data = malloc (w*h*SIZE_UINT_8*4);
                        glGetTexImage (GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, (VOIDP) data);  glTexImage2D (GL_TEXTURE_2D, 0, GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT, w, h, 0, GL_RGBA,  GL_UNSIGNED_BYTE, data); break;
                    case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:  data = malloc (w*h*SIZE_UINT_8*4);
                        glGetTexImage (GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, (VOIDP) data);  glTexImage2D (GL_TEXTURE_2D, 0, GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT, w, h, 0, GL_RGBA,  GL_UNSIGNED_BYTE, data); break;
                }

                free (data);
            }
        }

    } else {

        for (UINT_32 i = 0; i < dr_materialsc; i ++) {

            if (dr_materials [i].diffuse > 0) { 

                glBindTexture (GL_TEXTURE_2D, dr_materials [i].diffuse);

                INT_32 w;	glGetTexLevelParameteriv (GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH,           &w);
                INT_32 h;	glGetTexLevelParameteriv (GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT,          &h);
                INT_32 f;	glGetTexLevelParameteriv (GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &f);

                VOIDP data = NULL;

                switch (f) {
                    case GL_SRGB8_EXT:                              data = malloc (w*h*SIZE_UINT_8*3);
                        glGetTexImage (GL_TEXTURE_2D, 0, GL_RGB,  GL_UNSIGNED_BYTE, (VOIDP) data); glTexImage2D (GL_TEXTURE_2D, 0, GL_RGB8,                             w, h, 0, GL_RGB,   GL_UNSIGNED_BYTE, data); break;
                    case GL_SRGB8_ALPHA8_EXT:                       data = malloc (w*h*SIZE_UINT_8*4);
                        glGetTexImage (GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, (VOIDP) data); glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA8,                            w, h, 0, GL_RGBA,  GL_UNSIGNED_BYTE, data); break;
                    case GL_COMPRESSED_SRGB_S3TC_DXT1_EXT:          data = malloc (w*h*SIZE_UINT_8*3);
                        glGetTexImage (GL_TEXTURE_2D, 0, GL_RGB,  GL_UNSIGNED_BYTE, (VOIDP) data); glTexImage2D (GL_TEXTURE_2D, 0, GL_COMPRESSED_RGB_S3TC_DXT1_EXT,     w, h, 0, GL_RGB,   GL_UNSIGNED_BYTE, data); break;
                    case GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT:    data = malloc (w*h*SIZE_UINT_8*4);
                        glGetTexImage (GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, (VOIDP) data); glTexImage2D (GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,    w, h, 0, GL_RGBA,  GL_UNSIGNED_BYTE, data); break;
                    case GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT:    data = malloc (w*h*SIZE_UINT_8*4);
                        glGetTexImage (GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, (VOIDP) data); glTexImage2D (GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,    w, h, 0, GL_RGBA,  GL_UNSIGNED_BYTE, data); break;
                }

                free (data);
            }
        }
    }
}

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// dr_AdjustShadowsRes
////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if 1

VOID	dr_AdjustShadowsRes (UINT_32 res)
{
    res = MAX (256,  res);
    res = MIN (4096, res);

    dr_SunUnload    (); dr_control_sun_res    = res;
    dr_SunLoad      ();
    dr_SunInit      ();
}

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// dr_AdjustShadowsSplits
////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if 1

VOID	dr_AdjustShadowsSplits (UINT_32 splits)
{
    splits = MAX (3, splits);
    splits = MIN (8, splits);

    dr_SunUnload    (); dr_control_sun_splits    = splits;
    dr_SunLoad      ();
    dr_SunInit      ();
}

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// dr_AdjustShadowsOffset
////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if 1

VOID	dr_AdjustShadowsOffset (FLOAT_32 offset)
{
    dr_SunUnload    (); dr_control_sun_offset    = offset;
    dr_SunLoad      ();
    dr_SunInit      ();
}

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// dr_AdjustShadowsTransition
////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if 1

VOID	dr_AdjustShadowsTransition (FLOAT_32 transition)
{
    dr_SunUnload    (); dr_control_sun_transition    = transition;
    dr_SunLoad      ();
    dr_SunInit      ();
}

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// dr_AdjustShadowsDistribution
////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if 1

VOID	dr_AdjustShadowsDistribution (FLOAT_32 distribution)
{
    distribution = MAX (1.0f,   distribution);
    distribution = MIN (100.0f, distribution);

    dr_SunUnload    (); dr_control_sun_distribution    = distribution;
    dr_SunLoad      ();
    dr_SunInit      ();
}

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// dr_AdjustShadowsScheme
////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if 1

VOID	dr_AdjustShadowsScheme (FLOAT_32 scheme)
{
    scheme = MAX (0.0f, scheme);
    scheme = MIN (1.0f, scheme);

    dr_SunUnload    (); dr_control_sun_scheme    = scheme;
    dr_SunLoad      ();
    dr_SunInit      ();
}

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// dr_AdjustShadowsZoom
////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if 1

VOID	dr_AdjustShadowsZoom (FLOAT_32 zoom)
{
    dr_SunUnload    (); dr_control_sun_zoom    = zoom;
    dr_SunLoad      ();
    dr_SunInit      ();
}

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// dr_AdjustShadowsDebug
////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if 1

VOID	dr_AdjustShadowsDebug (BOOL debug)
{
    dr_SunUnload    (); dr_control_sun_debug    = debug;
    dr_SunLoad      ();
    dr_SunInit      ();
}

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// dr_AdjustBloomRes
////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if 1

VOID	dr_AdjustBloomRes (UINT_32 res)
{
    res = MAX (0,                   res);
    res = MIN (dr_control_mip - 1,  res);

    dr_control_bloom_res = res;

    // unload shader

    lo_UnloadShaders (dr_program_hdr, dr_program_hdrv, dr_program_hdrf);

    // load new shader

    CHAR prefix [64];   sprintf (prefix, "\n#define M_MINLEVEL %i\n#define M_MAXLEVEL %i\n", dr_control_bloom_res, dr_control_mip - 1);

    lo_LoadShaders ("Shaders\\dr_hdr.vert",
        "Shaders\\dr_hdr.frag", prefix, 
        &dr_program_hdr,
        &dr_program_hdrv,
        &dr_program_hdrf);

    glUseProgram  (dr_program_hdr);
    glUniform1i  (glGetUniformLocation (dr_program_hdr,         "base"),   0);

    if ((dr_control_bloom_res <= 0) && (dr_control_mip >= 1)) 
        glUniform1i  (glGetUniformLocation (dr_program_hdr,     "tex0"),   1);
    if ((dr_control_bloom_res <= 1) && (dr_control_mip >= 2)) 
        glUniform1i  (glGetUniformLocation (dr_program_hdr,     "tex1"),   2);
    if ((dr_control_bloom_res <= 2) && (dr_control_mip >= 3)) 
        glUniform1i  (glGetUniformLocation (dr_program_hdr,     "tex2"),   3);
    if ((dr_control_bloom_res <= 3) && (dr_control_mip >= 4)) 
        glUniform1i  (glGetUniformLocation (dr_program_hdr,     "tex3"),   4);
    if ((dr_control_bloom_res <= 4) && (dr_control_mip >= 5)) 
        glUniform1i  (glGetUniformLocation (dr_program_hdr,     "tex4"),   5);
    if ((dr_control_bloom_res <= 5) && (dr_control_mip >= 6)) 
        glUniform1i  (glGetUniformLocation (dr_program_hdr,     "tex5"),   6);
    if ((dr_control_bloom_res <= 6) && (dr_control_mip >= 7)) 
        glUniform1i  (glGetUniformLocation (dr_program_hdr,     "tex6"),   7);
    if ((dr_control_bloom_res <= 7) && (dr_control_mip >= 8)) 
        glUniform1i  (glGetUniformLocation (dr_program_hdr,     "tex7"),   8);
    if ((dr_control_bloom_res <= 8) && (dr_control_mip >= 9)) 
        glUniform1i  (glGetUniformLocation (dr_program_hdr,     "tex8"),   9);
}

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// dr_AdjustBloomStrength
////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if 1

VOID	dr_AdjustBloomStrength (FLOAT_32 strength)
{
    strength = MAX (0.0f,   strength);
    strength = MIN (10.0f,  strength);

    dr_control_bloom_strength = strength;
}

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// dr_AdjustSSAORes
////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if 1

VOID    dr_AdjustSSAORes (UINT_32 res)
{
    res = MAX (0,                   res);
    res = MIN (dr_control_mip - 1,  res);

    dr_control_ssao_res = res;

    glUseProgram    (dr_program_ssao_blurhblend);
    glUniform1f     (glGetUniformLocation (dr_program_ssao_blurvert, "scalex"), (FLOAT_32) dr_w [dr_control_ssao_res] / dr_width);
    glUniform1f     (glGetUniformLocation (dr_program_ssao_blurvert, "scaley"), (FLOAT_32) dr_h [dr_control_ssao_res] / dr_height);
}

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// dr_AdjustMIP
////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if 1

VOID    dr_AdjustMIP (UINT_32 level)
{
    level = MAX (1, level);
    level = MIN (9, level);

    for (UINT_32 i = 0; i < dr_control_mip; i ++) {

        glDeleteLists (dr_quads [i], 1);
    }

    glDeleteTextures    (dr_control_mip, dr_auxA);
    glDeleteTextures    (dr_control_mip, dr_auxB);

    free (dr_intensitydata);

    free (dr_auxA);
    free (dr_auxB);

    free (dr_w);
    free (dr_h);

    dr_control_mip = level;

    dr_w = (UINT_32P) malloc (SIZE_UINT_32 * dr_control_mip);
    dr_h = (UINT_32P) malloc (SIZE_UINT_32 * dr_control_mip);

    UINT_32 fraction = 1;
    
    for (UINT_32 i = 0; i < dr_control_mip; i ++) {

        dr_w [i] = dr_width  / fraction;
        dr_h [i] = dr_height / fraction;

        fraction *= 2;
    }

    dr_auxA = (UINT_32P) malloc (SIZE_UINT_32 * dr_control_mip);
    dr_auxB = (UINT_32P) malloc (SIZE_UINT_32 * dr_control_mip);

    for (UINT_32 i = 0; i < dr_control_mip; i ++) {

        glGenTextures    (1, &dr_auxA [i]);
        glBindTexture    (GL_TEXTURE_RECTANGLE_ARB, dr_auxA [i]);
        glTexParameteri  (GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri  (GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri  (GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri  (GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D     (GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA8, dr_w [i],  dr_h [i],  0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

        glGenTextures    (1, &dr_auxB [i]);
        glBindTexture    (GL_TEXTURE_RECTANGLE_ARB, dr_auxB [i]);
        glTexParameteri  (GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri  (GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri  (GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri  (GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D     (GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA8, dr_w [i],  dr_h [i],  0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    }

    dr_quads = (UINT_32P) malloc (SIZE_UINT_32 * dr_control_mip);

    for (UINT_32 i = 0; i < dr_control_mip; i ++) {

        dr_quads [i] = glGenLists (1);

        glNewList (dr_quads [i], GL_COMPILE);
        glBegin (GL_QUADS);
        glMultiTexCoord2f (GL_TEXTURE0,  0.0f,		            0.0f);
        glVertex3f ( - 1.0f, - 1.0f, 0.0f);
        glMultiTexCoord2f (GL_TEXTURE0, (FLOAT_32) dr_w [i],	0.0f);
        glVertex3f (   1.0f, - 1.0f, 0.0f);
        glMultiTexCoord2f (GL_TEXTURE0, (FLOAT_32) dr_w [i],	(FLOAT_32) dr_h [i]);
        glVertex3f (   1.0f,   1.0f, 0.0f);
        glMultiTexCoord2f (GL_TEXTURE0,  0.0f,					(FLOAT_32) dr_h [i]);
        glVertex3f ( - 1.0f,   1.0f, 0.0f);
        glEnd ();
        glEndList ();
    }

    dr_intensitydata = (UINT_8P) malloc (SIZE_UINT_8 * (dr_intensitysize =  dr_w [dr_control_mip - 1] * 
                                                                            dr_h [dr_control_mip - 1]) * 2);		/// * 2 (crashes sometimes)

    dr_AdjustBloomRes   (MIN (dr_control_bloom_res, level));
    dr_AdjustSSAORes    (MIN (dr_control_ssao_res,  level));
}

#endif
