
#ifndef _DEBUG_H
#define _DEBUG_H

#include "types.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CONSTANTS
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// #define M_DEBUG
// #define M_DEBUG_PROFILE_GPU
// #define M_DEBUG_PROFILE_FILE

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FUNCTIONS
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

INT_32  debug_Init          ();
VOID    debug_Destroy       ();
VOID    debug_Print         (CHARP msg);
VOID    debug_Print         (CHARP msg, CHARP file, BOOL lineend);
VOID    debug_Print         (CHARP msg, UINT_32 code, BOOL lineend);
VOID    debug_PrintIntend   (CHARP msg);
VOID    debug_PrintIntend   (CHARP msg, CHARP file);

///////////////////////////////////////////////////////////
//
//  PARAMS :
//
//  id -   ID of timer to start (to handle overlapping timers)
//         Ranges from 0 to  M_DEBUG_MAX_TIMERS
//
//  DESCRIPTION :
//
//      Returns log file handler
//
///////////////////////////////////////////////////////////
VOID    debug_StartTimer        (UINT_32 id);
VOID    debug_StartTimerQuery   (UINT_32 id);

///////////////////////////////////////////////////////////
//
//  PARAMS :
//
//  id -   ID of timer to start (to handle overlapping timers)
//         Ranges from 0 to  M_DEBUG_MAX_TIMERS
//  msg -   Text message to identify timer in log file
//
//  DESCRIPTION :
//
//      Returns time delta (in ticks [ms])
//
///////////////////////////////////////////////////////////
VOID    debug_EndTimer          (UINT_32 id, CHARP msg, FLOAT_64 &elapsed);
VOID    debug_EndTimerQuery     (UINT_32 id, CHARP msg, FLOAT_64 &elapsed, FLOAT_64 &elapsedg);

#endif
