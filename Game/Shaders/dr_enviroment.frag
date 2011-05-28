
uniform sampler2DRect tex_enviroment;

void main ()
{
    // COLOR

    gl_FragColor = texture2DRect (tex_enviroment, gl_TexCoord [0].st);

    gl_FragColor.a = 255.0;
}
