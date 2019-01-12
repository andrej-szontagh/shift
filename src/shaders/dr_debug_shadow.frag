
uniform sampler2D tex;

void main ()
{
    gl_FragColor = 5000.0 * pow (texture2D (tex, gl_TexCoord [0].st).xxxx, 20.0);
}
