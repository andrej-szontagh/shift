
uniform vec2 uvscale;

varying float factor;

// DETAIL       -> gl_FogCoord
// ANIM         -> gl_MultiTexCoord1.x
// SCALE        -> gl_MultiTexCoord3.xyz
// PUSH         -> gl_MultiTexCoord3.w

// NOT INSTANCED
// MODELMATRIX  -> gl_MultiTexCoord4.xyzw
// MODELMATRIX  -> gl_MultiTexCoord5.xyzw
// MODELMATRIX  -> gl_MultiTexCoord6.xyzw
// MODELMATRIX  -> gl_MultiTexCoord7.xyzw

// INSTANCED
// TRANSFORM    -> gl_MultiTexCoord4.xyz
// TRANSFORM    -> gl_MultiTexCoord5.xyzw

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

    // UV ---------------------------------------------------------------------------------------------------------------------------------------------------
    
            gl_TexCoord [0] = vec4 (gl_MultiTexCoord0.st * uvscale, 0.0, 0.0);
            
    // DETAIL -----------------------------------------------------------------------------------------------------------------------------------------------

            vec2 factors    = min (1.0, max (0.0, gl_TexCoord [0].xy - vec2 (1.0, 1.0)) * 1000000.0) * gl_FogCoord;
                        
            factor          = factors.x;
            
            vec4            v = gl_Vertex;	v.xyz *= (vec3 (1.0, 1.0, 1.0) - gl_MultiTexCoord3.xyz * factors.yyy);   v.z += gl_MultiTexCoord3.w * factors.y;
        
    // ANIMATION --------------------------------------------------------------------------------------------------------------------------------------------
    
            v.y             = v.y + dot (v, v) * gl_MultiTexCoord1.x;
            v.x             = v.x + dot (v, v) * gl_MultiTexCoord1.x;
            
    // COMPLETE VIEW TRANSFORM ------------------------------------------------------------------------------------------------------------------------------
    
            matrix          = gl_ModelViewMatrix * matrix;

    // TRANSFORM --------------------------------------------------------------------------------------------------------------------------------------------

            gl_ClipVertex   = matrix * v;
            gl_Position     = gl_ProjectionMatrix * gl_ClipVertex;
}
