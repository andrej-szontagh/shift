
uniform vec2 uvscale;

uniform float planefar;

varying float depth;

varying vec3 normal;
varying mat3 tbni;

// DETAIL       -> gl_FogCoord
// ANIM         -> gl_MultiTexCoord1.x
// SCALE        -> gl_MultiTexCoord3.xyz
// PUSH         -> gl_MultiTexCoord3.w
// MODELMATRIX  -> gl_MultiTexCoord4.xyzw
// MODELMATRIX  -> gl_MultiTexCoord5.xyzw
// MODELMATRIX  -> gl_MultiTexCoord6.xyzw
// MODELMATRIX  -> gl_MultiTexCoord7.xyzw

void main ()
{

    // MATRIX -----------------------------------------------------------------------------------------------------------------------------------------------

            mat4 matrix     = mat4 (vec4 (gl_MultiTexCoord4.xyz, 0.0), 
                                    vec4 (gl_MultiTexCoord5.xyz, 0.0), 
                                    vec4 (gl_MultiTexCoord6.xyz, 0.0), 
                                    vec4 (gl_MultiTexCoord7.xyz, 1.0));

    // UV ---------------------------------------------------------------------------------------------------------------------------------------------------

            gl_TexCoord [0] = vec4 (gl_MultiTexCoord0.st * uvscale, 0.0, 0.0);
                
    // COMPLETE VIEW TRANSFORM ------------------------------------------------------------------------------------------------------------------------------
                            
            matrix          = gl_ModelViewMatrix * matrix;
            
    // DETAIL -----------------------------------------------------------------------------------------------------------------------------------------------

            vec2 factors    = min (1.0, max (0.0, gl_TexCoord [0].xy - vec2 (1.0, 1.0)) * 1000000.0) * gl_FogCoord;
                        
            vec4            v = gl_Vertex;  v.xyz *= (vec3 (1.0, 1.0, 1.0) - gl_MultiTexCoord3.xyz * factors.yyy);   v.z += gl_MultiTexCoord3.w * factors.y;
                    
    // ANIMATION --------------------------------------------------------------------------------------------------------------------------------------------
    
            v.y             = v.y + dot (v, v) * gl_MultiTexCoord1.x;
            v.x             = v.x + dot (v, v) * gl_MultiTexCoord1.x;
            
    // TRANSFORM --------------------------------------------------------------------------------------------------------------------------------------------
    
            gl_ClipVertex   = matrix * v;
            gl_Position     = gl_ProjectionMatrix * gl_ClipVertex;

    // LINEAR DEPTH -----------------------------------------------------------------------------------------------------------------------------------------
    
            depth           = - gl_ClipVertex.z / planefar;
            
    // TBN MATRIX -------------------------------------------------------------------------------------------------------------------------------------------
    
                 normal     = normalize (vec3 (gl_ModelViewMatrix * vec4 (0.0, 1.0, 0.0, 0.0)));
            vec3 tangent    = normalize (vec3 (gl_ModelViewMatrix * vec4 (1.0, 0.0, 0.0, 0.0)));
            vec3 binormal   = cross     (tangent, normal);
            
    ///     Inverted TBN matrix (is orthonormal -> invert = transpose)
    
            tbni [0]        = vec3 (tangent.x, binormal.x, normal.x);
            tbni [1]        = vec3 (tangent.y, binormal.y, normal.y);
            tbni [2]        = vec3 (tangent.z, binormal.z, normal.z);            
}
