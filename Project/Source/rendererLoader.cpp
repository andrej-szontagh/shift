
////////////////////////////////////////////////////////////////////////////////////////////////////////////
// dr_Init
////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if 1

VOID	dr_Init (

                UINT_32 width, UINT_32 height
    )
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

    dr_enableautoexposure	= true;
    dr_enablebloom			= true;
    dr_enablessao			= true;
    dr_enablefog			= true;
    dr_enableaa			    = true;

    dr_levelculling = 1;
    dr_levelbloom	= 0;
    dr_levelssao	= 0;

    dr_detailculling = M_DR_CULLING_SAMPLES;

    // DIMENSIONS

    dr_width = width;	dr_height = height;

    UINT_32 fraction = 1;

    for (i = 0; i < M_DR_MIPLEVELS; i ++) {

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
    glTexImage2D     (GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA16F, dr_width, dr_height, 0, GL_RGBA, GL_FLOAT, NULL);

    // G2

    glGenTextures    (1, &dr_G2);
    glBindTexture    (GL_TEXTURE_RECTANGLE_ARB, dr_G2);
    glTexParameteri  (GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri  (GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri  (GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri  (GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D     (GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA16F, dr_width, dr_height, 0, GL_RGBA, GL_FLOAT, NULL);

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

    for (i = 0; i < M_DR_MIPLEVELS; i ++) {

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

    // SUN SHADOWMAP

    FLOAT_32 bordercolor [4] = {1.0, 1.0, 1.0, 1.0};

    for (i = 0; i < M_DR_SUN_SPLITS; i ++) {

        glGenTextures    (1, &dr_sunshadows [i]);
        glBindTexture    (GL_TEXTURE_2D, dr_sunshadows [i]);
        glTexParameteri  (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri  (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri  (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri  (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glTexParameteri  (GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
        glTexParameteri  (GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
        glTexParameterfv (GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, bordercolor);
        glTexImage2D     (GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, M_DR_SUN_SHADOW, M_DR_SUN_SHADOW, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);
    }

    glGenTextures    (1, &dr_sunshadowtmp);
    glBindTexture    (GL_TEXTURE_2D, dr_sunshadowtmp);
    glTexParameteri  (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri  (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri  (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri  (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri  (GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
    glTexParameteri  (GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
    glTexParameterfv (GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, bordercolor);
    glTexImage2D     (GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, M_DR_SUN_SHADOW, M_DR_SUN_SHADOW, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);


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

    // SUN SHADOWMAP

    glGenFramebuffersEXT  (1, &dr_framebuffer_sun);
    glGenRenderbuffersEXT (1, &dr_renderbuffer_sun);
    
    glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, dr_framebuffer_sun);

    glBindRenderbufferEXT		 (GL_RENDERBUFFER_EXT, dr_renderbuffer_sun);
    glRenderbufferStorageEXT	 (GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT32, M_DR_SUN_SHADOW, M_DR_SUN_SHADOW);
    glFramebufferRenderbufferEXT (GL_FRAMEBUFFER_EXT,  GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, dr_renderbuffer_sun);

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
    // sun
    lo_LoadShaders ("Shaders\\dr_sun.vert",
                    "Shaders\\dr_sun.frag", "\n", 
                    &dr_program_sun,
                    &dr_program_sunv,
                    &dr_program_sunf);

    INT_32 units [M_DR_SUN_SPLITS] = { 3,4,5,6,7 };

    glUseProgram    (dr_program_sun);
    glUniform1i     (glGetUniformLocation (dr_program_sun, "tex_G1"),       0);
    glUniform1i     (glGetUniformLocation (dr_program_sun, "tex_G2"),       1);
    glUniform1i     (glGetUniformLocation (dr_program_sun, "tex_rand"),     2);
    glUniform1iv    (glGetUniformLocation (dr_program_sun, "tex_shadow"),   M_DR_SUN_SPLITS, units);
    glUniform1f     (glGetUniformLocation (dr_program_sun, "saturate"),     M_DR_SUN_SATURATE);
    glUniform1f     (glGetUniformLocation (dr_program_sun, "intensity"),    dr_sun_intensity);
    glUniform1f     (glGetUniformLocation (dr_program_sun, "ambient"),      dr_sun_ambient);
    glUniform1f     (glGetUniformLocation (dr_program_sun, "width"),        (FLOAT_32) dr_width);
    glUniform1f     (glGetUniformLocation (dr_program_sun, "height"),       (FLOAT_32) dr_height);
    glUniform1f     (glGetUniformLocation (dr_program_sun, "invsize"),      1.0f / (FLOAT_32) M_DR_SUN_SHADOW);
    glUniform1f     (glGetUniformLocation (dr_program_sun, "farplane"),     dr_planefar);
    glUniform1f     (glGetUniformLocation (dr_program_sun, "farw"),         (FLOAT_32) dr_farw);
    glUniform1f     (glGetUniformLocation (dr_program_sun, "farh"),         (FLOAT_32) dr_farh);

    dr_program_sun_matrix = glGetUniformLocation (dr_program_sun, "matrix");

    /////////////////////////////
    // fog
    lo_LoadShaders ("Shaders\\dr_fog.vert",
                    "Shaders\\dr_fog.frag", "\n", 
                    &dr_program_fog,
                    &dr_program_fogv,
                    &dr_program_fogf);

    glUseProgram    (dr_program_fog);
    glUniform1i     (glGetUniformLocation (dr_program_fog, "tex_G2"),       0);
    glUniform1i     (glGetUniformLocation (dr_program_fog, "tex"),          1);
    glUniform1f     (glGetUniformLocation (dr_program_fog, "farplane"),     dr_planefar);
    glUniform1f     (glGetUniformLocation (dr_program_fog, "farw"),         (FLOAT_32) dr_farw);
    glUniform1f     (glGetUniformLocation (dr_program_fog, "farh"),         (FLOAT_32) dr_farh);

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
    glUniform1i     (glGetUniformLocation (dr_program_aa_blurvert,     "tex"),          0);
    glUniform1i     (glGetUniformLocation (dr_program_aa_blurvert,     "tex_depth"),    1);
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
    glUniform1i     (glGetUniformLocation (dr_program_aa_blurhoriz,    "tex"),          0);
    glUniform1i     (glGetUniformLocation (dr_program_aa_blurhoriz,    "tex_depth"),    1);
    glUniform1f     (glGetUniformLocation (dr_program_aa_blurhoriz,    "nom"),          2.0f * dr_planenear);
    glUniform1f     (glGetUniformLocation (dr_program_aa_blurhoriz,    "denom1"),       dr_planefar + dr_planenear);
    glUniform1f     (glGetUniformLocation (dr_program_aa_blurhoriz,    "denom2"),       dr_planefar - dr_planenear);

    //////////////////////////////////////////////////////////////////////////////
    // HDR SHADERS
    //////////////////////////////////////////////////////////////////////////////

    /////////////////////////////
    // HDR
    CHAR prefix [64];   sprintf (prefix, "\n#define M_MINLEVEL %i\n#define M_MAXLEVEL %i\n", dr_levelbloom, M_DR_MIPLEVELS - 1);

    lo_LoadShaders ("Shaders\\dr_hdr.vert",
                    "Shaders\\dr_hdr.frag", prefix, 
                    &dr_program_hdr,
                    &dr_program_hdrv,
                    &dr_program_hdrf);

    glUseProgram  (dr_program_hdr);
    glUniform1i  (glGetUniformLocation (dr_program_hdr,         "base"),   0);
    
    if ((dr_levelbloom <= 0) && (M_DR_MIPLEVELS >= 1)) 
        glUniform1i  (glGetUniformLocation (dr_program_hdr,     "tex0"),   1);
    if ((dr_levelbloom <= 1) && (M_DR_MIPLEVELS >= 2)) 
        glUniform1i  (glGetUniformLocation (dr_program_hdr,     "tex1"),   2);
    if ((dr_levelbloom <= 2) && (M_DR_MIPLEVELS >= 3)) 
        glUniform1i  (glGetUniformLocation (dr_program_hdr,     "tex2"),   3);
    if ((dr_levelbloom <= 3) && (M_DR_MIPLEVELS >= 4)) 
        glUniform1i  (glGetUniformLocation (dr_program_hdr,     "tex3"),   4);
    if ((dr_levelbloom <= 4) && (M_DR_MIPLEVELS >= 5)) 
        glUniform1i  (glGetUniformLocation (dr_program_hdr,     "tex4"),   5);
    if ((dr_levelbloom <= 5) && (M_DR_MIPLEVELS >= 6)) 
        glUniform1i  (glGetUniformLocation (dr_program_hdr,     "tex5"),   6);
    if ((dr_levelbloom <= 6) && (M_DR_MIPLEVELS >= 7)) 
        glUniform1i  (glGetUniformLocation (dr_program_hdr,     "tex6"),   7);

    glUniform1f  (glGetUniformLocation (dr_program_hdr,        "scale"),   1.0);
    glUniform1f  (glGetUniformLocation (dr_program_hdr,        "bloom"),   M_DR_HDR_BLOOM);

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
    glUniform1f     (glGetUniformLocation (dr_program_hdr_hipass,     "scale"),     1.0);


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
    glUniform1i     (glGetUniformLocation (dr_program_ssao,     "tex_G2"),      0);
    glUniform1i     (glGetUniformLocation (dr_program_ssao,     "tex_rand"),    1);
    glUniform1i     (glGetUniformLocation (dr_program_ssao,     "tex_shade"),   2);
    glUniform1f     (glGetUniformLocation (dr_program_ssao,     "randscale"),   M_DR_SSAO_RANDSCALE);
    glUniform1f     (glGetUniformLocation (dr_program_ssao,     "farplane"),    dr_planefar);
    glUniform1f     (glGetUniformLocation (dr_program_ssao,     "farw"),        (FLOAT_32) dr_farw);
    glUniform1f     (glGetUniformLocation (dr_program_ssao,     "farh"),        (FLOAT_32) dr_farh);


    /////////////////////////////
    // vertical blur
    lo_LoadShaders ("Shaders\\dr_ssao_blurv.vert",
                    "Shaders\\dr_ssao_blurv.frag", "\n", 
                    &dr_program_ssao_blurvert,
                    &dr_program_ssao_blurvertv,
                    &dr_program_ssao_blurvertf);

    glUseProgram    (dr_program_ssao_blurvert);
    glUniform1i     (glGetUniformLocation (dr_program_ssao_blurvert,    "tex"),     0);
    glUniform1i     (glGetUniformLocation (dr_program_ssao_blurvert,    "tex_G2"),  1);

    /////////////////////////////
    // debug
    lo_LoadShaders ("Shaders\\dr_ssao_debug.vert",
                    "Shaders\\dr_ssao_debug.frag", "\n", 
                    &dr_program_ssao_debug,
                    &dr_program_ssao_debugv,
                    &dr_program_ssao_debugf);

    glUseProgram    (dr_program_ssao_debug);
    glUniform1i     (glGetUniformLocation (dr_program_ssao_debug,       "tex"),     0);

    /////////////////////////////
    // horizontal blur and blend
    lo_LoadShaders ("Shaders\\dr_ssao_blurhblend.vert",
                    "Shaders\\dr_ssao_blurhblend.frag", "\n", 
                    &dr_program_ssao_blurhblend,
                    &dr_program_ssao_blurhblendv,
                    &dr_program_ssao_blurhblendf);

    glUseProgram    (dr_program_ssao_blurhblend);
    glUniform1i     (glGetUniformLocation (dr_program_ssao_blurhblend,  "tex_vblur"),   0);
    glUniform1i     (glGetUniformLocation (dr_program_ssao_blurhblend,  "tex_raw"),     1);
    glUniform1i     (glGetUniformLocation (dr_program_ssao_blurhblend,  "tex_G2"),      2);
    glUniform1f     (glGetUniformLocation (dr_program_ssao_blurhblend,  "scalex"),      (FLOAT_32) dr_w [dr_levelssao] / dr_width);
    glUniform1f     (glGetUniformLocation (dr_program_ssao_blurhblend,  "scaley"),      (FLOAT_32) dr_h [dr_levelssao] / dr_height);

    //////////////////////////////////////////////////////////////////////////////
    // RENDER LISTS
    //////////////////////////////////////////////////////////////////////////////

    for (i = 0; i < M_DR_MIPLEVELS; i ++) {

        dr_quads [i] = glGenLists (1);

        /// REMOVE SECOND UV .. CAUSE THEY ARE THE SAME AS gl_Vertex !!

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

    dr_intensityscale = 0.0;

    // storage for intensity sample
    dr_intensitydata = (UINT_8P) malloc (SIZE_UINT_8 * (dr_intensitysize = dr_w [M_DR_MIPLEVELS - 1] * dr_h [M_DR_MIPLEVELS - 1]) * 2);		/// * 2 (crashes sometimes)

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

    dr_SunInit ();

    glUseProgram    (dr_program_sun);
    
    glUniform1fv    (glGetUniformLocation (dr_program_sun, "offset"),   M_DR_SUN_SPLITS,    &dr_sun_offsets  [0]);
    glUniform1fv    (glGetUniformLocation (dr_program_sun, "depthmin"), M_DR_SUN_SPLITS,    &dr_sun_depthmin [0]);
    glUniform1fv    (glGetUniformLocation (dr_program_sun, "depthmax"), M_DR_SUN_SPLITS,    &dr_sun_depthmax [0]);
    glUniform1f     (glGetUniformLocation (dr_program_sun, "depthend"),                      dr_sun_splitend);

    for (UINT_32 i = 0; i < dr_objectsc; i ++) {

        UINT_32 split = 0;
        for (;  split < dr_sun_count; split ++) {

            if (dr_object_disappear_shadow [i] <= dr_sun_splits [split][0]) break;
        }

        dr_object_disappear_split [i] = (UINT_8) split;
    }

    //////////////////////////////////////////////////////////////////////////////
    // ENVIROMENT
    //////////////////////////////////////////////////////////////////////////////

    glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);

    //////////////////////////////////////////////////////////////////////////////
    // STATES
    //////////////////////////////////////////////////////////////////////////////

    dr_state_color   = 0;
    dr_state_colors  = 0;
    dr_state_vertex  = 0;
    dr_state_normal  = 0;
    dr_state_indices = 0;

    dr_state_tex0 = 0;
    dr_state_tex1 = 0;
    dr_state_tex2 = 0;
    dr_state_tex3 = 0;
    dr_state_tex4 = 0;
    dr_state_tex5 = 0;
    dr_state_tex6 = 0;
    dr_state_tex7 = 0;
    dr_state_tex8 = 0;
    dr_state_tex9 = 0;

    dr_state_tex0_rect = 0;
    dr_state_tex1_rect = 0;
    dr_state_tex2_rect = 0;

    dr_state_tex0_coord = 0;
    dr_state_tex1_coord = 0;
    dr_state_tex2_coord = 0;

    for (i = 0; i < M_DR_MAX_CLIP_PLANES; i ++) {

        dr_state_planes [i] = 0;
    }

    glGetIntegerv (GL_MATRIX_MODE,     (GLint *)     &dr_state_matrixmode);
    glGetBooleanv (GL_DEPTH_WRITEMASK, (GLboolean *) &dr_state_depthmask);

    dr_state_cullface   = glIsEnabled (GL_CULL_FACE);
    dr_state_depthtest  = glIsEnabled (GL_DEPTH_TEST);
}

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// dr_Free
////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if 1

void		dr_Free (
    )
{

    // unload HDR data

    free (dr_intensitydata);

}

#endif
