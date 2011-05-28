
void main ()
{
    gl_Position      = gl_Vertex;

    gl_TexCoord [0]  = gl_MultiTexCoord0;
    gl_TexCoord [1]  = gl_MultiTexCoord0 + vec4 (0.0,   1.5, 0.0, 0.0);
    gl_TexCoord [2]  = gl_MultiTexCoord0 + vec4 (0.0, - 1.5, 0.0, 0.0);
    gl_TexCoord [3]  = gl_MultiTexCoord0 + vec4 (0.0,   3.5, 0.0, 0.0);
    gl_TexCoord [4]  = gl_MultiTexCoord0 + vec4 (0.0, - 3.5, 0.0, 0.0);
}
