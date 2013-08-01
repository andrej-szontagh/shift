
uniform sampler2DRect tex_depth;
uniform sampler2DRect tex_raw;
uniform sampler2DRect tex;

uniform float depth1;
uniform float depth2;
uniform float depth3;

void main ()
{
    vec4 sample     = texture2DRect (tex, gl_TexCoord [0].st);

    float depth     = depth1 / (depth2 - depth3 * texture2DRect (tex_depth, gl_TexCoord [0].st).x);
    float depths;
    
    float ao, sum   = 1.0;
    
    float depthfactor = log2 (1.0 + 1000.0 * depth) / log2 (1001.0);

    ao = sample.r;

    depths = depth1 / (depth2 - depth3 * texture2DRect (tex_depth, gl_TexCoord [1].st).x);  depths = 1.0 / (1.0 + 1000.0 * abs (depth - depths) * depthfactor);
    ao += texture2DRect (tex, gl_TexCoord [1].st).r * depths;    sum += depths;

    depths = depth1 / (depth2 - depth3 * texture2DRect (tex_depth, gl_TexCoord [2].st).x);  depths = 1.0 / (1.0 + 1000.0 * abs (depth - depths) * depthfactor);
    ao += texture2DRect (tex, gl_TexCoord [2].st).r * depths;    sum += depths;

    depths = depth1 / (depth2 - depth3 * texture2DRect (tex_depth, gl_TexCoord [3].st).x);  depths = 1.0 / (1.0 + 1000.0 * abs (depth - depths) * depthfactor);
    ao += texture2DRect (tex, gl_TexCoord [3].st).r * depths;    sum += depths;

    depths = depth1 / (depth2 - depth3 * texture2DRect (tex_depth, gl_TexCoord [4].st).x);  depths = 1.0 / (1.0 + 1000.0 * abs (depth - depths) * depthfactor);
    ao += texture2DRect (tex, gl_TexCoord [4].st).r * depths;    sum += depths;

    ao /= sum;

    gl_FragColor = texture2DRect (tex_raw, gl_TexCoord [0].st) * vec4 (ao, ao, ao, 1.0);
}
