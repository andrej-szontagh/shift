
uniform vec2 uvscale;

uniform float planefar;

varying float shininess;
varying float gloss;

varying float depth;

varying vec3 normal;
varying mat3 tbni;

// DETAIL       -> gl_FogCoord
// ANIM         -> gl_MultiTexCoord1.x
// GLOSS        -> gl_MultiTexCoord2.x
// SHININESS    -> gl_MultiTexCoord2.y
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
                        
            vec4 v          = gl_Vertex;  v.xyz *= 1.0 - factors.y;
                    
    // ANIMATION --------------------------------------------------------------------------------------------------------------------------------------------
    
            v.xyz           = v.xyz + (0.005 * sin (0.01 * gl_MultiTexCoord1.xxx + 0.5 * v.xyz) + vec3 (0.05, 0.0125, 0.025) * sin (0.005 * gl_MultiTexCoord1.x + 0.5 * v.x)) * gl_TexCoord [0].t;
                        
    // TRANSFORM --------------------------------------------------------------------------------------------------------------------------------------------
    
            gl_ClipVertex   = matrix * v;
            gl_Position     = gl_ProjectionMatrix * gl_ClipVertex;

    // LINEAR DEPTH -----------------------------------------------------------------------------------------------------------------------------------------
    
            depth           = (- gl_ClipVertex.z / planefar) * 65535.0;

    // GLOSS ------------------------------------------------------------------------------------------------------------------------------------------------
                
            gloss           = min (0.999999, gl_MultiTexCoord2.x * 0.5);    // we scale to maximum value 2.0
    
    // SHININESS --------------------------------------------------------------------------------------------------------------------------------------------
                            
            shininess       = gl_MultiTexCoord2.y * 0.1;                    // we leave more accurancy to gloss value, step is 10.0
            
    // TBN MATRIX -------------------------------------------------------------------------------------------------------------------------------------------

                 normal     = normalize (vec3 (gl_ModelViewMatrix * vec4 (0.0, 1.0, 0.0, 0.0)));
            vec3 tangent    = normalize (vec3 (gl_ModelViewMatrix * vec4 (1.0, 0.0, 0.0, 0.0)));
            vec3 binormal   = cross     (tangent, normal);
            
    ///     Inverted TBN matrix (is orthonormal -> invert = transpose)
    
            tbni [0]        = vec3 (tangent.x, binormal.x, normal.x);
            tbni [1]        = vec3 (tangent.y, binormal.y, normal.y);
            tbni [2]        = vec3 (tangent.z, binormal.z, normal.z);            
}
