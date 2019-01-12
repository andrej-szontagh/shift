
uniform sampler2DRect   tex_G1;
uniform sampler2DRect   tex_G2;
uniform sampler2DRect   tex_G3;
uniform sampler2DRect   tex_depth;
uniform sampler2D       tex_rand;
uniform sampler2DShadow tex_shadow [SPLITS];

uniform float farplane;
uniform float nearplane;

uniform float intensity;
uniform float ambient;

uniform float offset   [SPLITS];
uniform float depthmin [SPLITS];
uniform float depthmax [SPLITS];
uniform float depthend;

uniform mat4 matrix [SPLITS];

varying float desaturation;
varying float brightness;
varying float contrast;

varying vec2 screen;

varying vec3 view;
varying vec3 ray;


void main ()
{

    // G1 READ
    vec4  G1        = texture2DRect (tex_G1, screen);
    
    // G2 READ
    vec4  G2        = texture2DRect (tex_G2, screen);

    // G3 READ
    vec4  G3        = texture2DRect (tex_G3, screen);
        
    // COLOR
    vec4  color     = G1 * gl_LightSource [0].diffuse;
    
    // EMISSION
    vec4  emission  = G3.z * color;

    // NORMAL
    vec3  normal;

    normal.xy       = (G2.xy + G2.zw * 0.00392156) * 4.0 - 2.0;    float d = dot (normal.xy, normal.xy);

    normal.xy      *= sqrt (1.0 - d * 0.25);
    normal.z        =       1.0 - d * 0.5;

    // ADJUST NORMAL FOR FOLIAGE

    if (G3.a == 1.0) {

        if (dot (gl_LightSource [0].position.xyz, normal) < 0.0) 

            normal  = -normal;
    }

    // LIGHT SLOPE

//  float slope     = max (dot (ray, normal) - 0.2, 0.0);
    float slope     = max (dot (ray, normal),       0.0);
        
    if (slope > 0.0) {
    
        // DECODE DEPTH
        float depth      = (2.0 * nearplane) / (farplane + nearplane - (2.0 * texture2DRect (tex_depth, screen).x - 1.0) * (farplane - nearplane));

        // VIEW POSITION
        vec3 viewpos     = depth * view;

        // WORLD POSITION
        vec4 worldpos    = gl_ModelViewMatrixInverse * vec4 (viewpos, 1.0);

        // SHADOW
        float shadow;
         
        // SHADOWMAP COORDS
        // vec4  coord = matrix * vec4 (worldpos.xyz, 1.0);   coord = coord * 0.5 + 0.5;
        
        // PCF + rotation
        
        /// x' = x*cosa - y*sina
        /// y' = y*cosa + x*sina
        
        // 4 samples    :   float rand  = texture2D (tex_rand, 200.0 * worldpos.xy).x * 1.57;
        // 2 samples    :   float rand  = texture2D (tex_rand, 350.0 * worldpos.xy).x * 0.785;
        // 1 sample     :   float rand  = texture2D (tex_rand, 500.0 * worldpos.xy).x * 0.392;

        float s;
        float s1;
        float s2;
        
        float disc  = (1.0 + pow ((1.0 - slope), 3.0));  //(1.0 + 3.0 * (1.0 - slope));
                
        float rand  = texture2D (tex_rand, 200.0 * worldpos.xy).x * 1.57;
        
        float cosa  = cos (rand) * disc;
        float sina  = sin (rand) * disc;

        // try manual depth bias calculation
        // http://www.gamedev.net/topic/556521-glsl-manual-shadow-map-biasscale/
        
        #if SPLITS == 3

            if ((depth > depthmin [0]) && (depth < depthmin [1])) {

                vec4  coord = matrix [0] * vec4 (worldpos.xyz, 1.0);   coord = coord * 0.5 + 0.5;
            
                s    = shadow2D (tex_shadow [0], coord.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;
                s   += shadow2D (tex_shadow [0], coord.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;
                s   += shadow2D (tex_shadow [0], coord.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;
                s   += shadow2D (tex_shadow [0], coord.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;
                shadow = s * 0.25;
            
                #ifdef DEBUG
                    gl_FragColor = vec4 (1.0, 0.0, 0.0, 1.0); return;
                #endif
        
            } else {

                // TRANSITION
                if ((depth > depthmin [1]) && (depth < depthmax [0])) {
            
                    vec4  coord1 = matrix [0] * vec4 (worldpos.xyz, 1.0);   coord1 = coord1 * 0.5 + 0.5;
                    vec4  coord2 = matrix [1] * vec4 (worldpos.xyz, 1.0);   coord2 = coord2 * 0.5 + 0.5;
                
                    s1   = shadow2D (tex_shadow [0], coord1.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;
                    s1  += shadow2D (tex_shadow [0], coord1.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;
                    s1  += shadow2D (tex_shadow [0], coord1.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;
                    s1  += shadow2D (tex_shadow [0], coord1.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;
                    s2   = shadow2D (tex_shadow [1], coord2.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;
                    s2  += shadow2D (tex_shadow [1], coord2.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;
                    s2  += shadow2D (tex_shadow [1], coord2.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;
                    s2  += shadow2D (tex_shadow [1], coord2.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;
                    shadow = (mix (s1, s2, (depth - depthmin [1]) / (depthmax [0] - depthmin [1])) * 0.25);

                    #ifdef DEBUG
                        gl_FragColor = vec4 (0.0, 0.0, 0.0, 1.0); return;
                    #endif
            
                } else {

                    if ((depth > depthmax [0]) && (depth < depthmin [2])) {

                        vec4  coord = matrix [1] * vec4 (worldpos.xyz, 1.0);   coord = coord * 0.5 + 0.5;
                    
                        s    = shadow2D (tex_shadow [1], coord.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;
                        s   += shadow2D (tex_shadow [1], coord.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;
                        s   += shadow2D (tex_shadow [1], coord.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;
                        s   += shadow2D (tex_shadow [1], coord.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;
                        shadow = s * 0.25;
                    
                        #ifdef DEBUG
                            gl_FragColor = vec4 (0.0, 1.0, 0.0, 1.0); return;
                        #endif
               
                    } else {
        
                        // TRANSITION
                        if ((depth > depthmin [2]) && (depth < depthmax [1])) {
                    
                            vec4  coord1 = matrix [1] * vec4 (worldpos.xyz, 1.0);   coord1 = coord1 * 0.5 + 0.5;
                            vec4  coord2 = matrix [2] * vec4 (worldpos.xyz, 1.0);   coord2 = coord2 * 0.5 + 0.5;
                        
                            s1   = shadow2D (tex_shadow [1], coord1.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;
                            s1  += shadow2D (tex_shadow [1], coord1.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;
                            s1  += shadow2D (tex_shadow [1], coord1.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;
                            s1  += shadow2D (tex_shadow [1], coord1.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;
                            s2   = shadow2D (tex_shadow [2], coord2.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;
                            s2  += shadow2D (tex_shadow [2], coord2.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;
                            s2  += shadow2D (tex_shadow [2], coord2.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;
                            s2  += shadow2D (tex_shadow [2], coord2.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;
                            shadow = (mix (s1, s2, (depth - depthmin [2]) / (depthmax [1] - depthmin [2])) * 0.25);
                        
                            #ifdef DEBUG
                                gl_FragColor = vec4 (0.0, 0.0, 0.0, 1.0); return;
                            #endif
                               
                        } else {
                                                                        
                            if ((depth > depthmax [1]) && (depth < depthend)) {
                                        
                                vec4  coord = matrix [2] * vec4 (worldpos.xyz, 1.0);   coord = coord * 0.5 + 0.5;
                                            
                                s    = shadow2D (tex_shadow [2], coord.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;
                                s   += shadow2D (tex_shadow [2], coord.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;
                                s   += shadow2D (tex_shadow [2], coord.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;
                                s   += shadow2D (tex_shadow [2], coord.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;
                                shadow = s * 0.25;
                                            
                                #ifdef DEBUG
                                    gl_FragColor = vec4 (0.0, 0.0, 1.0, 1.0); return;
                                #endif
                                            
                            } else {

                                // TRANSITION
                                if ((depth > depthend) && (depth < depthmax [2])) {
                                            
                                    vec4  coord1 = matrix [2] * vec4 (worldpos.xyz, 1.0);   coord1 = coord1 * 0.5 + 0.5;
                                                
                                    s1   = shadow2D (tex_shadow [2], coord1.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;
                                    s1  += shadow2D (tex_shadow [2], coord1.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;
                                    s1  += shadow2D (tex_shadow [2], coord1.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;
                                    s1  += shadow2D (tex_shadow [2], coord1.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;
                                    shadow = (mix (s1, 4.0, (depth - depthend) / (depthmax [2] - depthend)) * 0.25);
                                                
                                    #ifdef DEBUG
                                        gl_FragColor = vec4 (0.0, 0.0, 0.0, 1.0); return;
                                    #endif
                                                
                                } else {
                                            
                                    shadow = 1.0;

                                    #ifdef DEBUG
                                        gl_FragColor = vec4 (0.2, 0.2, 0.2, 1.0); return;
                                    #endif
                                }
                            }
                        }
                    }
                }
            }

        #endif

        #if SPLITS == 4

            if ((depth > depthmin [0]) && (depth < depthmin [1])) {

                vec4  coord = matrix [0] * vec4 (worldpos.xyz, 1.0);   coord = coord * 0.5 + 0.5;
            
                s    = shadow2D (tex_shadow [0], coord.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;
                s   += shadow2D (tex_shadow [0], coord.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;
                s   += shadow2D (tex_shadow [0], coord.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;
                s   += shadow2D (tex_shadow [0], coord.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;
                shadow = s * 0.25;
            
                #ifdef DEBUG
                    gl_FragColor = vec4 (1.0, 0.0, 0.0, 1.0); return;
                #endif
        
            } else {

                // TRANSITION
                if ((depth > depthmin [1]) && (depth < depthmax [0])) {
            
                    vec4  coord1 = matrix [0] * vec4 (worldpos.xyz, 1.0);   coord1 = coord1 * 0.5 + 0.5;
                    vec4  coord2 = matrix [1] * vec4 (worldpos.xyz, 1.0);   coord2 = coord2 * 0.5 + 0.5;
                
                    s1   = shadow2D (tex_shadow [0], coord1.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;
                    s1  += shadow2D (tex_shadow [0], coord1.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;
                    s1  += shadow2D (tex_shadow [0], coord1.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;
                    s1  += shadow2D (tex_shadow [0], coord1.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;
                    s2   = shadow2D (tex_shadow [1], coord2.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;
                    s2  += shadow2D (tex_shadow [1], coord2.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;
                    s2  += shadow2D (tex_shadow [1], coord2.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;
                    s2  += shadow2D (tex_shadow [1], coord2.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;
                    shadow = (mix (s1, s2, (depth - depthmin [1]) / (depthmax [0] - depthmin [1])) * 0.25);

                    #ifdef DEBUG
                        gl_FragColor = vec4 (0.0, 0.0, 0.0, 1.0); return;
                    #endif
            
                } else {

                    if ((depth > depthmax [0]) && (depth < depthmin [2])) {

                        vec4  coord = matrix [1] * vec4 (worldpos.xyz, 1.0);   coord = coord * 0.5 + 0.5;
                    
                        s    = shadow2D (tex_shadow [1], coord.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;
                        s   += shadow2D (tex_shadow [1], coord.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;
                        s   += shadow2D (tex_shadow [1], coord.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;
                        s   += shadow2D (tex_shadow [1], coord.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;
                        shadow = s * 0.25;
                    
                        #ifdef DEBUG
                            gl_FragColor = vec4 (0.0, 1.0, 0.0, 1.0); return;
                        #endif
                
                    } else {
        
                        // TRANSITION
                        if ((depth > depthmin [2]) && (depth < depthmax [1])) {
                    
                            vec4  coord1 = matrix [1] * vec4 (worldpos.xyz, 1.0);   coord1 = coord1 * 0.5 + 0.5;
                            vec4  coord2 = matrix [2] * vec4 (worldpos.xyz, 1.0);   coord2 = coord2 * 0.5 + 0.5;
                        
                            s1   = shadow2D (tex_shadow [1], coord1.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;
                            s1  += shadow2D (tex_shadow [1], coord1.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;
                            s1  += shadow2D (tex_shadow [1], coord1.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;
                            s1  += shadow2D (tex_shadow [1], coord1.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;
                            s2   = shadow2D (tex_shadow [2], coord2.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;
                            s2  += shadow2D (tex_shadow [2], coord2.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;
                            s2  += shadow2D (tex_shadow [2], coord2.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;
                            s2  += shadow2D (tex_shadow [2], coord2.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;
                            shadow = (mix (s1, s2, (depth - depthmin [2]) / (depthmax [1] - depthmin [2])) * 0.25);
                        
                            #ifdef DEBUG
                                gl_FragColor = vec4 (0.0, 0.0, 0.0, 1.0); return;                                
                            #endif

                        } else {
                    
                            if ((depth > depthmax [1]) && (depth < depthmin [3])) {
                        
                                vec4  coord = matrix [2] * vec4 (worldpos.xyz, 1.0);   coord = coord * 0.5 + 0.5;

                                s    = shadow2D (tex_shadow [2], coord.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;
                                s   += shadow2D (tex_shadow [2], coord.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;
                                s   += shadow2D (tex_shadow [2], coord.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;
                                s   += shadow2D (tex_shadow [2], coord.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;
                                shadow = s * 0.25;
                            
                                #ifdef DEBUG
                                    gl_FragColor = vec4 (0.0, 0.0, 1.0, 1.0); return;
                                #endif
                                            
                            } else {

                                // TRANSITION
                                if ((depth > depthmin [3]) && (depth < depthmax [2])) {
                            
                                    vec4  coord1 = matrix [2] * vec4 (worldpos.xyz, 1.0);   coord1 = coord1 * 0.5 + 0.5;
                                    vec4  coord2 = matrix [3] * vec4 (worldpos.xyz, 1.0);   coord2 = coord2 * 0.5 + 0.5;
                                
                                    s1   = shadow2D (tex_shadow [2], coord1.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;
                                    s1  += shadow2D (tex_shadow [2], coord1.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;
                                    s1  += shadow2D (tex_shadow [2], coord1.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;
                                    s1  += shadow2D (tex_shadow [2], coord1.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;
                                    s2   = shadow2D (tex_shadow [3], coord2.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;
                                    s2  += shadow2D (tex_shadow [3], coord2.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;
                                    s2  += shadow2D (tex_shadow [3], coord2.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;
                                    s2  += shadow2D (tex_shadow [3], coord2.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;
                                    shadow = (mix (s1, s2, (depth - depthmin [3]) / (depthmax [2] - depthmin [3])) * 0.25);
                                
                                    #ifdef DEBUG
                                        gl_FragColor = vec4 (0.0, 0.0, 0.0, 1.0); return;
                                    #endif
                                        
                                } else {
                                                    
                                    if ((depth > depthmax [2]) && (depth < depthend)) {
                                        
                                        vec4  coord = matrix [3] * vec4 (worldpos.xyz, 1.0);   coord = coord * 0.5 + 0.5;
                                            
                                        s    = shadow2D (tex_shadow [3], coord.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;
                                        s   += shadow2D (tex_shadow [3], coord.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;
                                        s   += shadow2D (tex_shadow [3], coord.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;
                                        s   += shadow2D (tex_shadow [3], coord.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;
                                        shadow = s * 0.25;
                                            
                                        #ifdef DEBUG
                                            gl_FragColor = vec4 (1.0, 0.0, 0.0, 1.0); return;
                                        #endif
                                            
                                    } else {

                                        // TRANSITION
                                        if ((depth > depthend) && (depth < depthmax [3])) {
                                            
                                            vec4  coord1 = matrix [3] * vec4 (worldpos.xyz, 1.0);   coord1 = coord1 * 0.5 + 0.5;
                                                
                                            s1   = shadow2D (tex_shadow [3], coord1.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;
                                            s1  += shadow2D (tex_shadow [3], coord1.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;
                                            s1  += shadow2D (tex_shadow [3], coord1.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;
                                            s1  += shadow2D (tex_shadow [3], coord1.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;
                                            shadow = (mix (s1, 4.0, (depth - depthend) / (depthmax [3] - depthend)) * 0.25);
                                                
                                            #ifdef DEBUG
                                                gl_FragColor = vec4 (0.0, 0.0, 0.0, 1.0); return;
                                            #endif
                                                
                                        } else {
                                            
                                            shadow = 1.0;

                                            #ifdef DEBUG
                                                gl_FragColor = vec4 (0.2, 0.2, 0.2, 1.0); return;
                                            #endif
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }

        #endif

        #if SPLITS == 5

            if ((depth > depthmin [0]) && (depth < depthmin [1])) {

                vec4  coord = matrix [0] * vec4 (worldpos.xyz, 1.0);   coord = coord * 0.5 + 0.5;
            
                s    = shadow2D (tex_shadow [0], coord.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;
                s   += shadow2D (tex_shadow [0], coord.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;
                s   += shadow2D (tex_shadow [0], coord.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;
                s   += shadow2D (tex_shadow [0], coord.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;
                shadow = s * 0.25;
            
                #ifdef DEBUG
                    gl_FragColor = vec4 (1.0, 0.0, 0.0, 1.0); return;
                #endif
        
            } else {

                // TRANSITION
                if ((depth > depthmin [1]) && (depth < depthmax [0])) {
            
                    vec4  coord1 = matrix [0] * vec4 (worldpos.xyz, 1.0);   coord1 = coord1 * 0.5 + 0.5;
                    vec4  coord2 = matrix [1] * vec4 (worldpos.xyz, 1.0);   coord2 = coord2 * 0.5 + 0.5;
                
                    s1   = shadow2D (tex_shadow [0], coord1.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;
                    s1  += shadow2D (tex_shadow [0], coord1.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;
                    s1  += shadow2D (tex_shadow [0], coord1.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;
                    s1  += shadow2D (tex_shadow [0], coord1.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;
                    s2   = shadow2D (tex_shadow [1], coord2.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;
                    s2  += shadow2D (tex_shadow [1], coord2.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;
                    s2  += shadow2D (tex_shadow [1], coord2.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;
                    s2  += shadow2D (tex_shadow [1], coord2.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;
                    shadow = (mix (s1, s2, (depth - depthmin [1]) / (depthmax [0] - depthmin [1])) * 0.25);

                    #ifdef DEBUG
                        gl_FragColor = vec4 (0.0, 0.0, 0.0, 1.0); return;
                    #endif
            
                } else {

                    if ((depth > depthmax [0]) && (depth < depthmin [2])) {

                        vec4  coord = matrix [1] * vec4 (worldpos.xyz, 1.0);   coord = coord * 0.5 + 0.5;
                    
                        s    = shadow2D (tex_shadow [1], coord.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;
                        s   += shadow2D (tex_shadow [1], coord.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;
                        s   += shadow2D (tex_shadow [1], coord.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;
                        s   += shadow2D (tex_shadow [1], coord.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;
                        shadow = s * 0.25;
                    
                        #ifdef DEBUG
                            gl_FragColor = vec4 (0.0, 1.0, 0.0, 1.0); return;
                        #endif
                
                    } else {
        
                        // TRANSITION
                        if ((depth > depthmin [2]) && (depth < depthmax [1])) {
                    
                            vec4  coord1 = matrix [1] * vec4 (worldpos.xyz, 1.0);   coord1 = coord1 * 0.5 + 0.5;
                            vec4  coord2 = matrix [2] * vec4 (worldpos.xyz, 1.0);   coord2 = coord2 * 0.5 + 0.5;
                        
                            s1   = shadow2D (tex_shadow [1], coord1.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;
                            s1  += shadow2D (tex_shadow [1], coord1.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;
                            s1  += shadow2D (tex_shadow [1], coord1.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;
                            s1  += shadow2D (tex_shadow [1], coord1.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;
                            s2   = shadow2D (tex_shadow [2], coord2.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;
                            s2  += shadow2D (tex_shadow [2], coord2.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;
                            s2  += shadow2D (tex_shadow [2], coord2.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;
                            s2  += shadow2D (tex_shadow [2], coord2.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;
                            shadow = (mix (s1, s2, (depth - depthmin [2]) / (depthmax [1] - depthmin [2])) * 0.25);
                        
                            #ifdef DEBUG
                                gl_FragColor = vec4 (0.0, 0.0, 0.0, 1.0); return;
                            #endif
                                
                        } else {
                    
                            if ((depth > depthmax [1]) && (depth < depthmin [3])) {
                        
                                vec4  coord = matrix [2] * vec4 (worldpos.xyz, 1.0);   coord = coord * 0.5 + 0.5;

                                s    = shadow2D (tex_shadow [2], coord.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;
                                s   += shadow2D (tex_shadow [2], coord.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;
                                s   += shadow2D (tex_shadow [2], coord.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;
                                s   += shadow2D (tex_shadow [2], coord.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;
                                shadow = s * 0.25;
                            
                                #ifdef DEBUG
                                    gl_FragColor = vec4 (0.0, 0.0, 1.0, 1.0); return;
                                #endif
                                            
                            } else {

                                // TRANSITION
                                if ((depth > depthmin [3]) && (depth < depthmax [2])) {
                            
                                    vec4  coord1 = matrix [2] * vec4 (worldpos.xyz, 1.0);   coord1 = coord1 * 0.5 + 0.5;
                                    vec4  coord2 = matrix [3] * vec4 (worldpos.xyz, 1.0);   coord2 = coord2 * 0.5 + 0.5;
                                
                                    s1   = shadow2D (tex_shadow [2], coord1.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;
                                    s1  += shadow2D (tex_shadow [2], coord1.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;
                                    s1  += shadow2D (tex_shadow [2], coord1.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;
                                    s1  += shadow2D (tex_shadow [2], coord1.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;
                                    s2   = shadow2D (tex_shadow [3], coord2.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;
                                    s2  += shadow2D (tex_shadow [3], coord2.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;
                                    s2  += shadow2D (tex_shadow [3], coord2.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;
                                    s2  += shadow2D (tex_shadow [3], coord2.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;
                                    shadow = (mix (s1, s2, (depth - depthmin [3]) / (depthmax [2] - depthmin [3])) * 0.25);
                                
                                    #ifdef DEBUG
                                        gl_FragColor = vec4 (0.0, 0.0, 0.0, 1.0); return;
                                    #endif
                                        
                                } else {
                    
                                    if ((depth > depthmax [2]) && (depth < depthmin [4])) {
                                
                                        vec4  coord = matrix [3] * vec4 (worldpos.xyz, 1.0);   coord = coord * 0.5 + 0.5;
                                    
                                        s    = shadow2D (tex_shadow [3], coord.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;
                                        s   += shadow2D (tex_shadow [3], coord.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;
                                        s   += shadow2D (tex_shadow [3], coord.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;
                                        s   += shadow2D (tex_shadow [3], coord.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;
                                        shadow = s * 0.25;
                                    
                                        #ifdef DEBUG
                                            gl_FragColor = vec4 (1.0, 0.0, 0.0, 1.0); return;
                                        #endif
                                                    
                                    } else {

                                        // TRANSITION
                                        if ((depth > depthmin [4]) && (depth < depthmax [3])) {
                                    
                                            vec4  coord1 = matrix [3] * vec4 (worldpos.xyz, 1.0);   coord1 = coord1 * 0.5 + 0.5;
                                            vec4  coord2 = matrix [4] * vec4 (worldpos.xyz, 1.0);   coord2 = coord2 * 0.5 + 0.5;
                                        
                                            s1   = shadow2D (tex_shadow [3], coord1.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;
                                            s1  += shadow2D (tex_shadow [3], coord1.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;
                                            s1  += shadow2D (tex_shadow [3], coord1.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;
                                            s1  += shadow2D (tex_shadow [3], coord1.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;
                                            s2   = shadow2D (tex_shadow [4], coord2.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;
                                            s2  += shadow2D (tex_shadow [4], coord2.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;
                                            s2  += shadow2D (tex_shadow [4], coord2.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;
                                            s2  += shadow2D (tex_shadow [4], coord2.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;
                                            shadow = (mix (s1, s2, (depth - depthmin [4]) / (depthmax [3] - depthmin [4])) * 0.25);
                                        
                                            #ifdef DEBUG
                                                gl_FragColor = vec4 (0.0, 0.0, 0.0, 1.0); return;
                                            #endif
                                                
                                        } else {
                                
                                            if ((depth > depthmax [3]) && (depth < depthend)) {
                                        
                                                vec4  coord = matrix [4] * vec4 (worldpos.xyz, 1.0);   coord = coord * 0.5 + 0.5;
                                            
                                                s    = shadow2D (tex_shadow [4], coord.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;
                                                s   += shadow2D (tex_shadow [4], coord.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;
                                                s   += shadow2D (tex_shadow [4], coord.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;
                                                s   += shadow2D (tex_shadow [4], coord.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;
                                                shadow = s * 0.25;
                                            
                                                #ifdef DEBUG
                                                    gl_FragColor = vec4 (0.0, 1.0, 0.0, 1.0); return;
                                                #endif
                                            
                                            } else {

                                                // TRANSITION
                                                if ((depth > depthend) && (depth < depthmax [4])) {
                                            
                                                    vec4  coord1 = matrix [4] * vec4 (worldpos.xyz, 1.0);   coord1 = coord1 * 0.5 + 0.5;
                                                
                                                    s1   = shadow2D (tex_shadow [4], coord1.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;
                                                    s1  += shadow2D (tex_shadow [4], coord1.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;
                                                    s1  += shadow2D (tex_shadow [4], coord1.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;
                                                    s1  += shadow2D (tex_shadow [4], coord1.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;
                                                    shadow = (mix (s1, 4.0, (depth - depthend) / (depthmax [4] - depthend)) * 0.25);
                                                
                                                    #ifdef DEBUG
                                                        gl_FragColor = vec4 (0.0, 0.0, 0.0, 1.0); return;
                                                    #endif
                                                
                                                } else {
                                            
                                                    shadow = 1.0;

                                                    #ifdef DEBUG
                                                        gl_FragColor = vec4 (0.2, 0.2, 0.2, 1.0); return;
                                                    #endif
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }

        #endif

        #if SPLITS == 6

            if ((depth > depthmin [0]) && (depth < depthmin [1])) {

                vec4  coord = matrix [0] * vec4 (worldpos.xyz, 1.0);   coord = coord * 0.5 + 0.5;
            
                s    = shadow2D (tex_shadow [0], coord.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;
                s   += shadow2D (tex_shadow [0], coord.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;
                s   += shadow2D (tex_shadow [0], coord.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;
                s   += shadow2D (tex_shadow [0], coord.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;
                shadow = s * 0.25;
            
                #ifdef DEBUG
                    gl_FragColor = vec4 (1.0, 0.0, 0.0, 1.0); return;
                #endif
        
            } else {

                // TRANSITION
                if ((depth > depthmin [1]) && (depth < depthmax [0])) {
            
                    vec4  coord1 = matrix [0] * vec4 (worldpos.xyz, 1.0);   coord1 = coord1 * 0.5 + 0.5;
                    vec4  coord2 = matrix [1] * vec4 (worldpos.xyz, 1.0);   coord2 = coord2 * 0.5 + 0.5;
                
                    s1   = shadow2D (tex_shadow [0], coord1.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;
                    s1  += shadow2D (tex_shadow [0], coord1.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;
                    s1  += shadow2D (tex_shadow [0], coord1.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;
                    s1  += shadow2D (tex_shadow [0], coord1.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;
                    s2   = shadow2D (tex_shadow [1], coord2.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;
                    s2  += shadow2D (tex_shadow [1], coord2.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;
                    s2  += shadow2D (tex_shadow [1], coord2.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;
                    s2  += shadow2D (tex_shadow [1], coord2.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;
                    shadow = (mix (s1, s2, (depth - depthmin [1]) / (depthmax [0] - depthmin [1])) * 0.25);

                    #ifdef DEBUG
                        gl_FragColor = vec4 (0.0, 0.0, 0.0, 1.0); return;
                    #endif
            
                } else {

                    if ((depth > depthmax [0]) && (depth < depthmin [2])) {

                        vec4  coord = matrix [1] * vec4 (worldpos.xyz, 1.0);   coord = coord * 0.5 + 0.5;
                    
                        s    = shadow2D (tex_shadow [1], coord.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;
                        s   += shadow2D (tex_shadow [1], coord.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;
                        s   += shadow2D (tex_shadow [1], coord.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;
                        s   += shadow2D (tex_shadow [1], coord.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;
                        shadow = s * 0.25;
                    
                        #ifdef DEBUG
                            gl_FragColor = vec4 (0.0, 1.0, 0.0, 1.0); return;
                        #endif
                
                    } else {
        
                        // TRANSITION
                        if ((depth > depthmin [2]) && (depth < depthmax [1])) {
                    
                            vec4  coord1 = matrix [1] * vec4 (worldpos.xyz, 1.0);   coord1 = coord1 * 0.5 + 0.5;
                            vec4  coord2 = matrix [2] * vec4 (worldpos.xyz, 1.0);   coord2 = coord2 * 0.5 + 0.5;
                        
                            s1   = shadow2D (tex_shadow [1], coord1.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;
                            s1  += shadow2D (tex_shadow [1], coord1.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;
                            s1  += shadow2D (tex_shadow [1], coord1.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;
                            s1  += shadow2D (tex_shadow [1], coord1.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;
                            s2   = shadow2D (tex_shadow [2], coord2.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;
                            s2  += shadow2D (tex_shadow [2], coord2.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;
                            s2  += shadow2D (tex_shadow [2], coord2.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;
                            s2  += shadow2D (tex_shadow [2], coord2.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;
                            shadow = (mix (s1, s2, (depth - depthmin [2]) / (depthmax [1] - depthmin [2])) * 0.25);
                        
                            #ifdef DEBUG
                                gl_FragColor = vec4 (0.0, 0.0, 0.0, 1.0); return;
                            #endif
                                
                        } else {
                    
                            if ((depth > depthmax [1]) && (depth < depthmin [3])) {
                        
                                vec4  coord = matrix [2] * vec4 (worldpos.xyz, 1.0);   coord = coord * 0.5 + 0.5;

                                s    = shadow2D (tex_shadow [2], coord.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;
                                s   += shadow2D (tex_shadow [2], coord.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;
                                s   += shadow2D (tex_shadow [2], coord.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;
                                s   += shadow2D (tex_shadow [2], coord.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;
                                shadow = s * 0.25;
                            
                                #ifdef DEBUG
                                    gl_FragColor = vec4 (0.0, 0.0, 1.0, 1.0); return;
                                #endif
                                            
                            } else {

                                // TRANSITION
                                if ((depth > depthmin [3]) && (depth < depthmax [2])) {
                            
                                    vec4  coord1 = matrix [2] * vec4 (worldpos.xyz, 1.0);   coord1 = coord1 * 0.5 + 0.5;
                                    vec4  coord2 = matrix [3] * vec4 (worldpos.xyz, 1.0);   coord2 = coord2 * 0.5 + 0.5;
                                
                                    s1   = shadow2D (tex_shadow [2], coord1.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;
                                    s1  += shadow2D (tex_shadow [2], coord1.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;
                                    s1  += shadow2D (tex_shadow [2], coord1.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;
                                    s1  += shadow2D (tex_shadow [2], coord1.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;
                                    s2   = shadow2D (tex_shadow [3], coord2.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;
                                    s2  += shadow2D (tex_shadow [3], coord2.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;
                                    s2  += shadow2D (tex_shadow [3], coord2.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;
                                    s2  += shadow2D (tex_shadow [3], coord2.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;
                                    shadow = (mix (s1, s2, (depth - depthmin [3]) / (depthmax [2] - depthmin [3])) * 0.25);
                                
                                    #ifdef DEBUG
                                        gl_FragColor = vec4 (0.0, 0.0, 0.0, 1.0); return;
                                    #endif
                                        
                                } else {
                    
                                    if ((depth > depthmax [2]) && (depth < depthmin [4])) {
                                
                                        vec4  coord = matrix [3] * vec4 (worldpos.xyz, 1.0);   coord = coord * 0.5 + 0.5;
                                    
                                        s    = shadow2D (tex_shadow [3], coord.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;
                                        s   += shadow2D (tex_shadow [3], coord.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;
                                        s   += shadow2D (tex_shadow [3], coord.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;
                                        s   += shadow2D (tex_shadow [3], coord.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;
                                        shadow = s * 0.25;
                                    
                                        #ifdef DEBUG
                                            gl_FragColor = vec4 (1.0, 0.0, 0.0, 1.0); return;
                                        #endif
                                                    
                                    } else {

                                        // TRANSITION
                                        if ((depth > depthmin [4]) && (depth < depthmax [3])) {
                                    
                                            vec4  coord1 = matrix [3] * vec4 (worldpos.xyz, 1.0);   coord1 = coord1 * 0.5 + 0.5;
                                            vec4  coord2 = matrix [4] * vec4 (worldpos.xyz, 1.0);   coord2 = coord2 * 0.5 + 0.5;
                                        
                                            s1   = shadow2D (tex_shadow [3], coord1.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;
                                            s1  += shadow2D (tex_shadow [3], coord1.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;
                                            s1  += shadow2D (tex_shadow [3], coord1.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;
                                            s1  += shadow2D (tex_shadow [3], coord1.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;
                                            s2   = shadow2D (tex_shadow [4], coord2.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;
                                            s2  += shadow2D (tex_shadow [4], coord2.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;
                                            s2  += shadow2D (tex_shadow [4], coord2.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;
                                            s2  += shadow2D (tex_shadow [4], coord2.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;
                                            shadow = (mix (s1, s2, (depth - depthmin [4]) / (depthmax [3] - depthmin [4])) * 0.25);
                                        
                                            #ifdef DEBUG
                                                gl_FragColor = vec4 (0.0, 0.0, 0.0, 1.0); return;
                                            #endif
                                                
                                        } else {

                                            if ((depth > depthmax [3]) && (depth < depthmin [5])) {
                                
                                                vec4  coord = matrix [4] * vec4 (worldpos.xyz, 1.0);   coord = coord * 0.5 + 0.5;
                                    
                                                s    = shadow2D (tex_shadow [4], coord.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;
                                                s   += shadow2D (tex_shadow [4], coord.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;
                                                s   += shadow2D (tex_shadow [4], coord.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;
                                                s   += shadow2D (tex_shadow [4], coord.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;
                                                shadow = s * 0.25;
                                    
                                                #ifdef DEBUG
                                                    gl_FragColor = vec4 (0.0, 1.0, 0.0, 1.0); return;
                                                #endif
                                                    
                                            } else {

                                                // TRANSITION
                                                if ((depth > depthmin [5]) && (depth < depthmax [4])) {
                                    
                                                    vec4  coord1 = matrix [4] * vec4 (worldpos.xyz, 1.0);   coord1 = coord1 * 0.5 + 0.5;
                                                    vec4  coord2 = matrix [5] * vec4 (worldpos.xyz, 1.0);   coord2 = coord2 * 0.5 + 0.5;
                                        
                                                    s1   = shadow2D (tex_shadow [4], coord1.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;
                                                    s1  += shadow2D (tex_shadow [4], coord1.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;
                                                    s1  += shadow2D (tex_shadow [4], coord1.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;
                                                    s1  += shadow2D (tex_shadow [4], coord1.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;
                                                    s2   = shadow2D (tex_shadow [5], coord2.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;
                                                    s2  += shadow2D (tex_shadow [5], coord2.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;
                                                    s2  += shadow2D (tex_shadow [5], coord2.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;
                                                    s2  += shadow2D (tex_shadow [5], coord2.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;
                                                    shadow = (mix (s1, s2, (depth - depthmin [5]) / (depthmax [4] - depthmin [5])) * 0.25);
                                        
                                                    #ifdef DEBUG
                                                        gl_FragColor = vec4 (0.0, 0.0, 0.0, 1.0); return;
                                                    #endif
                                                
                                                } else {
                                
                                                    if ((depth > depthmax [4]) && (depth < depthend)) {
                                        
                                                        vec4  coord = matrix [5] * vec4 (worldpos.xyz, 1.0);   coord = coord * 0.5 + 0.5;
                                            
                                                        s    = shadow2D (tex_shadow [5], coord.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;
                                                        s   += shadow2D (tex_shadow [5], coord.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;
                                                        s   += shadow2D (tex_shadow [5], coord.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;
                                                        s   += shadow2D (tex_shadow [5], coord.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;
                                                        shadow = s * 0.25;
                                            
                                                        #ifdef DEBUG
                                                            gl_FragColor = vec4 (0.0, 0.0, 1.0, 1.0); return;
                                                        #endif
                                            
                                                    } else {

                                                        // TRANSITION
                                                        if ((depth > depthend) && (depth < depthmax [5])) {
                                            
                                                            vec4  coord1 = matrix [5] * vec4 (worldpos.xyz, 1.0);   coord1 = coord1 * 0.5 + 0.5;
                                                
                                                            s1   = shadow2D (tex_shadow [5], coord1.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;
                                                            s1  += shadow2D (tex_shadow [5], coord1.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;
                                                            s1  += shadow2D (tex_shadow [5], coord1.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;
                                                            s1  += shadow2D (tex_shadow [5], coord1.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;
                                                            shadow = (mix (s1, 4.0, (depth - depthend) / (depthmax [5] - depthend)) * 0.25);
                                                
                                                            #ifdef DEBUG
                                                                gl_FragColor = vec4 (0.0, 0.0, 0.0, 1.0); return;
                                                            #endif
                                                
                                                        } else {
                                            
                                                            shadow = 1.0;

                                                            #ifdef DEBUG
                                                                gl_FragColor = vec4 (0.2, 0.2, 0.2, 1.0); return;
                                                            #endif
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }

        #endif

        #if SPLITS == 7

            if ((depth > depthmin [0]) && (depth < depthmin [1])) {

                vec4  coord = matrix [0] * vec4 (worldpos.xyz, 1.0);   coord = coord * 0.5 + 0.5;
            
                s    = shadow2D (tex_shadow [0], coord.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;
                s   += shadow2D (tex_shadow [0], coord.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;
                s   += shadow2D (tex_shadow [0], coord.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;
                s   += shadow2D (tex_shadow [0], coord.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;
                shadow = s * 0.25;
            
                #ifdef DEBUG
                    gl_FragColor = vec4 (1.0, 0.0, 0.0, 1.0); return;
                #endif
        
            } else {

                // TRANSITION
                if ((depth > depthmin [1]) && (depth < depthmax [0])) {
            
                    vec4  coord1 = matrix [0] * vec4 (worldpos.xyz, 1.0);   coord1 = coord1 * 0.5 + 0.5;
                    vec4  coord2 = matrix [1] * vec4 (worldpos.xyz, 1.0);   coord2 = coord2 * 0.5 + 0.5;
                
                    s1   = shadow2D (tex_shadow [0], coord1.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;
                    s1  += shadow2D (tex_shadow [0], coord1.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;
                    s1  += shadow2D (tex_shadow [0], coord1.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;
                    s1  += shadow2D (tex_shadow [0], coord1.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;
                    s2   = shadow2D (tex_shadow [1], coord2.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;
                    s2  += shadow2D (tex_shadow [1], coord2.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;
                    s2  += shadow2D (tex_shadow [1], coord2.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;
                    s2  += shadow2D (tex_shadow [1], coord2.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;
                    shadow = (mix (s1, s2, (depth - depthmin [1]) / (depthmax [0] - depthmin [1])) * 0.25);

                    #ifdef DEBUG
                        gl_FragColor = vec4 (0.0, 0.0, 0.0, 1.0); return;
                    #endif
            
                } else {

                    if ((depth > depthmax [0]) && (depth < depthmin [2])) {

                        vec4  coord = matrix [1] * vec4 (worldpos.xyz, 1.0);   coord = coord * 0.5 + 0.5;
                    
                        s    = shadow2D (tex_shadow [1], coord.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;
                        s   += shadow2D (tex_shadow [1], coord.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;
                        s   += shadow2D (tex_shadow [1], coord.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;
                        s   += shadow2D (tex_shadow [1], coord.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;
                        shadow = s * 0.25;
                    
                        #ifdef DEBUG
                            gl_FragColor = vec4 (0.0, 1.0, 0.0, 1.0); return;
                        #endif
                
                    } else {
        
                        // TRANSITION
                        if ((depth > depthmin [2]) && (depth < depthmax [1])) {
                    
                            vec4  coord1 = matrix [1] * vec4 (worldpos.xyz, 1.0);   coord1 = coord1 * 0.5 + 0.5;
                            vec4  coord2 = matrix [2] * vec4 (worldpos.xyz, 1.0);   coord2 = coord2 * 0.5 + 0.5;
                        
                            s1   = shadow2D (tex_shadow [1], coord1.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;
                            s1  += shadow2D (tex_shadow [1], coord1.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;
                            s1  += shadow2D (tex_shadow [1], coord1.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;
                            s1  += shadow2D (tex_shadow [1], coord1.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;
                            s2   = shadow2D (tex_shadow [2], coord2.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;
                            s2  += shadow2D (tex_shadow [2], coord2.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;
                            s2  += shadow2D (tex_shadow [2], coord2.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;
                            s2  += shadow2D (tex_shadow [2], coord2.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;
                            shadow = (mix (s1, s2, (depth - depthmin [2]) / (depthmax [1] - depthmin [2])) * 0.25);
                        
                            #ifdef DEBUG
                                gl_FragColor = vec4 (0.0, 0.0, 0.0, 1.0); return;
                            #endif
                                
                        } else {
                    
                            if ((depth > depthmax [1]) && (depth < depthmin [3])) {
                        
                                vec4  coord = matrix [2] * vec4 (worldpos.xyz, 1.0);   coord = coord * 0.5 + 0.5;

                                s    = shadow2D (tex_shadow [2], coord.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;
                                s   += shadow2D (tex_shadow [2], coord.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;
                                s   += shadow2D (tex_shadow [2], coord.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;
                                s   += shadow2D (tex_shadow [2], coord.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;
                                shadow = s * 0.25;
                            
                                #ifdef DEBUG
                                    gl_FragColor = vec4 (0.0, 0.0, 1.0, 1.0); return;
                                #endif
                                            
                            } else {

                                // TRANSITION
                                if ((depth > depthmin [3]) && (depth < depthmax [2])) {
                            
                                    vec4  coord1 = matrix [2] * vec4 (worldpos.xyz, 1.0);   coord1 = coord1 * 0.5 + 0.5;
                                    vec4  coord2 = matrix [3] * vec4 (worldpos.xyz, 1.0);   coord2 = coord2 * 0.5 + 0.5;
                                
                                    s1   = shadow2D (tex_shadow [2], coord1.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;
                                    s1  += shadow2D (tex_shadow [2], coord1.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;
                                    s1  += shadow2D (tex_shadow [2], coord1.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;
                                    s1  += shadow2D (tex_shadow [2], coord1.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;
                                    s2   = shadow2D (tex_shadow [3], coord2.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;
                                    s2  += shadow2D (tex_shadow [3], coord2.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;
                                    s2  += shadow2D (tex_shadow [3], coord2.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;
                                    s2  += shadow2D (tex_shadow [3], coord2.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;
                                    shadow = (mix (s1, s2, (depth - depthmin [3]) / (depthmax [2] - depthmin [3])) * 0.25);
                                
                                    #ifdef DEBUG
                                        gl_FragColor = vec4 (0.0, 0.0, 0.0, 1.0); return;
                                    #endif
                                        
                                } else {
                    
                                    if ((depth > depthmax [2]) && (depth < depthmin [4])) {
                                
                                        vec4  coord = matrix [3] * vec4 (worldpos.xyz, 1.0);   coord = coord * 0.5 + 0.5;
                                    
                                        s    = shadow2D (tex_shadow [3], coord.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;
                                        s   += shadow2D (tex_shadow [3], coord.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;
                                        s   += shadow2D (tex_shadow [3], coord.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;
                                        s   += shadow2D (tex_shadow [3], coord.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;
                                        shadow = s * 0.25;
                                    
                                        #ifdef DEBUG
                                            gl_FragColor = vec4 (1.0, 0.0, 0.0, 1.0); return;
                                        #endif
                                                    
                                    } else {

                                        // TRANSITION
                                        if ((depth > depthmin [4]) && (depth < depthmax [3])) {
                                    
                                            vec4  coord1 = matrix [3] * vec4 (worldpos.xyz, 1.0);   coord1 = coord1 * 0.5 + 0.5;
                                            vec4  coord2 = matrix [4] * vec4 (worldpos.xyz, 1.0);   coord2 = coord2 * 0.5 + 0.5;
                                        
                                            s1   = shadow2D (tex_shadow [3], coord1.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;
                                            s1  += shadow2D (tex_shadow [3], coord1.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;
                                            s1  += shadow2D (tex_shadow [3], coord1.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;
                                            s1  += shadow2D (tex_shadow [3], coord1.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;
                                            s2   = shadow2D (tex_shadow [4], coord2.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;
                                            s2  += shadow2D (tex_shadow [4], coord2.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;
                                            s2  += shadow2D (tex_shadow [4], coord2.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;
                                            s2  += shadow2D (tex_shadow [4], coord2.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;
                                            shadow = (mix (s1, s2, (depth - depthmin [4]) / (depthmax [3] - depthmin [4])) * 0.25);
                                        
                                            #ifdef DEBUG
                                                gl_FragColor = vec4 (0.0, 0.0, 0.0, 1.0); return;
                                            #endif
                                                
                                        } else {

                                            if ((depth > depthmax [3]) && (depth < depthmin [5])) {
                                
                                                vec4  coord = matrix [4] * vec4 (worldpos.xyz, 1.0);   coord = coord * 0.5 + 0.5;
                                    
                                                s    = shadow2D (tex_shadow [4], coord.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;
                                                s   += shadow2D (tex_shadow [4], coord.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;
                                                s   += shadow2D (tex_shadow [4], coord.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;
                                                s   += shadow2D (tex_shadow [4], coord.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;
                                                shadow = s * 0.25;
                                    
                                                #ifdef DEBUG
                                                    gl_FragColor = vec4 (0.0, 1.0, 0.0, 1.0); return;
                                                #endif
                                                    
                                            } else {

                                                // TRANSITION
                                                if ((depth > depthmin [5]) && (depth < depthmax [4])) {
                                    
                                                    vec4  coord1 = matrix [4] * vec4 (worldpos.xyz, 1.0);   coord1 = coord1 * 0.5 + 0.5;
                                                    vec4  coord2 = matrix [5] * vec4 (worldpos.xyz, 1.0);   coord2 = coord2 * 0.5 + 0.5;
                                        
                                                    s1   = shadow2D (tex_shadow [4], coord1.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;
                                                    s1  += shadow2D (tex_shadow [4], coord1.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;
                                                    s1  += shadow2D (tex_shadow [4], coord1.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;
                                                    s1  += shadow2D (tex_shadow [4], coord1.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;
                                                    s2   = shadow2D (tex_shadow [5], coord2.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;
                                                    s2  += shadow2D (tex_shadow [5], coord2.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;
                                                    s2  += shadow2D (tex_shadow [5], coord2.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;
                                                    s2  += shadow2D (tex_shadow [5], coord2.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;
                                                    shadow = (mix (s1, s2, (depth - depthmin [5]) / (depthmax [4] - depthmin [5])) * 0.25);
                                        
                                                    #ifdef DEBUG
                                                        gl_FragColor = vec4 (0.0, 0.0, 0.0, 1.0); return;
                                                    #endif
                                                
                                                } else {

                                                    if ((depth > depthmax [4]) && (depth < depthmin [6])) {
                                
                                                        vec4  coord = matrix [5] * vec4 (worldpos.xyz, 1.0);   coord = coord * 0.5 + 0.5;
                                    
                                                        s    = shadow2D (tex_shadow [5], coord.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;
                                                        s   += shadow2D (tex_shadow [5], coord.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;
                                                        s   += shadow2D (tex_shadow [5], coord.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;
                                                        s   += shadow2D (tex_shadow [5], coord.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;
                                                        shadow = s * 0.25;
                                    
                                                        #ifdef DEBUG
                                                            gl_FragColor = vec4 (0.0, 0.0, 1.0, 1.0); return;
                                                        #endif
                                                    
                                                    } else {

                                                        // TRANSITION
                                                        if ((depth > depthmin [6]) && (depth < depthmax [5])) {
                                    
                                                            vec4  coord1 = matrix [5] * vec4 (worldpos.xyz, 1.0);   coord1 = coord1 * 0.5 + 0.5;
                                                            vec4  coord2 = matrix [6] * vec4 (worldpos.xyz, 1.0);   coord2 = coord2 * 0.5 + 0.5;
                                        
                                                            s1   = shadow2D (tex_shadow [5], coord1.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;
                                                            s1  += shadow2D (tex_shadow [5], coord1.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;
                                                            s1  += shadow2D (tex_shadow [5], coord1.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;
                                                            s1  += shadow2D (tex_shadow [5], coord1.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;
                                                            s2   = shadow2D (tex_shadow [6], coord2.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;
                                                            s2  += shadow2D (tex_shadow [6], coord2.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;
                                                            s2  += shadow2D (tex_shadow [6], coord2.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;
                                                            s2  += shadow2D (tex_shadow [6], coord2.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;
                                                            shadow = (mix (s1, s2, (depth - depthmin [6]) / (depthmax [5] - depthmin [6])) * 0.25);
                                        
                                                            #ifdef DEBUG
                                                                gl_FragColor = vec4 (0.0, 0.0, 0.0, 1.0); return;
                                                            #endif
                                                
                                                        } else {
                                
                                                            if ((depth > depthmax [5]) && (depth < depthend)) {
                                        
                                                                vec4  coord = matrix [6] * vec4 (worldpos.xyz, 1.0);   coord = coord * 0.5 + 0.5;
                                            
                                                                s    = shadow2D (tex_shadow [6], coord.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;
                                                                s   += shadow2D (tex_shadow [6], coord.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;
                                                                s   += shadow2D (tex_shadow [6], coord.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;
                                                                s   += shadow2D (tex_shadow [6], coord.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;
                                                                shadow = s * 0.25;
                                            
                                                                #ifdef DEBUG
                                                                    gl_FragColor = vec4 (1.0, 0.0, 0.0, 1.0); return;
                                                                #endif
                                            
                                                            } else {

                                                                // TRANSITION
                                                                if ((depth > depthend) && (depth < depthmax [6])) {
                                            
                                                                    vec4  coord1 = matrix [6] * vec4 (worldpos.xyz, 1.0);   coord1 = coord1 * 0.5 + 0.5;
                                                
                                                                    s1   = shadow2D (tex_shadow [6], coord1.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;
                                                                    s1  += shadow2D (tex_shadow [6], coord1.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;
                                                                    s1  += shadow2D (tex_shadow [6], coord1.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;
                                                                    s1  += shadow2D (tex_shadow [6], coord1.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;
                                                                    shadow = (mix (s1, 4.0, (depth - depthend) / (depthmax [6] - depthend)) * 0.25);
                                                
                                                                    #ifdef DEBUG
                                                                        gl_FragColor = vec4 (0.0, 0.0, 0.0, 1.0); return;
                                                                    #endif
                                                
                                                                } else {
                                            
                                                                    shadow = 1.0;

                                                                    #ifdef DEBUG
                                                                        gl_FragColor = vec4 (0.2, 0.2, 0.2, 1.0); return;
                                                                    #endif
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }

        #endif

        #if SPLITS == 8

            if ((depth > depthmin [0]) && (depth < depthmin [1])) {

                vec4  coord = matrix [0] * vec4 (worldpos.xyz, 1.0);   coord = coord * 0.5 + 0.5;
            
                s    = shadow2D (tex_shadow [0], coord.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;
                s   += shadow2D (tex_shadow [0], coord.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;
                s   += shadow2D (tex_shadow [0], coord.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;
                s   += shadow2D (tex_shadow [0], coord.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;
                shadow = s * 0.25;
            
                #ifdef DEBUG
                    gl_FragColor = vec4 (1.0, 0.0, 0.0, 1.0); return;
                #endif
        
            } else {

                // TRANSITION
                if ((depth > depthmin [1]) && (depth < depthmax [0])) {
            
                    vec4  coord1 = matrix [0] * vec4 (worldpos.xyz, 1.0);   coord1 = coord1 * 0.5 + 0.5;
                    vec4  coord2 = matrix [1] * vec4 (worldpos.xyz, 1.0);   coord2 = coord2 * 0.5 + 0.5;
                
                    s1   = shadow2D (tex_shadow [0], coord1.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;
                    s1  += shadow2D (tex_shadow [0], coord1.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;
                    s1  += shadow2D (tex_shadow [0], coord1.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;
                    s1  += shadow2D (tex_shadow [0], coord1.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;
                    s2   = shadow2D (tex_shadow [1], coord2.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;
                    s2  += shadow2D (tex_shadow [1], coord2.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;
                    s2  += shadow2D (tex_shadow [1], coord2.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;
                    s2  += shadow2D (tex_shadow [1], coord2.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;
                    shadow = (mix (s1, s2, (depth - depthmin [1]) / (depthmax [0] - depthmin [1])) * 0.25);

                    #ifdef DEBUG
                        gl_FragColor = vec4 (0.0, 0.0, 0.0, 1.0); return;
                    #endif
            
                } else {

                    if ((depth > depthmax [0]) && (depth < depthmin [2])) {

                        vec4  coord = matrix [1] * vec4 (worldpos.xyz, 1.0);   coord = coord * 0.5 + 0.5;
                    
                        s    = shadow2D (tex_shadow [1], coord.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;
                        s   += shadow2D (tex_shadow [1], coord.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;
                        s   += shadow2D (tex_shadow [1], coord.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;
                        s   += shadow2D (tex_shadow [1], coord.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;
                        shadow = s * 0.25;
                    
                        #ifdef DEBUG
                            gl_FragColor = vec4 (0.0, 1.0, 0.0, 1.0); return;
                        #endif
                
                    } else {
        
                        // TRANSITION
                        if ((depth > depthmin [2]) && (depth < depthmax [1])) {
                    
                            vec4  coord1 = matrix [1] * vec4 (worldpos.xyz, 1.0);   coord1 = coord1 * 0.5 + 0.5;
                            vec4  coord2 = matrix [2] * vec4 (worldpos.xyz, 1.0);   coord2 = coord2 * 0.5 + 0.5;
                        
                            s1   = shadow2D (tex_shadow [1], coord1.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;
                            s1  += shadow2D (tex_shadow [1], coord1.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;
                            s1  += shadow2D (tex_shadow [1], coord1.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;
                            s1  += shadow2D (tex_shadow [1], coord1.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;
                            s2   = shadow2D (tex_shadow [2], coord2.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;
                            s2  += shadow2D (tex_shadow [2], coord2.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;
                            s2  += shadow2D (tex_shadow [2], coord2.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;
                            s2  += shadow2D (tex_shadow [2], coord2.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;
                            shadow = (mix (s1, s2, (depth - depthmin [2]) / (depthmax [1] - depthmin [2])) * 0.25);
                        
                            #ifdef DEBUG
                                gl_FragColor = vec4 (0.0, 0.0, 0.0, 1.0); return;
                            #endif
                                
                        } else {
                    
                            if ((depth > depthmax [1]) && (depth < depthmin [3])) {
                        
                                vec4  coord = matrix [2] * vec4 (worldpos.xyz, 1.0);   coord = coord * 0.5 + 0.5;

                                s    = shadow2D (tex_shadow [2], coord.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;
                                s   += shadow2D (tex_shadow [2], coord.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;
                                s   += shadow2D (tex_shadow [2], coord.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;
                                s   += shadow2D (tex_shadow [2], coord.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;
                                shadow = s * 0.25;
                            
                                #ifdef DEBUG
                                    gl_FragColor = vec4 (0.0, 0.0, 1.0, 1.0); return;
                                #endif
                                            
                            } else {

                                // TRANSITION
                                if ((depth > depthmin [3]) && (depth < depthmax [2])) {
                            
                                    vec4  coord1 = matrix [2] * vec4 (worldpos.xyz, 1.0);   coord1 = coord1 * 0.5 + 0.5;
                                    vec4  coord2 = matrix [3] * vec4 (worldpos.xyz, 1.0);   coord2 = coord2 * 0.5 + 0.5;
                                
                                    s1   = shadow2D (tex_shadow [2], coord1.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;
                                    s1  += shadow2D (tex_shadow [2], coord1.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;
                                    s1  += shadow2D (tex_shadow [2], coord1.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;
                                    s1  += shadow2D (tex_shadow [2], coord1.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;
                                    s2   = shadow2D (tex_shadow [3], coord2.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;
                                    s2  += shadow2D (tex_shadow [3], coord2.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;
                                    s2  += shadow2D (tex_shadow [3], coord2.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;
                                    s2  += shadow2D (tex_shadow [3], coord2.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;
                                    shadow = (mix (s1, s2, (depth - depthmin [3]) / (depthmax [2] - depthmin [3])) * 0.25);
                                
                                    #ifdef DEBUG
                                        gl_FragColor = vec4 (0.0, 0.0, 0.0, 1.0); return;
                                    #endif
                                        
                                } else {
                    
                                    if ((depth > depthmax [2]) && (depth < depthmin [4])) {
                                
                                        vec4  coord = matrix [3] * vec4 (worldpos.xyz, 1.0);   coord = coord * 0.5 + 0.5;
                                    
                                        s    = shadow2D (tex_shadow [3], coord.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;
                                        s   += shadow2D (tex_shadow [3], coord.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;
                                        s   += shadow2D (tex_shadow [3], coord.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;
                                        s   += shadow2D (tex_shadow [3], coord.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;
                                        shadow = s * 0.25;
                                    
                                        #ifdef DEBUG
                                            gl_FragColor = vec4 (1.0, 0.0, 0.0, 1.0); return;
                                        #endif
                                                    
                                    } else {

                                        // TRANSITION
                                        if ((depth > depthmin [4]) && (depth < depthmax [3])) {
                                    
                                            vec4  coord1 = matrix [3] * vec4 (worldpos.xyz, 1.0);   coord1 = coord1 * 0.5 + 0.5;
                                            vec4  coord2 = matrix [4] * vec4 (worldpos.xyz, 1.0);   coord2 = coord2 * 0.5 + 0.5;
                                        
                                            s1   = shadow2D (tex_shadow [3], coord1.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;
                                            s1  += shadow2D (tex_shadow [3], coord1.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;
                                            s1  += shadow2D (tex_shadow [3], coord1.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;
                                            s1  += shadow2D (tex_shadow [3], coord1.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;
                                            s2   = shadow2D (tex_shadow [4], coord2.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;
                                            s2  += shadow2D (tex_shadow [4], coord2.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;
                                            s2  += shadow2D (tex_shadow [4], coord2.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;
                                            s2  += shadow2D (tex_shadow [4], coord2.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;
                                            shadow = (mix (s1, s2, (depth - depthmin [4]) / (depthmax [3] - depthmin [4])) * 0.25);
                                        
                                            #ifdef DEBUG
                                                gl_FragColor = vec4 (0.0, 0.0, 0.0, 1.0); return;
                                            #endif
                                                
                                        } else {

                                            if ((depth > depthmax [3]) && (depth < depthmin [5])) {
                                
                                                vec4  coord = matrix [4] * vec4 (worldpos.xyz, 1.0);   coord = coord * 0.5 + 0.5;
                                    
                                                s    = shadow2D (tex_shadow [4], coord.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;
                                                s   += shadow2D (tex_shadow [4], coord.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;
                                                s   += shadow2D (tex_shadow [4], coord.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;
                                                s   += shadow2D (tex_shadow [4], coord.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;
                                                shadow = s * 0.25;
                                    
                                                #ifdef DEBUG
                                                    gl_FragColor = vec4 (0.0, 1.0, 0.0, 1.0); return;
                                                #endif
                                                    
                                            } else {

                                                // TRANSITION
                                                if ((depth > depthmin [5]) && (depth < depthmax [4])) {
                                    
                                                    vec4  coord1 = matrix [4] * vec4 (worldpos.xyz, 1.0);   coord1 = coord1 * 0.5 + 0.5;
                                                    vec4  coord2 = matrix [5] * vec4 (worldpos.xyz, 1.0);   coord2 = coord2 * 0.5 + 0.5;
                                        
                                                    s1   = shadow2D (tex_shadow [4], coord1.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;
                                                    s1  += shadow2D (tex_shadow [4], coord1.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;
                                                    s1  += shadow2D (tex_shadow [4], coord1.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;
                                                    s1  += shadow2D (tex_shadow [4], coord1.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;
                                                    s2   = shadow2D (tex_shadow [5], coord2.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;
                                                    s2  += shadow2D (tex_shadow [5], coord2.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;
                                                    s2  += shadow2D (tex_shadow [5], coord2.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;
                                                    s2  += shadow2D (tex_shadow [5], coord2.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;
                                                    shadow = (mix (s1, s2, (depth - depthmin [5]) / (depthmax [4] - depthmin [5])) * 0.25);
                                        
                                                    #ifdef DEBUG
                                                        gl_FragColor = vec4 (0.0, 0.0, 0.0, 1.0); return;
                                                    #endif
                                                
                                                } else {

                                                    if ((depth > depthmax [4]) && (depth < depthmin [6])) {
                                
                                                        vec4  coord = matrix [5] * vec4 (worldpos.xyz, 1.0);   coord = coord * 0.5 + 0.5;
                                    
                                                        s    = shadow2D (tex_shadow [5], coord.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;
                                                        s   += shadow2D (tex_shadow [5], coord.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;
                                                        s   += shadow2D (tex_shadow [5], coord.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;
                                                        s   += shadow2D (tex_shadow [5], coord.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;
                                                        shadow = s * 0.25;
                                    
                                                        #ifdef DEBUG
                                                            gl_FragColor = vec4 (0.0, 0.0, 1.0, 1.0); return;
                                                        #endif
                                                    
                                                    } else {

                                                        // TRANSITION
                                                        if ((depth > depthmin [6]) && (depth < depthmax [5])) {
                                    
                                                            vec4  coord1 = matrix [5] * vec4 (worldpos.xyz, 1.0);   coord1 = coord1 * 0.5 + 0.5;
                                                            vec4  coord2 = matrix [6] * vec4 (worldpos.xyz, 1.0);   coord2 = coord2 * 0.5 + 0.5;
                                        
                                                            s1   = shadow2D (tex_shadow [5], coord1.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;
                                                            s1  += shadow2D (tex_shadow [5], coord1.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;
                                                            s1  += shadow2D (tex_shadow [5], coord1.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;
                                                            s1  += shadow2D (tex_shadow [5], coord1.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;
                                                            s2   = shadow2D (tex_shadow [6], coord2.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;
                                                            s2  += shadow2D (tex_shadow [6], coord2.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;
                                                            s2  += shadow2D (tex_shadow [6], coord2.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;
                                                            s2  += shadow2D (tex_shadow [6], coord2.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;
                                                            shadow = (mix (s1, s2, (depth - depthmin [6]) / (depthmax [5] - depthmin [6])) * 0.25);
                                        
                                                            #ifdef DEBUG
                                                                gl_FragColor = vec4 (0.0, 0.0, 0.0, 1.0); return;
                                                            #endif
                                                
                                                        } else {

                                                            if ((depth > depthmax [5]) && (depth < depthmin [7])) {
                                
                                                                vec4  coord = matrix [6] * vec4 (worldpos.xyz, 1.0);   coord = coord * 0.5 + 0.5;
                                    
                                                                s    = shadow2D (tex_shadow [6], coord.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;
                                                                s   += shadow2D (tex_shadow [6], coord.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;
                                                                s   += shadow2D (tex_shadow [6], coord.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;
                                                                s   += shadow2D (tex_shadow [6], coord.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;
                                                                shadow = s * 0.25;
                                    
                                                                #ifdef DEBUG
                                                                    gl_FragColor = vec4 (1.0, 0.0, 0.0, 1.0); return;
                                                                #endif
                                                    
                                                            } else {

                                                                // TRANSITION
                                                                if ((depth > depthmin [7]) && (depth < depthmax [6])) {
                                    
                                                                    vec4  coord1 = matrix [6] * vec4 (worldpos.xyz, 1.0);   coord1 = coord1 * 0.5 + 0.5;
                                                                    vec4  coord2 = matrix [7] * vec4 (worldpos.xyz, 1.0);   coord2 = coord2 * 0.5 + 0.5;
                                        
                                                                    s1   = shadow2D (tex_shadow [6], coord1.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;
                                                                    s1  += shadow2D (tex_shadow [6], coord1.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;
                                                                    s1  += shadow2D (tex_shadow [6], coord1.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;
                                                                    s1  += shadow2D (tex_shadow [6], coord1.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;
                                                                    s2   = shadow2D (tex_shadow [7], coord2.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;
                                                                    s2  += shadow2D (tex_shadow [7], coord2.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;
                                                                    s2  += shadow2D (tex_shadow [7], coord2.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;
                                                                    s2  += shadow2D (tex_shadow [7], coord2.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;
                                                                    shadow = (mix (s1, s2, (depth - depthmin [7]) / (depthmax [6] - depthmin [7])) * 0.25);
                                        
                                                                    #ifdef DEBUG
                                                                        gl_FragColor = vec4 (0.0, 0.0, 0.0, 1.0); return;
                                                                    #endif
                                                
                                                                } else {
                                
                                                                    if ((depth > depthmax [6]) && (depth < depthend)) {
                                        
                                                                        vec4  coord = matrix [7] * vec4 (worldpos.xyz, 1.0);   coord = coord * 0.5 + 0.5;
                                            
                                                                        s    = shadow2D (tex_shadow [7], coord.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;
                                                                        s   += shadow2D (tex_shadow [7], coord.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;
                                                                        s   += shadow2D (tex_shadow [7], coord.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;
                                                                        s   += shadow2D (tex_shadow [7], coord.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;
                                                                        shadow = s * 0.25;
                                            
                                                                        #ifdef DEBUG
                                                                            gl_FragColor = vec4 (0.0, 1.0, 0.0, 1.0); return;
                                                                        #endif
                                            
                                                                    } else {

                                                                        // TRANSITION
                                                                        if ((depth > depthend) && (depth < depthmax [7])) {
                                            
                                                                            vec4  coord1 = matrix [7] * vec4 (worldpos.xyz, 1.0);   coord1 = coord1 * 0.5 + 0.5;
                                                
                                                                            s1   = shadow2D (tex_shadow [7], coord1.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;
                                                                            s1  += shadow2D (tex_shadow [7], coord1.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;
                                                                            s1  += shadow2D (tex_shadow [7], coord1.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;
                                                                            s1  += shadow2D (tex_shadow [7], coord1.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;
                                                                            shadow = (mix (s1, 4.0, (depth - depthend) / (depthmax [6] - depthend)) * 0.25);
                                                
                                                                            #ifdef DEBUG
                                                                                gl_FragColor = vec4 (0.0, 0.0, 0.0, 1.0); return;
                                                                            #endif
                                                
                                                                        } else {
                                            
                                                                            shadow = 1.0;

                                                                            #ifdef DEBUG
                                                                                gl_FragColor = vec4 (0.2, 0.2, 0.2, 1.0); return;
                                                                            #endif
                                                                        }
                                                                    }
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }

        #endif
        
        // little trick to hide some self-shadowing
        // remove shadow on steep slopes
        //
        // shadow = mix (shadow * slope, slope, max (0.0, min (1.0, 15.0 * slope - 5.0)));
        
        if (shadow > 0.0) {

            // SHADING VECTORS
            vec3  eye       = - normalize (viewpos);
            
            // SCALE SPECULAR PARAMETERS
            float shininess = max (G3.x * 40.0, 1.0);
            float gloss     = max (G3.y *  4.0, 0.1);
            float sss       = G1.a;

            shadow *= slope;

            // SHADING
            gl_FragColor    = ambient * color + emission + intensity * shadow * sss * (color + gloss * color * pow (max (dot (reflect (- ray, normal), eye), 0.0), shininess));

			// INTENSITY
			float shine     = dot (gl_FragColor.rgb, vec3 (0.333, 0.333, 0.333));

			// COLOR CORRECTIONS
			gl_FragColor    = pow (brightness * mix (gl_FragColor, vec4 (shine, shine, shine, 1.0), desaturation), contrast);
            
            // ALPHA
            gl_FragColor.a  = shadow;
            
            return;
        }        
    }
    
    // SHADING
    gl_FragColor    = ambient * color + emission;

    // INTENSITY
	float shine     = dot (gl_FragColor.rgb, vec3 (0.333, 0.333, 0.333));

	// COLOR CORRECTIONS
	gl_FragColor    = pow (brightness * mix (gl_FragColor, vec4 (shine, shine, shine, 1.0), desaturation), contrast);
}
