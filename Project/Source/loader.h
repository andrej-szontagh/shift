
#ifndef _LOADER_H
#define _LOADER_H

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CONSTATNS
////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define M_LOADER_MESHPATH       "Data\\meshes\\"
#define M_LOADER_IMAGEPATH      "Data\\images\\"
#define M_LOADER_SHADERPATH     "Data\\shaders\\"
#define M_LOADER_INSTANCESPATH  "Data\\instances\\"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FUNCTIONS
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// SHADERS

UINT_32 lo_LoadShaders  (
                         
                CHARP       filev,
                CHARP       filef,
                CHARP       prefix,
                UINT_32P    idp,
                UINT_32P    idv,
                UINT_32P    idf
    );

VOID lo_UnloadShaders (

                UINT_32 prog,
                UINT_32 vert,
                UINT_32 frag
    );

// IMAGES

INT_32 lo_LoadImage (CHARP filename, TImage *image);

// WORLD

INT_32 lo_LoadWorld (CHARP filename);

VOID lo_UnloadWorld ();

#endif
