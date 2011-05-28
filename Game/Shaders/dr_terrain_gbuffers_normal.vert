
uniform float planefar;

uniform vec2 uv1scale;
uniform vec2 uv2scale;

varying vec3 shininess;
varying vec3 gloss;

varying vec3 normal;
varying vec3 tangent;
varying vec3 binormal;

varying float depth;

// UV1          -> gl_MultiTexCoord0.st
// UV2          -> gl_MultiTexCoord1.st
// GLOSS1       -> gl_MultiTexCoord2.x
// GLOSS2       -> gl_MultiTexCoord2.y
// GLOSS3       -> gl_MultiTexCoord2.z
// SHININESS1   -> gl_MultiTexCoord3.x
// SHININESS2   -> gl_MultiTexCoord3.y
// SHININESS3   -> gl_MultiTexCoord3.z
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

    // LINEAR DEPTH -----------------------------------------------------------------------------------------------------------------------------------------
    
    ///     gl_Position holds clip coordinates
    ///     http://www.songho.ca/opengl/gl_projectionmatrix.html
    ///
    ///     divide gl_Position / gl_Position.w  maps to unit cube BUT depth is not linear !!
    ///        
    ///     linear depth is view coordinate which is negative.
    ///     We dont scale it from near plane to far plane so it starts at camera with 0 and ends on far plane with 1
    ///     http://www.songho.ca/opengl/gl_projectionmatrix.html
    ///
    ///     this depth actually hodls view coordinate z (scaled to positive 0..65535.0 camera/farplane)

            depth           = (- gl_ClipVertex.z / planefar) * 65535.0;

    // UV ---------------------------------------------------------------------------------------------------------------------------------------------------
    
            gl_TexCoord [0] = gl_MultiTexCoord0 * vec4 (uv1scale, 0.0, 0.0);
            gl_TexCoord [1] = gl_MultiTexCoord1 * vec4 (uv2scale, 0.0, 0.0);

    // GLOSS ------------------------------------------------------------------------------------------------------------------------------------------------
    
            gloss           = gl_MultiTexCoord2.xyz * 2.0;  // !!!!!!!!

    // SHININESS --------------------------------------------------------------------------------------------------------------------------------------------

            shininess       = gl_MultiTexCoord3.xyz;

    /// TBN MATRIX COMPONENTS -------------------------------------------------------------------------------------------------------------------------------
    
            normal          = normalize (mat3 (matrix) *        gl_Normal);
            tangent         = normalize (mat3 (matrix) * (2.0 * gl_SecondaryColor.xyz - 1.0));
            binormal        = cross     (tangent, normal);
                        
}
