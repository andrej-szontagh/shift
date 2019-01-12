
uniform float planefar;

uniform vec2 uv1scale;
uniform vec2 uv2scale;

varying mat3 tbni;

// UV1          -> gl_MultiTexCoord0.st
// UV2          -> gl_MultiTexCoord1.st
// MODELMATRIX  -> gl_MultiTexCoord4.xyzw
// MODELMATRIX  -> gl_MultiTexCoord5.xyzw
// MODELMATRIX  -> gl_MultiTexCoord6.xyzw
// MODELMATRIX  -> gl_MultiTexCoord7.xyzw

void main ()
{

    // MATRIX -----------------------------------------------------------------------------------------------------------------------------------------------
    
            /// ONLY UNIFROM SCALING (otherwise use normalmatrix)
    
            mat4 matrix     = mat4 (vec4 (gl_MultiTexCoord4.xyz, 0.0), 
                                    vec4 (gl_MultiTexCoord5.xyz, 0.0), 
                                    vec4 (gl_MultiTexCoord6.xyz, 0.0), 
                                    vec4 (gl_MultiTexCoord7.xyz, 1.0));
    
    // COMPLETE VIEW TRANSFORM ------------------------------------------------------------------------------------------------------------------------------
                                
            matrix = gl_ModelViewMatrix * matrix;
                            
    // TRANSFORM --------------------------------------------------------------------------------------------------------------------------------------------

            gl_ClipVertex   = matrix * gl_Vertex;
            gl_Position     = gl_ProjectionMatrix * gl_ClipVertex;

    // UV ---------------------------------------------------------------------------------------------------------------------------------------------------
    
            gl_TexCoord [0] = gl_MultiTexCoord0 * vec4 (uv1scale, 0.0, 0.0);
            gl_TexCoord [1] = gl_MultiTexCoord1 * vec4 (uv2scale, 0.0, 0.0);

    // TBN MATRIX -------------------------------------------------------------------------------------------------------------------------------------------
    
            vec3 normal     = normalize (mat3 (matrix) *        gl_Normal);
            vec3 tangent    = normalize (mat3 (matrix) * (2.0 * gl_SecondaryColor.xyz - 1.0));
            vec3 binormal   = cross     (tangent, normal);

    ///     Inverted TBN matrix (is orthonormal -> invert = transpose)
    
            tbni [0]        = vec3 (tangent.x, binormal.x, normal.x);
            tbni [1]        = vec3 (tangent.y, binormal.y, normal.y);
            tbni [2]        = vec3 (tangent.z, binormal.z, normal.z);
}
