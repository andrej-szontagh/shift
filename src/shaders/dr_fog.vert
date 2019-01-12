
uniform float farw;
uniform float farh;
uniform float farplane;

varying vec3 view;

void main ()
{
    gl_Position     = gl_Vertex;
    gl_TexCoord [0] = gl_MultiTexCoord0;

    // vector for viewspace position reconstruction

    view = vec3 (gl_Vertex.xy * vec2 (farw, farh), - farplane);

    gl_FrontColor   = gl_Color;
}
