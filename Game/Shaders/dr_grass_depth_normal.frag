
uniform sampler2D tex_diffuse;

varying float factor;

void main ()
{
    // ALPHA TEST -------------------------------------------------------------------------------------------------------------------------------------------

        if (texture2D (tex_diffuse, gl_TexCoord [0].st).a < (0.1 + 0.9 * factor)) discard;
}