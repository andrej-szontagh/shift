
uniform float scalex;
uniform float scaley;

void main ()
{
    gl_Position      = gl_Vertex;

    gl_TexCoord [0]  = vec4 (gl_MultiTexCoord0.st, gl_MultiTexCoord0.st * vec2 (scalex, scaley));

    gl_TexCoord [1]  = gl_TexCoord [0] + vec4 (   1.5, 0.0,   1.5, 0.0);
    gl_TexCoord [2]  = gl_TexCoord [0] + vec4 ( - 1.5, 0.0, - 1.5, 0.0);
    gl_TexCoord [3]  = gl_TexCoord [0] + vec4 (   3.5, 0.0,   3.5, 0.0);
    gl_TexCoord [4]  = gl_TexCoord [0] + vec4 ( - 3.5, 0.0, - 3.5, 0.0);
}
