
uniform sampler2DRect   tex_G2;
uniform sampler2D       tex_rand;
uniform sampler2DRect   tex_shade;

uniform float randscale;

varying vec3 view;

#define RADIUS      500.0
#define RADIUSMAX   100.0
#define SAMPLES     16
#define SAMPLESINV  0.0625
#define STRENGTH    20.0
#define DISTANCEQI  1.0
#define TRANSITION  10000.0

void main ()
{

    vec4 G2 = texture2DRect (tex_G2, gl_TexCoord [0].xy);

    // skipping sky, big flat parts, lighted parts
    
    float shade = texture2DRect (tex_shade, gl_TexCoord [0].xy).a;
    
    if (shade >= 0.5) {
        
        gl_FragColor = vec4 (1.0, 1.0, 1.0, 1.0);
        return; 
    }
            
    // these are the random vectors inside a unit sphere (no unit vectors)
    
    // ATI :

    vec2 sphere [SAMPLES];

    sphere [ 0] = vec2 (0.23703703, 0.12592593);
    sphere [ 1] = vec2 (0.10370370, 0.73333335);
    sphere [ 2] = vec2 (0.25185186, 0.88148147);
    sphere [ 3] = vec2 (0.91111112, 0.73333335);
        
    sphere [ 4] = vec2 (0.60740739, 0.12592593);
    sphere [ 5] = vec2 (0.17777778, 0.35555556);
    sphere [ 6] = vec2 (0.70370370, 0.82222223);
    sphere [ 7] = vec2 (0.85925925, 0.54814816);
    
    sphere [ 8] = vec2 (0.41481480, 0.25185186);
    sphere [ 9] = vec2 (0.28888890, 0.40000001);
    sphere [10] = vec2 (0.54814816, 0.77777779);
    sphere [11] = vec2 (0.74074072, 0.53333336);

    sphere [12] = vec2 (0.52592593, 0.36296296);
    sphere [13] = vec2 (0.42222223, 0.48148149);
    sphere [14] = vec2 (0.57037038, 0.51111114);
    sphere [15] = vec2 (0.51111114, 0.61481482);

    // DECODE DEPTH
    float depth     = (G2.z + G2.w) * 0.00001525902;    // 1.0 / 65535.0

    // VIEW POSITION   
    vec3 viewpos    = depth * view;

    // WORLD POSITION
    vec4 worldpos   = gl_ModelViewMatrixInverse * vec4 (viewpos, 1.0);

    // NORMAL DECODE

        // SPHEREMAP TRANSFORM
        //
        // http://aras-p.info/texts/CompactNormalStorage.html#method04spheremap
        //
        
    vec3 normal;

    normal.xy       = G2.xy * 4.0 - 2.0;           float d = dot (normal.xy, normal.xy);
    
    normal.xy      *= sqrt (1.0 - d * 0.25);    
    normal.z        =       1.0 - d * 0.5;
    
    normal          = normalize (normal);

    float rand      = texture2D (tex_rand, 0.5 * gl_TexCoord [0].st).x * 3.14;
    
//  float rand      = texture2D (tex_rand, 100000.0 * (worldpos.xz + worldpos.y)).x * 6.28;
        
    float cosa      = cos (rand);
    float sina      = sin (rand);

    float radius    = min (RADIUS / viewpos.z, RADIUSMAX);

    float occlusion = 0.0;
    float sum       = 1.0;

    for (int i = 0; i < SAMPLES; i ++) {

        // 1. ROTATING SPHERE KERNEL RANDOMLY
        
        vec2 coord      = radius * (2.0 * sphere [i] - 1.0);

        vec4 occluder   = texture2DRect (tex_G2, gl_TexCoord [0].xy - vec2 (coord.x*cosa - coord.y*sina, coord.x*sina - coord.y*cosa));

        // 2. REFLECTING VECTORS BY RANDOM VECTOR
        //
        //      vec4 occluder   = texture2DRect (tex_G2, gl_TexCoord [0].xy + vec2 (radius * reflect (sphere [i], randvec)));

        // DECODE DEPTH
        float odepth    = (occluder.z + occluder.w) * 0.00001525902;    // 1.0 / 65535.0  

        // VIEW POSITION   
        vec3 oviewpos   = odepth * view;

        // NORMAL DECODE

            // SPHEREMAP TRANSFORM
            //
            // http://aras-p.info/texts/CompactNormalStorage.html#method04spheremap
            //
            
        vec3 onormal;

        onormal.xy      = occluder.xy * 4.0 - 2.0;           float d = dot (onormal.xy, onormal.xy);
        
        onormal.xy     *= sqrt (1.0 - d * 0.25);    
        onormal.z       =       1.0 - d * 0.5;
        
        onormal         = normalize (onormal);

        // DISTANCE VECTOR
        vec3 v          = oviewpos - viewpos;

        // Calculate the difference between the normals as a weight
        // The falloff equation, starts at falloff and is kind of 1/x^2 falling
        //
        // 1.0 - dot -> sin (a) -> = 0 if angle = 0 
        
        vec3 vn = normalize (v);
        
        float rfactor = max (dot (vn, normal), 0.2);

        float nfactor = (1.0 - max (0.0, dot (normal, onormal)));
        
        float dfactor = clamp (TRANSITION * (1.0 - DISTANCEQI * dot (v, v)), 0.0, 1.0);

        occlusion += dfactor * rfactor * nfactor;
        
        sum += dfactor;
    }

    // output the result
    occlusion = clamp (1.0 - STRENGTH * SAMPLESINV * occlusion * (1.0 - shade) / sum, 0.0, 1.0);
    
    gl_FragColor = vec4 (occlusion, occlusion, occlusion, shade);
}
