
uniform sampler2D tex_diffuse;

varying float factor;

void main ()
{
    // ALPHA TEST -------------------------------------------------------------------------------------------------------------------------------------------

//          if (texture2D (tex_diffuse, gl_TexCoord [0].st).a < (0.5 + 0.5 * factor)) discard;
            if (texture2D (tex_diffuse, gl_TexCoord [0].st).a < (0.5 - 0.3 * factor)) discard;  // helps preventing disappearing caused by alpha channel blurring on mipmaps
}
