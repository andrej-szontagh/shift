
uniform sampler2DRect tex_enviroment;

varying float desaturation;
varying float brightness;
varying float contrast;

void main ()
{
    // COLOR

    gl_FragColor	= texture2DRect (tex_enviroment, gl_TexCoord [0].st);

	// COLOR CORRECTIONS

	float intensity	= dot (gl_FragColor.rgb, vec3 (0.333, 0.333, 0.333));

	gl_FragColor    = pow (brightness * mix (gl_FragColor, vec4 (intensity, intensity, intensity, 1.0), desaturation), contrast);

	// ALPHA

    gl_FragColor.a	= 255.0;
}
