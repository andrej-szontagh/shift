 
// ANIM         -> gl_MultiTexCoord1.xyzw
// SCALECO      -> gl_MultiTexCoord2.x
// SCALENO      -> gl_MultiTexCoord2.y
// SCALEUV      -> gl_MultiTexCoord2.z
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

    // MORPHING ---------------------------------------------------------------------------------------------------------------------------------------------
    
            vec4 vec = (32767.0 + gl_MultiTexCoord1.xyzw) * 1.0/256.0;
                
            vec4 vv1 = (2.0 * (floor (vec) *   1.0/255.0) - 1.0) * gl_FogCoord;
            vec4 vv2 = (2.0 *  fract (vec) * 256.0/255.0  - 1.0) * gl_FogCoord;
            
            vec4 v = gl_Vertex + vec4 (vv1.x, vv2.x, vv1.y, 0.0) * gl_MultiTexCoord2.x;
    
    // COMPLETE VIEW TRANSFORM ------------------------------------------------------------------------------------------------------------------------------
                            
            matrix = gl_ModelViewMatrix * matrix;

    // TRANSFORM --------------------------------------------------------------------------------------------------------------------------------------------

            gl_ClipVertex   = matrix * v;
            gl_Position     = gl_ProjectionMatrix * gl_ClipVertex;
}
