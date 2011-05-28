
uniform sampler2D tex_diffuse1;
uniform sampler2D tex_diffuse2;
uniform sampler2D tex_diffuse3;

uniform sampler2D tex_normal1;
uniform sampler2D tex_normal2;
uniform sampler2D tex_normal3;

uniform sampler2D tex_mask;

varying vec3 shininess;
varying vec3 gloss;

varying vec3 normal;
varying vec3 tangent;
varying vec3 binormal;

varying float depth;

//      R      G      B             A
// G1 : R16  | G16  | B16  | GLOSS8 SHININESS8 |
// G2 : NX16 | NY16 |         LDEPTH32         |

void main ()
{

    // MASK -------------------------------------------------------------------------------------------------------------------------------------------------
    
            vec3 mask = texture2D (tex_mask, gl_TexCoord [1].st).rgb;

    // TEXTURES ---------------------------------------------------------------------------------------------------------------------------------------------
            
            vec4 diffuse1   = texture2D (tex_diffuse1,  gl_TexCoord [0].st);
            vec4 diffuse2   = texture2D (tex_diffuse2,  gl_TexCoord [0].st);
            vec4 diffuse3   = texture2D (tex_diffuse3,  gl_TexCoord [0].st);

            vec4 normal1    = texture2D (tex_normal1,   gl_TexCoord [0].st);
            vec4 normal2    = texture2D (tex_normal2,   gl_TexCoord [0].st);
            vec4 normal3    = texture2D (tex_normal3,   gl_TexCoord [0].st);
            
    // MASK NORMALIZING -------------------------------------------------------------------------------------------------------------------------------------
    
            mask = mask / (mask.r + mask.g + mask.b);
            
    // G1 ---------------------------------------------------------------------------------------------------------------------------------------------------
    
            gl_FragData [0] = vec4 (diffuse1.rgb * mask.r +
                                    diffuse2.rgb * mask.g + 
                                    diffuse3.rgb * mask.b,      floor (max (1.0,      dot (shininess,                                       mask.rgb) * 0.1)) +
                                                                fract (min (0.999999, dot (gloss * vec3 (normal1.r, normal2.r, normal3.r),  mask.rgb) * 0.5)));


/*    
            gl_FragData [0] = vec4 (vec3 (0.5, 0.5, 0.5),       floor (max (1.0,      dot (shininess,                                       mask.rgb) * 0.1)) +
                                                                fract (min (0.999999, dot (gloss * vec3 (normal1.r, normal2.r, normal3.r),  mask.rgb) * 0.5)));
*/
    // G2 ---------------------------------------------------------------------------------------------------------------------------------------------------

            /// INVERTED TBN MATRIX (is orthonormal -> invert = transpose)
            
            mat3 tbni;
            
            vec3 n = normalize (normal.xyz);
            vec3 t = normalize (tangent.xyz);
            vec3 b = normalize (binormal.xyz);
    
            tbni [0] = vec3 (t.x, b.x, n.x);
            tbni [1] = vec3 (t.y, b.y, n.y);
            tbni [2] = vec3 (t.z, b.z, n.z);
            
            vec3 normalm    = normalize ((2.0 * (normal1.ayz * mask.r + 
                                                 normal2.ayz * mask.g + 
                                                 normal3.ayz * mask.b) - 1.0) * tbni);
                                                 
    // SPHEREMAP TRANSFORM
    //
    // http://aras-p.info/texts/CompactNormalStorage.html#method04spheremap
    //
    
            normalm.xy = normalm.xy / sqrt (normalm.z * 8.0 + 8.0) + 0.5;

            gl_FragData [1] = vec4 (normalm.x, normalm.y, floor (depth), fract (depth));
}