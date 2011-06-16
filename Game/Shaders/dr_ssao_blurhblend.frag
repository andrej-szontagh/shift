
uniform sampler2DRect tex_vblur;
uniform sampler2DRect tex_raw;
uniform sampler2DRect tex_G2;

void main ()
{
    vec4 sample     = texture2DRect (tex_vblur, gl_TexCoord [0].pq);
    vec4 G2         = texture2DRect (tex_G2,    gl_TexCoord [0].st);

    float depth     = (G2.z + G2.w);
    float depths;
    
    float ao, sum   = 1.0;
    
    ao = sample.r;

    sample = texture2DRect (tex_G2, gl_TexCoord [1].st);    depths = 1.0 / (1.0 + 1.0 * abs (depth - (sample.z + sample.w)));
    ao += texture2DRect (tex_vblur, gl_TexCoord [1].pq).r * depths;    sum += depths;

    sample = texture2DRect (tex_G2, gl_TexCoord [2].st);    depths = 1.0 / (1.0 + 1.0 * abs (depth - (sample.z + sample.w)));
    ao += texture2DRect (tex_vblur, gl_TexCoord [2].pq).r * depths;    sum += depths;

    sample = texture2DRect (tex_G2, gl_TexCoord [3].st);    depths = 1.0 / (1.0 + 1.0 * abs (depth - (sample.z + sample.w)));
    ao += texture2DRect (tex_vblur, gl_TexCoord [3].pq).r * depths;    sum += depths;

    sample = texture2DRect (tex_G2, gl_TexCoord [4].st);    depths = 1.0 / (1.0 + 1.0 * abs (depth - (sample.z + sample.w)));
    ao += texture2DRect (tex_vblur, gl_TexCoord [4].pq).r * depths;    sum += depths;

    ao /= sum;

    gl_FragColor = texture2DRect (tex_raw, gl_TexCoord [0].st) * vec4 (ao, ao, ao, 1.0);


    //gl_FragColor = vec4 (texture2DRect (tex_vblur, gl_TexCoord [0].pq).rrr, 1.0);  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    //gl_FragColor = vec4 (ao, ao, ao, 1.0); // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
}
