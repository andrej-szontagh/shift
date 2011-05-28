
uniform vec2 uvscale;

uniform float planefar;

varying float shininess;
varying float gloss;

varying float depth;

varying vec3 normal;

// SWING        -> gl_MultiTexCoord2.x
// GLOSS        -> gl_MultiTexCoord3.x
// SHININESS    -> gl_MultiTexCoord3.y
// TRANSFORM    -> gl_MultiTexCoord4.xyz
// TRANSFORM    -> gl_MultiTexCoord5.xyzw

//// MODELMATRIX    -> gl_MultiTexCoord4.xyzw
//// MODELMATRIX    -> gl_MultiTexCoord5.xyzw
//// MODELMATRIX    -> gl_MultiTexCoord6.xyzw
//// MODELMATRIX    -> gl_MultiTexCoord7.xyzw

void main ()
{

    // MATRIX -----------------------------------------------------------------------------------------------------------------------------------------------

            vec3 co = cos (gl_MultiTexCoord5.xyz);
            vec3 si = sin (gl_MultiTexCoord5.xyz);
            
            mat4 matrix     = mat4 (vec4 ( co.y*co.z,                  -co.y*si.z,                   si.y,      0.0) * gl_MultiTexCoord5.w, 
                                    vec4 ( co.x*si.z + si.x*si.y*co.z,  co.x*co.z - si.x*si.y*si.z, -si.x*co.y, 0.0) * gl_MultiTexCoord5.w, 
                                    vec4 ( si.x*si.z - co.x*si.y*co.z,  si.x*co.z + co.x*si.y*si.z,  co.x*co.y, 0.0) * gl_MultiTexCoord5.w, 
                                    vec4 (gl_MultiTexCoord4.xyz, 1.0));

    // ANIMATION --------------------------------------------------------------------------------------------------------------------------------------------
    
            vec4 v          = gl_Vertex;    v.y = v.y + v.z * gl_MultiTexCoord2.x;
    
    // COMPLETE VIEW TRANSFORM ------------------------------------------------------------------------------------------------------------------------------
                                
            matrix          = gl_ModelViewMatrix * matrix;
                
    // TRANSFORM --------------------------------------------------------------------------------------------------------------------------------------------
    
            gl_ClipVertex   = matrix * v;
            gl_Position     = gl_ProjectionMatrix * gl_ClipVertex;

    // LINEAR DEPTH -----------------------------------------------------------------------------------------------------------------------------------------
    
            depth           = (- gl_ClipVertex.z / planefar) * 65535.0;

    // UV ---------------------------------------------------------------------------------------------------------------------------------------------------

            gl_TexCoord [0] = vec4 (gl_MultiTexCoord0.st * uvscale, 0.0, 0.0);

    // GLOSS ------------------------------------------------------------------------------------------------------------------------------------------------
                
            gloss           = min (0.999999, gl_MultiTexCoord3.x * 0.5);    // we scale to maximum value 2.0
    
    // SHININESS --------------------------------------------------------------------------------------------------------------------------------------------
                            
            shininess       = gl_MultiTexCoord3.y * 0.1;                    // we leave more accurancy to gloss value, step is 10.0
    
    // NORMAL -----------------------------------------------------------------------------------------------------------------------------------------------
    
            // normal       = normalize (mat3 (gl_ModelViewMatrix) * vec3 (0.0, 0.0, 1.0));
            
            normal          = normalize (mat3 (matrix) * vec3 (0.0, 0.0, 1.0));
}
