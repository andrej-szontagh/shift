
////////////////////////////////////////////////////////////////////////////////////////////////////
// INCLUDE
////////////////////////////////////////////////////////////////////////////////////////////////////

#include	   <stdlib.h>
#include	    <stdio.h>

#include	  <windows.h>

#include	     "glee.h"
#include	    "types.h"
#include	    "macro.h"
#include	 "skeleton.h"
#include	    "debug.h"

#include	 "SDL_timer.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// GLOBALS
////////////////////////////////////////////////////////////////////////////////////////////////////

#define M_LOG_FILE          "loader.log"
#define M_LOG_INTEND        80

#define M_DEBUG_MAX_TIMERS  256

#ifdef WIN32

    FLOAT_64 startfreq  [M_DEBUG_MAX_TIMERS];
    UINT_64  starttime  [M_DEBUG_MAX_TIMERS];
#else

    UINT_32  starttime  [M_DEBUG_MAX_TIMERS];
#endif

UINT_32 timequery;  FILE* flog;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// debug_Init
////////////////////////////////////////////////////////////////////////////////////////////////////////////

INT_32 debug_Init () {
    
    #ifdef M_DEBUG

        flog = fopen (M_LOG_FILE, "wb");

        if (flog == NULL)   return -1;

        // LOG first line
        fprintf (flog, "--=== LOADER LOG FILE ===--\n");        

        glGenQueries (1, &timequery);
        
    #endif

    return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// debug_Destroy
////////////////////////////////////////////////////////////////////////////////////////////////////////////

VOID debug_Destroy () {
    
    #ifdef M_DEBUG

        fclose (flog);    

        glDeleteQueries (1, &timequery);
        
    #endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// debug_Print
////////////////////////////////////////////////////////////////////////////////////////////////////////////

VOID debug_Print (CHARP msg) {
    
    #ifdef M_DEBUG

        fprintf (flog, msg);   fflush (flog);
        
    #endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// debug_Print
////////////////////////////////////////////////////////////////////////////////////////////////////////////

VOID debug_Print (CHARP msg, CHARP file, BOOL lineend) {
    
    #ifdef M_DEBUG
        if (lineend) {
            fprintf (flog, "%s'%s'\n",  msg, file); fflush (flog);  } else {
            fprintf (flog, "%s'%s'",    msg, file); fflush (flog);
        }
    #endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// debug_Print
////////////////////////////////////////////////////////////////////////////////////////////////////////////

VOID debug_Print (CHARP msg, UINT_32 code, BOOL lineend) {
    
    #ifdef M_DEBUG
        if (lineend) {
            fprintf (flog, "%s%u\n",    msg, code); fflush (flog);  } else {
            fprintf (flog, "%s%u",      msg, code); fflush (flog);
        }
    #endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// debug_PrintWithIntend
////////////////////////////////////////////////////////////////////////////////////////////////////////////

VOID debug_PrintIntend (CHARP msg) {
    
    #ifdef M_DEBUG

        fprintf (flog, msg);   fflush (flog);

        fprintf (flog, "%*s", MAX (0, M_LOG_INTEND - strlen (msg)), " ");
        
    #endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// debug_PrintWithIntend
////////////////////////////////////////////////////////////////////////////////////////////////////////////

VOID debug_PrintIntend (CHARP msg, CHARP file) {
    
    #ifdef M_DEBUG

        fprintf (flog, "%s%s", msg, file);  fflush (flog);
        
        fprintf (flog, "%*s", MAX (0, M_LOG_INTEND - strlen (msg) - strlen (file)), " ");
        
    #endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// debug_StartTimer
////////////////////////////////////////////////////////////////////////////////////////////////////////////

VOID debug_StartTimer (UINT_32 id) {
    
    #ifdef M_DEBUG
        #ifdef WIN32

            LARGE_INTEGER li;

            QueryPerformanceFrequency   (&li);  startfreq [id] = ((FLOAT_64) li.QuadPart) / 1000.0;
            QueryPerformanceCounter     (&li);

            starttime [id] = li.QuadPart;
        #else

            starttime [id] = SDL_GetTicks ();
        #endif
    #endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// debug_StartTimerQuery
////////////////////////////////////////////////////////////////////////////////////////////////////////////

VOID debug_StartTimerQuery (UINT_32 id) {
    
    #ifdef M_DEBUG
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
    #endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// debug_EndTimer
////////////////////////////////////////////////////////////////////////////////////////////////////////////

VOID debug_EndTimer (UINT_32 id, CHARP msg, FLOAT_64 &elapsed) {
    
    #ifdef M_DEBUG
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
    #endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// debug_EndTimerQuery
////////////////////////////////////////////////////////////////////////////////////////////////////////////

VOID debug_EndTimerQuery (UINT_32 id, CHARP msg, FLOAT_64 &elapsed, FLOAT_64 &elapsedg) {
    
    #ifdef M_DEBUG
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
    #endif
}
