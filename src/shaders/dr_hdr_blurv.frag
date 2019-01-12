uniform sampler2DRect tex;

void main ()
{
/*
    gl_FragColor  = texture2DRect (tex, gl_TexCoord [0].st);

    gl_FragColor += texture2DRect (tex, gl_TexCoord [0].st + vec2 (0.0,   1.5));
    gl_FragColor += texture2DRect (tex, gl_TexCoord [0].st + vec2 (0.0, - 1.5));        gl_FragColor *= 0.3333333333;
*/

    // GAUSSIAN BLUR

    gl_FragColor  = texture2DRect (tex, gl_TexCoord [0].st) * 4;
    gl_FragColor += texture2DRect (tex, gl_TexCoord [0].st + vec2 (0.0,   1.0)) * 4;
    gl_FragColor += texture2DRect (tex, gl_TexCoord [0].st + vec2 (0.0, - 1.0)) * 4;
    gl_FragColor += texture2DRect (tex, gl_TexCoord [0].st + vec2 (0.0,   2.0)) * 1;
    gl_FragColor += texture2DRect (tex, gl_TexCoord [0].st + vec2 (0.0, - 2.0)) * 1;

    gl_FragColor /= 14;
}

