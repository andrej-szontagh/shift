
#include         <stdlib.h>
#include          <stdio.h>
#include          <float.h>
#include           <math.h>
#include             <io.h>

#include           "glee.h"

#include            "SDL.h"
#include     "SDL_opengl.h"

#include          "types.h"
#include          "macro.h"
#include         "random.h"
#include           "sort.h"
#include       "skeleton.h"
#include         "loader.h"
#include        "physics.h"
#include       "renderer.h"
#include          "debug.h"

#define M_WORLDFILE     "Data\\blender.shift"

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// GLOBALS
////////////////////////////////////////////////////////////////////////////////////////////////////////////

FLOAT_64    fps                 = 0;
UINT_32     frame               = 0;
UINT_32     frames              = 1;	// to avoid delta corruption
UINT_32     framen              = 0;
UINT_32     tstart              = 0;
UINT_32     frametime           = 0;
UINT_32     frametimeo          = 0;
FLOAT_32    delta               = 0;

CHAR        caption [256];

INT_32      width               = 1920;		//1920; //1280; //800; //640;
INT_32      height              = 1080;		//1080; //800;  //600; //400;

BOOL        fullscreen			= true;

INT_32      debug               = 0;

FLOAT_32    vel;

FLOAT_32    anglex              = 0.0;
FLOAT_32    angley              = 0.0;
FLOAT_32    anglez              = 0.0;

BOOL        move_forward        = false;
BOOL        move_backward       = false;
BOOL        move_left           = false;
BOOL        move_right          = false;
BOOL        move_up             = false;
BOOL        move_down           = false;

BOOL        inc_debug_param1    = false;
BOOL        dec_debug_param1    = false;
BOOL        inc_debug_param2    = false;
BOOL        dec_debug_param2    = false;

INT_32      debug_param1        = 0;
INT_32      debug_param2        = 0;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// QUIT
////////////////////////////////////////////////////////////////////////////////////////////////////////////

VOID quit ()
{
    #ifdef M_DEBUG
        debug_Destroy ();
    #endif

    lo_UnloadWorld ();

    dr_Free ();
    ph_Free ();

    SDL_Quit ();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// MAIN
////////////////////////////////////////////////////////////////////////////////////////////////////////////

INT_32 main (INT_32 argc, CHARP argv [])
{

    SDL_Event event;
    SDL_Surface *screen;

    POINT mousepos;
    POINT mouseposold;

    BOOL press = false;

    UINT_32 bkcolor;

    if (SDL_Init (SDL_INIT_VIDEO) < 0 ) {

        MessageBox (NULL, "SDL INIT ERROR", "Error", MB_OK | MB_ICONERROR | MB_TOPMOST);
        return - 1;
    }

    atexit (quit);

    sprintf (caption, "SHIFTe. Tech-Demo");

    SDL_WM_SetCaption (caption, caption);

    SDL_EnableKeyRepeat (10, 20);

    SDL_GL_SetAttribute (SDL_GL_RED_SIZE,       8);
    SDL_GL_SetAttribute (SDL_GL_GREEN_SIZE,     8);
    SDL_GL_SetAttribute (SDL_GL_BLUE_SIZE,      8);
    SDL_GL_SetAttribute (SDL_GL_ALPHA_SIZE,     8);
    SDL_GL_SetAttribute (SDL_GL_DEPTH_SIZE,     32);
    SDL_GL_SetAttribute (SDL_GL_DOUBLEBUFFER,   1);

	if (fullscreen) 
		screen = SDL_SetVideoMode (width, height, 0, SDL_OPENGL | SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_FULLSCREEN); else
		screen = SDL_SetVideoMode (width, height, 0, SDL_OPENGL | SDL_HWSURFACE | SDL_DOUBLEBUF);

    bkcolor = SDL_MapRGBA (screen->format, 0, 0, 0, 0);

    SDL_FillRect (screen, NULL, bkcolor);

    // INITIALIZING RANDOM GENERATOR

    randSeed ();

    // CREATING LOGFILE

    #ifdef M_DEBUG
        debug_Init ();
    #endif

    // SETTING UP CAMERA

    FLOAT_32 l;

    l = vLENGTH3 (dr_worldup);  vNORMALIZE3 (dr_worldup, l);
    l = vLENGTH3 (dr_camdir);   vNORMALIZE3 (dr_camdir,  l);

    vCROSS (dr_cambi, dr_camdir, dr_worldup);

    l = vLENGTH3 (dr_cambi);  vNORMALIZE3 (dr_cambi,  l);

    vCROSS (dr_camup, dr_cambi, dr_camdir);

    l = vLENGTH3 (dr_camup);  vNORMALIZE3 (dr_camup,  l);

    // LOADING WORLD

    lo_LoadWorld (M_WORLDFILE);

    #ifdef M_DEBUG
    ///     debug_OctTree (&dr_tree, dr_objects);
    #endif

    // INITIALIZING DEFFERED RENDERER

    dr_Init (width, height);

    // INITIALIZING PHYSICS

    ph_Init ();

    // VSYNC OFF

    wglSwapIntervalEXT (0);

    // FPS

    frametimeo  = SDL_GetTicks ();

    // GET MOUSE

    GetCursorPos (&mousepos);

    while (true) {

        while (SDL_PollEvent (&event)) {
            switch (event.type) {

                case SDL_QUIT:  return 0;

                case SDL_KEYUP:

                    switch (event.key.keysym.sym) {

                        case SDLK_w:        move_forward  = false; break;
                        case SDLK_s:        move_backward = false; break;
                        case SDLK_a:        move_left     = false; break;
                        case SDLK_d:        move_right    = false; break;
                        case SDLK_SPACE:    move_up       = false; break;
                        case SDLK_LCTRL:    move_down     = false; break;

                        case SDLK_RIGHT:    inc_debug_param1 = false; break;
                        case SDLK_LEFT:     dec_debug_param1 = false; break;
                        case SDLK_UP:       inc_debug_param2 = false; break;
                        case SDLK_DOWN:     dec_debug_param2 = false; break;

                    } break;

                case SDL_KEYDOWN:

                    if (event.key.keysym.sym == SDLK_ESCAPE)
                        return 0;

                    switch (event.key.keysym.sym) {

                        case SDLK_w:        move_forward  = true; break;
                        case SDLK_s:        move_backward = true; break;
                        case SDLK_a:        move_left     = true; break;
                        case SDLK_d:        move_right    = true; break;
                        case SDLK_SPACE:    move_up       = true; break;
                        case SDLK_LCTRL:    move_down     = true; break;

                        case SDLK_RIGHT:    inc_debug_param1 = true; break;
                        case SDLK_LEFT:     dec_debug_param1 = true; break;
                        case SDLK_UP:       inc_debug_param2 = true; break;
                        case SDLK_DOWN:     dec_debug_param2 = true; break;

                        case SDLK_1:    if ((GetTickCount () - tstart) > 200) { tstart = GetTickCount ();   if (debug ==  1) debug = 0; else debug =  1;  } break;
                        case SDLK_2:    if ((GetTickCount () - tstart) > 200) { tstart = GetTickCount ();   if (debug ==  2) debug = 0; else debug =  2;  } break;
                        case SDLK_3:    if ((GetTickCount () - tstart) > 200) { tstart = GetTickCount ();   if (debug ==  3) debug = 0; else debug =  3;  } break;
                        case SDLK_4:    if ((GetTickCount () - tstart) > 200) { tstart = GetTickCount ();   if (debug ==  4) debug = 0; else debug =  4;  } break;
                        case SDLK_5:    if ((GetTickCount () - tstart) > 200) { tstart = GetTickCount ();   if (debug ==  5) debug = 0; else debug =  5;  } break;
                        case SDLK_6:    if ((GetTickCount () - tstart) > 200) { tstart = GetTickCount ();   if (debug ==  6) debug = 0; else debug =  6;  } break;
                        case SDLK_7:    if ((GetTickCount () - tstart) > 200) { tstart = GetTickCount ();   if (debug ==  7) debug = 0; else debug =  7;  } break;
                        case SDLK_8:    if ((GetTickCount () - tstart) > 200) { tstart = GetTickCount ();   if (debug ==  8) debug = 0; else debug =  8;  } break;
                        case SDLK_9:    if ((GetTickCount () - tstart) > 200) { tstart = GetTickCount ();   if (debug ==  9) debug = 0; else debug =  9;  } break;

                        case SDLK_o:    if ((GetTickCount () - tstart) > 200) { tstart = GetTickCount ();
                            
                                            if (debug == 11) {  debug = 0;  break; }
                                            if (debug == 10) {  debug = 11; break; } 

                                            debug = 10; break;
                                        }

                        case SDLK_j:    if ((GetTickCount () - tstart) > 200) { tstart = GetTickCount ();   dr_enableaa = !dr_enableaa; } break;
                    }

                break;

                case SDL_MOUSEBUTTONDOWN:
                        press = true;
                    break;

                case SDL_MOUSEBUTTONUP:
                        press = false;
                    break;
            }
        }

        // SCENE TRANSFORMATIONS

        vel = delta * 0.01f;

        if (SDL_GetModState () & KMOD_SHIFT)    vel = delta * 0.1f;    ///0.05f;
        if (SDL_GetModState () & KMOD_ALT)      vel = delta * 0.0025f;

        if (move_forward) {
            dr_campos [0] += dr_camdir [0] * vel;
            dr_campos [1] += dr_camdir [1] * vel;
            dr_campos [2] += dr_camdir [2] * vel;
        }

        if (move_backward) {
            dr_campos [0] -= dr_camdir [0] * vel;
            dr_campos [1] -= dr_camdir [1] * vel;
            dr_campos [2] -= dr_camdir [2] * vel;
        }

        if (move_left) {
            dr_campos [0] -= dr_cambi [0] * vel;
            dr_campos [1] -= dr_cambi [1] * vel;
            dr_campos [2] -= dr_cambi [2] * vel;
        }

        if (move_right) {
            dr_campos [0] += dr_cambi [0] * vel;
            dr_campos [1] += dr_cambi [1] * vel;
            dr_campos [2] += dr_cambi [2] * vel;
        }

        if (move_up) {
            dr_campos [1] += vel;
        }

        if (move_down) {
            dr_campos [1] -= vel;
        }

        if (inc_debug_param1) {
            if ((GetTickCount () - tstart) > 200) { tstart = GetTickCount ();       debug_param1 ++;
            } 
        }
        if (dec_debug_param1) {
            if ((GetTickCount () - tstart) > 200) { tstart = GetTickCount ();       debug_param1 --;
            } 
        }
        if (inc_debug_param2) {
            if ((GetTickCount () - tstart) > 200) { tstart = GetTickCount ();       debug_param2 ++;
            } 
        }
        if (dec_debug_param2) {
            if ((GetTickCount () - tstart) > 200) { tstart = GetTickCount ();       debug_param2 --;
            } 
        }

        // MOUSE

        mouseposold = mousepos;

        GetCursorPos (&mousepos);

        FLOAT_32 nx, ny;

        if (press) {

            anglex = 0.0f;
            angley = 0.0f;
            anglez = 0.0f;
            
            nx = (mousepos.x + mouseposold.x) * 0.5f;
            ny = (mousepos.y + mouseposold.y) * 0.5f;

            anglex = (GLfloat) (mouseposold.x - nx);
            angley = (GLfloat) (mouseposold.y - ny);

            vFLY (dr_camdir, dr_worldup, dr_cambi, (- anglex * 0.01f), (angley * 0.01f), (anglez * 0.01f));

            l = vLENGTH3 (dr_camdir); vNORMALIZE3 (dr_camdir, l);
            l = vLENGTH3 (dr_cambi);  vNORMALIZE3 (dr_cambi,  l);

            vCROSS (dr_camup, dr_cambi, dr_camdir);

            l = vLENGTH3 (dr_camup);  vNORMALIZE3 (dr_camup,  l);
        }

        // SIMULATING PHYSICS

        ph_Step ();

        // RENDERING SCENE

        dr_Render (delta);

        // FPS

        frametime  = SDL_GetTicks ();

        if ((frametime - frametimeo) > 200) {

            fps = (frames * 1000.0 / (FLOAT_64) (delta = (FLOAT_32) frametime - frametimeo));   delta /= frames;

            sprintf (caption, "FPS %6.2f", fps);        // SDL_WM_SetCaption (caption, caption);

            frametimeo = frametime;
            frames = 0;
        }

		M_STATE_TEX0_RECT_CLEAR;

		dr_DrawText (- 0.95f, 0.9f, 0.03f, 0.05f, dr_text, true, caption);

        // SWAP BUFFERS

        SDL_GL_SwapBuffers ();

        frame ++;  framen ++;   frames ++;
    }

    return 0;
}
