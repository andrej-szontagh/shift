
#ifndef _RENDERER_H
#define _RENDERER_H

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CONSTATNS
////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if 1

#define M_DR_HDR_BLOOM                  1.0f                // bloom factor
#define M_DR_HDR_EXPOSURE               0.5f                // exposure balanced level
#define M_DR_HDR_EXPOSURE_SPEED         0.001f              // speed of exposure adaptation

#define M_DR_SUN_SPLITS                 5                   // number of splits (shadowmaps)
#define M_DR_SUN_SHADOW                 1024                // resolution of shadow map
#define M_DR_SUN_SHADOWH                512                 // half resolution
#define M_DR_SUN_OFFSET                 0.005f              // depth offset factor for shadow mapping
#define M_DR_SUN_TRAN                   0.05                // width of split transition
#define M_DR_SUN_ZOOM                   1.07                // factor to scale shadowmap viewport
#define M_DR_SUN_GRASS                  0.7f                // factor to advance grass range for shadowing
#define M_DR_SUN_SATURATE               1.0f                // sun color saturation

#define M_DR_SSAO_RAND                  4                   // size of random texture
#define M_DR_SSAO_RANDSCALE             0.02f               // scale of random texture mapping

#define M_DR_FOVY                       60.0f               // vertical FOV in degrees

#define M_DR_CULLING_SAMPLES            0                   // maximum sample count to cull object

#define M_DR_TREE_MAX_DEPTH             4                   // maximum depth of tree

#define M_DR_MAX_CLIP_PLANES            6                   // maximum count of clip planes for allocations

#define M_DR_OCCLUDER                   0.4f                // occluder selection treshold

#define M_DR_MIPLEVELS                  6                   // postprocessing levels of minification
#define M_DR_ANISOTROPY                 4                   // default anisotrophy level

#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// INCLUDE
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if 1

#include        "rendererGui.h"
#include        "rendererClip.h"
#include        "rendererInternals.h"

#include        <GL/glu.h>

#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FUNCTIONS
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if 1

// LOADER

VOID dr_Init            (UINT_32  width, UINT_32 height);
VOID dr_Free            ();

// SORTING

VOID dr_Sort            ();

// SUN

VOID dr_SunInit         ();
VOID dr_SunDraw         ();
VOID dr_SunPrepare      ();
VOID dr_SunShadows      ();

// RENDER

VOID dr_Draw            ();
VOID dr_DrawDepth       ();
VOID dr_DrawBlend       ();
VOID dr_DrawShadows     (UINT_32 split);
VOID dr_DrawOcclusions  ();

VOID dr_Render          (FLOAT_32 delta);
VOID dr_Matrices        ();

#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// INLINE FUNCTIONS
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------
INLINEF FLOAT_32 dr_SetDistanceq (UINT_32 ID)
{
    // shortcut
    register FLOAT_32P center = &dr_object_centers [ID * 3];

    // camera vector
    register FLOAT_32 v [3] = {  dr_campos [0] - center [0],
                                 dr_campos [1] - center [1],
                                 dr_campos [2] - center [2] };

    // quadratic distance
    return (dr_object_distancesq [ID] = vLENGTHQ3 (v));
}

// ----------------------------------------------------
INLINEF VOID dr_SetDetail (UINT_32 ID, FLOAT_32 dist)
{
    register TDetail * detail = &dr_object_details [ID];

    if (detail->lodbase != 0xffff) {
        if (dist < detail->lod1_distance) {             dr_object_models [ID] = detail->lodbase; dr_object_morph1 [ID] = detail->start;         dr_object_morph2 [ID] = 1.0f / (detail->lod1_distance - detail->start);         } else {
            if (dist < detail->lod2_distance) {         dr_object_models [ID] = detail->lod1;    dr_object_morph1 [ID] = detail->lod1_distance; dr_object_morph2 [ID] = 1.0f / (detail->lod2_distance - detail->lod1_distance); } else {
                if (dist < detail->lod3_distance) {     dr_object_models [ID] = detail->lod2;    dr_object_morph1 [ID] = detail->lod2_distance; dr_object_morph2 [ID] = 1.0f / (detail->lod3_distance - detail->lod2_distance); } else {
                                                        dr_object_models [ID] = detail->lod3;    dr_object_morph1 [ID] = detail->lod3_distance; dr_object_morph2 [ID] = 1.0f / (detail->end           - detail->lod3_distance);
                }
            }
        }
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// MACRO FUNCTIONS
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------
#define M_DISAPPEAR_CULLING(tags) {             \
                                                \
    if (tags & M_TAG_DISTANCEQ) {               \
                                                \
        if (tags & M_TAG_DISAPPEAR) continue;   \
                                                \
    } else {                                    \
                                                \
        register FLOAT_32P center    = &dr_object_centers   [ID * 3];   \
        register FLOAT_32  disappear =  dr_object_disappear [ID];       \
                                                                        \
        register FLOAT_32 vx = dr_campos [0] - center [0];  if ((vx *= vx) > disappear) continue;   \
        register FLOAT_32 vy = dr_campos [1] - center [1];  if ((vy *= vy) > disappear) continue;   \
        register FLOAT_32 vz = dr_campos [2] - center [2];  if ((vz *= vz) > disappear) continue;   \
                                                                                                    \
        if ((dr_object_distancesq [ID] = vx + vy + vz) > disappear) {               \
                                                                                    \
            dr_object_tags [ID] |= M_TAG_DISAPPEAR | M_TAG_DISTANCEQ; continue;     \
        }                                                                           \
                                                                                    \
        tags |= M_TAG_DISTANCEQ;    \
    }                               \
}

// ----------------------------------------------------

#define M_STATE_MATRIXMODE_PROJECTION                       if ( dr_state_matrixmode != GL_PROJECTION)  { glMatrixMode (GL_PROJECTION); dr_state_matrixmode = GL_PROJECTION; }
#define M_STATE_MATRIXMODE_MODELVIEW                        if ( dr_state_matrixmode != GL_MODELVIEW)   { glMatrixMode (GL_MODELVIEW);  dr_state_matrixmode = GL_MODELVIEW; }
#define M_STATE_MATRIXMODE_TEXTURE                          if ( dr_state_matrixmode != GL_TEXTURE)     { glMatrixMode (GL_TEXTURE);    dr_state_matrixmode = GL_TEXTURE; }

#define M_STATE_PLANE_CLEAR(plane)                          if ( dr_state_planes [plane])       { glDisable (GL_CLIP_PLANE0 + plane); dr_state_planes [plane] = false; }
#define M_STATE_PLANE_SET(plane)                            if (!dr_state_planes [plane])       { glEnable  (GL_CLIP_PLANE0 + plane); dr_state_planes [plane] = true; }

#define M_STATE_DEPTHMASK_CLEAR                             if ( dr_state_depthmask)            { glDepthMask (GL_FALSE);       dr_state_depthmask = false; }
#define M_STATE_DEPTHMASK_SET                               if (!dr_state_depthmask)            { glDepthMask (GL_TRUE);        dr_state_depthmask = true; }

#define M_STATE_DEPTHTEST_CLEAR                             if ( dr_state_depthtest)            { glDisable (GL_DEPTH_TEST);    dr_state_depthtest = false; }
#define M_STATE_DEPTHTEST_SET                               if (!dr_state_depthtest)            { glEnable  (GL_DEPTH_TEST);    dr_state_depthtest = true; }

#define M_STATE_CULLFACE_CLEAR                              if ( dr_state_cullface)             { glDisable (GL_CULL_FACE);     dr_state_cullface = false; }
#define M_STATE_CULLFACE_SET                                if (!dr_state_cullface)             { glEnable  (GL_CULL_FACE);     dr_state_cullface = true; }

#define M_STATE_INDICES_CLEAR                               if ( dr_state_indices)              { glBindBufferARB (GL_ELEMENT_ARRAY_BUFFER_ARB, 0);     dr_state_indices    = 0;    }
#define M_STATE_INDICES_SET(ebo)                            if ( dr_state_indices   != ebo)     { glBindBufferARB (GL_ELEMENT_ARRAY_BUFFER_ARB, ebo);   dr_state_indices    = ebo;  }

#define M_STATE_ARRAY_COLOR_CLEAR                           if ( dr_state_color)                { glDisableClientState (GL_COLOR_ARRAY);                dr_state_color      = 0; }
#define M_STATE_ARRAY_COLOR_SET(vbo, size, type)            if ( dr_state_color     != vbo)     { glEnableClientState  (GL_COLOR_ARRAY);                dr_state_color      = vbo;  glBindBufferARB (GL_ARRAY_BUFFER_ARB, vbo); glColorPointer (size, type, 0, NULL);   }

#define M_STATE_ARRAY_SECONDARY_COLOR_CLEAR                 if ( dr_state_colors)               { glDisableClientState (GL_SECONDARY_COLOR_ARRAY);      dr_state_colors     = 0; }
#define M_STATE_ARRAY_SECONDARY_COLOR_SET(vbo, size, type)  if ( dr_state_colors    != vbo)     { glEnableClientState  (GL_SECONDARY_COLOR_ARRAY);      dr_state_colors     = vbo;  glBindBufferARB (GL_ARRAY_BUFFER_ARB, vbo); glSecondaryColorPointer (size, type, 0, NULL);  }

#define M_STATE_ARRAY_VERTEX_CLEAR                          if ( dr_state_vertex)               { glDisableClientState (GL_VERTEX_ARRAY);               dr_state_vertex     = 0; }
#define M_STATE_ARRAY_VERTEX_SET(vbo, size, type)           if ( dr_state_vertex    != vbo)     { glEnableClientState  (GL_VERTEX_ARRAY);               dr_state_vertex     = vbo;  glBindBufferARB (GL_ARRAY_BUFFER_ARB, vbo); glVertexPointer (size, type, 0, NULL);  }

#define M_STATE_ARRAY_NORMAL_CLEAR                          if ( dr_state_normal)               { glDisableClientState (GL_NORMAL_ARRAY);               dr_state_normal     = 0; }
#define M_STATE_ARRAY_NORMAL_SET(vbo, type)                 if ( dr_state_normal    != vbo)     { glEnableClientState  (GL_NORMAL_ARRAY);               dr_state_normal     = vbo;  glBindBufferARB (GL_ARRAY_BUFFER_ARB, vbo); glNormalPointer (type, 0, NULL);    }

#define M_STATE_ARRAY_TEX0_COORDS_CLEAR                     if ( dr_state_tex0_coord)           { glClientActiveTexture (GL_TEXTURE0);  glDisableClientState (GL_TEXTURE_COORD_ARRAY);   dr_state_tex0_coord = 0;   }
#define M_STATE_ARRAY_TEX0_COORDS_SET(vbo, size, type)      if ( dr_state_tex0_coord != vbo)    { glClientActiveTexture (GL_TEXTURE0);  glEnableClientState  (GL_TEXTURE_COORD_ARRAY);   dr_state_tex0_coord = vbo; glBindBufferARB (GL_ARRAY_BUFFER_ARB, vbo); glTexCoordPointer (size, type, 0, NULL);    }
#define M_STATE_ARRAY_TEX1_COORDS_CLEAR                     if ( dr_state_tex1_coord)           { glClientActiveTexture (GL_TEXTURE1);  glDisableClientState (GL_TEXTURE_COORD_ARRAY);   dr_state_tex1_coord = 0;   }
#define M_STATE_ARRAY_TEX1_COORDS_SET(vbo, size, type)      if ( dr_state_tex1_coord != vbo)    { glClientActiveTexture (GL_TEXTURE1);  glEnableClientState  (GL_TEXTURE_COORD_ARRAY);   dr_state_tex1_coord = vbo; glBindBufferARB (GL_ARRAY_BUFFER_ARB, vbo); glTexCoordPointer (size, type, 0, NULL);    }
#define M_STATE_ARRAY_TEX2_COORDS_CLEAR                     if ( dr_state_tex2_coord)           { glClientActiveTexture (GL_TEXTURE2);  glDisableClientState (GL_TEXTURE_COORD_ARRAY);   dr_state_tex2_coord = 0;   }
#define M_STATE_ARRAY_TEX2_COORDS_SET(vbo, size, type)      if ( dr_state_tex2_coord != vbo)    { glClientActiveTexture (GL_TEXTURE2);  glEnableClientState  (GL_TEXTURE_COORD_ARRAY);   dr_state_tex2_coord = vbo; glBindBufferARB (GL_ARRAY_BUFFER_ARB, vbo); glTexCoordPointer (size, type, 0, NULL);    }

#define M_STATE_TEX0_RECT_CLEAR                             if ( dr_state_tex0_rect)            { glActiveTexture (GL_TEXTURE0); glDisable (GL_TEXTURE_RECTANGLE_ARB);  dr_state_tex0_rect = 0;     }
#define M_STATE_TEX0_RECT_SET(tex)                          if ( dr_state_tex0_rect != tex)     { glActiveTexture (GL_TEXTURE0); glEnable  (GL_TEXTURE_RECTANGLE_ARB);  dr_state_tex0_rect = tex;   glBindTexture (GL_TEXTURE_RECTANGLE_ARB, tex);    }
#define M_STATE_TEX1_RECT_CLEAR                             if ( dr_state_tex1_rect)            { glActiveTexture (GL_TEXTURE1); glDisable (GL_TEXTURE_RECTANGLE_ARB);  dr_state_tex1_rect = 0;     }
#define M_STATE_TEX1_RECT_SET(tex)                          if ( dr_state_tex1_rect != tex)     { glActiveTexture (GL_TEXTURE1); glEnable  (GL_TEXTURE_RECTANGLE_ARB);  dr_state_tex1_rect = tex;   glBindTexture (GL_TEXTURE_RECTANGLE_ARB, tex);    }
#define M_STATE_TEX2_RECT_CLEAR                             if ( dr_state_tex2_rect)            { glActiveTexture (GL_TEXTURE2); glDisable (GL_TEXTURE_RECTANGLE_ARB);  dr_state_tex2_rect = 0;     }
#define M_STATE_TEX2_RECT_SET(tex)                          if ( dr_state_tex2_rect != tex)     { glActiveTexture (GL_TEXTURE2); glEnable  (GL_TEXTURE_RECTANGLE_ARB);  dr_state_tex2_rect = tex;   glBindTexture (GL_TEXTURE_RECTANGLE_ARB, tex);    }

#define M_STATE_TEX0_CLEAR                                  if ( dr_state_tex0)                 { glActiveTexture (GL_TEXTURE0); glDisable (GL_TEXTURE_2D);  dr_state_tex0 = 0;      }
#define M_STATE_TEX0_SET(tex)                               if ( dr_state_tex0 != tex)          { glActiveTexture (GL_TEXTURE0); glEnable  (GL_TEXTURE_2D);  dr_state_tex0 = tex;   glBindTexture (GL_TEXTURE_2D, tex);      }
#define M_STATE_TEX1_CLEAR                                  if ( dr_state_tex1)                 { glActiveTexture (GL_TEXTURE1); glDisable (GL_TEXTURE_2D);  dr_state_tex1 = 0;      }
#define M_STATE_TEX1_SET(tex)                               if ( dr_state_tex1 != tex)          { glActiveTexture (GL_TEXTURE1); glEnable  (GL_TEXTURE_2D);  dr_state_tex1 = tex;   glBindTexture (GL_TEXTURE_2D, tex);      }
#define M_STATE_TEX2_CLEAR                                  if ( dr_state_tex2)                 { glActiveTexture (GL_TEXTURE2); glDisable (GL_TEXTURE_2D);  dr_state_tex2 = 0;      }
#define M_STATE_TEX2_SET(tex)                               if ( dr_state_tex2 != tex)          { glActiveTexture (GL_TEXTURE2); glEnable  (GL_TEXTURE_2D);  dr_state_tex2 = tex;   glBindTexture (GL_TEXTURE_2D, tex);      }
#define M_STATE_TEX3_CLEAR                                  if ( dr_state_tex3)                 { glActiveTexture (GL_TEXTURE3); glDisable (GL_TEXTURE_2D);  dr_state_tex3 = 0;      }
#define M_STATE_TEX3_SET(tex)                               if ( dr_state_tex3 != tex)          { glActiveTexture (GL_TEXTURE3); glEnable  (GL_TEXTURE_2D);  dr_state_tex3 = tex;   glBindTexture (GL_TEXTURE_2D, tex);      }
#define M_STATE_TEX4_CLEAR                                  if ( dr_state_tex4)                 { glActiveTexture (GL_TEXTURE4); glDisable (GL_TEXTURE_2D);  dr_state_tex4 = 0;      }
#define M_STATE_TEX4_SET(tex)                               if ( dr_state_tex4 != tex)          { glActiveTexture (GL_TEXTURE4); glEnable  (GL_TEXTURE_2D);  dr_state_tex4 = tex;   glBindTexture (GL_TEXTURE_2D, tex);      }
#define M_STATE_TEX5_CLEAR                                  if ( dr_state_tex5)                 { glActiveTexture (GL_TEXTURE5); glDisable (GL_TEXTURE_2D);  dr_state_tex5 = 0;      }
#define M_STATE_TEX5_SET(tex)                               if ( dr_state_tex5 != tex)          { glActiveTexture (GL_TEXTURE5); glEnable  (GL_TEXTURE_2D);  dr_state_tex5 = tex;   glBindTexture (GL_TEXTURE_2D, tex);      }
#define M_STATE_TEX6_CLEAR                                  if ( dr_state_tex6)                 { glActiveTexture (GL_TEXTURE6); glDisable (GL_TEXTURE_2D);  dr_state_tex6 = 0;      }
#define M_STATE_TEX6_SET(tex)                               if ( dr_state_tex6 != tex)          { glActiveTexture (GL_TEXTURE6); glEnable  (GL_TEXTURE_2D);  dr_state_tex6 = tex;   glBindTexture (GL_TEXTURE_2D, tex);      }
#define M_STATE_TEX7_CLEAR                                  if ( dr_state_tex7)                 { glActiveTexture (GL_TEXTURE7); glDisable (GL_TEXTURE_2D);  dr_state_tex7 = 0;      }
#define M_STATE_TEX7_SET(tex)                               if ( dr_state_tex7 != tex)          { glActiveTexture (GL_TEXTURE7); glEnable  (GL_TEXTURE_2D);  dr_state_tex7 = tex;   glBindTexture (GL_TEXTURE_2D, tex);      }
#define M_STATE_TEX8_CLEAR                                  if ( dr_state_tex8)                 { glActiveTexture (GL_TEXTURE8); glDisable (GL_TEXTURE_2D);  dr_state_tex8 = 0;      }
#define M_STATE_TEX8_SET(tex)                               if ( dr_state_tex8 != tex)          { glActiveTexture (GL_TEXTURE8); glEnable  (GL_TEXTURE_2D);  dr_state_tex8 = tex;   glBindTexture (GL_TEXTURE_2D, tex);      }
#define M_STATE_TEX9_CLEAR                                  if ( dr_state_tex9)                 { glActiveTexture (GL_TEXTURE9); glDisable (GL_TEXTURE_2D);  dr_state_tex9 = 0;      }
#define M_STATE_TEX9_SET(tex)                               if ( dr_state_tex9 != tex)          { glActiveTexture (GL_TEXTURE9); glEnable  (GL_TEXTURE_2D);  dr_state_tex9 = tex;   glBindTexture (GL_TEXTURE_2D, tex);      }

#endif