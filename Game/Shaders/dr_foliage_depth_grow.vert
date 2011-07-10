
uniform vec2 uvscale;

varying float factor;

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
            
    // DETAIL -----------------------------------------------------------------------------------------------------------------------------------------------

            vec2 factors    = min (1.0, max (0.0, gl_TexCoord [0].xy - vec2 (1.0, 1.0)) * 1000000.0) * gl_FogCoord;
                        
            factor          = factors.x;
            
            vec4            v = gl_Vertex;  v.xyz *= (vec3 (1.0, 1.0, 1.0) - gl_MultiTexCoord3.xyz * factors.yyy);   v.z += gl_MultiTexCoord3.w * factors.y;
        
    // ANIMATION --------------------------------------------------------------------------------------------------------------------------------------------
    
            v.y             = v.y + dot (v, v) * gl_MultiTexCoord1.x;
            v.x             = v.x + dot (v, v) * gl_MultiTexCoord1.x;
            
    // COMPLETE VIEW TRANSFORM ------------------------------------------------------------------------------------------------------------------------------
    
            matrix          = gl_ModelViewMatrix * matrix;

    // TRANSFORM --------------------------------------------------------------------------------------------------------------------------------------------

            gl_ClipVertex   = matrix * v;
            gl_Position     = gl_ProjectionMatrix * gl_ClipVertex;
}
