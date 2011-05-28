
////////////////////////////////////////////////////////////////////////////////////////////////////////////
// lo_LoadShaders
////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if 1

UINT_32 lo_LoadShaders	(
                         
                 CHARP		filev,
                 CHARP		filef,
                 CHARP		prefix,
                 UINT_32P	idp, 
                 UINT_32P	idv, 
                 UINT_32P	idf
    )
{
    UINT_32 l;

    FILE *f;

    CHARP vv;	CHARP ff;
    CHARP vs;	CHARP fs;

    // reading vertex shader file
    if ((f = fopen (filev, "rb")) == NULL)	return -1;

    vs = (CHARP) malloc (l = _filelength (_fileno(f)) + 1);

    fread ((VOIDP) vs, 1, l - 1, f);	vs [l - 1] = 0;

    fclose (f);

    // reading fragment shader file
    if ((f = fopen (filef, "rb")) == NULL)	return -1;

    fs = (CHARP) malloc (l = _filelength (_fileno(f)) + 1);

    fread ((VOIDP) fs, 1, l - 1, f);	fs [l - 1] = 0;

    fclose (f);

    // adding prefix (for macros)
    ff = (CHARP) malloc (strlen (prefix) + strlen (fs) + 1);	vv = vs;
        
    sprintf ((CHARP) ff, "%s%s", prefix, fs);

    // creating shaders
    * idv  = glCreateShader (GL_VERTEX_SHADER);
    * idf  = glCreateShader (GL_FRAGMENT_SHADER);	

    // compiling
    glShaderSource (* idv, 1, (const CHARPP) &vv, NULL); free (vs);
    glShaderSource (* idf, 1, (const CHARPP) &ff, NULL); free (fs); free (ff);
    
    glCompileShader (* idv);
    glCompileShader (* idf);
    
    * idp = glCreateProgram ();
    
    glAttachShader (* idp, * idv);
    glAttachShader (* idp, * idf);
    
    // linking
    glLinkProgram (* idp);

    return (* idp);
}

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// lo_UnloadShaders
////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if 1

VOID lo_UnloadShaders (

                UINT_32 prog,
                UINT_32 vert,
                UINT_32 frag
    )
{
    glDetachShader  (prog, vert);
    glDetachShader  (prog, frag);

    glDeleteShader  (vert);
    glDeleteShader  (frag);

    glDeleteProgram (prog);
}

#endif

