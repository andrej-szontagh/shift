
uniform float invsize;

uniform float width;
uniform float height;

varying float desaturation;
varying float brightness;
varying float contrast;

varying vec2 screen;

varying vec3 view;
varying vec3 ray;

uniform float farw;
uniform float farh;
uniform float farplane;

uniform float nearw;
uniform float nearh;
uniform float nearplane;

void main ()
{
    gl_Position     = gl_Vertex;
    
    screen = ((gl_Vertex.xy * 0.5) + 0.5) * vec2 (width, height);
    
    gl_TexCoord [0] = vec4 (- 0.5 * invsize, - 0.5 * invsize, 0.0, 0.0);
    gl_TexCoord [1] = vec4 (- 0.5 * invsize,   0.5 * invsize, 0.0, 0.0);
    gl_TexCoord [2] = vec4 (  0.5 * invsize, - 0.5 * invsize, 0.0, 0.0);
    gl_TexCoord [3] = vec4 (  0.5 * invsize,   0.5 * invsize, 0.0, 0.0);

    gl_TexCoord [4] = vec4 (- 1.5 * invsize,   0.0, 0.0, 0.0);
    gl_TexCoord [5] = vec4 (  0.0,           - 1.5 * invsize, 0.0, 0.0);
    gl_TexCoord [6] = vec4 (  1.5 * invsize,   0.0, 0.0, 0.0);
    gl_TexCoord [7] = vec4 (  0.0,             1.5 * invsize, 0.0, 0.0);

    // VECTOR FOR VIEWSPACE POSITION RECONSTRUCTION

    view = vec3 (gl_Vertex.xy * vec2 (farw, farh), - farplane);

    // LIGHT VECTOR INTO VIEW - SPACE

    ray = gl_LightSource [0].position;

	desaturation	= gl_MultiTexCoord1.x;
	brightness		= gl_MultiTexCoord1.y;
	contrast		= gl_MultiTexCoord1.z;
}
