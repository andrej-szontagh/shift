
uniform float scalex;
uniform float scaley;

void main ()
{
    gl_Position      = gl_Vertex;

    gl_TexCoord [0]  = gl_MultiTexCoord0.stst * vec4 (1.0, 1.0, scalex, scaley);

    gl_TexCoord [1]  = (gl_MultiTexCoord0.stst + vec4 (0.0,   1.5, 0.0,   1.5)) * vec4 (1.0, 1.0, scalex, scaley);
    gl_TexCoord [2]  = (gl_MultiTexCoord0.stst + vec4 (0.0, - 1.5, 0.0, - 1.5)) * vec4 (1.0, 1.0, scalex, scaley);
    gl_TexCoord [3]  = (gl_MultiTexCoord0.stst + vec4 (0.0,   3.5, 0.0,   3.5)) * vec4 (1.0, 1.0, scalex, scaley);
    gl_TexCoord [4]  = (gl_MultiTexCoord0.stst + vec4 (0.0, - 3.5, 0.0, - 3.5)) * vec4 (1.0, 1.0, scalex, scaley);
}
