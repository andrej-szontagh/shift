
uniform sampler2D tex_diffuse;
uniform sampler2D tex_composite;
uniform sampler2D tex_weights1;
uniform sampler2D tex_weights2;
uniform sampler2D tex_weights3;
uniform sampler2D tex_weights4;

varying mat3 tbni;

//      R      G      B             A
// G1 : R16  | G16  | B16  | GLOSS8 SHININESS8 |
// G2 : NX16 | NY16 |         LDEPTH32         |

void main ()
{
    
    // EVALUATE UV AND LOD ----------------------------------------------------------------------------------------------------------------------------------

            vec2 uv = gl_TexCoord [0].st * 0.25;

            vec2 dx = dFdx (uv * 4096);
            vec2 dy = dFdy (uv * 4096);
        
            float lod = clamp (0.5 * log2 (max (dot (dx, dx), dot (dy, dy))), 0.0, 8.0);       // LOAD MAX LOD (8) !!!
            
            float pix = pow (2, lod) / 4096;

            uv = (gl_TexCoord [0].st - floor (gl_TexCoord [0].st)) * (0.25 - pix) + 0.5 * vec2 (pix, pix);

            vec2 uv_scaled = gl_TexCoord [0].st * 0.03;

            vec2 uv_mod = uv_scaled * 0.25;

            dx = dFdx (uv_mod * 4096);
            dy = dFdy (uv_mod * 4096);
        
            float lod_mod = clamp (0.5 * log2 (max (dot (dx, dx), dot (dy, dy))), 0.0, 8.0);   // LOAD MAX LOD (8) !!!
            
            pix = pow (2, lod_mod) / 4096;

            uv_mod = (uv_scaled - floor (uv_scaled)) * (0.25 - pix) + 0.5 * vec2 (pix, pix);

    // WEIGHTS ----------------------------------------------------------------------------------------------------------------------------------------------

            // CONSIDER CHANGE WEIGHTS TO 4 BIT AND REDUCE 4 LOOKUPS TO 2 LOOKUPS

            vec4 weights1 = texture2D (tex_weights1, gl_TexCoord [1].st);
            vec4 weights2 = texture2D (tex_weights2, gl_TexCoord [1].st);
            vec4 weights3 = texture2D (tex_weights3, gl_TexCoord [1].st);   
            vec4 weights4 = texture2D (tex_weights4, gl_TexCoord [1].st);

            vec4 factor1 = vec4 (1.0, 1.0, 1.0, 1.0);
            vec4 factor2 = vec4 (1.0, 1.0, 1.0, 1.0);
            vec4 factor3 = vec4 (1.0, 1.0, 1.0, 1.0);
            vec4 factor4 = vec4 (1.0, 1.0, 1.0, 1.0);

            vec4 sum = vec4 (0.0, 0.0, 0.0, 0.0);

            if (dot (weights1, weights1) > 0.0) {
                if (weights1.r > 0.0)   factor1.r = texture2DLod (tex_diffuse, uv_mod + vec2 (0.0,  0.0),  lod_mod).a;
                if (weights1.g > 0.0)   factor1.g = texture2DLod (tex_diffuse, uv_mod + vec2 (0.25, 0.0),  lod_mod).a;
                if (weights1.b > 0.0)   factor1.b = texture2DLod (tex_diffuse, uv_mod + vec2 (0.5,  0.0),  lod_mod).a;
                if (weights1.a > 0.0)   factor1.a = texture2DLod (tex_diffuse, uv_mod + vec2 (0.75, 0.0),  lod_mod).a;
                factor1 = max (1.3 * pow (factor1, 3.0), 0.2);   weights1 *= pow (factor1, 1.0);    sum.r = dot (weights1, vec4 (1.0, 1.0, 1.0, 1.0));
            }
            if (dot (weights2, weights2) > 0.0) {
                if (weights2.r > 0.0)   factor2.r = texture2DLod (tex_diffuse, uv_mod + vec2 (0.0,  0.25), lod_mod).a;
                if (weights2.g > 0.0)   factor2.g = texture2DLod (tex_diffuse, uv_mod + vec2 (0.25, 0.25), lod_mod).a;
                if (weights2.b > 0.0)   factor2.b = texture2DLod (tex_diffuse, uv_mod + vec2 (0.5,  0.25), lod_mod).a;
                if (weights2.a > 0.0)   factor2.a = texture2DLod (tex_diffuse, uv_mod + vec2 (0.75, 0.25), lod_mod).a;
                factor2 = max (1.3 * pow (factor2, 3.0), 0.2);   weights2 *= pow (factor2, 1.0);    sum.g = dot (weights2, vec4 (1.0, 1.0, 1.0, 1.0));
            }
            if (dot (weights3, weights3) > 0.0) {
                if (weights3.r > 0.0)   factor3.r = texture2DLod (tex_diffuse, uv_mod + vec2 (0.0,  0.5),  lod_mod).a;
                if (weights3.g > 0.0)   factor3.g = texture2DLod (tex_diffuse, uv_mod + vec2 (0.25, 0.5),  lod_mod).a;
                if (weights3.b > 0.0)   factor3.b = texture2DLod (tex_diffuse, uv_mod + vec2 (0.5,  0.5),  lod_mod).a;
                if (weights3.a > 0.0)   factor3.a = texture2DLod (tex_diffuse, uv_mod + vec2 (0.75, 0.5),  lod_mod).a;
                factor3 = max (1.3 * pow (factor3, 3.0), 0.2);   weights3 *= pow (factor3, 1.0);    sum.b = dot (weights3, vec4 (1.0, 1.0, 1.0, 1.0));
            }
            if (dot (weights4, weights4) > 0.0) {
                if (weights4.r > 0.0)   factor4.r = texture2DLod (tex_diffuse, uv_mod + vec2 (0.0,  0.75), lod_mod).a;
                if (weights4.g > 0.0)   factor4.g = texture2DLod (tex_diffuse, uv_mod + vec2 (0.25, 0.75), lod_mod).a;
                if (weights4.b > 0.0)   factor4.b = texture2DLod (tex_diffuse, uv_mod + vec2 (0.5,  0.75), lod_mod).a;
                if (weights4.a > 0.0)   factor4.a = texture2DLod (tex_diffuse, uv_mod + vec2 (0.75, 0.75), lod_mod).a;
                factor4 = max (1.3 * pow (factor4, 3.0), 0.2);   weights4 *= pow (factor4, 1.0);    sum.a = dot (weights4, vec4 (1.0, 1.0, 1.0, 1.0));
            }

    // NORMALIZING WEIGHTS ----------------------------------------------------------------------------------------------------------------------------------
    
            float scale = 1.0 / dot (sum, vec4 (1.0, 1.0, 1.0, 1.0));

    // TEXTURES ---------------------------------------------------------------------------------------------------------------------------------------------

            vec4 diffuse   = vec4 (0.0, 0.0, 0.0, 0.0);
            vec4 composite = vec4 (0.0, 0.0, 0.0, 0.0);

            if (sum.r > 0.0) {   weights1 *= scale;
                if (weights1.r > 0.0) {
                    diffuse   += texture2DLod (tex_diffuse,     uv + vec2 (0.0,  0.0),  lod) * weights1.r * factor1.r;
                    composite += texture2DLod (tex_composite,   uv + vec2 (0.0,  0.0),  lod) * weights1.r;
                }
                if (weights1.g > 0.0) {
                    diffuse   += texture2DLod (tex_diffuse,     uv + vec2 (0.25, 0.0),  lod) * weights1.g * factor1.g;
                    composite += texture2DLod (tex_composite,   uv + vec2 (0.25, 0.0),  lod) * weights1.g;
                }
                if (weights1.b > 0.0) {
                    diffuse   += texture2DLod (tex_diffuse,     uv + vec2 (0.5,  0.0),  lod) * weights1.b * factor1.b;
                    composite += texture2DLod (tex_composite,   uv + vec2 (0.5,  0.0),  lod) * weights1.b;
                }
                if (weights1.a > 0.0) {
                    diffuse   += texture2DLod (tex_diffuse,     uv + vec2 (0.75, 0.0),  lod) * weights1.a * factor2.a;
                    composite += texture2DLod (tex_composite,   uv + vec2 (0.75, 0.0),  lod) * weights1.a;
                }
            }
            if (sum.g > 0.0) {   weights2 *= scale;
                if (weights2.r > 0.0) {
                    diffuse   += texture2DLod (tex_diffuse,     uv + vec2 (0.0,  0.25), lod) * weights2.r * factor2.r;
                    composite += texture2DLod (tex_composite,   uv + vec2 (0.0,  0.25), lod) * weights2.r;
                }
                if (weights2.g > 0.0) {
                    diffuse   += texture2DLod (tex_diffuse,     uv + vec2 (0.25, 0.25), lod) * weights2.g * factor2.g;
                    composite += texture2DLod (tex_composite,   uv + vec2 (0.25, 0.25), lod) * weights2.g;
                }
                if (weights2.b > 0.0) {
                    diffuse   += texture2DLod (tex_diffuse,     uv + vec2 (0.5,  0.25), lod) * weights2.b * factor2.b;
                    composite += texture2DLod (tex_composite,   uv + vec2 (0.5,  0.25), lod) * weights2.b;
                }
                if (weights2.a > 0.0) {
                    diffuse   += texture2DLod (tex_diffuse,     uv + vec2 (0.75, 0.25), lod) * weights2.a * factor2.a;
                    composite += texture2DLod (tex_composite,   uv + vec2 (0.75, 0.25), lod) * weights2.a;
                }
            }
            if (sum.b > 0.0) {   weights3 *= scale;
                if (weights3.r > 0.0) {
                    diffuse   += texture2DLod (tex_diffuse,     uv + vec2 (0.0,  0.5),  lod) * weights3.r * factor3.r;
                    composite += texture2DLod (tex_composite,   uv + vec2 (0.0,  0.5),  lod) * weights3.r;
                }
                if (weights3.g > 0.0) {
                    diffuse   += texture2DLod (tex_diffuse,     uv + vec2 (0.25, 0.5),  lod) * weights3.g * factor3.g;
                    composite += texture2DLod (tex_composite,   uv + vec2 (0.25, 0.5),  lod) * weights3.g;
                }
                if (weights3.b > 0.0) {
                    diffuse   += texture2DLod (tex_diffuse,     uv + vec2 (0.5,  0.5),  lod) * weights3.b * factor3.b;
                    composite += texture2DLod (tex_composite,   uv + vec2 (0.5,  0.5),  lod) * weights3.b;
                }
                if (weights3.a > 0.0) {
                    diffuse   += texture2DLod (tex_diffuse,     uv + vec2 (0.75, 0.5),  lod) * weights3.a * factor3.a;
                    composite += texture2DLod (tex_composite,   uv + vec2 (0.75, 0.5),  lod) * weights3.a;
                }
            }
            if (sum.a > 0.0) {   weights4 *= scale;
                if (weights4.r > 0.0) {
                    diffuse   += texture2DLod (tex_diffuse,     uv + vec2 (0.0,  0.75), lod) * weights4.r * factor4.r;
                    composite += texture2DLod (tex_composite,   uv + vec2 (0.0,  0.75), lod) * weights4.r;
                }
                if (weights4.g > 0.0) {
                    diffuse   += texture2DLod (tex_diffuse,     uv + vec2 (0.25, 0.75), lod) * weights4.g * factor4.g;
                    composite += texture2DLod (tex_composite,   uv + vec2 (0.25, 0.75), lod) * weights4.g;
                }
                if (weights4.b > 0.0) {
                    diffuse   += texture2DLod (tex_diffuse,     uv + vec2 (0.5,  0.75), lod) * weights4.b * factor4.b;
                    composite += texture2DLod (tex_composite,   uv + vec2 (0.5,  0.75), lod) * weights4.b;
                }
                if (weights4.a > 0.0) {
                    diffuse   += texture2DLod (tex_diffuse,     uv + vec2 (0.75, 0.75), lod) * weights4.a * factor4.a;
                    composite += texture2DLod (tex_composite,   uv + vec2 (0.75, 0.75), lod) * weights4.a;
                }
            }

    // G1 ---------------------------------------------------------------------------------------------------------------------------------------------------

            gl_FragData [0] = vec4 (diffuse.rgb, 1.0);

    // G2 ---------------------------------------------------------------------------------------------------------------------------------------------------            

            vec3 normal  = 2.0 * composite.gaa - 1.0;   normal.z = sqrt (1 - dot (normal.xy, normal.xy));   normal *= tbni;

            // SPHEREMAP TRANSFORM
            //
            // http://aras-p.info/texts/CompactNormalStorage.html#method04spheremap
            //
    
            vec2 normalm    = (normal.xy / sqrt (normal.z * 8.0 + 8.0) + 0.5) * 255.0;

            gl_FragData [1] = vec4 (floor (normalm.xy) * 0.00392156, fract (normalm.xy));

    // G3 ---------------------------------------------------------------------------------------------------------------------------------------------------            

            gl_FragData [2] = vec4 (composite.r, composite.b, 0.0, 0.0);

}