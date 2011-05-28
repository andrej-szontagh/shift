
uniform sampler2DRect tex;
uniform sampler2DRect tex_depth;

uniform float nom;
uniform float denom1;
uniform float denom2;

void main ()
{

    float depth  = nom / (denom1 - texture2DRect (tex_depth, gl_TexCoord [0].st).x * denom2);
    float depth1 = nom / (denom1 - texture2DRect (tex_depth, gl_TexCoord [0].st + vec2 (   1.0, 0.0)).x * denom2);
    float depth2 = nom / (denom1 - texture2DRect (tex_depth, gl_TexCoord [0].st + vec2 ( - 1.0, 0.0)).x * denom2);

    float f1 = max (0.0, depth1 - depth);
    float f2 = max (0.0, depth2 - depth);

    float sum = max (0.0001, f1 + f2);

    float factor = min (1.0, 1000.0 * sum);
        
    f1 = f1 / sum;
    f2 = f2 / sum;

    vec4 color = texture2DRect (tex, gl_TexCoord [0].st);
    
    gl_FragColor    = color * 1.2;
    gl_FragColor   += texture2DRect (tex, gl_TexCoord [0].st + vec2 (   1.0, 0.0)) * f1;
    gl_FragColor   += texture2DRect (tex, gl_TexCoord [0].st + vec2 ( - 1.0, 0.0)) * f2;
    
    gl_FragColor   /= (1.2 + f1 + f2);
    
    gl_FragColor    = factor * gl_FragColor + (1.0 - factor) * color;
    
    gl_FragColor.a  = color.a;


/*
    float depth  = nom / (denom1 - texture2DRect (tex_depth, gl_TexCoord [0].st).x * denom2);
    float depth1 = nom / (denom1 - texture2DRect (tex_depth, gl_TexCoord [0].st + vec2 (   1.0, 0.0)).x * denom2);
    float depth2 = nom / (denom1 - texture2DRect (tex_depth, gl_TexCoord [0].st + vec2 ( - 1.0, 0.0)).x * denom2);
    
//  float factor = min (1.0, 1.0 * log (1.0 + (abs (depth - depth1) + abs (depth - depth2)) * 100.0));
    float factor = min (1.0, 1.0 * log (1.0 + (max (0.0, depth1 - depth) + max (0.0, depth2 - depth)) * 1000.0));
    
    // BLUR

    vec4 color = texture2DRect (tex, gl_TexCoord [0].st);

    gl_FragColor  = color * (4.0 / 14.0);
    gl_FragColor += texture2DRect (tex, gl_TexCoord [0].st + vec2 (   1.0, 0.0)) * (4.0 / 14.0);
    gl_FragColor += texture2DRect (tex, gl_TexCoord [0].st + vec2 ( - 1.0, 0.0)) * (4.0 / 14.0);
    gl_FragColor += texture2DRect (tex, gl_TexCoord [0].st + vec2 (   2.0, 0.0)) * (1.0 / 14.0);
    gl_FragColor += texture2DRect (tex, gl_TexCoord [0].st + vec2 ( - 2.0, 0.0)) * (1.0 / 14.0);
    
    gl_FragColor = factor * gl_FragColor + (1.0 - factor) * color;
*/

//  gl_FragColor = vec4 (factor, factor, factor, 1.0);
}

