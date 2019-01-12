
uniform sampler2DRect tex;

varying float scale;

void main ()
{
    vec4 color = texture2DRect (tex, gl_TexCoord [0].xy) * scale;

    float intensity = ( color.r + 
                        color.g + 
                        color.b) * 0.333333333;

//  gl_FragColor = vec4 (color.rgb * (intensity - 1.0), intensity);

    gl_FragColor = vec4 (color.rgb * (max (max (color.r, color.g), color.b) - 1.0), intensity);
}
