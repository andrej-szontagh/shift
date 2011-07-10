
uniform sampler2D tex_diffuse;

varying float depth;

varying vec3 normal;

//      R      G      B             A
// G1 : R16  | G16  | B16  | GLOSS8 SHININESS8 |
// G2 : NX16 | NY16 |         LDEPTH32         |

void main ()
{

    // READ TEXTURE -----------------------------------------------------------------------------------------------------------------------------------------

            vec4 diffuse = texture2D (tex_diffuse, gl_TexCoord [0].st);

    // G1 ---------------------------------------------------------------------------------------------------------------------------------------------------
                
            gl_FragData [0] = vec4 (diffuse.rgb, 0.01);
    
    // G2 ---------------------------------------------------------------------------------------------------------------------------------------------------

    // SPHEREMAP TRANSFORM
    //
    // http://aras-p.info/texts/CompactNormalStorage.html#method04spheremap
    //
    
            vec2 normalm = normal.xy / sqrt (normal.z * 8.0 + 8.0) + 0.5;

            gl_FragData [1] = vec4 (normalm.x, normalm.y, depth, 0.01);

}
