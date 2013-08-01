
uniform sampler2D tex_diffuse;
uniform sampler2D tex_composite;

varying vec3 color;

varying mat3 tbni;

//      R      G      B             A
// G1 : R16  | G16  | B16  | GLOSS8 SHININESS8 |
// G2 : NX16 | NY16 |         LDEPTH32         |

void main ()
{
    // READ TEXTURES ----------------------------------------------------------------------------------------------------------------------------------------

            vec4 diffuse   = texture2D (tex_diffuse,    gl_TexCoord [0].st);
            vec4 composite = texture2D (tex_composite,  gl_TexCoord [0].st);

    // NORMAL -----------------------------------------------------------------------------------------------------------------------------------------------

            // NEED NORMALIZED NORMALS IN COMPOSITE !!!!!!!!!!!!!! sqrt (negative) causes visual errors

            vec3 normal     = 2.0 * composite.gaa - 1.0;   normal.z = sqrt (max (1.0 - dot (normal.xy, normal.xy), 0.0));   
            
            normal *= tbni;

            if (dot (vec3 (0.0, 0.0, -1.0), normal) > 0.0)

                normal  = -normal;      // normal facing camera

    // G1 ---------------------------------------------------------------------------------------------------------------------------------------------------

            float angle = dot (gl_LightSource [0].position.xyz, normal);

            // looking at backside
            if (angle < 0.0) {

                angle = abs (angle);

                // SET POWER BY SSS MASK !!!!!!!!!!!!!!!!!!!!!!!
                
                gl_FragData [0] = vec4 (diffuse.rgb * color.rgb, 0.3 * (1.0 - angle) + 0.4 * angle);   // side SSS + direct SSS

            // looking at frontside
            } else {

                gl_FragData [0] = vec4 (diffuse.rgb * color.rgb, 1.0);
            }

    // G2 ---------------------------------------------------------------------------------------------------------------------------------------------------

            // SPHEREMAP TRANSFORM
            //
            // http://aras-p.info/texts/CompactNormalStorage.html#method04spheremap
            //

            vec2 normalm    = (normal.xy / sqrt (normal.z * 8.0 + 8.0) + 0.5) * 255.0;

            gl_FragData [1] = vec4 (floor (normalm.xy) * 0.00392156, fract (normalm.xy));

    // G3 ---------------------------------------------------------------------------------------------------------------------------------------------------            

            // SET EMISSIVE BY SSS MASK !!!!!!!!!!!!!!!!!!!!!!!

            gl_FragData [2] = vec4 (composite.r, composite.b, 0.15, 1.0);
}
