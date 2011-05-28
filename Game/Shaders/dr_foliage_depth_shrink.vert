
uniform vec2 uvscale;

varying float factor;

// DETAIL       -> gl_FogCoord
// ANIM         -> gl_MultiTexCoord1.x
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
            
    // DETAIL -----------------------------------------------------------------------------------------------------------------------------------------------

            vec2 factors    = min (1.0, max (0.0, gl_TexCoord [0].xy - vec2 (1.0, 1.0)) * 1000000.0) * gl_FogCoord;
                        
            factor          = factors.x;
            
            vec4 v          = gl_Vertex;  v.xyz *= 1.0 - factors.y;
        
    // ANIMATION --------------------------------------------------------------------------------------------------------------------------------------------
                
            v.xyz           = v.xyz + (0.005 * sin (0.01 * gl_MultiTexCoord1.xxx + 0.5 * v.xyz) + vec3 (0.05, 0.0125, 0.025) * sin (0.005 * gl_MultiTexCoord1.x + 0.5 * v.x)) * gl_TexCoord [0].t;

    // COMPLETE VIEW TRANSFORM ------------------------------------------------------------------------------------------------------------------------------
    
            matrix          = gl_ModelViewMatrix * matrix;

    // TRANSFORM --------------------------------------------------------------------------------------------------------------------------------------------

            gl_ClipVertex   = matrix * v;
            gl_Position     = gl_ProjectionMatrix * gl_ClipVertex;
}
