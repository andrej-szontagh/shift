
////////////////////////////////////////////////////////////////////////////////////////////////////////////
// dr_SunInit
////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if 1

VOID    dr_SunInit ()
{

    dr_sun_count = M_DR_SUN_SPLITS;

    // sun view space vectors
    vCROSS (dr_sun_directionx, dr_sun_directionz, dr_worldup);
    vCROSS (dr_sun_directiony, dr_sun_directionx, dr_sun_directionz);

    FLOAT_64 l;

    l = vLENGTH3 (dr_sun_directionx); vNORMALIZE3 (dr_sun_directionx, l);
    l = vLENGTH3 (dr_sun_directiony); vNORMALIZE3 (dr_sun_directiony, l);
    l = vLENGTH3 (dr_sun_directionz); vNORMALIZE3 (dr_sun_directionz, l);

    FLOAT_64 trans;
    FLOAT_64 split_end;
    FLOAT_64 split_endt;
    FLOAT_64 split_start;
    FLOAT_64 split_startt;

    UINT_32 i, c = dr_sun_count - 1;

    FLOAT_64 scale = 1.0 / pow (dr_sun_distance, 9.0);

    /////////////////////////////////////////////////////
    // FIRST SPLIT
    /////////////////////////////////////////////////////

    split_start = 0;
    split_end   = dr_sun_distance / dr_sun_count;

    // PRACTICAL SPLIT SCHEME

    split_end = 0.9 * pow (split_end, 10.0) * scale + 0.1 * split_end;

    dr_sun_planesside	[0] =             split_end * M_DR_SUN_ZOOM;
    dr_sun_offsets		[0] = (FLOAT_32)((split_end * M_DR_SUN_OFFSET) / dr_sun_planefar);                  /// OFFSET DEPENDS ON SHADOWMAP RESOLUTION !!!!!!!!!!!!!!!!!!!!!!!!!!

    // TRANSITION WIDTH

    trans = split_end * M_DR_SUN_TRAN;

    dr_sun_transitions  [0] = trans;

    // SPLIT START/END + TRANSITION

    split_startt = split_start;
    split_endt   = split_end   + dr_sun_transitions [0];

    dr_sun_splits [0][0] = split_startt;
    dr_sun_splits [0][1] = split_endt;
                
    // SPLIT LINEAR DEPTH

    dr_sun_depthmin [0] = (FLOAT_32) (split_startt / dr_planefar);    // split start
    dr_sun_depthmax [0] = (FLOAT_32) (split_endt   / dr_planefar);    // split end

    // EVALUATE FRUSTUM DIMENSIONS

    dr_sun_split_nearh [0] = split_startt   * dr_avertical;             // near plane frustum quad up vector length
    dr_sun_split_nearw [0] = dr_sun_split_nearh [0] * dr_aspect;        // near plane frustum quad side vector length

    dr_sun_split_farh  [0] = split_endt     * dr_avertical;             // far plane frustum quad up vector length
    dr_sun_split_farw  [0] = dr_sun_split_farh  [0] * dr_aspect;        // far plane frustum quad side vector length

    /////////////////////////////////////////////////////
    // INNER SPLITS
    /////////////////////////////////////////////////////

    for (i = 1; i < c; i ++) {

        split_start =   split_end;
                        split_end   = (i + 1) * dr_sun_distance / dr_sun_count;

        // PRACTICAL SPLIT SCHEME

        split_end = 0.9 * pow (split_end, 10.0) * scale + 0.1 * split_end;

        dr_sun_planesside	[i] =             split_end * M_DR_SUN_ZOOM;
        dr_sun_offsets		[i] = (FLOAT_32)((split_end * M_DR_SUN_OFFSET) / dr_sun_planefar);              /// OFFSET DEPENDS ON SHADOWMAP RESOLUTION !!!!!!!!!!!!!!!!!!!!!!!!!!

        // TRANSITION WIDTH

        trans = split_end * M_DR_SUN_TRAN;

        dr_sun_transitions  [i] = trans;

        // SPLIT START/END + TRANSITION

        split_startt = split_start - dr_sun_transitions [i - 1];
        split_endt   = split_end   + dr_sun_transitions [i    ];

        dr_sun_splits [i][0] = split_startt;
        dr_sun_splits [i][1] = split_endt;
                    
        // SPLIT LINEAR DEPTH

        dr_sun_depthmin [i] = (FLOAT_32) (split_startt / dr_planefar);    // split start
        dr_sun_depthmax [i] = (FLOAT_32) (split_endt   / dr_planefar);    // split end

        // EVALUATE FRUSTUM DIMENSIONS

        dr_sun_split_nearh [i] = split_startt   * dr_avertical;             // near plane frustum quad up vector length
        dr_sun_split_nearw [i] = dr_sun_split_nearh [i] * dr_aspect;        // near plane frustum quad side vector length

        dr_sun_split_farh  [i] = split_endt     * dr_avertical;             // far plane frustum quad up vector length
        dr_sun_split_farw  [i] = dr_sun_split_farh  [i] * dr_aspect;        // far plane frustum quad side vector length
    }

    /////////////////////////////////////////////////////
    // LAST SPLIT
    /////////////////////////////////////////////////////

    split_start =   split_end;
                    split_end   = (i + 1) * dr_sun_distance / dr_sun_count;

    // PRACTICAL SPLIT SCHEME

    split_end = 0.9 * pow (split_end, 10.0) * scale + 0.1 * split_end;

    dr_sun_planesside	[i] =             split_end * M_DR_SUN_ZOOM;
    dr_sun_offsets		[i] = (FLOAT_32)((split_end * M_DR_SUN_OFFSET) / dr_sun_planefar);                  /// OFFSET DEPENDS ON SHADOWMAP RESOLUTION !!!!!!!!!!!!!!!!!!!!!!!!!!

    // TRANSITION WIDTH

    trans = split_end * M_DR_SUN_TRAN;

    dr_sun_transitions  [i] = trans;

    // SPLIT START/END + TRANSITION

    split_startt = split_start - dr_sun_transitions [i - 1];
    split_endt   = split_end   + dr_sun_transitions [i    ];

    dr_sun_splits [i][0] = split_startt;
    dr_sun_splits [i][1] = split_endt;

    // SPLIT LINEAR DEPTH

    dr_sun_depthmin [i] = (FLOAT_32) (split_startt / dr_planefar);      // split start
    dr_sun_depthmax [i] = (FLOAT_32) (split_endt   / dr_planefar);      // split end
    dr_sun_splitend     = (FLOAT_32) (split_end    / dr_planefar);

    // EVALUATE FRUSTUM DIMENSIONS

    dr_sun_split_nearh [i] = split_startt   * dr_avertical;             // near plane frustum quad up vector length
    dr_sun_split_nearw [i] = dr_sun_split_nearh [i] * dr_aspect;        // near plane frustum quad side vector length

    dr_sun_split_farh  [i] = split_endt     * dr_avertical;             // far plane frustum quad up vector length
    dr_sun_split_farw  [i] = dr_sun_split_farh  [i] * dr_aspect;        // far plane frustum quad side vector length

}

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// dr_SunPrepare
////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if 1

VOID    dr_SunPrepare ()
{

    UINT_32 i, n, m;

    // shortcuts
    FLOAT_64 dnear = 0.0;
    FLOAT_64 dfar  = dr_planefar;

    // origin is in the center of frustum
    FLOAT_64 dist = (dnear + dfar) * 0.5;

    FLOAT_64 originview [3];
    FLOAT_64 origin     [3] = { (FLOAT_64) dr_campos [0] + dr_camdir [0] * dist, 
                                (FLOAT_64) dr_campos [1] + dr_camdir [1] * dist, 
                                (FLOAT_64) dr_campos [2] + dr_camdir [2] * dist };

    ////////////////////////////////////////////
    // VIEW MATRIX
    ////////////////////////////////////////////

    FLOAT_64P axisx = dr_sun_directionx;
    FLOAT_64P axisy = dr_sun_directiony;
    FLOAT_64P axisz = dr_sun_directionz;    
    
    // our basic orthonormal view matrix, set by sun direction
    // Z coord is negative, because we are looking into -Z
    // we add translation part later after clamping

    FLOAT_64 matview [16] = {   axisx [0], axisx [1], -axisx [2], 0.0,
                                axisy [0], axisy [1], -axisy [2], 0.0,
                                axisz [0], axisz [1], -axisz [2], 0.0,
                                0.0,       0.0,       0.0,        1.0 };

    // transformed origin, we will need it later below
    pTRANSFORM_COL4x4p3 (originview, origin, matview);

    ////////////////////////////////////////////
    // BUILDING FRUSTUM VERTICES
    ////////////////////////////////////////////

    FLOAT_64 campost [2], points [4][3], pointst [4][2];    FLOAT_64P edges [8][2];

    // near plane center point
    FLOAT_64 nearp [3] = {  dr_campos [0] + dr_camdir [0] * dnear,
                            dr_campos [1] + dr_camdir [1] * dnear,
                            dr_campos [2] + dr_camdir [2] * dnear };

    // far plane center point
    FLOAT_64 farp  [3] = {  dr_campos [0] + dr_camdir [0] * dfar,
                            dr_campos [1] + dr_camdir [1] * dfar,
                            dr_campos [2] + dr_camdir [2] * dfar };

    // camera position transform
    pTRANSFORM_COL4x4p2 (campost, dr_campos, matview);    

    // FAR PLANE POINTS

    points [0][0] = farp[0] + dr_cambi[0] * dr_farw + dr_camup[0] * dr_farh;
    points [0][1] = farp[1] + dr_cambi[1] * dr_farw + dr_camup[1] * dr_farh;
    points [0][2] = farp[2] + dr_cambi[2] * dr_farw + dr_camup[2] * dr_farh;
    pTRANSFORM_COL4x4p2 (pointst [0], points [0], matview);
    points [1][0] = farp[0] + dr_cambi[0] * dr_farw - dr_camup[0] * dr_farh;
    points [1][1] = farp[1] + dr_cambi[1] * dr_farw - dr_camup[1] * dr_farh;
    points [1][2] = farp[2] + dr_cambi[2] * dr_farw - dr_camup[2] * dr_farh;
    pTRANSFORM_COL4x4p2 (pointst [1], points [1], matview);
    points [2][0] = farp[0] - dr_cambi[0] * dr_farw + dr_camup[0] * dr_farh;
    points [2][1] = farp[1] - dr_cambi[1] * dr_farw + dr_camup[1] * dr_farh;
    points [2][2] = farp[2] - dr_cambi[2] * dr_farw + dr_camup[2] * dr_farh;
    pTRANSFORM_COL4x4p2 (pointst [2], points [2], matview);
    points [3][0] = farp[0] - dr_cambi[0] * dr_farw - dr_camup[0] * dr_farh;
    points [3][1] = farp[1] - dr_cambi[1] * dr_farw - dr_camup[1] * dr_farh;
    points [3][2] = farp[2] - dr_cambi[2] * dr_farw - dr_camup[2] * dr_farh;
    pTRANSFORM_COL4x4p2 (pointst [3], points [3], matview);

/*
    // far plane quad edges (clokwise order)

    edges [0][0] = &pointst [0][0];     edges [0][1] = &pointst [1][0];
    edges [1][0] = &pointst [1][0];     edges [1][1] = &pointst [3][0];
    edges [2][0] = &pointst [3][0];     edges [2][1] = &pointst [2][0];
    edges [3][0] = &pointst [2][0];     edges [3][1] = &pointst [0][0];

    // camera-far plane connection edges

    edges [4][0] = campost;             edges [4][1] = &pointst [0][0];
    edges [5][0] = campost;             edges [5][1] = &pointst [1][0];
    edges [6][0] = campost;             edges [6][1] = &pointst [2][0];
    edges [7][0] = campost;             edges [7][1] = &pointst [3][0];
*/

    // camera-far plane connection edges

    edges [0][0] = campost;             edges [0][1] = &pointst [0][0];
    edges [1][0] = campost;             edges [1][1] = &pointst [1][0];
    edges [2][0] = campost;             edges [2][1] = &pointst [2][0];
    edges [3][0] = campost;             edges [3][1] = &pointst [3][0];

    // 2D EDGES lENGTHS

    FLOAT_64 lengths [8], v [2];

    vSUB2 (v, edges [ 0][0], edges [ 0][1]);      lengths [ 0] = vLENGTHQ2 (v);
    vSUB2 (v, edges [ 1][0], edges [ 1][1]);      lengths [ 1] = vLENGTHQ2 (v);
    vSUB2 (v, edges [ 2][0], edges [ 2][1]);      lengths [ 2] = vLENGTHQ2 (v);
    vSUB2 (v, edges [ 3][0], edges [ 3][1]);      lengths [ 3] = vLENGTHQ2 (v);

    ///vSUB2 (v, edges [ 4][0], edges [ 4][1]);      lengths [ 4] = vLENGTHQ2 (v);
    ///vSUB2 (v, edges [ 5][0], edges [ 5][1]);      lengths [ 5] = vLENGTHQ2 (v);
    ///vSUB2 (v, edges [ 6][0], edges [ 6][1]);      lengths [ 6] = vLENGTHQ2 (v);
    ///vSUB2 (v, edges [ 7][0], edges [ 7][1]);      lengths [ 7] = vLENGTHQ2 (v);

    ////////////////////////////////////////////
    // SORTING EDGES
    ////////////////////////////////////////////

    FLOAT_64  tmp;
    FLOAT_64P tmpp;

    /// CHANGE TO INSERT SORT !!!!!!!!!!!!!!!!!!!!!!!

    ///for (n = 0; n < 8; n ++) {
    for (n = 0; n < 4; n ++) {

        ///for (m = n + 1; m < 8; m ++) {
        for (m = n + 1; m < 4; m ++) {

            if (lengths [m] > lengths [n]) {

                tmpp =  edges [n][0];
                        edges [n][0] =  edges [m][0];
                                        edges [m][0] = tmpp;
                tmpp =  edges [n][1];
                        edges [n][1] =  edges [m][1];
                                        edges [m][1] = tmpp;
                tmp  =  lengths [n];
                        lengths [n] =   lengths [m];
                                        lengths [m] = tmp;
            }
        }
    }

    FLOAT_64 A, B, vx, vy;

    // init
    dr_sun_frustum_planesc = 0;

    ////////////////////////////////////////////
    // CHECKING ALL EDGES
    ////////////////////////////////////////////

    ///for (n = 0; n < 8; n ++) {
    for (n = 0; n < 4; n ++) {

        ///
        FLOAT_64 l;

        // vector perpendicular to edge
        A =   (edges [n][1])[1] - (edges [n][0])[1];
        B = - (edges [n][1])[0] + (edges [n][0])[0];

        /// normalizing
        l = sqrt (A*A + B*B);       A /= l;     B /= l;

        vx = originview [0] - (edges [n][0])[0];
        vy = originview [1] - (edges [n][0])[1];

        /// normalizing
        l = sqrt (vx*vx + vy*vy);   vx /= l;    vy /= l;

        // has the same side of plane ?     (dot product)
        if ((vx*A + vy*B) < 0) {

            // inverting normal
            A = - A;    B = - B;
        }

        FLOAT_64 d = - A*(edges [n][0])[0] - B*(edges [n][0])[1];

        // is all points at the same side of plane
        if ((((A * campost[0])    + (B * campost[1])    + d) > - 0.0001) && \
            (((A * pointst[0][0]) + (B * pointst[0][1]) + d) > - 0.0001) && \
            (((A * pointst[1][0]) + (B * pointst[1][1]) + d) > - 0.0001) && \
            (((A * pointst[2][0]) + (B * pointst[2][1]) + d) > - 0.0001) && \
            (((A * pointst[3][0]) + (B * pointst[3][1]) + d) > - 0.0001)) {

            // NOTE: we have to use offset above becouse some points are laing on this plane

            ////////////////////////////////////////////
            // RECONSTRUCTING CLIP PLANE
            ////////////////////////////////////////////

            FLOAT_64 point [3], u [4];

            // TRANSFORMING BACK EDGE POINT

            FLOAT_64P p1 = edges [n][0];
            FLOAT_64P p2 = edges [n][1];

            u [0] = (p1 [0] + p2 [0]) * 0.5;  
            u [1] = (p1 [1] + p2 [1]) * 0.5;  u [2] = 0.0;

            // rotating back to world space
            vTRANSFORM_ROW4x4v3 (point, u, matview);

            // TRANSFORMING BACK EDGE NORMAL VECTOR

            FLOAT_64 v [3] = { A, B, 0.0 };

            // rotating back to world space
            vTRANSFORM_ROW4x4v3 (u, v, matview);

            /// normalize
            FLOAT_64 l = vLENGTH3 (u);   vNORMALIZE3 (u, l);

            // D param
            u [3] = - u [0] * point [0] - u [1] * point [1] - u [2] * point [2];

            // clipping plane
            dr_sun_frustum_planesd [dr_sun_frustum_planesc][0] = u [0];
            dr_sun_frustum_planes  [dr_sun_frustum_planesc][0] = (FLOAT_32) u [0];
            dr_sun_frustum_planesd [dr_sun_frustum_planesc][1] = u [1];
            dr_sun_frustum_planes  [dr_sun_frustum_planesc][1] = (FLOAT_32) u [1];
            dr_sun_frustum_planesd [dr_sun_frustum_planesc][2] = u [2];
            dr_sun_frustum_planes  [dr_sun_frustum_planesc][2] = (FLOAT_32) u [2];
            dr_sun_frustum_planesd [dr_sun_frustum_planesc][3] = u [3];
            dr_sun_frustum_planes  [dr_sun_frustum_planesc][3] = (FLOAT_32) u [3];

            // advance
            dr_sun_frustum_planesc ++;
        }
    }

    ////////////////////////////////////////////
    // ADDITIONAL OUTWARD PLANES
    ////////////////////////////////////////////

    if (vDOT3 (dr_frustum_planes [0], axisz) < 0) {
        dr_sun_frustum_planesd [dr_sun_frustum_planesc][0] = dr_frustum_planes [0][0];
        dr_sun_frustum_planes  [dr_sun_frustum_planesc][0] = dr_frustum_planes [0][0];
        dr_sun_frustum_planesd [dr_sun_frustum_planesc][1] = dr_frustum_planes [0][1];
        dr_sun_frustum_planes  [dr_sun_frustum_planesc][1] = dr_frustum_planes [0][1];
        dr_sun_frustum_planesd [dr_sun_frustum_planesc][2] = dr_frustum_planes [0][2];
        dr_sun_frustum_planes  [dr_sun_frustum_planesc][2] = dr_frustum_planes [0][2];
        dr_sun_frustum_planesd [dr_sun_frustum_planesc][3] = dr_frustum_planes [0][3];
        dr_sun_frustum_planes  [dr_sun_frustum_planesc][3] = dr_frustum_planes [0][3];
        dr_sun_frustum_planesc ++;
    }

    if (vDOT3 (dr_frustum_planes [1], axisz) < 0) {
        dr_sun_frustum_planesd [dr_sun_frustum_planesc][0] = dr_frustum_planes [1][0];
        dr_sun_frustum_planes  [dr_sun_frustum_planesc][0] = dr_frustum_planes [1][0];
        dr_sun_frustum_planesd [dr_sun_frustum_planesc][1] = dr_frustum_planes [1][1];
        dr_sun_frustum_planes  [dr_sun_frustum_planesc][1] = dr_frustum_planes [1][1];
        dr_sun_frustum_planesd [dr_sun_frustum_planesc][2] = dr_frustum_planes [1][2];
        dr_sun_frustum_planes  [dr_sun_frustum_planesc][2] = dr_frustum_planes [1][2];
        dr_sun_frustum_planesd [dr_sun_frustum_planesc][3] = dr_frustum_planes [1][3];
        dr_sun_frustum_planes  [dr_sun_frustum_planesc][3] = dr_frustum_planes [1][3];
        dr_sun_frustum_planesc ++;
    }

    if (vDOT3 (dr_frustum_planes [2], axisz) < 0) {
        dr_sun_frustum_planesd [dr_sun_frustum_planesc][0] = dr_frustum_planes [2][0];
        dr_sun_frustum_planes  [dr_sun_frustum_planesc][0] = dr_frustum_planes [2][0];
        dr_sun_frustum_planesd [dr_sun_frustum_planesc][1] = dr_frustum_planes [2][1];
        dr_sun_frustum_planes  [dr_sun_frustum_planesc][1] = dr_frustum_planes [2][1];
        dr_sun_frustum_planesd [dr_sun_frustum_planesc][2] = dr_frustum_planes [2][2];
        dr_sun_frustum_planes  [dr_sun_frustum_planesc][2] = dr_frustum_planes [2][2];
        dr_sun_frustum_planesd [dr_sun_frustum_planesc][3] = dr_frustum_planes [2][3];
        dr_sun_frustum_planes  [dr_sun_frustum_planesc][3] = dr_frustum_planes [2][3];
        dr_sun_frustum_planesc ++;
    }

    if (vDOT3 (dr_frustum_planes [3], axisz) < 0) {
        dr_sun_frustum_planesd [dr_sun_frustum_planesc][0] = dr_frustum_planes [3][0];
        dr_sun_frustum_planes  [dr_sun_frustum_planesc][0] = dr_frustum_planes [3][0];
        dr_sun_frustum_planesd [dr_sun_frustum_planesc][1] = dr_frustum_planes [3][1];
        dr_sun_frustum_planes  [dr_sun_frustum_planesc][1] = dr_frustum_planes [3][1];
        dr_sun_frustum_planesd [dr_sun_frustum_planesc][2] = dr_frustum_planes [3][2];
        dr_sun_frustum_planes  [dr_sun_frustum_planesc][2] = dr_frustum_planes [3][2];
        dr_sun_frustum_planesd [dr_sun_frustum_planesc][3] = dr_frustum_planes [3][3];
        dr_sun_frustum_planes  [dr_sun_frustum_planesc][3] = dr_frustum_planes [3][3];
        dr_sun_frustum_planesc ++;
    }

    ////////////////////////////////////////////
    // SPLITS LOOP
    ////////////////////////////////////////////

    UINT_32 planes  = 0;
    UINT_32 count   = dr_sun_count;

    for (i = 0; i < count; i ++) {

        // shortcuts
        FLOAT_64 dnear = dr_sun_splits [i][0];
        FLOAT_64 dfar  = dr_sun_splits [i][1];

        // origin is in the middle between two splits
        FLOAT_64 dist = (dnear + dfar) * 0.5;

        FLOAT_64 originview [3];
        FLOAT_64 originprj  [2];

        FLOAT_64 origin  [3] = { (FLOAT_64) dr_campos [0] + dr_camdir [0] * dist, 
                                 (FLOAT_64) dr_campos [1] + dr_camdir [1] * dist, 
                                 (FLOAT_64) dr_campos [2] + dr_camdir [2] * dist };

        ////////////////////////////////////////////
        // PROJECTION MATRIX
        ////////////////////////////////////////////

        FLOAT_64 sdist  = dr_sun_distance; 

        FLOAT_64 depth  = dr_sun_planefar;
        FLOAT_64 depthi = 1.0 / depth;

        FLOAT_64 side   = dr_sun_planesside [i];
        FLOAT_64 sidei  = 1.0 / side;

        // this is our orthogonal projection matrix
        // it just scales our viewspace into clip space [-1,1]

        FLOAT_64 matprj  [16] = {  sidei,  0.0,     0.0,     0.0,
                                   0.0,    sidei,   0.0,     0.0,
                                   0.0,    0.0,    -depthi,  0.0,
                                   0.0,    0.0,     0.0,     1.0 };

        ////////////////////////////////////////////
        // VIEW MATRIX
        ////////////////////////////////////////////

        FLOAT_64P axisx = dr_sun_directionx;
        FLOAT_64P axisy = dr_sun_directiony;
        FLOAT_64P axisz = dr_sun_directionz;    
        
        // our basic orthonormal view matrix, set by sun direction
        // z coord is negative, because we are looking into -z
        // we add translation part later after clamping

        FLOAT_64 matview [16] = {   axisx [0], axisx [1], -axisx [2], 0.0,
                                    axisy [0], axisy [1], -axisy [2], 0.0,
                                    axisz [0], axisz [1], -axisz [2], 0.0,
                                    0.0,       0.0,       0.0,        1.0 };

        ////////////////////////////////////////////
        // CLAMPING VIEW ORIGIN INTO SHADOWMAP RASTER
        ////////////////////////////////////////////

        // We translate our origin first to have camera (sun) in 0,0,0
        origin [0] -= sdist * axisz [0];
        origin [1] -= sdist * axisz [1];
        origin [2] -= sdist * axisz [2];

        // now we rotate origin into sun view space
        // our sun camera is lookin to 0,0,0 of world space
        // we only use this view to clamp origin to avoid aliasing 
        vTRANSFORM_COL4x4v3 (originview, origin,  matview);

        // projecting into shadowmap space
        originprj [0] = originview [0] * sidei;
        originprj [1] = originview [1] * sidei;

        // we clamp projected coords to shadowmap texels to avoid 'saw' aliasing
        originprj [0] = floor (originprj [0] * M_DR_SUN_SHADOWH) / M_DR_SUN_SHADOWH;
        originprj [1] = floor (originprj [1] * M_DR_SUN_SHADOWH) / M_DR_SUN_SHADOWH;

        // unprojecting back to sun view space
        originview [0] = originprj [0] * side;
        originview [1] = originprj [1] * side;

        // rotating back to world space
        vTRANSFORM_ROW4x4v3 (origin, originview, matview);

        // post translation (not in matrix)
        origin [0] += sdist * axisz [0];
        origin [1] += sdist * axisz [1];
        origin [2] += sdist * axisz [2];

        ////////////////////////////////////////////
        // BUILDING NEW MATRIX
        ////////////////////////////////////////////

        // to add pre-translation to matrix we have to transform (rotate) our new position
        // we rotate into sun view space

        FLOAT_64 u [3], v [3];

        u [0] = origin [0] - sdist * axisx [2];
        u [1] = origin [1] - sdist * axisy [2];
        u [2] = origin [2] - sdist * axisz [2];

        vTRANSFORM_COL4x4v3 (v, u, matview);

        // adding new translation into matrix
        matview [12] = - v [0];
        matview [13] = - v [1];
        matview [14] = - v [2];

        ////////////////////////////////////////////
        // SAVE MATRICES
        ////////////////////////////////////////////

        memcpy ((VOIDP) dr_sun_projection   [i], (VOIDP) matprj,  SIZE_FLOAT_64 * 16);
        memcpy ((VOIDP) dr_sun_view         [i], (VOIDP) matview, SIZE_FLOAT_64 * 16);

        ////////////////////////////////////////////
        // COMBINED MATRIX
        ////////////////////////////////////////////

        FLOAT_64 mat [16] = { matview [ 0] * sidei, matview [ 1] * sidei, matview [ 2] * -depthi, 0.0,
                              matview [ 4] * sidei, matview [ 5] * sidei, matview [ 6] * -depthi, 0.0,
                              matview [ 8] * sidei, matview [ 9] * sidei, matview [10] * -depthi, 0.0,
                              matview [12] * sidei, matview [13] * sidei, matview [14] * -depthi, 1.0 };

        FLOAT_32P pmat = dr_sun_matrices [i];   mASSIGN (pmat, (FLOAT_32) mat);
        
        // projected origin, we will need it later below
        pTRANSFORM_COL4x4p2 (originprj, origin, mat);

        ////////////////////////////////////////////
        // BUILDING FRUSTUM VERTICES
        ////////////////////////////////////////////

        FLOAT_64 pointst [8][2], lengths [12];  FLOAT_64P edges [12][2];

        // near plane center point
        FLOAT_64 nearp [3] = {  dr_campos [0] + dr_camdir [0] * dnear,
                                dr_campos [1] + dr_camdir [1] * dnear,
                                dr_campos [2] + dr_camdir [2] * dnear };

        // far plane center point
        FLOAT_64 farp  [3] = {  dr_campos [0] + dr_camdir [0] * dfar,
                                dr_campos [1] + dr_camdir [1] * dfar,
                                dr_campos [2] + dr_camdir [2] * dfar };

        // shortcuts
        FLOAT_64 nearw  = dr_sun_split_nearw    [i];
        FLOAT_64 farw   = dr_sun_split_farw     [i];
        FLOAT_64 nearh  = dr_sun_split_nearh    [i];
        FLOAT_64 farh   = dr_sun_split_farh     [i];

        // NEAR PLANE POINTS

        dr_sun_split_points [i][0][0] = nearp[0] + dr_cambi[0] * nearw + dr_camup[0] * nearh;
        dr_sun_split_points [i][0][1] = nearp[1] + dr_cambi[1] * nearw + dr_camup[1] * nearh;
        dr_sun_split_points [i][0][2] = nearp[2] + dr_cambi[2] * nearw + dr_camup[2] * nearh;
        pTRANSFORM_COL4x4p2 (pointst [0], dr_sun_split_points [i][0], mat);
        dr_sun_split_points [i][1][0] = nearp[0] + dr_cambi[0] * nearw - dr_camup[0] * nearh;
        dr_sun_split_points [i][1][1] = nearp[1] + dr_cambi[1] * nearw - dr_camup[1] * nearh;
        dr_sun_split_points [i][1][2] = nearp[2] + dr_cambi[2] * nearw - dr_camup[2] * nearh;
        pTRANSFORM_COL4x4p2 (pointst [1], dr_sun_split_points [i][1], mat);
        dr_sun_split_points [i][2][0] = nearp[0] - dr_cambi[0] * nearw + dr_camup[0] * nearh;
        dr_sun_split_points [i][2][1] = nearp[1] - dr_cambi[1] * nearw + dr_camup[1] * nearh;
        dr_sun_split_points [i][2][2] = nearp[2] - dr_cambi[2] * nearw + dr_camup[2] * nearh;
        pTRANSFORM_COL4x4p2 (pointst [2], dr_sun_split_points [i][2], mat);
        dr_sun_split_points [i][3][0] = nearp[0] - dr_cambi[0] * nearw - dr_camup[0] * nearh;
        dr_sun_split_points [i][3][1] = nearp[1] - dr_cambi[1] * nearw - dr_camup[1] * nearh;
        dr_sun_split_points [i][3][2] = nearp[2] - dr_cambi[2] * nearw - dr_camup[2] * nearh;
        pTRANSFORM_COL4x4p2 (pointst [3], dr_sun_split_points [i][3], mat);

        // FAR PLANE POINTS

        dr_sun_split_points [i][4][0] = farp[0] + dr_cambi[0] * farw + dr_camup[0] * farh;
        dr_sun_split_points [i][4][1] = farp[1] + dr_cambi[1] * farw + dr_camup[1] * farh;
        dr_sun_split_points [i][4][2] = farp[2] + dr_cambi[2] * farw + dr_camup[2] * farh;
        pTRANSFORM_COL4x4p2 (pointst [4], dr_sun_split_points [i][4], mat);
        dr_sun_split_points [i][5][0] = farp[0] + dr_cambi[0] * farw - dr_camup[0] * farh;
        dr_sun_split_points [i][5][1] = farp[1] + dr_cambi[1] * farw - dr_camup[1] * farh;
        dr_sun_split_points [i][5][2] = farp[2] + dr_cambi[2] * farw - dr_camup[2] * farh;
        pTRANSFORM_COL4x4p2 (pointst [5], dr_sun_split_points [i][5], mat);
        dr_sun_split_points [i][6][0] = farp[0] - dr_cambi[0] * farw + dr_camup[0] * farh;
        dr_sun_split_points [i][6][1] = farp[1] - dr_cambi[1] * farw + dr_camup[1] * farh;
        dr_sun_split_points [i][6][2] = farp[2] - dr_cambi[2] * farw + dr_camup[2] * farh;
        pTRANSFORM_COL4x4p2 (pointst [6], dr_sun_split_points [i][6], mat);
        dr_sun_split_points [i][7][0] = farp[0] - dr_cambi[0] * farw - dr_camup[0] * farh;
        dr_sun_split_points [i][7][1] = farp[1] - dr_cambi[1] * farw - dr_camup[1] * farh;
        dr_sun_split_points [i][7][2] = farp[2] - dr_cambi[2] * farw - dr_camup[2] * farh;
        pTRANSFORM_COL4x4p2 (pointst [7], dr_sun_split_points [i][7], mat);

        // near plane quad edges (clokwise order)

        edges [ 0][0] = &pointst [0][0];        edges [ 0][1] = &pointst [1][0];
        edges [ 1][0] = &pointst [1][0];        edges [ 1][1] = &pointst [3][0];
        edges [ 2][0] = &pointst [3][0];        edges [ 2][1] = &pointst [2][0];
        edges [ 3][0] = &pointst [2][0];        edges [ 3][1] = &pointst [0][0];

        // far plane quad edges (clokwise order)

        edges [ 4][0] = &pointst [4][0];        edges [ 4][1] = &pointst [5][0];
        edges [ 5][0] = &pointst [5][0];        edges [ 5][1] = &pointst [7][0];
        edges [ 6][0] = &pointst [7][0];        edges [ 6][1] = &pointst [6][0];
        edges [ 7][0] = &pointst [6][0];        edges [ 7][1] = &pointst [4][0];

        // near-far plane connection edges

        ///edges [ 8][0] = &pointst [0][0];        edges [ 8][1] = &pointst [4][0];
        ///edges [ 9][0] = &pointst [1][0];        edges [ 9][1] = &pointst [5][0];
        ///edges [10][0] = &pointst [2][0];        edges [10][1] = &pointst [6][0];
        ///edges [11][0] = &pointst [3][0];        edges [11][1] = &pointst [7][0];

        // 2D EDGES lENGTHS

        vSUB2 (v, edges [ 0][0], edges [ 0][1]);      lengths [ 0] = vLENGTHQ2 (v);
        vSUB2 (v, edges [ 1][0], edges [ 1][1]);      lengths [ 1] = vLENGTHQ2 (v);
        vSUB2 (v, edges [ 2][0], edges [ 2][1]);      lengths [ 2] = vLENGTHQ2 (v);
        vSUB2 (v, edges [ 3][0], edges [ 3][1]);      lengths [ 3] = vLENGTHQ2 (v);
        vSUB2 (v, edges [ 4][0], edges [ 4][1]);      lengths [ 4] = vLENGTHQ2 (v);
        vSUB2 (v, edges [ 5][0], edges [ 5][1]);      lengths [ 5] = vLENGTHQ2 (v);
        vSUB2 (v, edges [ 6][0], edges [ 6][1]);      lengths [ 6] = vLENGTHQ2 (v);
        vSUB2 (v, edges [ 7][0], edges [ 7][1]);      lengths [ 7] = vLENGTHQ2 (v);

        // we dont need to clip side planes
        ///vSUB2 (v, edges [ 8][0], edges [ 8][1]);      lengths [ 8] = vLENGTHQ2 (v);
        ///vSUB2 (v, edges [ 9][0], edges [ 9][1]);      lengths [ 9] = vLENGTHQ2 (v);
        ///vSUB2 (v, edges [10][0], edges [10][1]);      lengths [10] = vLENGTHQ2 (v);
        ///vSUB2 (v, edges [11][0], edges [11][1]);      lengths [11] = vLENGTHQ2 (v);

        ////////////////////////////////////////////
        // SORTING EDGES
        ////////////////////////////////////////////

        FLOAT_64  tmp;
        FLOAT_64P tmpp;

        /// CHANGE TO INSERT SORT !!!!!!!!!!!!!!!!!!!!!!!

        ///for (n = 0; n < 12; n ++) {
        for (n = 0; n < 8; n ++) {

            ///for (m = n + 1; m < 12; m ++) {
            for (m = n + 1; m < 8; m ++) {

                if (lengths [m] > lengths [n]) {

                    tmpp =  edges [n][0];
                            edges [n][0] =  edges [m][0];
                                            edges [m][0] = tmpp;
                    tmpp =  edges [n][1];
                            edges [n][1] =  edges [m][1];
                                            edges [m][1] = tmpp;
                    tmp  =  lengths [n];
                            lengths [n] =   lengths [m];
                                            lengths [m] = tmp;
                }
            }
        }

        // init
        dr_sun_split_planesc [i] = 0;

        ////////////////////////////////////////////
        // CHECKING ALL EDGES
        ////////////////////////////////////////////

        ///for (n = 0; n < 12; n ++) {
        for (n = 0; n < 8; n ++) {

            // too short to mess with ?
            if (lengths [n] < 0.1) {

                int ii = 0;
                break;
            }

            ///
            FLOAT_64 A, B, l, vx, vy;

            // vector perpendicular to edge
            A =   (edges [n][1])[1] - (edges [n][0])[1];
            B = - (edges [n][1])[0] + (edges [n][0])[0];

            /// normalizing
            l = sqrt (A*A + B*B);       A /= l;     B /= l;

            vx = originprj [0] - (edges [n][0])[0];
            vy = originprj [1] - (edges [n][0])[1];

            /// normalizing
            l = sqrt (vx*vx + vy*vy);   vx /= l;    vy /= l;

            // has the same side of plane ?     (dot product)
            if ((vx*A + vy*B) < 0) {

                // inverting normal
                A = - A;    B = - B;
            }

            FLOAT_64 d = - A*(edges [n][0])[0] - B*(edges [n][0])[1];

            // all points at the same side of plane ?
            if ((((A * pointst[0][0]) + (B * pointst[0][1]) + d) > - 0.0001) && \
                (((A * pointst[1][0]) + (B * pointst[1][1]) + d) > - 0.0001) && \
                (((A * pointst[2][0]) + (B * pointst[2][1]) + d) > - 0.0001) && \
                (((A * pointst[3][0]) + (B * pointst[3][1]) + d) > - 0.0001) && \
                (((A * pointst[4][0]) + (B * pointst[4][1]) + d) > - 0.0001) && \
                (((A * pointst[5][0]) + (B * pointst[5][1]) + d) > - 0.0001) && \
                (((A * pointst[6][0]) + (B * pointst[6][1]) + d) > - 0.0001) && \
                (((A * pointst[7][0]) + (B * pointst[7][1]) + d) > - 0.0001)) {

                ////////////////////////////////////////////
                // RECONSTRUCTING CLIP PLANE
                ////////////////////////////////////////////

                FLOAT_64 point [3], u [4];

                // TRANSFORMING BACK EDGE POINT

                FLOAT_64P p1 = (edges [n][0]);
                FLOAT_64P p2 = (edges [n][1]);

                // unprojecting back to sun view space
                u [0] = (p1 [0] + p2 [0]) * 0.5 * side;  
                u [1] = (p1 [1] + p2 [1]) * 0.5 * side;  u [2] = 0.0;

                ///FLOAT_64P p = (edges [n][0]);

                // unprojecting back to sun view space
                ///u [0] = p [0] * side;
                ///u [1] = p [1] * side;
                ///u [2] = 0.0;

                // rotating back to world space
                vTRANSFORM_ROW4x4v3 (point, u, matview);

                // post translation (not from matrix)
                point [0] += (origin [0] - sdist * axisx [2]);
                point [1] += (origin [1] - sdist * axisy [2]);
                point [2] += (origin [2] - sdist * axisz [2]);

                // TRANSFORMING BACK EDGE NORMAL VECTOR

                FLOAT_64 v [3] = { A, B, 0.0 };

                // unprojecting back to sun view space
                v [0] *= side;
                v [1] *= side;

                // rotating back to world space
                vTRANSFORM_ROW4x4v3 (u, v, matview);

                /// normalize
                l = vLENGTH3 (u);   vNORMALIZE3 (u, l);

                // d param
                u [3] = - u [0] * point [0] - u [1] * point [1] - u [2] * point [2];

                // shortcut
                UINT_32 index = dr_sun_split_planesc [i];

                // clipping plane
                dr_sun_split_planesd [i][index][0] = u [0];
                dr_sun_split_planesd [i][index][1] = u [1];
                dr_sun_split_planesd [i][index][2] = u [2];
                dr_sun_split_planesd [i][index][3] = u [3];

                dr_sun_split_planes [i][index][0] = (FLOAT_32) u [0];
                dr_sun_split_planes [i][index][1] = (FLOAT_32) u [1];
                dr_sun_split_planes [i][index][2] = (FLOAT_32) u [2];
                dr_sun_split_planes [i][index][3] = (FLOAT_32) u [3];

                dr_sun_split_planesc [i] ++;
            }
        }

        ////////////////////////////////////////////
        // ADDITIONAL OUTWARD PLANES
        ////////////////////////////////////////////

        u [0] = dr_camdir [0];
        u [1] = dr_camdir [1];
        u [2] = dr_camdir [2];

        if (vDOT3 (u, axisz) < 0) {

            UINT_32 index = dr_sun_split_planesc [i];

            FLOAT_64 D = - u [0] * nearp [0] - u [1] * nearp [1] - u [2] * nearp [2];

            dr_sun_split_planesd [i][index][0] = u [0];
            dr_sun_split_planesd [i][index][1] = u [1];
            dr_sun_split_planesd [i][index][2] = u [2];
            dr_sun_split_planesd [i][index][3] = D;

            dr_sun_split_planes [i][index][0] = (FLOAT_32) u [0];
            dr_sun_split_planes [i][index][1] = (FLOAT_32) u [1];
            dr_sun_split_planes [i][index][2] = (FLOAT_32) u [2];
            dr_sun_split_planes [i][index][3] = (FLOAT_32) D;

            dr_sun_split_planesc [i] ++;
        }

        u [0] = - dr_camdir [0];
        u [1] = - dr_camdir [1];
        u [2] = - dr_camdir [2];

        if (vDOT3 (u, axisz) < 0) {

            UINT_32 index = dr_sun_split_planesc [i];

            FLOAT_64 D = - u [0] * farp [0] - u [1] * farp [1] - u [2] * farp [2];

            dr_sun_split_planesd [i][index][0] = u [0];
            dr_sun_split_planesd [i][index][1] = u [1];
            dr_sun_split_planesd [i][index][2] = u [2];
            dr_sun_split_planesd [i][index][3] = D;

            dr_sun_split_planes [i][index][0] = (FLOAT_32) u [0];
            dr_sun_split_planes [i][index][1] = (FLOAT_32) u [1];
            dr_sun_split_planes [i][index][2] = (FLOAT_32) u [2];
            dr_sun_split_planes [i][index][3] = (FLOAT_32) D;

            dr_sun_split_planesc [i] ++;
        }
    }
}

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// dr_SunShadows
////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if 1

VOID    dr_SunShadows ()
{

    // MATRICES

    M_STATE_MATRIXMODE_MODELVIEW;   glPushMatrix ();
    M_STATE_MATRIXMODE_PROJECTION;  glPushMatrix ();

    // STATES

    M_STATE_TEX0_RECT_CLEAR;
    M_STATE_TEX1_RECT_CLEAR;
    M_STATE_TEX2_RECT_CLEAR;

    M_STATE_TEX0_CLEAR;
    M_STATE_TEX1_CLEAR;
    M_STATE_TEX2_CLEAR;
    M_STATE_TEX3_CLEAR;
    M_STATE_TEX4_CLEAR;
    M_STATE_TEX5_CLEAR;
    M_STATE_TEX6_CLEAR;
    M_STATE_TEX7_CLEAR;
    M_STATE_TEX8_CLEAR;
    M_STATE_TEX9_CLEAR;

    M_STATE_ARRAY_COLOR_CLEAR;
    M_STATE_ARRAY_SECONDARY_COLOR_CLEAR;
    M_STATE_ARRAY_NORMAL_CLEAR;
    M_STATE_ARRAY_VERTEX_CLEAR;
    M_STATE_ARRAY_TEX0_COORDS_CLEAR;
    M_STATE_ARRAY_TEX1_COORDS_CLEAR;
    M_STATE_ARRAY_TEX2_COORDS_CLEAR;

    M_STATE_DEPTHTEST_SET;
    M_STATE_DEPTHMASK_SET;

    // SPLIT LOOP

    for (UINT_32 i = 0; i < dr_sun_count; i ++) {

        ////////////////////////////////////////////
        // LOADING MATRICES
        ////////////////////////////////////////////

        M_STATE_MATRIXMODE_PROJECTION;   glLoadMatrixd (dr_sun_projection [i]);
        M_STATE_MATRIXMODE_MODELVIEW;    glLoadMatrixd (dr_sun_view       [i]);

        ////////////////////////////////////////////
        // CLEAR AREA TO RENDER
        ////////////////////////////////////////////

        // target texture
        glFramebufferTexture2DEXT (GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, dr_sunshadows [i], 0);

        #ifdef M_DEBUG
            glClear (GL_DEPTH_BUFFER_BIT);
        #endif

        // clear by frustum geometry
        glUseProgram (0);   M_STATE_CULLFACE_SET;    
        
        glDepthFunc (GL_ALWAYS);    glDepthRange (1.0, 1.0);

        glBegin (GL_QUADS);
            glVertex3dv (&dr_sun_split_points [i][2][0]);   glVertex3dv (&dr_sun_split_points [i][3][0]);   
            glVertex3dv (&dr_sun_split_points [i][1][0]);   glVertex3dv (&dr_sun_split_points [i][0][0]);   // near
            glVertex3dv (&dr_sun_split_points [i][4][0]);   glVertex3dv (&dr_sun_split_points [i][5][0]);   
            glVertex3dv (&dr_sun_split_points [i][7][0]);   glVertex3dv (&dr_sun_split_points [i][6][0]);   // far
            glVertex3dv (&dr_sun_split_points [i][6][0]);   glVertex3dv (&dr_sun_split_points [i][7][0]);   
            glVertex3dv (&dr_sun_split_points [i][3][0]);   glVertex3dv (&dr_sun_split_points [i][2][0]);   // left
            glVertex3dv (&dr_sun_split_points [i][0][0]);   glVertex3dv (&dr_sun_split_points [i][1][0]);   
            glVertex3dv (&dr_sun_split_points [i][5][0]);   glVertex3dv (&dr_sun_split_points [i][4][0]);   // right
            glVertex3dv (&dr_sun_split_points [i][4][0]);   glVertex3dv (&dr_sun_split_points [i][6][0]);   
            glVertex3dv (&dr_sun_split_points [i][2][0]);   glVertex3dv (&dr_sun_split_points [i][0][0]);   // top
            glVertex3dv (&dr_sun_split_points [i][1][0]);   glVertex3dv (&dr_sun_split_points [i][3][0]);   
            glVertex3dv (&dr_sun_split_points [i][7][0]);   glVertex3dv (&dr_sun_split_points [i][5][0]);   // bottom
        glEnd ();

        glDepthFunc (GL_LESS);      glDepthRange (0.0, 1.0);

        dr_state_shader = 0;

        // dont forget to disable planes

        UINT_32 c, plane = 0;

        ////////////////////////////////////////////
        // SET CLIP PLANE
        ////////////////////////////////////////////

        // Note :
        //      we have to set matrices before setting clip planes
        //      clip planes are transformed right when glClipPlane is called.

        c = MIN (dr_sun_frustum_planesc, 2);
        for (UINT_32 j = 0; j < c; j ++) {

            M_STATE_PLANE_SET (plane);
            glClipPlane	(GL_CLIP_PLANE0 + plane, &dr_sun_frustum_planesd [j][0]);    plane ++;
        }

        c = MIN (dr_sun_split_planesc [i], 4);
        for (UINT_32 j = 0; j < c; j ++) {

            M_STATE_PLANE_SET (plane);
            glClipPlane	(GL_CLIP_PLANE0 + plane, &dr_sun_split_planesd [i][j][0]);   plane ++;
        }

        // polygon offset applies nonuniform bias to remove projection aliasing
        glEnable (GL_POLYGON_OFFSET_FILL);  glPolygonOffset (2.0, 100.0);

        // render split objects
        dr_DrawShadows (i);

        // offset off
        glDisable (GL_POLYGON_OFFSET_FILL);

        // planes off
        for (UINT_32 j = 0; j < plane; j ++) { M_STATE_PLANE_CLEAR (j); }
    }

    // RESTORE

    for (UINT_32 j = 0; j < 6; j ++) { M_STATE_PLANE_CLEAR (j); }

    M_STATE_MATRIXMODE_PROJECTION;	glPopMatrix ();
    M_STATE_MATRIXMODE_MODELVIEW;	glPopMatrix ();
}

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// dr_SunDraw
////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if 1

VOID    dr_SunDraw ()
{
    M_STATE_DEPTHTEST_CLEAR;
    M_STATE_DEPTHMASK_CLEAR;

    glUseProgram (dr_program_sun);

    // shadowmaps
    M_STATE_TEX0_RECT_SET   (dr_G1);
    M_STATE_TEX1_RECT_SET   (dr_G2);
    M_STATE_TEX2_SET        (dr_rand);
    M_STATE_TEX3_SET        (dr_sunshadows [0]);
    M_STATE_TEX4_SET        (dr_sunshadows [1]);
    M_STATE_TEX5_SET        (dr_sunshadows [2]);
    M_STATE_TEX6_SET        (dr_sunshadows [3]);
    M_STATE_TEX7_SET        (dr_sunshadows [4]);
    M_STATE_TEX8_CLEAR;
    M_STATE_TEX9_CLEAR;

    // load sun matrices
    glUniformMatrix4fv (dr_program_sun_matrix, M_DR_SUN_SPLITS, false, &dr_sun_matrices[0][0]);

    // draw
    glCallList (dr_quad);

    // BACK

    M_STATE_DEPTHTEST_SET;
    M_STATE_DEPTHMASK_SET;

    glDepthRange (0.0, 1.0);    glColor3f (1.0, 1.0, 1.0);
}

#endif

