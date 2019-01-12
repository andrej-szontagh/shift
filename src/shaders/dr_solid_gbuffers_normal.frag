
uniform sampler2D tex_diffuse;
uniform sampler2D tex_composite;

varying float depth;

varying vec3 color;

varying mat3 tbni;

//      R      G      B             A
// G1 : R16  | G16  | B16  | GLOSS8 SHININESS8 |
// G2 : NX16 | NY16 |         LDEPTH32         |

void main ()
{
          
    // G1 ---------------------------------------------------------------------------------------------------------------------------------------------------
    
            vec4 diffuse    = texture2D (tex_diffuse,    gl_TexCoord [0].st);
            vec4 composite  = texture2D (tex_composite,  gl_TexCoord [0].st);

            gl_FragData [0] = vec4 (diffuse.rgb * color.rgb, 1.0);
 
    // G2 ---------------------------------------------------------------------------------------------------------------------------------------------------

            vec3 normal     = 2.0 * composite.gaa - 1.0;   normal.z = sqrt (1 - dot (normal.xy, normal.xy));   normal *= tbni;

            // SPHEREMAP TRANSFORM
            //
            // http://aras-p.info/texts/CompactNormalStorage.html#method04spheremap
            //

            vec2 normalm    = (normal.xy / sqrt (normal.z * 8.0 + 8.0) + 0.5) * 255.0;

            gl_FragData [1] = vec4 (floor (normalm.xy) * 0.00392156, fract (normalm.xy));

    // G3 ---------------------------------------------------------------------------------------------------------------------------------------------------            

            gl_FragData [2] = vec4 (composite.r, composite.b, 0.0, 0.0);

}