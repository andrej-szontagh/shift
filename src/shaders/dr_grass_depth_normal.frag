
uniform sampler2D tex_diffuse;

varying float threshold;
varying float damping;
varying float factor;

void main ()
{
    // ALPHA TEST -------------------------------------------------------------------------------------------------------------------------------------------

		if (texture2D (tex_diffuse, gl_TexCoord [0].st).a < (threshold + damping * factor)) discard;
}
