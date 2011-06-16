
uniform sampler2DRect   tex_G1;
uniform sampler2DRect   tex_G2;
uniform sampler2D       tex_rand;
uniform sampler2DShadow tex_shadow [5];

uniform float intensity;
uniform float saturate;
uniform float ambient;

uniform float offset   [5];
uniform float depthmin [5];
uniform float depthmax [5];
uniform float depthend;

uniform mat4 matrix [5];
varying vec2 screen;

varying vec3 view;
varying vec3 ray;

void main ()
{
    
    // G1 READ
    vec4  G1         = texture2DRect (tex_G1, screen);
    
    // G2 READ
    vec4  G2         = texture2DRect (tex_G2, screen);
    
    // NORMAL DECODE

        // SPHEREMAP TRANSFORM
        //
        // http://aras-p.info/texts/CompactNormalStorage.html#method04spheremap
        //
    
    vec3 normal;

    normal.xy        = G2.xy * 4.0 - 2.0;           float d = dot (normal.xy, normal.xy);
    
    normal.xy       *= sqrt (1.0 - d * 0.25);
    normal.z         =       1.0 - d * 0.5;
    
    // COLOR
    vec4  color      = G1 * gl_LightSource [0].diffuse;    
        
    // LIGHT SLOPE
    float slope      = max (dot (ray, normal) - 0.2, 0.0);
        
    if (slope > 0.0) {
    
        // DECODE DEPTH
        float depth      = (G2.z + G2.w) * 0.00001525902;    // 1.0 / 65535.0
    
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
        
        if ((depth > depthmin [0]) && (depth < depthmin [1])) {

            vec4  coord = matrix [0] * vec4 (worldpos.xyz, 1.0);   coord = coord * 0.5 + 0.5;
            
            s    = shadow2D (tex_shadow [0], coord.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;    //- offset [0])).r;
            s   += shadow2D (tex_shadow [0], coord.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;    //- offset [0])).r;
            s   += shadow2D (tex_shadow [0], coord.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;    //- offset [0])).r;
            s   += shadow2D (tex_shadow [0], coord.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;    //- offset [0])).r;
            shadow = s * 0.25;
            
            //gl_FragColor = vec4 (1.0, 0.0, 0.0, 1.0); return;
        
        } else {

            // TRANSITION
            if ((depth > depthmin [1]) && (depth < depthmax [0])) {
            
                vec4  coord1 = matrix [0] * vec4 (worldpos.xyz, 1.0);   coord1 = coord1 * 0.5 + 0.5;
                vec4  coord2 = matrix [1] * vec4 (worldpos.xyz, 1.0);   coord2 = coord2 * 0.5 + 0.5;
                
                s1   = shadow2D (tex_shadow [0], coord1.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;    //- offset [0])).r;
                s1  += shadow2D (tex_shadow [0], coord1.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;    //- offset [0])).r;
                s1  += shadow2D (tex_shadow [0], coord1.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;    //- offset [0])).r;
                s1  += shadow2D (tex_shadow [0], coord1.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;    //- offset [0])).r;
                s2   = shadow2D (tex_shadow [1], coord2.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;    //- offset [1])).r;
                s2  += shadow2D (tex_shadow [1], coord2.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;    //- offset [1])).r;
                s2  += shadow2D (tex_shadow [1], coord2.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;    //- offset [1])).r;
                s2  += shadow2D (tex_shadow [1], coord2.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;    //- offset [1])).r;
                shadow = (mix (s1, s2, (depth - depthmin [1]) / (depthmax [0] - depthmin [1])) * 0.25);

                //gl_FragColor = vec4 (0.0, 0.0, 0.0, 1.0); return;
            
            } else {

                if ((depth > depthmax [0]) && (depth < depthmin [2])) {

                    vec4  coord = matrix [1] * vec4 (worldpos.xyz, 1.0);   coord = coord * 0.5 + 0.5;
                    
                    s    = shadow2D (tex_shadow [1], coord.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;    //- offset [1])).r;
                    s   += shadow2D (tex_shadow [1], coord.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;    //- offset [1])).r;
                    s   += shadow2D (tex_shadow [1], coord.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;    //- offset [1])).r;
                    s   += shadow2D (tex_shadow [1], coord.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;    //- offset [1])).r;
                    shadow = s * 0.25;
                    
                    //gl_FragColor = vec4 (0.0, 1.0, 0.0, 1.0); return;
                
                } else {
        
                    // TRANSITION
                    if ((depth > depthmin [2]) && (depth < depthmax [1])) {
                    
                        vec4  coord1 = matrix [1] * vec4 (worldpos.xyz, 1.0);   coord1 = coord1 * 0.5 + 0.5;
                        vec4  coord2 = matrix [2] * vec4 (worldpos.xyz, 1.0);   coord2 = coord2 * 0.5 + 0.5;
                        
                        s1   = shadow2D (tex_shadow [1], coord1.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;    //- offset [1])).r;
                        s1  += shadow2D (tex_shadow [1], coord1.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;    //- offset [1])).r;
                        s1  += shadow2D (tex_shadow [1], coord1.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;    //- offset [1])).r;
                        s1  += shadow2D (tex_shadow [1], coord1.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;    //- offset [1])).r;
                        s2   = shadow2D (tex_shadow [2], coord2.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;    //- offset [2])).r;
                        s2  += shadow2D (tex_shadow [2], coord2.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;    //- offset [2])).r;
                        s2  += shadow2D (tex_shadow [2], coord2.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;    //- offset [2])).r;
                        s2  += shadow2D (tex_shadow [2], coord2.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;    //- offset [2])).r;
                        shadow = (mix (s1, s2, (depth - depthmin [2]) / (depthmax [1] - depthmin [2])) * 0.25);
                        
                        //gl_FragColor = vec4 (0.0, 0.0, 0.0, 1.0); return;
                                
                    } else {
                    
                        if ((depth > depthmax [1]) && (depth < depthmin [3])) {
                        
                            vec4  coord = matrix [2] * vec4 (worldpos.xyz, 1.0);   coord = coord * 0.5 + 0.5;

                            s    = shadow2D (tex_shadow [2], coord.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;    //- offset [2])).r;
                            s   += shadow2D (tex_shadow [2], coord.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;    //- offset [2])).r;
                            s   += shadow2D (tex_shadow [2], coord.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;    //- offset [2])).r;
                            s   += shadow2D (tex_shadow [2], coord.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;    //- offset [2])).r;
                            shadow = s * 0.25;
                            
                            //gl_FragColor = vec4 (0.0, 0.0, 1.0, 1.0); return;
                                            
                        } else {

                            // TRANSITION
                            if ((depth > depthmin [3]) && (depth < depthmax [2])) {
                            
                                vec4  coord1 = matrix [2] * vec4 (worldpos.xyz, 1.0);   coord1 = coord1 * 0.5 + 0.5;
                                vec4  coord2 = matrix [3] * vec4 (worldpos.xyz, 1.0);   coord2 = coord2 * 0.5 + 0.5;
                                
                                s1   = shadow2D (tex_shadow [2], coord1.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;    //- offset [2])).r;
                                s1  += shadow2D (tex_shadow [2], coord1.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;    //- offset [2])).r;
                                s1  += shadow2D (tex_shadow [2], coord1.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;    //- offset [2])).r;
                                s1  += shadow2D (tex_shadow [2], coord1.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;    //- offset [2])).r;
                                s2   = shadow2D (tex_shadow [3], coord2.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;    //- offset [3])).r;
                                s2  += shadow2D (tex_shadow [3], coord2.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;    //- offset [3])).r;
                                s2  += shadow2D (tex_shadow [3], coord2.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;    //- offset [3])).r;
                                s2  += shadow2D (tex_shadow [3], coord2.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;    //- offset [3])).r;
                                shadow = (mix (s1, s2, (depth - depthmin [3]) / (depthmax [2] - depthmin [3])) * 0.25);
                                
                                //gl_FragColor = vec4 (0.0, 0.0, 0.0, 1.0); return;
                                        
                            } else {
                    
                                if ((depth > depthmax [2]) && (depth < depthmin [4])) {
                                
                                    vec4  coord = matrix [3] * vec4 (worldpos.xyz, 1.0);   coord = coord * 0.5 + 0.5;
                                    
                                    s    = shadow2D (tex_shadow [3], coord.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;    //- offset [3])).r;
                                    s   += shadow2D (tex_shadow [3], coord.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;    //- offset [3])).r;
                                    s   += shadow2D (tex_shadow [3], coord.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;    //- offset [3])).r;
                                    s   += shadow2D (tex_shadow [3], coord.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;    //- offset [3])).r;
                                    shadow = s * 0.25;
                                    
                                    //gl_FragColor = vec4 (1.0, 0.0, 0.0, 1.0); return;
                                                    
                                } else {

                                    // TRANSITION
                                    if ((depth > depthmin [4]) && (depth < depthmax [3])) {
                                    
                                        vec4  coord1 = matrix [3] * vec4 (worldpos.xyz, 1.0);   coord1 = coord1 * 0.5 + 0.5;
                                        vec4  coord2 = matrix [4] * vec4 (worldpos.xyz, 1.0);   coord2 = coord2 * 0.5 + 0.5;
                                        
                                        s1   = shadow2D (tex_shadow [3], coord1.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;    //- offset [3])).r;
                                        s1  += shadow2D (tex_shadow [3], coord1.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;    //- offset [3])).r;
                                        s1  += shadow2D (tex_shadow [3], coord1.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;    //- offset [3])).r;
                                        s1  += shadow2D (tex_shadow [3], coord1.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;    //- offset [3])).r;
                                        s2   = shadow2D (tex_shadow [4], coord2.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;    //- offset [4])).r;
                                        s2  += shadow2D (tex_shadow [4], coord2.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;    //- offset [4])).r;
                                        s2  += shadow2D (tex_shadow [4], coord2.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;    //- offset [4])).r;
                                        s2  += shadow2D (tex_shadow [4], coord2.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;    //- offset [4])).r;
                                        shadow = (mix (s1, s2, (depth - depthmin [4]) / (depthmax [3] - depthmin [4])) * 0.25);
                                        
                                        //gl_FragColor = vec4 (0.0, 0.0, 0.0, 1.0); return;
                                                
                                    } else {
                                
                                        if ((depth > depthmax [3]) && (depth < depthend)) {
                                        
                                            vec4  coord = matrix [4] * vec4 (worldpos.xyz, 1.0);   coord = coord * 0.5 + 0.5;
                                            
                                            s    = shadow2D (tex_shadow [4], coord.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;    //- offset [4])).r;
                                            s   += shadow2D (tex_shadow [4], coord.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;    //- offset [4])).r;
                                            s   += shadow2D (tex_shadow [4], coord.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;    //- offset [4])).r;
                                            s   += shadow2D (tex_shadow [4], coord.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;    //- offset [4])).r;
                                            shadow = s * 0.25;
                                            
                                            //gl_FragColor = vec4 (0.0, 1.0, 0.0, 1.0); return;
                                            
                                        } else {

                                            // TRANSITION
                                            if ((depth > depthend) && (depth < depthmax [4])) {
                                            
                                                vec4  coord1 = matrix [4] * vec4 (worldpos.xyz, 1.0);   coord1 = coord1 * 0.5 + 0.5;
                                                
                                                s1   = shadow2D (tex_shadow [4], coord1.xyz + vec3 (gl_TexCoord [0].x*cosa - gl_TexCoord [0].y*sina, gl_TexCoord [0].y*cosa + gl_TexCoord [0].x*sina, 0.0)).r;    //- offset [4])).r;
                                                s1  += shadow2D (tex_shadow [4], coord1.xyz + vec3 (gl_TexCoord [1].x*cosa - gl_TexCoord [1].y*sina, gl_TexCoord [1].y*cosa + gl_TexCoord [1].x*sina, 0.0)).r;    //- offset [4])).r;
                                                s1  += shadow2D (tex_shadow [4], coord1.xyz + vec3 (gl_TexCoord [2].x*cosa - gl_TexCoord [2].y*sina, gl_TexCoord [2].y*cosa + gl_TexCoord [2].x*sina, 0.0)).r;    //- offset [4])).r;
                                                s1  += shadow2D (tex_shadow [4], coord1.xyz + vec3 (gl_TexCoord [3].x*cosa - gl_TexCoord [3].y*sina, gl_TexCoord [3].y*cosa + gl_TexCoord [3].x*sina, 0.0)).r;    //- offset [4])).r;
                                                shadow = (mix (s1, 4.0, (depth - depthend) / (depthmax [4] - depthend)) * 0.25);
                                                
                                                //gl_FragColor = vec4 (0.0, 0.0, 0.0, 1.0); return;
                                                
                                            } else {
                                            
                                                shadow = 1.0;                                        
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
        
        // little trick to hide some self-shadowing
        // remove shadow on steep slopes
        
        // shadow = mix (shadow * slope, slope, max (0.0, min (1.0, 15.0 * slope - 5.0)));

        shadow *= slope;
                        
        if (shadow > 0.0) {

            // SCALE SPECULAR PARAMETERS            
            float shininess  = floor (G1.a) * 10.0;
            float gloss      = fract (G1.a) *  2.0       * 5.0;
                                    
            // SHADING VECTORS
            vec3  eye        = - normalize (viewpos);
                        
            // SHADING
            gl_FragColor     = saturate * (ambient * color + intensity * shadow * (color + gloss * color * pow (max (dot (reflect (- ray, normal), eye), 0.0), shininess)));
            
            // ALPHA
            gl_FragColor.a   = shadow;
            
            return;
        }
        
        // SHADING
        gl_FragColor     = saturate * ambient * color;
                
        return;
    }
    
    // SHADING
    gl_FragColor     = saturate * (ambient * color + intensity * slope * color);
}
