
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// INCLUDE
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define M_DR_BODY

#include     <stdlib.h>
#include      <stdio.h>
#include      <float.h>
#include       <math.h>
#include         <io.h>

#include       "glee.h"
#include      "types.h"
#include      "macro.h"
#include      "debug.h"
#include       "sort.h"
#include   "skeleton.h"
#include   "renderer.h"
#include     "loader.h"

#ifdef M_DEBUG
    extern      INT_32 debug;
#endif

#include    "rendererProcess.cpp"
#include    "rendererRoutines.cpp"
#include    "rendererSpecificGrass.cpp"
#include    "rendererSpecificFoliage.cpp"
#include    "rendererSpecificSolid.cpp"
#include    "rendererSpecificTerrain.cpp"

#include	"rendererGui.cpp"
#include    "rendererSun.cpp"
#include    "rendererSort.cpp"
#include    "rendererClip.cpp"
#include    "rendererCore.cpp"
#include    "rendererDraw.cpp"
#include    "rendererLoader.cpp"
