
////////////////////////////////////////////////////////////////////////////////////////////////////
// INCLUDE
////////////////////////////////////////////////////////////////////////////////////////////////////

#if 1

#include	   <stdlib.h>
#include	    <stdio.h>

#include	  <windows.h>

#include	     "glee.h"
#include	    "types.h"
#include	 "skeleton.h"
#include	    "debug.h"

#include	 "SDL_timer.h"

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////
// GLOBALS
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef WIN32

    FLOAT_64 startfreq  [M_DEBUG_MAX_TIMERS];
    UINT_64  starttime  [M_DEBUG_MAX_TIMERS];
#else

    UINT_32  starttime  [M_DEBUG_MAX_TIMERS];
#endif

FILE * flog;    UINT_32 timequery;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// debug_Init
////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if 1

VOID debug_Init ()
{
	flog = fopen (M_DEBUG_FILE, "w");

    glGenQueries (1, &timequery);
}

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// debug_Destroy
////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if 1

VOID debug_Destroy ()
{
    glDeleteQueries (1, &timequery);

	fclose (flog);
}

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// debug_StartTimer
////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if 1

VOID debug_StartTimer (UINT_32 id)
{
    #ifdef WIN32

        LARGE_INTEGER li;

        QueryPerformanceFrequency   (&li);  startfreq [id] = ((FLOAT_64) li.QuadPart) / 1000.0;
        QueryPerformanceCounter     (&li);

        starttime [id] = li.QuadPart;
    #else

    	starttime [id] = SDL_GetTicks ();
    #endif
}

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// debug_StartTimerQuery
////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if 1

VOID debug_StartTimerQuery (UINT_32 id)
{

    #ifdef M_DEBUG_PROFILE_GPU

	    glFinish ();

        glBeginQuery (GL_TIME_ELAPSED_EXT, timequery);

    #endif

    #ifdef WIN32

        LARGE_INTEGER li;

        QueryPerformanceFrequency   (&li);  startfreq [id] = ((FLOAT_64) li.QuadPart) / 1000.0;
        QueryPerformanceCounter     (&li);

        starttime [id] = li.QuadPart;
    #else

    	starttime [id] = SDL_GetTicks ();
    #endif
}

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// debug_EndTimer
////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if 1

VOID debug_EndTimer (UINT_32 id, CHARP msg, FLOAT_64 &elapsed)
{
    #ifdef WIN32

        LARGE_INTEGER li;   FLOAT_64 endfreq;

        QueryPerformanceFrequency   (&li);  endfreq = ((FLOAT_64) li.QuadPart) / 1000.0;

        if (endfreq == startfreq [id]) {

            QueryPerformanceCounter (&li);

            elapsed = ((FLOAT_64) (li.QuadPart - starttime [id])) / endfreq;

            #ifdef M_DEBUG_PROFILE_FILE
        	    fprintf	(flog, "%s (%Lf / %f ms) \n", msg, elapsed);
            	fflush	(flog);
            #endif

        } else {

            elapsed = 0.0;

            #ifdef M_DEBUG_PROFILE_FILE
            	fprintf	(flog, "%s (invalid / %f ms) \n", msg);
            	fflush	(flog);
            #endif
        }

    #else

    	elapsed = (FLOAT_64) (SDL_GetTicks () - starttime [id]);

        #ifdef M_DEBUG_PROFILE_FILE
    	    fprintf	(flog, "%s (%lu / %f ms) \n", msg, elapsed);
    	    fflush	(flog);
        #endif
    #endif
}

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// debug_EndTimerQuery
////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if 1

VOID debug_EndTimerQuery (UINT_32 id, CHARP msg, FLOAT_64 &elapsed, FLOAT_64 &elapsedg)
{
    #ifdef M_DEBUG_PROFILE_GPU

	    glFinish ();

        glEndQuery (GL_TIME_ELAPSED_EXT);   INT_32 available = 0;

        for (UINT_32 i = 0; ((i < 1000000) && (!available)); i ++)  glGetQueryObjectiv (timequery, GL_QUERY_RESULT_AVAILABLE, &available);

        UINT_64 delta;  glGetQueryObjectui64vEXT (timequery, GL_QUERY_RESULT, &delta);

        elapsedg = (FLOAT_64) delta / 1000000.0;

    #endif

    #ifdef WIN32

        LARGE_INTEGER li;   FLOAT_64 endfreq;

        QueryPerformanceFrequency   (&li);  endfreq = ((FLOAT_64) li.QuadPart) / 1000.0;

        if (endfreq == startfreq [id]) {

            QueryPerformanceCounter (&li);

            elapsed = ((FLOAT_64) (li.QuadPart - starttime [id])) / endfreq;

            #ifdef M_DEBUG_PROFILE_FILE
        	    fprintf	(flog, "%s (%Lf / %f ms) \n", msg, elapsed, elapsedg);
            	fflush	(flog);
            #endif

        } else {

            elapsed = 0.0;

            #ifdef M_DEBUG_PROFILE_FILE
            	fprintf	(flog, "%s (invalid / %f ms) \n", msg, elapsedg);
            	fflush	(flog);
            #endif
        }

    #else

    	elapsed = (FLOAT_64) (SDL_GetTicks () - starttime [id]);

        #ifdef M_DEBUG_PROFILE_FILE
    	    fprintf	(flog, "%s (%lu / %f ms) \n", msg, elapsed, elapsedg);
    	    fflush	(flog);
        #endif
    #endif
}

#endif
