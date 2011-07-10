
uniform vec2 uvscale;

varying float desaturation;
varying float brightness;
varying float contrast;

void main ()
{
    // TRANSFORM

    gl_Position     = gl_ProjectionMatrix * gl_Vertex;

    // VARYINGS

    gl_TexCoord [0] = gl_MultiTexCoord0 * vec4 (uvscale, 0.0, 0.0);

	desaturation = gl_MultiTexCoord1.x;
	brightness	 = gl_MultiTexCoord1.y;
	contrast	 = gl_MultiTexCoord1.z;
}
