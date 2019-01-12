
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "types.h"
#include "debug.h"

#include "shaderGenerator.h"

VOID dr_GenerateShader (UINT_32 flags, CHARPP sh_vertex, CHARPP sh_fragment) {

	UINT_8 shaderid = flags;

    CHAR sv [2048];     sv [0] = 0;
    CHAR sf [16384];    sf [0] = 0;

    // ------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // ------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // ------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // VERTEX SHADER
    // ------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // ------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // ------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    
    strcat (sv, "\n");

    // -------------------------
    // INPUTS & OUTPUTS
    // -------------------------

    // DEPTH
    if (flags & M_SHFLAG_DEPTH) {

        if (flags & M_SHFLAG_TRANSPARENT) {

            // TODO: considere what is actually factor for ..
            //       what it exactly is that we can use it every time with transparency to help peventing alpha disappearing cause of mipmap blurring
            //       it uses gl_FogCoord .. which also always should represent the same thing .. INVESTIGATE

            strcat (sv, "varying float factor;\n\n");
        }

    // GBUFFERS
    } else {

        if (flags & M_SHFLAG_VERTEX_COLORS) {

            strcat (sv, "varying vec3 color;\n\n");
        }

        if (flags & M_SHFLAG_COMPOSITE) {

            strcat (sv, "varying mat3 tbni;\n\n");

        } else {

            strcat (sv, "varying vec3 normal;\n\n");
        }
    }

    strcat (sv, "void main () {\n\n");

    // -------------------------
    // MODEL MATRIX
    // -------------------------

    if (flags & M_SHFLAG_INSTANCED) {

        strcat (sv,
                "   vec3 co = cos (gl_MultiTexCoord5.xyz);\n"
                "   vec3 si = sin (gl_MultiTexCoord5.xyz);\n\n"
                "   mat4 matrix     = mat4 (vec4 ( co.y*co.z,                  -co.y*si.z,                   si.y,      0.0) * gl_MultiTexCoord5.w, \n"
                "                           vec4 ( co.x*si.z + si.x*si.y*co.z,  co.x*co.z - si.x*si.y*si.z, -si.x*co.y, 0.0) * gl_MultiTexCoord5.w, \n"
                "                           vec4 ( si.x*si.z - co.x*si.y*co.z,  si.x*co.z + co.x*si.y*si.z,  co.x*co.y, 0.0) * gl_MultiTexCoord5.w, \n"
                "                           vec4 (gl_MultiTexCoord4.xyz, 1.0)); \n\n"
                );

    } else {

        strcat (sv,
                "   mat4 matrix     = mat4 (vec4 (gl_MultiTexCoord4.xyz, 0.0), \n"
                "                           vec4 (gl_MultiTexCoord5.xyz, 0.0), \n"
                "                           vec4 (gl_MultiTexCoord6.xyz, 0.0), \n"
                "                           vec4 (gl_MultiTexCoord7.xyz, 1.0)); \n\n"
                );
    }

    strcat (sv, "   matrix          = gl_ModelViewMatrix * matrix;\n\n");

    // -------------------------
    // UV
    // -------------------------

    // DEPTH
    if (flags & M_SHFLAG_DEPTH) {

        if (flags & M_SHFLAG_TRANSPARENT) {

            if (flags & M_SHFLAG_UV1_PRECISION_HI) {
                strcat (sv, "   gl_TexCoord [0] = vec4 (gl_MultiTexCoord0.st, 0.0, 0.0);\n\n"); } else {
                strcat (sv, "   gl_TexCoord [0] = vec4 (gl_MultiTexCoord0.st * vec2 (10.0 / 32767.0, 10.0 / 32767.0), 0.0, 0.0);\n\n");
            }
        }

    // GBUFFERS
    } else {

        if (flags & M_SHFLAG_UV1_PRECISION_HI) {
            strcat (sv, "   gl_TexCoord [0] = vec4 (gl_MultiTexCoord0.st, 0.0, 0.0);\n"); } else {
            strcat (sv, "   gl_TexCoord [0] = vec4 (gl_MultiTexCoord0.st * vec2 (10.0 / 32767.0, 10.0 / 32767.0), 0.0, 0.0);\n");
        }
        
        if (flags & M_SHFLAG_MULTILAYER) {
            
            if (flags & M_SHFLAG_UV2_PRECISION_HI) {
                strcat (sv, "   gl_TexCoord [1] = vec4 (gl_MultiTexCoord1.st, 0.0, 0.0);\n\n"); } else {
                strcat (sv, "   gl_TexCoord [1] = vec4 (gl_MultiTexCoord1.st * vec2 (10.0 / 32767.0, 10.0 / 32767.0), 0.0, 0.0);\n\n");
            }
            
        } else {            
            strcat (sv, "\n");
        }
    }

    // -------------------------
    // TRANSLUCENT ALPHA THRESHOLD (FACTOR)
    // -------------------------

    BOOL hasfactor = false;

    // NOTE: we do alpha testing only in depth shader .. (no need to do in gbuffers stage cause of early Z-culling )
    
    if ((flags & M_SHFLAG_TRANSPARENT) && (flags & M_SHFLAG_DEPTH)) {

        if (flags & M_SHFLAG_DEFORM_SHRINK) {   strcat (sv, "   factor          = max (0.0, 1.0 - gl_FogCoord);\n\n");  hasfactor = true; } else
        if (flags & M_SHFLAG_DEFORM_MORPH)  {   strcat (sv, "   factor          = max (0.0, 1.0 - gl_FogCoord);\n\n");  hasfactor = true; } else
        if (flags & M_SHFLAG_DEFORM_GROW)   {   strcat (sv, "   factor          = factors.x;\n\n");                     hasfactor = true; } else {

            if (flags & M_SHFLAG_DISSOLVE) {
                strcat (sv, "   factor          = gl_FogCoord * min (1.0, max (0.0, gl_TexCoord [0].x - 1.0) * 1000000.0) + gl_MultiTexCoord2.y;\n\n"); } else {
                strcat (sv, "   factor          = max (0.0, 1.0 - gl_FogCoord);\n\n");
            }

            hasfactor = true; 
        }
    }
    
    // -------------------------
    // VERTEX DEFORMATIONS
    // -------------------------

    strcat (sv, "   vec4 v          = gl_Vertex;\n\n");

    if (flags & M_SHFLAG_DEFORM_SHRINK) {

        if (!hasfactor) {

            strcat (sv, "   float factor    = max (0.0, 1.0 - gl_FogCoord);\n\n");
        }

        strcat (sv, "   v               = v * vec4 (factor, factor, factor, 1.0);\n\n");

    } else
    if (flags & M_SHFLAG_DEFORM_MORPH) {

        strcat (sv, "   vec4 vec = (32767.0 + gl_MultiTexCoord1.xyzw) * 1.0/256.0;\n\n");

        strcat (sv, "   vec4 vv1 = (2.0 * (floor (vec) *   1.0/255.0) - 1.0) * gl_FogCoord;\n");
        strcat (sv, "   vec4 vv2 = (2.0 *  fract (vec) * 256.0/255.0  - 1.0) * gl_FogCoord;\n\n");

        strcat (sv, "   v = v + vec4 (vv1.x, vv2.x, vv1.y, 0.0) * gl_MultiTexCoord2.x;\n\n");

        // TODO: OPTIMIZE HERE .. evaluate factor beforehand and update morphing to use factor

    } else
    if (flags & M_SHFLAG_DEFORM_GROW) {

        strcat (sv, "   vec2 factors    = min (1.0, max (0.0, gl_TexCoord [0].xy - vec2 (1.0, 1.0)) * 1000000.0) * gl_FogCoord;\n\n");

        strcat (sv, "   v.xyz *= (vec3 (1.0, 1.0, 1.0) - gl_MultiTexCoord3.xyz * factors.yyy);   v.z += gl_MultiTexCoord3.w * factors.y;\n\n");
    }

    // -------------------------
    // VERTEX ANIMATIONS
    // -------------------------

    if (flags & M_SHFLAG_ANIM_WIND1) {

        strcat (sv, "   v.y             = v.y + dot (v, v) * gl_MultiTexCoord1.x;\n");
        strcat (sv, "   v.x             = v.x + dot (v, v) * gl_MultiTexCoord1.x;\n\n");
    }

    if (flags & M_SHFLAG_ANIM_WIND2) {

        strcat (sv, "   v.y             = v.y + v.z * gl_MultiTexCoord2.x;\n\n");
    }
    
    // -------------------------
    // VERTEX TRANSFORM
    // -------------------------

    strcat (sv, "   gl_ClipVertex   = matrix * v;\n");

    strcat (sv, "   gl_Position     = gl_ProjectionMatrix * gl_ClipVertex;\n\n");

    // -------------------------
    // COLORS & NORMALS
    // -------------------------
    
    // DEPTH
    if (flags & M_SHFLAG_DEPTH) {

    // GBUFFERS
    } else {

        if (flags & M_SHFLAG_VERTEX_COLORS) {

            strcat (sv, "   color           = gl_Color.rgb;\n\n");
        }

        if (flags & M_SHFLAG_COMPOSITE) {

            strcat (sv, "   vec3 normal     = normalize (mat3 (matrix) * gl_Normal);\n");
            strcat (sv, "   vec3 tangent    = normalize (mat3 (matrix) * (2.0 * gl_SecondaryColor.xyz - 1.0));\n");
            strcat (sv, "   vec3 binormal   = cross     (tangent, normal);\n\n");

            strcat (sv, "   tbni [0]        = vec3 (tangent.x, binormal.x, normal.x);\n");
            strcat (sv, "   tbni [1]        = vec3 (tangent.y, binormal.y, normal.y);\n");
            strcat (sv, "   tbni [2]        = vec3 (tangent.z, binormal.z, normal.z);\n\n");

        } else {

            if (flags & M_SHFLAG_SINGLE_NORMAL) {
            
                strcat (sv, "   normal          = normalize (normalize (mat3 (matrix) * vec3 (0.0, 0.0, 1.0)) + 0.5 * gl_LightSource [0].position);\n");    } else {
                strcat (sv, "   normal          = normalize (mat3 (matrix) * gl_Normal);\n");                            
            }
        }
    }

    strcat (sv, "}\n\n");

    debug_Print ("-------------------------------------------------------------------------------------------------------------------\n");
    debug_Print ("-- VERTEX SHADER\n");
    debug_Print ("-------------------------------------------------------------------------------------------------------------------\n");
    debug_Print (sv);

    // ------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // ------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // ------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // FRAGMENT SHADER
    // ------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // ------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // ------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    strcat (sf, "\n");

    // -------------------------
    // INPUTS & OUTPUTS
    // -------------------------
    
    // DEPTH
    if (flags & M_SHFLAG_DEPTH) {

        if (flags & M_SHFLAG_TRANSPARENT) {

            strcat (sf, "uniform sampler2D tex_diffuse;\n\n");
            strcat (sf, "varying float factor;\n\n");
        }

    // GBUFFERS
    } else {

        strcat (sf, "uniform sampler2D tex_diffuse;\n");
        
        if (flags & M_SHFLAG_COMPOSITE) {
            
            strcat (sf, "uniform sampler2D tex_composite;\n\n");    } else {
            strcat (sf, "\n");
        }
        
        if (flags & M_SHFLAG_MULTILAYER) {
            
            strcat (sf, "uniform sampler2D tex_weights1;\n");
            strcat (sf, "uniform sampler2D tex_weights2;\n");
            strcat (sf, "uniform sampler2D tex_weights3;\n");
            strcat (sf, "uniform sampler2D tex_weights4;\n\n");
        }
        
        if (flags & M_SHFLAG_COMPOSITE) {
                        
            strcat (sf, "varying mat3 tbni;\n\n");        } else {            
            strcat (sf, "varying vec3 normal;\n\n");
        }

        if (flags & M_SHFLAG_VERTEX_COLORS) {

            strcat (sf, "varying vec3 color;\n\n");
        }
    }

    strcat (sf, "void main () {\n\n");

    // -------------------------
    // DEPTH
    // -------------------------
    
    if (flags & M_SHFLAG_DEPTH) {

        if (flags & M_SHFLAG_TRANSPARENT) {

            // ALPHA TESTING
            
            // TODO: if (texture2D (tex_diffuse, gl_TexCoord [0].st).a < (threshold + damping * factor)) discard;
            
            // helps preventing disappearing caused by alpha channel blurring on mipmaps
            strcat (sf, "    if (texture2D (tex_diffuse, gl_TexCoord [0].st).a < (0.5 - 0.3 * factor)) discard;\n\n");
        }

    // -------------------------
    // GBUFFERS
    // -------------------------
        
    } else {

        // -------------------------
        // TEXTURES
        // -------------------------
        
        if (flags & M_SHFLAG_MULTILAYER) {
            
            // EVALUATE UV AND LOD ----------------------------------------------------------------------------------------------------------------------------------
            
            strcat (sf, "   vec2 uv = gl_TexCoord [0].st * 0.25;\n"
                        "   \n"
                        "   vec2 dx = dFdx (uv * 4096);\n"
                        "   vec2 dy = dFdy (uv * 4096);\n"
                        "   \n"
                        "   float lod = clamp (0.5 * log2 (max (dot (dx, dx), dot (dy, dy))), 0.0, 8.0);       // LOAD MAX LOD (8) !!!\n"
                        "   \n"
                        "   float pix = pow (2, lod) / 4096;\n"
                        "   \n"
                        "   uv = (gl_TexCoord [0].st - floor (gl_TexCoord [0].st)) * (0.25 - pix) + 0.5 * vec2 (pix, pix);\n"
                        "   \n"
                        "   vec2 uv_scaled = gl_TexCoord [0].st * 0.03;\n"
                        "   \n"
                        "   vec2 uv_mod = uv_scaled * 0.25;\n"
                        "   \n"
                        "   dx = dFdx (uv_mod * 4096);\n"
                        "   dy = dFdy (uv_mod * 4096);\n"
                        "   \n"
                        "   float lod_mod = clamp (0.5 * log2 (max (dot (dx, dx), dot (dy, dy))), 0.0, 8.0);   // LOAD MAX LOD (8) !!!\n"
                        "   \n"
                        "   pix = pow (2, lod_mod) / 4096;\n"
                        "   \n"
                        "   uv_mod = (uv_scaled - floor (uv_scaled)) * (0.25 - pix) + 0.5 * vec2 (pix, pix);\n"
                        "   \n");
                    
            // WEIGHTS ----------------------------------------------------------------------------------------------------------------------------------------------

            // TODO: CONSIDER CHANGE WEIGHTS TO 4 BIT AND REDUCE 4 LOOKUPS TO 2 LOOKUPS

            strcat (sf, "   vec4 weights1 = texture2D (tex_weights1, gl_TexCoord [1].st);\n"
                        "   vec4 weights2 = texture2D (tex_weights2, gl_TexCoord [1].st);\n"
                        "   vec4 weights3 = texture2D (tex_weights3, gl_TexCoord [1].st);\n"
                        "   vec4 weights4 = texture2D (tex_weights4, gl_TexCoord [1].st);\n\n");

            strcat (sf, "   vec4 factor1 = vec4 (1.0, 1.0, 1.0, 1.0);\n"
                        "   vec4 factor2 = vec4 (1.0, 1.0, 1.0, 1.0);\n"
                        "   vec4 factor3 = vec4 (1.0, 1.0, 1.0, 1.0);\n"
                        "   vec4 factor4 = vec4 (1.0, 1.0, 1.0, 1.0);\n\n");

            strcat (sf, "   vec4 sum = vec4 (0.0, 0.0, 0.0, 0.0);\n\n");

            strcat (sf, "   if (dot (weights1, weights1) > 0.0) {\n"
                        "       if (weights1.r > 0.0)   factor1.r = texture2DLod (tex_diffuse, uv_mod + vec2 (0.0,  0.0),  lod_mod).a;\n"
                        "       if (weights1.g > 0.0)   factor1.g = texture2DLod (tex_diffuse, uv_mod + vec2 (0.25, 0.0),  lod_mod).a;\n"
                        "       if (weights1.b > 0.0)   factor1.b = texture2DLod (tex_diffuse, uv_mod + vec2 (0.5,  0.0),  lod_mod).a;\n"
                        "       if (weights1.a > 0.0)   factor1.a = texture2DLod (tex_diffuse, uv_mod + vec2 (0.75, 0.0),  lod_mod).a;\n"
                        "       factor1 = max (1.3 * pow (factor1, 3.0), 0.2);   weights1 *= pow (factor1, 1.0);    sum.r = dot (weights1, vec4 (1.0, 1.0, 1.0, 1.0));\n"
                        "   }\n\n");
                    
            strcat (sf, "   if (dot (weights2, weights2) > 0.0) {\n"
                        "       if (weights2.r > 0.0)   factor2.r = texture2DLod (tex_diffuse, uv_mod + vec2 (0.0,  0.25), lod_mod).a;\n"
                        "       if (weights2.g > 0.0)   factor2.g = texture2DLod (tex_diffuse, uv_mod + vec2 (0.25, 0.25), lod_mod).a;\n"
                        "       if (weights2.b > 0.0)   factor2.b = texture2DLod (tex_diffuse, uv_mod + vec2 (0.5,  0.25), lod_mod).a;\n"
                        "       if (weights2.a > 0.0)   factor2.a = texture2DLod (tex_diffuse, uv_mod + vec2 (0.75, 0.25), lod_mod).a;\n"
                        "       factor2 = max (1.3 * pow (factor2, 3.0), 0.2);   weights2 *= pow (factor2, 1.0);    sum.g = dot (weights2, vec4 (1.0, 1.0, 1.0, 1.0));\n"
                        "   }\n\n");
                    
            strcat (sf, "   if (dot (weights3, weights3) > 0.0) {\n"
                        "       if (weights3.r > 0.0)   factor3.r = texture2DLod (tex_diffuse, uv_mod + vec2 (0.0,  0.5),  lod_mod).a;\n"
                        "       if (weights3.g > 0.0)   factor3.g = texture2DLod (tex_diffuse, uv_mod + vec2 (0.25, 0.5),  lod_mod).a;\n"
                        "       if (weights3.b > 0.0)   factor3.b = texture2DLod (tex_diffuse, uv_mod + vec2 (0.5,  0.5),  lod_mod).a;\n"
                        "       if (weights3.a > 0.0)   factor3.a = texture2DLod (tex_diffuse, uv_mod + vec2 (0.75, 0.5),  lod_mod).a;\n"
                        "       factor3 = max (1.3 * pow (factor3, 3.0), 0.2);   weights3 *= pow (factor3, 1.0);    sum.b = dot (weights3, vec4 (1.0, 1.0, 1.0, 1.0));\n"
                        "   }\n\n");
                    
            strcat (sf, "   if (dot (weights4, weights4) > 0.0) {\n"
                        "       if (weights4.r > 0.0)   factor4.r = texture2DLod (tex_diffuse, uv_mod + vec2 (0.0,  0.75), lod_mod).a;\n"
                        "       if (weights4.g > 0.0)   factor4.g = texture2DLod (tex_diffuse, uv_mod + vec2 (0.25, 0.75), lod_mod).a;\n"
                        "       if (weights4.b > 0.0)   factor4.b = texture2DLod (tex_diffuse, uv_mod + vec2 (0.5,  0.75), lod_mod).a;\n"
                        "       if (weights4.a > 0.0)   factor4.a = texture2DLod (tex_diffuse, uv_mod + vec2 (0.75, 0.75), lod_mod).a;\n"
                        "       factor4 = max (1.3 * pow (factor4, 3.0), 0.2);   weights4 *= pow (factor4, 1.0);    sum.a = dot (weights4, vec4 (1.0, 1.0, 1.0, 1.0));\n"
                        "   }\n\n");

            // NORMALIZING WEIGHTS ----------------------------------------------------------------------------------------------------------------------------------

            strcat (sf, "   float scale = 1.0 / dot (sum, vec4 (1.0, 1.0, 1.0, 1.0));\n\n");

            // TEXTURES ---------------------------------------------------------------------------------------------------------------------------------------------

            if (flags & M_SHFLAG_COMPOSITE) {
                
                strcat (sf, "   vec4 diffuse   = vec4 (0.0, 0.0, 0.0, 0.0);\n");
                strcat (sf, "   vec4 composite = vec4 (0.0, 0.0, 0.0, 0.0);\n\n");

                strcat (sf, "   if (sum.r > 0.0) {   weights1 *= scale;\n"
                            "       if (weights1.r > 0.0) {\n"
                            "           diffuse   += texture2DLod (tex_diffuse,     uv + vec2 (0.0,  0.0),  lod) * weights1.r * factor1.r;\n"
                            "           composite += texture2DLod (tex_composite,   uv + vec2 (0.0,  0.0),  lod) * weights1.r;\n"
                            "       }\n"
                            "       if (weights1.g > 0.0) {\n"
                            "           diffuse   += texture2DLod (tex_diffuse,     uv + vec2 (0.25, 0.0),  lod) * weights1.g * factor1.g;\n"
                            "           composite += texture2DLod (tex_composite,   uv + vec2 (0.25, 0.0),  lod) * weights1.g;\n"
                            "       }\n"
                            "       if (weights1.b > 0.0) {\n"
                            "           diffuse   += texture2DLod (tex_diffuse,     uv + vec2 (0.5,  0.0),  lod) * weights1.b * factor1.b;\n"
                            "           composite += texture2DLod (tex_composite,   uv + vec2 (0.5,  0.0),  lod) * weights1.b;\n"
                            "       }\n"
                            "       if (weights1.a > 0.0) {\n"
                            "           diffuse   += texture2DLod (tex_diffuse,     uv + vec2 (0.75, 0.0),  lod) * weights1.a * factor2.a;\n"
                            "           composite += texture2DLod (tex_composite,   uv + vec2 (0.75, 0.0),  lod) * weights1.a;\n"
                            "       }\n"
                            "   }\n\n");

                strcat (sf, "   if (sum.g > 0.0) {   weights2 *= scale;\n"
                            "       if (weights2.r > 0.0) {\n"
                            "           diffuse   += texture2DLod (tex_diffuse,     uv + vec2 (0.0,  0.25), lod) * weights2.r * factor2.r;\n"
                            "           composite += texture2DLod (tex_composite,   uv + vec2 (0.0,  0.25), lod) * weights2.r;\n"
                            "       }\n"
                            "       if (weights2.g > 0.0) {\n"
                            "           diffuse   += texture2DLod (tex_diffuse,     uv + vec2 (0.25, 0.25), lod) * weights2.g * factor2.g;\n"
                            "           composite += texture2DLod (tex_composite,   uv + vec2 (0.25, 0.25), lod) * weights2.g;\n"
                            "       }\n"
                            "       if (weights2.b > 0.0) {\n"
                            "           diffuse   += texture2DLod (tex_diffuse,     uv + vec2 (0.5,  0.25), lod) * weights2.b * factor2.b;\n"
                            "           composite += texture2DLod (tex_composite,   uv + vec2 (0.5,  0.25), lod) * weights2.b;\n"
                            "       }\n"
                            "       if (weights2.a > 0.0) {\n"
                            "           diffuse   += texture2DLod (tex_diffuse,     uv + vec2 (0.75, 0.25), lod) * weights2.a * factor2.a;\n"
                            "           composite += texture2DLod (tex_composite,   uv + vec2 (0.75, 0.25), lod) * weights2.a;\n"
                            "       }\n"
                            "   }\n\n");

                strcat (sf, "   if (sum.b > 0.0) {   weights3 *= scale;\n"
                            "       if (weights3.r > 0.0) {\n"
                            "           diffuse   += texture2DLod (tex_diffuse,     uv + vec2 (0.0,  0.5),  lod) * weights3.r * factor3.r;\n"
                            "           composite += texture2DLod (tex_composite,   uv + vec2 (0.0,  0.5),  lod) * weights3.r;\n"
                            "       }\n"
                            "       if (weights3.g > 0.0) {\n"
                            "           diffuse   += texture2DLod (tex_diffuse,     uv + vec2 (0.25, 0.5),  lod) * weights3.g * factor3.g;\n"
                            "           composite += texture2DLod (tex_composite,   uv + vec2 (0.25, 0.5),  lod) * weights3.g;\n"
                            "       }\n"
                            "       if (weights3.b > 0.0) {\n"
                            "           diffuse   += texture2DLod (tex_diffuse,     uv + vec2 (0.5,  0.5),  lod) * weights3.b * factor3.b;\n"
                            "           composite += texture2DLod (tex_composite,   uv + vec2 (0.5,  0.5),  lod) * weights3.b;\n"
                            "       }\n"
                            "       if (weights3.a > 0.0) {\n"
                            "           diffuse   += texture2DLod (tex_diffuse,     uv + vec2 (0.75, 0.5),  lod) * weights3.a * factor3.a;\n"
                            "           composite += texture2DLod (tex_composite,   uv + vec2 (0.75, 0.5),  lod) * weights3.a;\n"
                            "       }\n"
                            "   }\n\n");

                strcat (sf, "   if (sum.a > 0.0) {   weights4 *= scale;\n"
                            "       if (weights4.r > 0.0) {\n"
                            "           diffuse   += texture2DLod (tex_diffuse,     uv + vec2 (0.0,  0.75), lod) * weights4.r * factor4.r;\n"
                            "           composite += texture2DLod (tex_composite,   uv + vec2 (0.0,  0.75), lod) * weights4.r;\n"
                            "       }\n"
                            "       if (weights4.g > 0.0) {\n"
                            "           diffuse   += texture2DLod (tex_diffuse,     uv + vec2 (0.25, 0.75), lod) * weights4.g * factor4.g;\n"
                            "           composite += texture2DLod (tex_composite,   uv + vec2 (0.25, 0.75), lod) * weights4.g;\n"
                            "       }\n"
                            "       if (weights4.b > 0.0) {\n"
                            "           diffuse   += texture2DLod (tex_diffuse,     uv + vec2 (0.5,  0.75), lod) * weights4.b * factor4.b;\n"
                            "           composite += texture2DLod (tex_composite,   uv + vec2 (0.5,  0.75), lod) * weights4.b;\n"
                            "       }\n"
                            "       if (weights4.a > 0.0) {\n"
                            "           diffuse   += texture2DLod (tex_diffuse,     uv + vec2 (0.75, 0.75), lod) * weights4.a * factor4.a;\n"
                            "           composite += texture2DLod (tex_composite,   uv + vec2 (0.75, 0.75), lod) * weights4.a;\n"
                            "       }\n"
                            "   }\n\n");
            
            } else {
                
                strcat (sf, "   vec4 diffuse = vec4 (0.0, 0.0, 0.0, 0.0);\n");

                strcat (sf, "   if (sum.r > 0.0) {   weights1 *= scale;\n"
                            "       if (weights1.r > 0.0) {\n"
                            "           diffuse   += texture2DLod (tex_diffuse,     uv + vec2 (0.0,  0.0),  lod) * weights1.r * factor1.r;\n"
                            "       }\n"
                            "       if (weights1.g > 0.0) {\n"
                            "           diffuse   += texture2DLod (tex_diffuse,     uv + vec2 (0.25, 0.0),  lod) * weights1.g * factor1.g;\n"
                            "       }\n"
                            "       if (weights1.b > 0.0) {\n"
                            "           diffuse   += texture2DLod (tex_diffuse,     uv + vec2 (0.5,  0.0),  lod) * weights1.b * factor1.b;\n"
                            "       }\n"
                            "       if (weights1.a > 0.0) {\n"
                            "           diffuse   += texture2DLod (tex_diffuse,     uv + vec2 (0.75, 0.0),  lod) * weights1.a * factor2.a;\n"
                            "       }\n"
                            "   }\n\n");

                strcat (sf, "   if (sum.g > 0.0) {   weights2 *= scale;\n"
                            "       if (weights2.r > 0.0) {\n"
                            "           diffuse   += texture2DLod (tex_diffuse,     uv + vec2 (0.0,  0.25), lod) * weights2.r * factor2.r;\n"
                            "       }\n"
                            "       if (weights2.g > 0.0) {\n"
                            "           diffuse   += texture2DLod (tex_diffuse,     uv + vec2 (0.25, 0.25), lod) * weights2.g * factor2.g;\n"
                            "       }\n"
                            "       if (weights2.b > 0.0) {\n"
                            "           diffuse   += texture2DLod (tex_diffuse,     uv + vec2 (0.5,  0.25), lod) * weights2.b * factor2.b;\n"
                            "       }\n"
                            "       if (weights2.a > 0.0) {\n"
                            "           diffuse   += texture2DLod (tex_diffuse,     uv + vec2 (0.75, 0.25), lod) * weights2.a * factor2.a;\n"
                            "       }\n"
                            "   }\n\n");

                strcat (sf, "   if (sum.b > 0.0) {   weights3 *= scale;\n"
                            "       if (weights3.r > 0.0) {\n"
                            "           diffuse   += texture2DLod (tex_diffuse,     uv + vec2 (0.0,  0.5),  lod) * weights3.r * factor3.r;\n"
                            "       }\n"
                            "       if (weights3.g > 0.0) {\n"
                            "           diffuse   += texture2DLod (tex_diffuse,     uv + vec2 (0.25, 0.5),  lod) * weights3.g * factor3.g;\n"
                            "       }\n"
                            "       if (weights3.b > 0.0) {\n"
                            "           diffuse   += texture2DLod (tex_diffuse,     uv + vec2 (0.5,  0.5),  lod) * weights3.b * factor3.b;\n"
                            "       }\n"
                            "       if (weights3.a > 0.0) {\n"
                            "           diffuse   += texture2DLod (tex_diffuse,     uv + vec2 (0.75, 0.5),  lod) * weights3.a * factor3.a;\n"
                            "       }\n"
                            "   }\n\n");

                strcat (sf, "   if (sum.a > 0.0) {   weights4 *= scale;\n"
                            "       if (weights4.r > 0.0) {\n"
                            "           diffuse   += texture2DLod (tex_diffuse,     uv + vec2 (0.0,  0.75), lod) * weights4.r * factor4.r;\n"
                            "       }\n"
                            "       if (weights4.g > 0.0) {\n"
                            "           diffuse   += texture2DLod (tex_diffuse,     uv + vec2 (0.25, 0.75), lod) * weights4.g * factor4.g;\n"
                            "       }\n"
                            "       if (weights4.b > 0.0) {\n"
                            "           diffuse   += texture2DLod (tex_diffuse,     uv + vec2 (0.5,  0.75), lod) * weights4.b * factor4.b;\n"
                            "       }\n"
                            "       if (weights4.a > 0.0) {\n"
                            "           diffuse   += texture2DLod (tex_diffuse,     uv + vec2 (0.75, 0.75), lod) * weights4.a * factor4.a;\n"
                            "       }\n"
                            "   }\n\n");
            }
            
        } else {
        
            strcat (sf, "   vec4 diffuse    = texture2D (tex_diffuse,    gl_TexCoord [0].st);\n");

            if (flags & M_SHFLAG_COMPOSITE) {

                strcat (sf, "   vec4 composite  = texture2D (tex_composite,  gl_TexCoord [0].st);\n\n");    } else {
                strcat (sf, "\n");            
            }
        }
        
        // -------------------------
        // EVALUATE NORMAL
        // -------------------------
        
        if (flags & M_SHFLAG_COMPOSITE) {

            // NEED NORMALIZED NORMALS IN COMPOSITE !!!!!!!!!!!!!! sqrt (negative) causes visual errors

            strcat (sf, "   vec3 normal     = 2.0 * composite.gaa - 1.0;   normal.z = sqrt (1 - dot (normal.xy, normal.xy));   normal *= tbni;\n\n");
        }

        if (flags & M_SHFLAG_DOUBLESIDE) {

            if (flags & M_SHFLAG_SINGLE_NORMAL) {
                
            } else {
                
                // NOTE: try to eliminate branching here, use some of the shader functions or just multiply value with normalized dotproduct (-1 or 1)

                // normal facing camera
                strcat (sf, "   if (dot (vec3 (0.0, 0.0, -1.0), normal) > 0.0)  normal  = -normal;\n\n");
            }                     
        }
        
        // -------------------------
        // G1
        // -------------------------

        if (flags & M_SHFLAG_TRANSLUCENT) {

            // SET POWER BY SSS MASK !!!!!!!!!!!!!!!!!!!!!!!

            strcat (sf, "   float angle = dot (gl_LightSource [0].position.xyz, normal);\n\n");

            // looking at backside
            strcat (sf, "   if (angle < 0.0) {\n\n");

            strcat (sf, "       angle = abs (angle);\n\n");

            // side SSS + direct SSS

            if (flags & M_SHFLAG_VERTEX_COLORS) {

                strcat (sf, "       gl_FragData [0] = vec4 (diffuse.rgb * color.rgb, 0.3 * (1.0 - angle) + 0.4 * angle);\n\n");
                strcat (sf, "   } else {\n\n");
                strcat (sf, "       gl_FragData [0] = vec4 (diffuse.rgb * color.rgb, 1.0);\n\n");

            } else {

                strcat (sf, "       gl_FragData [0] = vec4 (diffuse.rgb, 0.3 * (1.0 - angle) + 0.4 * angle);\n\n");
                strcat (sf, "   } else {\n\n");
                strcat (sf, "       gl_FragData [0] = vec4 (diffuse.rgb, 1.0);\n\n");
            }

            strcat (sf, "   }\n\n");


        } else {

            if (flags & M_SHFLAG_VERTEX_COLORS) {

                strcat (sf, "   gl_FragData [0] = vec4 (diffuse.rgb * color.rgb, 1.0);\n\n");

            } else {

                strcat (sf, "   gl_FragData [0] = vec4 (diffuse.rgb, 1.0);\n\n");
            }
        }

        // -------------------------
        // G2
        // -------------------------

        // SPHEREMAP TRANSFORM
        //
        // http://aras-p.info/texts/CompactNormalStorage.html#method04spheremap
        //

        strcat (sf, "   vec2 normalm    = (normal.xy / sqrt (normal.z * 8.0 + 8.0) + 0.5) * 255.0;\n\n");

        strcat (sf, "   gl_FragData [1] = vec4 (floor (normalm.xy) * 0.00392156, fract (normalm.xy));\n\n");

        // -------------------------
        // G3
        // -------------------------

        if (flags & M_SHFLAG_COMPOSITE) {
            
            if (flags & M_SHFLAG_TRANSLUCENT) {

                strcat (sf, "   gl_FragData [2] = vec4 (composite.r, composite.b, 0.0, 0.0);\n\n");

            } else {

                strcat (sf, "   gl_FragData [2] = vec4 (composite.r, composite.b, 0.15, 1.0);\n\n");
            }
            
        } else {
            
            // TODO: add here some values for particular material
            
            strcat (sf, "   gl_FragData [2] = vec4 (0.1, 0.1, 0.0, 0.0);\n\n");
        }
    }

    strcat (sf, "}\n\n");

    debug_Print ("-------------------------------------------------------------------------------------------------------------------\n");
    debug_Print ("-- FRAGMENT SHADER\n");
    debug_Print ("-------------------------------------------------------------------------------------------------------------------\n");
    debug_Print (sf);

    // -------------------------------------------------------------------------------------------------------------------------------
    // WRITE SHADERS
    // -------------------------------------------------------------------------------------------------------------------------------

    * sh_vertex     = (CHARP) malloc (strlen (sv) + 1);
    * sh_fragment   = (CHARP) malloc (strlen (sf) + 1);

    strcpy (* sh_vertex,    sv);
    strcpy (* sh_fragment,  sf);
}

