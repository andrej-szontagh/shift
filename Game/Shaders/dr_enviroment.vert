
uniform vec2 uvscale;

void main ()
{
    // TRANSFORM

    gl_Position     = gl_ProjectionMatrix * gl_Vertex;

    // VARYINGS

    gl_TexCoord [0] = gl_MultiTexCoord0 * vec4 (uvscale, 0.0, 0.0);
}
