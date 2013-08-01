
uniform vec2 uvscale;

varying float factor;

// DETAIL       -> gl_FogCoord
// ANIM         -> gl_MultiTexCoord1.x

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
                        
//          factor          = factors.x;
            factor          = log2 (1.0 + 100.0 * gl_FogCoord) / log2 (101.0);      // distance factor used to remove 'alpha disappearing'
            
            vec4 v          = gl_Vertex;  v.xyz *= 1.0 - factors.y;
        
    // ANIMATION --------------------------------------------------------------------------------------------------------------------------------------------
                
/*
            #define M_WAVE1     0.005
            #define M_WAVE2X    0.05
            #define M_WAVE2Y    0.0125
            #define M_WAVE2Z    0.025
*/   
            #define M_WAVE1     0.0025
            #define M_WAVE2X    0.025
            #define M_WAVE2Y    0.00625
            #define M_WAVE2Z    0.0125

            v.xyz           = v.xyz + (       M_WAVE1                       * sin (0.01  * gl_MultiTexCoord1.xxx + 0.5 * v.xyz) + 
                                        vec3 (M_WAVE2X, M_WAVE2Y, M_WAVE2Z) * sin (0.005 * gl_MultiTexCoord1.x   + 0.5 * v.x)) * gl_TexCoord [0].t;

    // COMPLETE VIEW TRANSFORM ------------------------------------------------------------------------------------------------------------------------------
    
            matrix          = gl_ModelViewMatrix * matrix;

    // TRANSFORM --------------------------------------------------------------------------------------------------------------------------------------------

            gl_ClipVertex   = matrix * v;
            gl_Position     = gl_ProjectionMatrix * gl_ClipVertex;
}
