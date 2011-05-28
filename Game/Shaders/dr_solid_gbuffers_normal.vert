
uniform vec2 uvscale;

uniform float planefar;

varying float shininess;
varying float gloss;
varying float depth;

varying vec3 normal;

varying mat3 tbni;

// UV1          -> gl_MultiTexCoord0
// GLOSS        -> gl_MultiTexCoord3.x
// SHININESS    -> gl_MultiTexCoord3.y
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
    
            matrix          = gl_ModelViewMatrix * matrix;

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

            gl_TexCoord [0] = vec4 (gl_MultiTexCoord0.st * uvscale, 0.0, 0.0);

    // GLOSS ------------------------------------------------------------------------------------------------------------------------------------------------
                
            gloss           = min (0.999999, gl_MultiTexCoord3.x * 0.5);    // we scale to maximum value 2.0
    
    // SHININESS --------------------------------------------------------------------------------------------------------------------------------------------
                            
            shininess       = gl_MultiTexCoord3.y * 0.1;                    // we leave more accurancy to gloss value, step is 10.0

    // TBN MATRIX -------------------------------------------------------------------------------------------------------------------------------------------
    
                 normal     = normalize (mat3 (matrix) *        gl_Normal);
            vec3 tangent    = normalize (mat3 (matrix) * (2.0 * gl_SecondaryColor.xyz - 1.0));
            vec3 binormal   = cross     (tangent, normal);

    ///     Inverted TBN matrix (is orthonormal -> invert = transpose)
    
            tbni [0]        = vec3 (tangent.x, binormal.x, normal.x);
            tbni [1]        = vec3 (tangent.y, binormal.y, normal.y);
            tbni [2]        = vec3 (tangent.z, binormal.z, normal.z);
}