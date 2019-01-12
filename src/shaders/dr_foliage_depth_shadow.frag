
uniform sampler2D tex_diffuse;

varying float factor;

void main ()
{
    // ALPHA TEST -------------------------------------------------------------------------------------------------------------------------------------------

            if (texture2D (tex_diffuse, gl_TexCoord [0].st).a < (0.5 + 0.5 * factor)) discard;

    // DEPTH OFFSET -----------------------------------------------------------------------------------------------------------------------------------------

            //gl_FragDepth = gl_FragCoord.z + 0.0001;  // * dot (texture2D (tex_diffuse, gl_TexCoord [0].st), vec3 (0.333, 0.333, 0.333));
}
