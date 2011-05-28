
uniform sampler2DRect tex_G2;
uniform sampler2DRect tex;

varying vec3 view;

void main ()
{

    // COLOR READ
    gl_FragColor    = texture2DRect (tex, gl_TexCoord [0].st);

    // G2 READ
    vec4  G2        = texture2DRect (tex_G2, gl_TexCoord [0].st);
    
    // DECODE DEPTH
    float depth     = (G2.z + G2.w) * 0.00001525902;    // 1.0 / 65535.0
    
    /// TODO : OPTIMIZE, WE NEED ONLY Y WORLD COORD

    // VIEW POSITION
    vec3 viewpos    = depth * view;

    // WORLD POSITION
    vec4 worldpos   = gl_ModelViewMatrixInverse * vec4 (viewpos, 1.0);

    // FACTOR
    float factor    = max (0.0, min (1.0, - 0.08 * worldpos.y));
    
          factor    = log10 (1.0 + factor) / log10 (2.0);

    gl_FragColor    = factor * gl_Color + (1.0 - factor) * gl_FragColor;
}
