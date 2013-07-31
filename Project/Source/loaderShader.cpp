
////////////////////////////////////////////////////////////////////////////////////////////////////////////
// lo_PrintShadersLog
////////////////////////////////////////////////////////////////////////////////////////////////////////////

VOID lo_PrintShadersLog	(UINT_32 id) {

    INT_32 buflen;

    glGetShaderiv (id, GL_INFO_LOG_LENGTH, &buflen);

    if (buflen > 1) {

        CHARP log_string = (CHARP) malloc (buflen + 1);

        glGetShaderInfoLog (id, buflen, 0, log_string);

        debug_Print (log_string);

        free (log_string);
    }        
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// lo_LoadShader
////////////////////////////////////////////////////////////////////////////////////////////////////////////

UINT_32 lo_LoadShader	(
                         
                 CHARP		file,
                 CHARP		prefix,
                 UINT_32    type,
                 UINT_32P	id
    )
{
    
    debug_PrintIntend ("Loading ", file);
    
    // open shader file
    FILE * f = fopen (file, "rb");
    
    if (f == NULL) {
        
        debug_Print (" .. FAILED\n");
        return -1;
    }
    
    debug_Print (" .. OK");
    
    // get the file size
    fseek (f, 0, SEEK_END); 
    
    UINT_32 l = ftell (f);  rewind (f);

    CHARP str1 = (CHARP) malloc (l + 1);

    // read shader file
    fread ((VOIDP) str1, 1, l, f);	str1 [l] = 0;

    fclose (f);

    // adding prefix into shader (for macros)
    CHARP str2 = (CHARP) malloc (strlen (prefix) + strlen (str1) + 1);
        
    sprintf ((CHARP) str2, "%s%s", prefix, str1);

    // creating shader
    * id  = glCreateShader (type);

    // compiling
    glShaderSource (* id, 1, (const CHARPP) &str2, NULL);
    
    debug_Print ("    Compiling");
    
    glCompileShader (* id);    
    
    INT_32 status;    
    glGetShaderiv (* id, GL_COMPILE_STATUS, &status);
    
    if (status == GL_FALSE) {
        
        debug_Print (" .. FAILED\n\n");
        
        lo_PrintShadersLog (* id);
        
        return -1;
    }
    
    debug_Print (" .. OK\n");
    
    lo_PrintShadersLog (* id);
    
    free (str1);
    free (str2);
    
    return (* id);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// lo_LoadShaders
////////////////////////////////////////////////////////////////////////////////////////////////////////////

UINT_32 lo_LoadShaders	(
                         
                 CHARP		filev,
                 CHARP		filef,
                 CHARP		prefix,
                 UINT_32P	idp, 
                 UINT_32P	idv, 
                 UINT_32P	idf
    )
{
    
    lo_LoadShader (filev, prefix, GL_VERTEX_SHADER,     idv);
    lo_LoadShader (filef, prefix, GL_FRAGMENT_SHADER,   idf);
    
    * idp = glCreateProgram ();
    
    glAttachShader (* idp, * idv);
    glAttachShader (* idp, * idf);
    
    // linking    
    debug_PrintIntend ("Linking Program");
    
    glLinkProgram (* idp);
    
    INT_32 status;
    glGetProgramiv (* idp, GL_LINK_STATUS, &status);
    
    if (status == GL_FALSE) {
        
        debug_Print (" .. FAILED\n\n");
        return -1;
    }
    
    debug_Print (" .. OK\n\n");
        
    return (* idp);    
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// lo_UnloadShaders
////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
