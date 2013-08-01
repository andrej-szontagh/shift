
#ifndef _SHADER_LOADER_H
#define _SHADER_LOADER_H

#include "types.h"

UINT_32 lo_LoadShaders	(
                         
    CHARP		filev,
    CHARP		filef,
    CHARP		prefix,
    UINT_32P	idp, 
    UINT_32P	idv, 
    UINT_32P	idf
);

UINT_32 lo_LoadShaders	(
                         
    CHARP		shaderv,
    CHARP		shaderf,
    UINT_32P	idp, 
    UINT_32P	idv, 
    UINT_32P	idf
);

VOID lo_UnloadShaders (

    UINT_32     prog,
    UINT_32     vert,
    UINT_32     frag
);

#endif

