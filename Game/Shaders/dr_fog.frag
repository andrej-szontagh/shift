
uniform sampler2DRect tex_depth;

uniform float farplane;
uniform float nearplane;

varying vec3 view;

void main ()
{    

    // READ DEPTH
    float depth      = texture2DRect (tex_depth, gl_TexCoord [0]).x;

    // SKIP ENVIROMENT
    if (depth == 1.0) discard;

    // DECODE DEPTH
    depth           = (2.0 * nearplane) / (farplane + nearplane - (2.0 * depth - 1.0) * (farplane - nearplane));
    
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
