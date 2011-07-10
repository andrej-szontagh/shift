
uniform sampler2DRect tex;
uniform sampler2DRect tex_G2;

void main ()
{
    vec4 sample     = texture2DRect (tex,    gl_TexCoord [0].st);
    vec4 G2         = texture2DRect (tex_G2, gl_TexCoord [0].st);

    float depth     = G2.z;
    float depths;
    
    float ao, sum   = 1.0;

    ao = sample.r;

    sample = texture2DRect (tex_G2, gl_TexCoord [1]);   depths = 1.0 / (1.0 + 65535.0 * abs (depth - sample.z));
    ao += texture2DRect (tex, gl_TexCoord [1]).r * depths;    sum += depths;

    sample = texture2DRect (tex_G2, gl_TexCoord [2]);   depths = 1.0 / (1.0 + 65535.0 * abs (depth - sample.z));
    ao += texture2DRect (tex, gl_TexCoord [2]).r * depths;    sum += depths;

    sample = texture2DRect (tex_G2, gl_TexCoord [3]);   depths = 1.0 / (1.0 + 65535.0 * abs (depth - sample.z));
    ao += texture2DRect (tex, gl_TexCoord [3]).r * depths;    sum += depths;

    sample = texture2DRect (tex_G2, gl_TexCoord [4]);   depths = 1.0 / (1.0 + 65535.0 * abs (depth - sample.z));
    ao += texture2DRect (tex, gl_TexCoord [4]).r * depths;    sum += depths;

    ao /= sum;

    gl_FragColor = vec4 (ao, ao, ao, 1.0);


    //gl_FragColor = vec4 (texture2DRect (tex,    gl_TexCoord [0].st).rrr, 1.0);  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
}

