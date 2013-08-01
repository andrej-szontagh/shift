
uniform vec2 uvscale;

uniform float planefar;

varying float shininess;
varying float gloss;

varying vec3 normal;
varying vec3 color;

varying mat3 tbni;

// UV1          -> gl_MultiTexCoord0
// MODELMATRIX  -> gl_MultiTexCoord4.xyzw
// MODELMATRIX  -> gl_MultiTexCoord5.xyzw
// MODELMATRIX  -> gl_MultiTexCoord6.xyzw
// MODELMATRIX  -> gl_MultiTexCoord7.xyzw

void main ()
{

    // MATRIX -----------------------------------------------------------------------------------------------------------------------------------------------

            #ifdef INSTANCED 

                vec3 co = cos (gl_MultiTexCoord5.xyz);
                vec3 si = sin (gl_MultiTexCoord5.xyz);
                
                mat4 matrix     = mat4 (vec4 ( co.y*co.z,                  -co.y*si.z,                   si.y,      0.0) * gl_MultiTexCoord5.w, 
                                        vec4 ( co.x*si.z + si.x*si.y*co.z,  co.x*co.z - si.x*si.y*si.z, -si.x*co.y, 0.0) * gl_MultiTexCoord5.w, 
                                        vec4 ( si.x*si.z - co.x*si.y*co.z,  si.x*co.z + co.x*si.y*si.z,  co.x*co.y, 0.0) * gl_MultiTexCoord5.w, 
                                        vec4 (gl_MultiTexCoord4.xyz, 1.0));
            #else
                                    
                mat4 matrix     = mat4 (vec4 (gl_MultiTexCoord4.xyz, 0.0), 
                                        vec4 (gl_MultiTexCoord5.xyz, 0.0), 
                                        vec4 (gl_MultiTexCoord6.xyz, 0.0), 
                                        vec4 (gl_MultiTexCoord7.xyz, 1.0));                                    
            #endif

    // COMPLETE VIEW TRANSFORM ------------------------------------------------------------------------------------------------------------------------------
    
            matrix          = gl_ModelViewMatrix * matrix;

    // TRANSFORM --------------------------------------------------------------------------------------------------------------------------------------------

            gl_ClipVertex   = matrix * gl_Vertex;
            gl_Position     = gl_ProjectionMatrix * gl_ClipVertex;

    // UV ---------------------------------------------------------------------------------------------------------------------------------------------------

            gl_TexCoord [0] = vec4 (gl_MultiTexCoord0.st * uvscale, 0.0, 0.0);

    // GLOSS ------------------------------------------------------------------------------------------------------------------------------------------------
                
            gloss           = min (0.999999, gl_MultiTexCoord3.x * 0.5);    // we scale to maximum value 2.0
    
    // SHININESS --------------------------------------------------------------------------------------------------------------------------------------------
                            
            shininess       = gl_MultiTexCoord3.y * 0.1;                    // we leave more accurancy to gloss value, step is 10.0

    // COLOR ------------------------------------------------------------------------------------------------------------------------------------------------

            color           = gl_Color.rgb;

    // TBN MATRIX -------------------------------------------------------------------------------------------------------------------------------------------
    
                 normal     = normalize (mat3 (matrix) *        gl_Normal);
            vec3 tangent    = normalize (mat3 (matrix) * (2.0 * gl_SecondaryColor.xyz - 1.0));
            vec3 binormal   = cross     (tangent, normal);

    ///     Inverted TBN matrix (is orthonormal -> invert = transpose)
    
            tbni [0]        = vec3 (tangent.x, binormal.x, normal.x);
            tbni [1]        = vec3 (tangent.y, binormal.y, normal.y);
            tbni [2]        = vec3 (tangent.z, binormal.z, normal.z);
}
