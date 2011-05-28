
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

    // COMPLETE VIEW TRANSFORM ------------------------------------------------------------------------------------------------------------------------------
                            
            matrix = gl_ModelViewMatrix * matrix;

    // TRANSFORM --------------------------------------------------------------------------------------------------------------------------------------------

            float factor = max (0.0, 1.0 - gl_FogCoord);

            gl_ClipVertex   = matrix * (gl_Vertex * vec4 (factor, factor, factor, 1.0));
            gl_Position     = gl_ProjectionMatrix * gl_ClipVertex;
}
