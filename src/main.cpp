
#include         <stdlib.h>
#include          <stdio.h>
#include          <float.h>
#include           <math.h>
#include             <io.h>

#include           "glee.h"
#include            "SDL.h"    

#include          "types.h"
#include          "macro.h"
#include         "random.h"
#include           "sort.h"
#include       "skeleton.h"
#include         "loader.h"
#include        "physics.h"
#include       "renderer.h"
#include          "debug.h"

#include          "shaderGenerator.h"

#define M_WORLDFILE     "Data\\blender.shift"

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// GLOBALS
////////////////////////////////////////////////////////////////////////////////////////////////////////////

FLOAT_64        fps                 = 0;
UINT_32         frame               = 0;
UINT_32         frames              = 1;	// to avoid delta corruption
UINT_32         framen              = 0;
UINT_32         tstart              = 0;
UINT_64         frametime           = 0;
UINT_64         frametimeo          = 0;
FLOAT_32        delta               = 0;

CHAR            caption [256];

INT_32          debug               = 0;

FLOAT_32        vel;

FLOAT_32        anglex              = 0.0;
FLOAT_32        angley              = 0.0;
FLOAT_32        anglez              = 0.0;

BOOL            move_forward        = false;
BOOL            move_backward       = false;
BOOL            move_left           = false;
BOOL            move_right          = false;
BOOL            move_up             = false;
BOOL            move_down           = false;

SDL_Window*		window              = NULL;
SDL_GLContext	glcontext           = NULL;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// QUIT
////////////////////////////////////////////////////////////////////////////////////////////////////////////

VOID quit ()
{
    
    debug_Destroy ();

    lo_UnloadWorld ();

    dr_Free ();
    ph_Free ();

	SDL_GL_DeleteContext (glcontext);

	SDL_DestroyWindow (window);

    SDL_Quit ();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// MAIN
////////////////////////////////////////////////////////////////////////////////////////////////////////////

INT_32 main (INT_32 argc, CHARP argv [])
{
    atexit (quit);

    // LOGGING

    if (debug_Init () < 0) {
        
        MessageBox (NULL, "LOGGING INITIALIZATION FAILED", "Error", MB_OK | MB_ICONERROR | MB_TOPMOST);
        return - 1;
    }

//    #define M_SHFLAG_DEPTH                  0x00000001
//    #define M_SHFLAG_INSTANCED              0x00000002
//    #define M_SHFLAG_DISSOLVE               0x00000008
//    #define M_SHFLAG_TRANSPARENT            0x00000010
//    #define M_SHFLAG_TRANSLUCENT            0x00000020
//    #define M_SHFLAG_MULTILAYER             0x00000040
//    #define M_SHFLAG_ANIM_WIND1             0x00000080
//    #define M_SHFLAG_ANIM_WIND2             0x00000100
//    #define M_SHFLAG_DEFORM_SHRINK          0x00000200
//    #define M_SHFLAG_DEFORM_GROW            0x00000400
//    #define M_SHFLAG_DEFORM_MORPH           0x00000800
//    #define M_SHFLAG_VERTEX_COLORS          0x00001000
//    #define M_SHFLAG_DOUBLESIDE             0x00002000
//    #define M_SHFLAG_COMPOSITE              0x00004000
//    #define M_SHFLAG_UV1_PRECISION_HI       0x00008000
//    #define M_SHFLAG_UV2_PRECISION_HI       0x00010000
//    #define M_SHFLAG_SINGLE_NORMAL          0x00020000

    CHARP sh_vertex;
    CHARP sh_fragment;
    
    // ROCKS
    //dr_GenerateShader (M_SHFLAG_INSTANCED | M_SHFLAG_COMPOSITE | M_SHFLAG_DEFORM_SHRINK | M_SHFLAG_DEPTH, &sh_vertex, &sh_fragment);
    //dr_GenerateShader (M_SHFLAG_INSTANCED | M_SHFLAG_COMPOSITE | M_SHFLAG_DEFORM_SHRINK,                  &sh_vertex, &sh_fragment);
        
    // TREES
    //dr_GenerateShader (M_SHFLAG_INSTANCED | M_SHFLAG_COMPOSITE | M_SHFLAG_VERTEX_COLORS | M_SHFLAG_DEPTH, &sh_vertex, &sh_fragment);
    //dr_GenerateShader (M_SHFLAG_INSTANCED | M_SHFLAG_COMPOSITE | M_SHFLAG_VERTEX_COLORS,                  &sh_vertex, &sh_fragment);
    
    // BOX
    //dr_GenerateShader (M_SHFLAG_COMPOSITE | M_SHFLAG_DEPTH, &sh_vertex, &sh_fragment);
    //dr_GenerateShader (M_SHFLAG_COMPOSITE,                  &sh_vertex, &sh_fragment);
    
    // BUSHES & BRANCHES
    dr_GenerateShader (M_SHFLAG_INSTANCED | M_SHFLAG_COMPOSITE | M_SHFLAG_TRANSPARENT | M_SHFLAG_ANIM_WIND1 | M_SHFLAG_TRANSLUCENT | M_SHFLAG_DOUBLESIDE | M_SHFLAG_VERTEX_COLORS | M_SHFLAG_DEPTH, &sh_vertex, &sh_fragment);
    dr_GenerateShader (M_SHFLAG_INSTANCED | M_SHFLAG_COMPOSITE | M_SHFLAG_TRANSPARENT | M_SHFLAG_ANIM_WIND1 | M_SHFLAG_TRANSLUCENT | M_SHFLAG_DOUBLESIDE | M_SHFLAG_VERTEX_COLORS,                  &sh_vertex, &sh_fragment);
    
    // GRASS
    //dr_GenerateShader (M_SHFLAG_INSTANCED | M_SHFLAG_TRANSPARENT | M_SHFLAG_DISSOLVE | M_SHFLAG_SINGLE_NORMAL | M_SHFLAG_ANIM_WIND2 | M_SHFLAG_DOUBLESIDE | M_SHFLAG_DEPTH, &sh_vertex, &sh_fragment);
    //dr_GenerateShader (M_SHFLAG_INSTANCED | M_SHFLAG_TRANSPARENT | M_SHFLAG_DISSOLVE | M_SHFLAG_SINGLE_NORMAL | M_SHFLAG_ANIM_WIND2 | M_SHFLAG_DOUBLESIDE,                  &sh_vertex, &sh_fragment);
    
    // TERRAIN
    //dr_GenerateShader (M_SHFLAG_MULTILAYER | M_SHFLAG_UV1_PRECISION_HI | M_SHFLAG_DEPTH, &sh_vertex, &sh_fragment);
    //dr_GenerateShader (M_SHFLAG_MULTILAYER | M_SHFLAG_UV1_PRECISION_HI,                  &sh_vertex, &sh_fragment);
    
    // LOAD USER SETTINGS

    dr_control_gammacorrection			= true;
    dr_control_autoexposure				= true;
    dr_control_fog						= true;
    dr_control_aa						= false;

    dr_control_bloom_enable			    = true;
    dr_control_bloom_enable_vblur       = true;
    dr_control_bloom_enable_hblur       = true;
    dr_control_bloom_strength			= 1.0f;
    dr_control_bloom_res				= 0;

    dr_control_ssao_enable				= true;
    dr_control_ssao_res				    = 0;

    dr_control_anisotrophy				= 16;

    dr_control_mip                      = 7;

    dr_control_culling				    = 1;

    dr_control_sun_res			        = 2048;
    dr_control_sun_splits               = 5;
    dr_control_sun_offset               = 0.005f;
    dr_control_sun_transition           = 0.05f;
    dr_control_sun_distribution         = 10.0f;
    dr_control_sun_scheme               = 0.9f;
    dr_control_sun_zoom                 = 1.07f;
    dr_control_sun_debug                = false;

    dr_control_image_desaturation		= 0.2f;
    dr_control_image_brightness			= 1.7f;
    dr_control_image_contrast			= 1.3f;

    dr_control_sky_desaturation			= 0.2f;
    dr_control_sky_brightness 			= 1.5f;
    dr_control_sky_contrast				= 0.8f;

    
    //dr_level_bloom					= 0.5f;

    //dr_level_desaturation				= 0.0f;
    //dr_level_desaturation_sky			= 0.0f;
    //dr_level_brightness				= 1.0f;
    //dr_level_brightness_sky 			= 1.0f;
    //dr_level_contrast					= 1.0f;
    //dr_level_contrast_sky				= 1.0f;
    
    dr_control_hdr_exposure				= 0.5f;
    dr_control_hdr_exposure_scale_min	= 0.5f;
    dr_control_hdr_exposure_scale_max	= 1.5f;
    dr_control_hdr_exposure_speed		= 0.001f;

    dr_width							= 1280;		//1920; //1600 //1280; //800; //640;
    dr_height							= 800;		//1080; //900  //800;  //600; //400;

    dr_fullscreen						= false;

    // SDL SETUP

    //SDL_EnableKeyRepeat (10, 20);

    if (SDL_Init (SDL_INIT_VIDEO) < 0 ) {

        MessageBox (NULL, "SDL INIT ERROR", "Error", MB_OK | MB_ICONERROR | MB_TOPMOST);
        return - 1;
    }

    SDL_GL_SetAttribute (SDL_GL_RED_SIZE,       8);
    SDL_GL_SetAttribute (SDL_GL_GREEN_SIZE,     8);
    SDL_GL_SetAttribute (SDL_GL_BLUE_SIZE,      8);
    SDL_GL_SetAttribute (SDL_GL_ALPHA_SIZE,     8);
    SDL_GL_SetAttribute (SDL_GL_DEPTH_SIZE,     24);
    SDL_GL_SetAttribute (SDL_GL_DOUBLEBUFFER,   1);

	SDL_SetHint (SDL_HINT_FRAMEBUFFER_ACCELERATION, "opengl");
	SDL_SetHint (SDL_HINT_RENDER_DRIVER,			"opengl");
	SDL_SetHint (SDL_HINT_RENDER_OPENGL_SHADERS,	"1");
	SDL_SetHint (SDL_HINT_RENDER_VSYNC,				"0");

    SDL_Event event;

    sprintf (caption, "SHIFT. Tech-Demo");

	UINT_32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_INPUT_FOCUS | SDL_WINDOW_MOUSE_FOCUS;

    SDL_SetRelativeMouseMode (SDL_TRUE);

    if (dr_fullscreen)

		window = SDL_CreateWindow (caption, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, dr_width, dr_height, flags | SDL_WINDOW_FULLSCREEN);    else
		window = SDL_CreateWindow (caption, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, dr_width, dr_height, flags);

    // Create an OpenGL context associated with the window.
    glcontext = SDL_GL_CreateContext (window);	

    // Init GLee
    GLeeInit ();

    if (!GLEE_VERSION_3_0) {

        MessageBox (NULL, "Your system do not support OpenGL 3.0 standard", "Error", MB_OK | MB_ICONERROR | MB_TOPMOST);
        return - 1;
    }

    SDL_Surface *screen = SDL_GetWindowSurface (window);

    SDL_FillRect (screen, NULL, SDL_MapRGBA (screen->format, 0, 0, 0, 0));

    // INITIALIZING RANDOM GENERATOR

    randSeed ();

    // SETTING UP CAMERA

    FLOAT_32 l;

    l = vLENGTH3 (dr_worldup);  vNORMALIZE3 (dr_worldup, l);
    l = vLENGTH3 (dr_camdir);   vNORMALIZE3 (dr_camdir,  l);

    vCROSS (dr_cambi, dr_camdir, dr_worldup);

    l = vLENGTH3 (dr_cambi);  vNORMALIZE3 (dr_cambi,  l);

    vCROSS (dr_camup, dr_cambi, dr_camdir);

    l = vLENGTH3 (dr_camup);  vNORMALIZE3 (dr_camup,  l);

    // LOADING WORLD

    if (lo_LoadWorld (M_WORLDFILE) < 0) return -1;
    
    //dr_sun_ambient = 0.4;

    //debug_OctTree (&dr_tree, dr_objects);

    // INITIALIZING DEFFERED RENDERER

    dr_Init ();

    // INITIALIZING PHYSICS

    ph_Init ();

    // VSYNC OFF

    wglSwapIntervalEXT (0);

    // FPS

    frametimeo  = SDL_GetPerformanceCounter ();

    BOOL press = false;

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

                        case SDLK_KP_0: if ((GetTickCount () - tstart) > 200) { tstart = GetTickCount ();   dr_control_aa				    = !dr_control_aa;				    } break;
                        case SDLK_KP_1: if ((GetTickCount () - tstart) > 200) { tstart = GetTickCount ();   dr_control_fog				    = !dr_control_fog;				    } break;
                        case SDLK_KP_2: if ((GetTickCount () - tstart) > 200) { tstart = GetTickCount ();   dr_control_ssao_enable		    = !dr_control_ssao_enable;		    } break;
                        case SDLK_KP_3: if ((GetTickCount () - tstart) > 200) { tstart = GetTickCount ();   dr_control_bloom_enable		    = !dr_control_bloom_enable;		    } break;
                        case SDLK_KP_4: if ((GetTickCount () - tstart) > 200) { tstart = GetTickCount ();   dr_control_autoexposure		    = !dr_control_autoexposure;		    } break;

                        case SDLK_KP_5: if ((GetTickCount () - tstart) > 200) { tstart = GetTickCount ();   dr_AdjustGamma (!dr_control_gammacorrection);                       } break;

                        case SDLK_KP_6: if ((GetTickCount () - tstart) > 200) { tstart = GetTickCount ();   dr_control_bloom_enable_vblur   = !dr_control_bloom_enable_vblur;	} break;
                        case SDLK_KP_7: if ((GetTickCount () - tstart) > 200) { tstart = GetTickCount ();   dr_control_bloom_enable_hblur   = !dr_control_bloom_enable_hblur;	} break;

                        case SDLK_KP_MINUS:		if ((GetTickCount () - tstart) > 200) { tstart = GetTickCount ();   

                            if (SDL_GetModState () & KMOD_SHIFT)	dr_control_sky_brightness	    -= 0.1f;	else
                                                                    dr_control_image_brightness		-= 0.1f;	} break;

                        case SDLK_KP_PLUS:		if ((GetTickCount () - tstart) > 200) { tstart = GetTickCount ();

                            if (SDL_GetModState () & KMOD_SHIFT)	dr_control_sky_brightness	    += 0.1f;	else
                                                                    dr_control_image_brightness		+= 0.1f;	} break;

                        case SDLK_KP_DIVIDE:	if ((GetTickCount () - tstart) > 200) { tstart = GetTickCount ();   

                            if (SDL_GetModState () & KMOD_SHIFT)	dr_control_sky_contrast	        -= 0.1f;	else
                                                                    dr_control_image_contrast		-= 0.1f;	} break;

                        case SDLK_KP_MULTIPLY:	if ((GetTickCount () - tstart) > 200) { tstart = GetTickCount ();

                            if (SDL_GetModState () & KMOD_SHIFT)	dr_control_sky_contrast	        += 0.1f;	else
                                                                    dr_control_image_contrast		+= 0.1f;	} break;

                        case SDLK_DOWN:			if ((GetTickCount () - tstart) > 200) { tstart = GetTickCount ();   dr_AdjustAnisotrophy (dr_control_anisotrophy >> 1);    } break;
                        case SDLK_UP:			if ((GetTickCount () - tstart) > 200) { tstart = GetTickCount ();   dr_AdjustAnisotrophy (dr_control_anisotrophy << 1);    } break;

                        case SDLK_LEFT:			if ((GetTickCount () - tstart) > 200) { tstart = GetTickCount ();

                            if (SDL_GetModState () & KMOD_SHIFT)	dr_control_sky_desaturation     -= 0.1f;	else
                                                                    dr_control_image_desaturation	-= 0.1f;	} break;

                        case SDLK_RIGHT:		if ((GetTickCount () - tstart) > 200) { tstart = GetTickCount ();

                            if (SDL_GetModState () & KMOD_SHIFT)	dr_control_sky_desaturation	    += 0.1f;	else
                                                                    dr_control_image_desaturation	+= 0.1f;	} break;

                        case SDLK_PAGEDOWN:		if ((GetTickCount () - tstart) > 200) { tstart = GetTickCount ();

                            if (SDL_GetModState () & KMOD_SHIFT)    dr_AdjustBloomRes               (dr_control_bloom_res           - 1);       else
                            if (SDL_GetModState () & KMOD_LALT)     dr_AdjustSSAORes                (dr_control_ssao_res            - 1);	    else
                            if (SDL_GetModState () & KMOD_RALT)     dr_AdjustMIP                    (dr_control_mip                 - 1);	    else
                                                                    dr_AdjustBloomStrength          (dr_control_bloom_strength      - 0.1f);    } break;

                        case SDLK_PAGEUP:		if ((GetTickCount () - tstart) > 200) { tstart = GetTickCount ();

                            if (SDL_GetModState () & KMOD_SHIFT)    dr_AdjustBloomRes               (dr_control_bloom_res           + 1);       else
                            if (SDL_GetModState () & KMOD_LALT)     dr_AdjustSSAORes                (dr_control_ssao_res            + 1);	    else
                            if (SDL_GetModState () & KMOD_RALT)     dr_AdjustMIP                    (dr_control_mip                 + 1);	    else
                                                                    dr_AdjustBloomStrength          (dr_control_bloom_strength      + 0.1f);    } break;

                        case SDLK_END:			if ((GetTickCount () - tstart) > 200) { tstart = GetTickCount ();

                            if (SDL_GetModState () & KMOD_SHIFT)    dr_AdjustShadowsRes             (dr_control_sun_res             >> 1);	    else
                            if (SDL_GetModState () & KMOD_LALT)     dr_AdjustShadowsDistribution    (dr_control_sun_distribution    - 1.0f);	else
                            if (SDL_GetModState () & KMOD_RALT)     dr_AdjustShadowsScheme          (dr_control_sun_scheme          - 0.01f);	else
                                                                    dr_AdjustShadowsSplits          (dr_control_sun_splits          - 1);	    } break;

                        case SDLK_HOME:			if ((GetTickCount () - tstart) > 200) { tstart = GetTickCount ();

                            if (SDL_GetModState () & KMOD_SHIFT)    dr_AdjustShadowsRes             (dr_control_sun_res             << 1);	    else
                            if (SDL_GetModState () & KMOD_LALT)     dr_AdjustShadowsDistribution    (dr_control_sun_distribution    + 1.0f);	else
                            if (SDL_GetModState () & KMOD_RALT)     dr_AdjustShadowsScheme          (dr_control_sun_scheme          + 0.01f);	else
                                                                    dr_AdjustShadowsSplits          (dr_control_sun_splits          + 1);	    } break;

                        case SDLK_DELETE:       if ((GetTickCount () - tstart) > 200) { tstart = GetTickCount (); 

                                dr_AdjustShadowsDebug (!dr_control_sun_debug);
                            } break;
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

        if (SDL_GetModState () & KMOD_SHIFT)    vel = delta * 0.1f;
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

        // MOUSE

        if (press || dr_fullscreen) {

            INT_32 nx, ny;

            SDL_GetRelativeMouseState (&nx, &ny);

            anglex = (FLOAT_32) - nx * 0.5f;
            angley = (FLOAT_32) - ny * 0.5f;
            anglez = 0.0f;

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

        frametime  = SDL_GetPerformanceCounter ();

		FLOAT_64 seconds = (frametime - frametimeo) / (FLOAT_64) SDL_GetPerformanceFrequency ();

        if (seconds > 0.2) {

            fps = (frames / seconds);   delta = (FLOAT_32) (1000 * seconds / frames);	// delta is in miliseconds

            sprintf (caption, "FPS %6.2f", fps);

            frametimeo = frametime;
            frames = 0;
        }

        M_STATE_TEX0_RECT_CLEAR;

        dr_DrawText (- 0.95f, 0.9f, 0.03f, 0.05f, dr_text, true, caption);

        // SWAP BUFFERS

        SDL_GL_SwapWindow (window);

        frame ++;  framen ++;   frames ++;
    }

    return 0;
}

INT_32 WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{    
    return main (0, NULL);
}
