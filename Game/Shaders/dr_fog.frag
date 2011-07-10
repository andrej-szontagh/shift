
uniform sampler2DRect tex_G2;

varying vec3 view;

void main ()
{
    // G2 READ
    vec4  G2        = texture2DRect (tex_G2, gl_TexCoord [0].st);
    
    // DECODE DEPTH
    float depth     = G2.z;
    
    /// TODO : OPTIMIZE, WE NEED ONLY Y WORLD COORD

    // VIEW POSITION
    vec3 viewpos    = depth * view;

    // WORLD POSITION
    vec4 worldpos   = gl_ModelViewMatrixInverse * vec4 (viewpos, 1.0);

    // FACTOR
    float factor    = max (0.0, min (1.0, - 0.008 * worldpos.y));
    
          factor    = log10 (1.0 + factor) / log10 (2.0);
          factor   *= log10 (1.0 + depth)  / log10 (2.0);

    gl_FragColor    = vec4 (gl_Color.rgb, factor);
}
