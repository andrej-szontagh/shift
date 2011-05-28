
uniform sampler2D tex_diffuse;
uniform sampler2D tex_normal;

varying float shininess;
varying float gloss;
varying float depth;

varying vec3 normal;
varying mat3 tbni;

void main ()
{

    // READ TEXTURE -----------------------------------------------------------------------------------------------------------------------------------------

            vec4 diffuse = texture2D (tex_diffuse, gl_TexCoord [0].st);

    // G1 ---------------------------------------------------------------------------------------------------------------------------------------------------
    
            // we do floor/fract per pixel cause of interpolation errors of values from vertex shader
            
            vec4 sample = texture2D (tex_diffuse, gl_TexCoord [0].st);
    
            gl_FragData [0] = vec4 (sample.rgb, floor (max (1.0, shininess)) + fract (gloss * sample.a));

    // G2 ---------------------------------------------------------------------------------------------------------------------------------------------------

            vec3 normalm = normalize ((2.0 * texture2D (tex_normal, gl_TexCoord [0].st).xyz - 1.0) * tbni);

    // SPHEREMAP TRANSFORM
    //
    // http://aras-p.info/texts/CompactNormalStorage.html#method04spheremap
    //
    
            normalm.xy = normalm.xy / sqrt (normalm.z * 8.0 + 8.0) + 0.5;
    
            gl_FragData [1] = vec4 (normalm.x, normalm.y, floor (depth), fract (depth));
            
}
