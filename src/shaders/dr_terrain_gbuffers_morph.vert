
uniform float planefar;

uniform vec2 uv1scale;
uniform vec2 uv2scale;

varying mat3 tbni;

// UV1          -> gl_MultiTexCoord0.st
// UV2          -> gl_MultiTexCoord1.st
// ANIM         -> gl_MultiTexCoord2.stpq
// SCALECO      -> gl_MultiTexCoord3.x
// SCALENO      -> gl_MultiTexCoord3.y
// SCALEUV      -> gl_MultiTexCoord3.z
// MODELMATRIX  -> gl_MultiTexCoord4.xyzw
// MODELMATRIX  -> gl_MultiTexCoord5.xyzw
// MODELMATRIX  -> gl_MultiTexCoord6.xyzw
// MODELMATRIX  -> gl_MultiTexCoord7.xyzw

void main ()
{

    // MORPHING ---------------------------------------------------------------------------------------------------------------------------------------------
            
            vec4 vec = (32768.0 + gl_MultiTexCoord2.stpq) * 1.0 / 256.0;
                
            vec4 vv1 = (2.0 * ((floor (vec) + 0.5) *  1.0  / 255.0) - 1.0) * gl_FogCoord;
            vec4 vv2 = (2.0 * ((fract (vec) * 256.0 + 0.5) / 255.0) - 1.0) * gl_FogCoord;
            
            vec4 v = gl_Vertex              + vec4 (vv1.x, vv2.x, vv1.y, 0.0)   * gl_MultiTexCoord3.x;
            vec3 n = gl_Normal              + vec3 (vv2.y, vv1.z, vv2.z)        * gl_MultiTexCoord3.y;
            vec2 t = gl_MultiTexCoord0.st   + vec2 (vv1.w, vv2.w)               * gl_MultiTexCoord3.z;
                        
    // MATRIX -----------------------------------------------------------------------------------------------------------------------------------------------
        
            mat4 matrix     = mat4 (vec4 (gl_MultiTexCoord4.xyz, 0.0), 
                                    vec4 (gl_MultiTexCoord5.xyz, 0.0), 
                                    vec4 (gl_MultiTexCoord6.xyz, 0.0), 
                                    vec4 (gl_MultiTexCoord7.xyz, 1.0));
    
    // COMPLETE VIEW TRANSFORM ------------------------------------------------------------------------------------------------------------------------------
                                
            matrix = gl_ModelViewMatrix * matrix;
                            
    // TRANSFORM --------------------------------------------------------------------------------------------------------------------------------------------

            gl_ClipVertex   = matrix * v;
            gl_Position     = gl_ProjectionMatrix * gl_ClipVertex;

    // UV ---------------------------------------------------------------------------------------------------------------------------------------------------
    
            gl_TexCoord [0] =                 vec4 (t * uv1scale, 0.0, 0.0);
            gl_TexCoord [1] = gl_MultiTexCoord1 * vec4 (uv2scale, 0.0, 0.0);
            
    /// TBN MATRIX ------------------------------------------------------------------------------------------------------------------------------------------
    
            vec3 normal     = normalize (mat3 (matrix) *        n);
            vec3 tangent    = normalize (mat3 (matrix) * (2.0 * gl_SecondaryColor.xyz - 1.0));
            vec3 binormal   = cross     (tangent, normal);
        
    /// INVERTED TBN MATRIX (is orthonormal -> invert = transpose)
    
            tbni [0] = vec3 (tangent.x, binormal.x, normal.x);
            tbni [1] = vec3 (tangent.y, binormal.y, normal.y);
            tbni [2] = vec3 (tangent.z, binormal.z, normal.z);
}
