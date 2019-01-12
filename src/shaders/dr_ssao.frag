
uniform sampler2DRect   tex_G2;
uniform sampler2DRect   tex_depth;
uniform sampler2D       tex_rand;

uniform float nearplane;
uniform float farplane;
uniform float farw;
uniform float farh;

uniform float hwidth;
uniform float hheight;

uniform float scalex;
uniform float scaley;

uniform float randscale;

uniform float depth1;
uniform float depth2;
uniform float depth3;

varying vec3 view;

#define RADIUS      50.0
#define RADIUSMAX   50.0
#define SAMPLES     16
#define SAMPLESINV  0.0625
#define STRENGTH    20.0
#define DISTANCEQI  10.0
#define TRANSITION  0.5

void main ()
{

    // these are the random vectors inside a unit sphere (no unit vectors)
    
    vec2 sphere [SAMPLES];

    sphere [ 0] = vec2 (-0.23703703,  0.12592593);
    sphere [ 1] = vec2 ( 0.10370370,  0.73333335);
    sphere [ 2] = vec2 (-0.25185186, -0.88148147);
    sphere [ 3] = vec2 ( 0.91111112, -0.73333335);
        
    sphere [ 4] = vec2 (-0.60740739,  0.12592593) * 1.0;
    sphere [ 5] = vec2 ( 0.17777778,  0.35555556) * 1.0;
    sphere [ 6] = vec2 (-0.70370370, -0.82222223) * 1.0;
    sphere [ 7] = vec2 ( 0.85925925, -0.54814816) * 1.0;
    
    sphere [ 8] = vec2 (-0.41481480,  0.25185186) * 2.0;
    sphere [ 9] = vec2 ( 0.28888890,  0.40000001) * 2.0;
    sphere [10] = vec2 (-0.54814816, -0.77777779) * 2.0;
    sphere [11] = vec2 ( 0.74074072, -0.53333336) * 2.0;

    sphere [12] = vec2 (-0.52592593,  0.36296296) * 3.0;
    sphere [13] = vec2 ( 0.42222223,  0.48148149) * 3.0;
    sphere [14] = vec2 (-0.57037038, -0.51111114) * 3.0;
    sphere [15] = vec2 ( 0.51111114, -0.61481482) * 3.0;

    // DEPTH
    float depth     = texture2DRect (tex_depth, gl_TexCoord [0]).x;

    // SKIP ENVIROMENT
    if (depth == 1.0) {
        
        gl_FragColor    = vec4 (1.0, 1.0, 1.0, 1.0);
        return; 
    }

    // LINEAR DEPTH

    // depth        = (2.0 * nearplane) / (farplane + nearplane - (2.0 * depth - 1.0) * (farplane - nearplane));

    depth           = depth1 / (depth2 - depth3 * depth);

    // VIEW POSITION   
    vec3 viewpos    = depth * view;

    // NORMAL DECODE
    vec4 G2         = texture2DRect (tex_G2, gl_TexCoord [0].xy);

    // SPHEREMAP TRANSFORM
    //
    // http://aras-p.info/texts/CompactNormalStorage.html#method04spheremap
    //
        
    vec3 normal;

    normal.xy       = (G2.xy + G2.zw * 0.00392156) * 4.0 - 2.0;    float d = dot (normal.xy, normal.xy);
    
    normal.xy      *= sqrt (1.0 - d * 0.25);    
    normal.z        =       1.0 - d * 0.5;
    
    normal          = normalize (normal);

    float rand      = texture2D (tex_rand, 0.5 * gl_TexCoord [0].st).x * 3.14;

    float cosa      = cos (rand);
    float sina      = sin (rand);

    float radius    = min (RADIUS / abs (viewpos.z), RADIUSMAX);

    float occlusion = 0.0;


    /// DIVIDE LOOP INTO TWO PART :
    ///  - FIRST PART (8 SAMPLES)
    //         - IF OCCLUSION IS TOO SMALL -> FINISH
    //   - SECOND PART IF OCCLUSION IS APPARENT




    for (int i = 0; i < SAMPLES; i ++) {

        // ROTATING SPHERE KERNEL RANDOMLY
        
        vec2 coord      = radius * sphere [i];
        
             coord      = gl_TexCoord [0].xy - vec2 (   coord.x*cosa - coord.y*sina, 
                                                        coord.x*sina - coord.y*cosa);
        // DECODE DEPTH

        // float odepth = (2.0 * nearplane) / (farplane + nearplane - (2.0 * texture2DRect (tex_depth, coord).x - 1.0) * (farplane - nearplane));

        float odepth    = depth1 / (depth2 - depth3 * texture2DRect (tex_depth, coord).x);

        // VIEW POSITION

        vec3 oviewpos   = odepth * vec3 ((coord.xy - vec2 (hwidth, hheight)) * vec2 (scalex, scaley), - farplane);

        // NORMAL DECODE

        G2              = texture2DRect (tex_G2, coord);

        // SPHEREMAP TRANSFORM
        //
        // http://aras-p.info/texts/CompactNormalStorage.html#method04spheremap
        //
            
        vec3 onormal;

        onormal.xy      = (G2.xy + G2.zw * 0.00392156) * 4.0 - 2.0;    float d = dot (onormal.xy, onormal.xy);
        
        onormal.xy     *= sqrt (1.0 - d * 0.25);    
        onormal.z       =       1.0 - d * 0.5;

        onormal         = normalize (onormal);

        // DISTANCE VECTOR

        vec3 v          = oviewpos - viewpos;        
        vec3 vn         = normalize (v);
        
        float factor_angle      = max (0.0, dot (vn,  normal));
        float factor_normal     = 0.5 - 0.5 * dot (normal, onormal);
        float factor_distance   = clamp (TRANSITION * (1.0 - DISTANCEQI * dot (v, v)), 0.0, 1.0);

        occlusion += factor_normal * factor_angle * factor_distance;
    }

    // output the result
    occlusion = 1.0 - max (0.0, STRENGTH * SAMPLESINV * occlusion);

    // post process
    // occlusion = max (0.4, log2 (1.0 + 100.0 * occlusion) / log2 (101.0));


    occlusion = max (0.3, 0.3 + pow (occlusion, 2.0));

    // out
    gl_FragColor = vec4 (occlusion, occlusion, occlusion, 1.0);
}
