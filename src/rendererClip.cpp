
////////////////////////////////////////////////////////////////////////////////////////////////////////////
// dr_ClipInit
////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if 1

VOID dr_ClipInit  (

    )
{

    // initializing root node
    dr_tree.root.depth = 0;
    dr_tree.root.chain = NULL;

    dr_tree.root.childs [0] = NULL;
    dr_tree.root.childs [1] = NULL;
    dr_tree.root.childs [2] = NULL;
    dr_tree.root.childs [3] = NULL;
    dr_tree.root.childs [4] = NULL;
    dr_tree.root.childs [5] = NULL;
    dr_tree.root.childs [6] = NULL;
    dr_tree.root.childs [7] = NULL;

    // transforming bounding into big cube (for better oct-tree volume representation)
    FLOAT_32 v [3] = {  dr_world_max [0] - dr_world_min [0],
                        dr_world_max [1] - dr_world_min [1],
                        dr_world_max [2] - dr_world_min [2] };

    // size of whole tree (cube side)
    FLOAT_32    clip_size = MAX (v [0], v [1]); clip_size = MAX (clip_size, v [2]);

    // setting half size
    FLOAT_32    clip_sizeh = clip_size * 0.5f;

    // root node size
    dr_tree.size  = clip_size;
    dr_tree.sizeh = clip_sizeh;

    // new cube bounding box
    dr_tree.root.boundary.boxmin [0] = dr_world_min [0];
    dr_tree.root.boundary.boxmin [1] = dr_world_min [1];
    dr_tree.root.boundary.boxmin [2] = dr_world_min [2];

    dr_tree.root.boundary.boxmax [0] = dr_world_min [0] + clip_size;
    dr_tree.root.boundary.boxmax [1] = dr_world_min [1] + clip_size;
    dr_tree.root.boundary.boxmax [2] = dr_world_min [2] + clip_size;

    // center
    dr_tree.root.boundary.center [0] = dr_world_min [0] + clip_size * 0.5f;
    dr_tree.root.boundary.center [1] = dr_world_min [1] + clip_size * 0.5f;
    dr_tree.root.boundary.center [2] = dr_world_min [2] + clip_size * 0.5f;

    // sphere bounding
    FLOAT_32 u [3] = {  dr_tree.root.boundary.center [0] - dr_tree.root.boundary.boxmax [0],
                        dr_tree.root.boundary.center [1] - dr_tree.root.boundary.boxmax [1],
                        dr_tree.root.boundary.center [2] - dr_tree.root.boundary.boxmax [2] };

    dr_tree.root.boundary.sphere = vLENGTH3 (u);
}

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// dr_ClipAdd
////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if 1

VOID dr_ClipAdd (UINT_32 object)
{
    // shortcut
    FLOAT_32 clip_sizeh = dr_tree.sizeh;

    // evaluating oct-tree subdivision level for this object
    UINT_8 depth = (UINT_8) floor (log (clip_sizeh / MIN (clip_sizeh, dr_object_boundaries [object].sphere)) / log (2.0f));     // log2 = log (x) / log (2)

    // adding resolution factor and clamping to max depth
    depth = MIN (depth + 1, M_DR_TREE_MAX_DEPTH);

    // NOTE: every object can use max 8 nodes

    // save depth
    dr_object_depths [object] = depth;

    // shortcut
    TObject * obj = &dr_objects [object];

    // node ref.
    obj->nodes [0] = NULL;
    obj->nodes [1] = NULL;
    obj->nodes [2] = NULL;
    obj->nodes [3] = NULL;
    obj->nodes [4] = NULL;
    obj->nodes [5] = NULL;
    obj->nodes [6] = NULL;
    obj->nodes [7] = NULL;

    obj->nodesc = 0;

    // find node / create / subdivide and attach object to the list
    dr_ClipAddRecursive  (&dr_tree.root, object);
}

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// dr_ClipInitNode
////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if 1

VOID dr_ClipInitNode  (TNode *node, TNode *parent, FLOAT_32 xmin, FLOAT_32 ymin, FLOAT_32 zmin, FLOAT_32 xmax, FLOAT_32 ymax, FLOAT_32 zmax)
{
    node->depth     = parent->depth + 1;
    node->chain     = NULL;

    node->childs [0] = NULL;
    node->childs [1] = NULL;
    node->childs [2] = NULL;
    node->childs [3] = NULL;
    node->childs [4] = NULL;
    node->childs [5] = NULL;
    node->childs [6] = NULL;
    node->childs [7] = NULL;

    node->boundary.boxmin [0] = xmin;
    node->boundary.boxmin [1] = ymin;
    node->boundary.boxmin [2] = zmin;
    node->boundary.boxmax [0] = xmax;
    node->boundary.boxmax [1] = ymax;
    node->boundary.boxmax [2] = zmax;

    node->boundary.center [0] = (xmin + xmax) * 0.5f;
    node->boundary.center [1] = (ymin + ymax) * 0.5f;
    node->boundary.center [2] = (zmin + zmax) * 0.5f;

    FLOAT_32 v [3] = {  node->boundary.center [0] - node->boundary.boxmax [0],
                        node->boundary.center [1] - node->boundary.boxmax [1],
                        node->boundary.center [2] - node->boundary.boxmax [2]   };

    node->boundary.sphere = vLENGTH3 (v);
}

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// dr_ClipDefragRecursive
////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if 1

VOIDP dr_ClipDefragRecursive (TNode *node, UINT_32 &indexn, UINT_32 &indexc)
{

    TChain ** ch = &node->chain;

    while (*ch) {

        // copy node in order of nodes traversal
        memcpy ((VOIDP) &dr_chainstmp [indexc], (VOIDP) *ch, sizeof (TChain));

        *ch = &dr_chainstmp [indexc ++];

        ch  = &((*ch)->prev);
    }

    // copy node in order of nodes traversal
    memcpy ((VOIDP) &dr_nodestmp [indexn], (VOIDP) node, sizeof (TNode));
    
    // set new node address
    node = &dr_nodestmp [indexn ++];
    
    // shortcut
    TNode ** childs = node->childs;

    // childs ..
    if (childs [0]) childs [0] = (TNode *) dr_ClipDefragRecursive (childs [0], indexn, indexc);
    if (childs [1]) childs [1] = (TNode *) dr_ClipDefragRecursive (childs [1], indexn, indexc);
    if (childs [2]) childs [2] = (TNode *) dr_ClipDefragRecursive (childs [2], indexn, indexc);
    if (childs [3]) childs [3] = (TNode *) dr_ClipDefragRecursive (childs [3], indexn, indexc);
    if (childs [4]) childs [4] = (TNode *) dr_ClipDefragRecursive (childs [4], indexn, indexc);
    if (childs [5]) childs [5] = (TNode *) dr_ClipDefragRecursive (childs [5], indexn, indexc);
    if (childs [6]) childs [6] = (TNode *) dr_ClipDefragRecursive (childs [6], indexn, indexc);
    if (childs [7]) childs [7] = (TNode *) dr_ClipDefragRecursive (childs [7], indexn, indexc);

    return ((VOIDP) node);
}

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// dr_ClipDefrag
////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if 1

VOID dr_ClipDefrag ()
{

    // allocate
    dr_nodestmp     = (TNode  *) malloc (sizeof (TNode)  * M_RESERVED_NODES);
    dr_chainstmp    = (TChain *) malloc (sizeof (TChain) * M_RESERVED_CHAINS);

    // init
    UINT_32 indexn = 0;
    UINT_32 indexc = 0;
    
    // COPY NODES RECURSIVELY IN RIGHT ORDER

    // root node is not taken cause it is a part of dr_tree structure

    // shortcut
    TNode ** childs = dr_tree.root.childs;

    // childs ..
    if (childs [0]) childs [0] = (TNode *) dr_ClipDefragRecursive (childs [0], indexn, indexc);
    if (childs [1]) childs [1] = (TNode *) dr_ClipDefragRecursive (childs [1], indexn, indexc);
    if (childs [2]) childs [2] = (TNode *) dr_ClipDefragRecursive (childs [2], indexn, indexc);
    if (childs [3]) childs [3] = (TNode *) dr_ClipDefragRecursive (childs [3], indexn, indexc);
    if (childs [4]) childs [4] = (TNode *) dr_ClipDefragRecursive (childs [4], indexn, indexc);
    if (childs [5]) childs [5] = (TNode *) dr_ClipDefragRecursive (childs [5], indexn, indexc);
    if (childs [6]) childs [6] = (TNode *) dr_ClipDefragRecursive (childs [6], indexn, indexc);
    if (childs [7]) childs [7] = (TNode *) dr_ClipDefragRecursive (childs [7], indexn, indexc);

    // swap nodes
    TNode  * tmpn = dr_nodes;
                    dr_nodes =  dr_nodestmp;
                                dr_nodestmp = tmpn;

    // swap chains
    TChain * tmpc = dr_chains;
                    dr_chains = dr_chainstmp;
                                dr_chainstmp = tmpc;

    // release
    free (dr_chainstmp);
    free (dr_nodestmp);
}

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// dr_ClipAddRecursive
////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if 1

VOID dr_ClipAddRecursive (TNode *node, UINT_32 object)
{

    // Can go further ?
    if (node->depth < dr_object_depths [object]) {

        /////////////////////////////////////////////
        // SUBDIVIDE
        /////////////////////////////////////////////

        FLOAT_32P objboxmin = dr_object_boundaries [object].boxmin;
        FLOAT_32P objboxmax = dr_object_boundaries [object].boxmax;

        FLOAT_32P boxmin  = node->boundary.boxmin;
        FLOAT_32P boxmax  = node->boundary.boxmax;
        FLOAT_32P center  = node->boundary.center;

        BOOL x_positive = !(objboxmax [0] < center [0]);
        BOOL x_negative = !(objboxmin [0] > center [0]);

        BOOL y_positive = !(objboxmax [1] < center [1]);
        BOOL y_negative = !(objboxmin [1] > center [1]);

        BOOL z_positive = !(objboxmax [2] < center [2]);
        BOOL z_negative = !(objboxmin [2] > center [2]);

        // Legend :

        // 0 = -X -Y -Z
        // 1 = +X -Y -Z
        // 2 = -X +Y -Z
        // 3 = +X +Y -Z
        // 4 = -X -Y +Z
        // 5 = +X -Y +Z
        // 6 = -X +Y +Z
        // 7 = +X +Y +Z

        if (z_positive) {
            if (y_positive) {
                if (x_positive) {
                    if (!node->childs [7])  dr_ClipInitNode  (node->childs [7] = &dr_nodes [dr_nodesc ++], node, 
                                                             center [0], center [1], center [2], 
                                                             boxmax [0], boxmax [1], boxmax [2]);
                                            dr_ClipAddRecursive  (node->childs [7], object);
                }
                if (x_negative) {
                    if (!node->childs [6])  dr_ClipInitNode  (node->childs [6] = &dr_nodes [dr_nodesc ++], node,
                                                             boxmin [0], center [1], center [2], 
                                                             center [0], boxmax [1], boxmax [2]);
                                            dr_ClipAddRecursive  (node->childs [6], object);
                }
            }
            if (y_negative) {
                if (x_positive) {
                    if (!node->childs [5])  dr_ClipInitNode  (node->childs [5] = &dr_nodes [dr_nodesc ++], node,
                                                             center [0], boxmin [1], center [2], 
                                                             boxmax [0], center [1], boxmax [2]);
                                            dr_ClipAddRecursive  (node->childs [5], object);
                }
                if (x_negative) {
                    if (!node->childs [4])  dr_ClipInitNode  (node->childs [4] = &dr_nodes [dr_nodesc ++], node,
                                                             boxmin [0], boxmin [1], center [2], 
                                                             center [0], center [1], boxmax [2]);
                                            dr_ClipAddRecursive  (node->childs [4], object);
                }
            }
        }
        if (z_negative) {
            if (y_positive) {
                if (x_positive) {
                    if (!node->childs [3])  dr_ClipInitNode  (node->childs [3] = &dr_nodes [dr_nodesc ++], node,
                                                             center [0], center [1], boxmin [2], 
                                                             boxmax [0], boxmax [1], center [2]);
                                            dr_ClipAddRecursive  (node->childs [3], object);
                }
                if (x_negative) {
                    if (!node->childs [2])  dr_ClipInitNode  (node->childs [2] = &dr_nodes [dr_nodesc ++], node,
                                                             boxmin [0], center [1], boxmin [2], 
                                                             center [0], boxmax [1], center [2]);
                                            dr_ClipAddRecursive  (node->childs [2], object);
                }
            }
            if (y_negative) {
                if (x_positive) {
                    if (!node->childs [1])  dr_ClipInitNode  (node->childs [1] = &dr_nodes [dr_nodesc ++], node,
                                                             center [0], boxmin [1], boxmin [2], 
                                                             boxmax [0], center [1], center [2]);
                                            dr_ClipAddRecursive  (node->childs [1], object);
                }
                if (x_negative) {
                    if (!node->childs [0])  dr_ClipInitNode  (node->childs [0] = &dr_nodes [dr_nodesc ++], node,
                                                             boxmin [0], boxmin [1], boxmin [2], 
                                                             center [0], center [1], center [2]);
                                            dr_ClipAddRecursive  (node->childs [0], object);
                }
            }
        }

    } else {

        /////////////////////////////////////////////
        // USING THIS NODE
        /////////////////////////////////////////////

        // REFERENCE TO NODE

        TChain * cho = node->chain;

        if (cho) {

            UINT_32 count = cho->count;

            if (count < M_CHAIN) { cho->list [count] = object; cho->count ++;
            } else {
                
                cho = &dr_chains [dr_chainsc ++];

                cho->count = 1; cho->list [0] = object;
                cho->next = NULL; cho->prev = node->chain; node->chain->next = cho; node->chain = cho;
            }
        } else {

            cho = &dr_chains [dr_chainsc ++];

            cho->count = 1; cho->list [0] = object; 
            cho->next = NULL; cho->prev = NULL; node->chain = cho;
        }

        // REFERENCE TO OBJECT

        TObject * obj = &dr_objects [object];   obj->nodes [obj->nodesc ++] = node;
    }
}

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// dr_ClipView
////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if 1

UINT_8 dr_ClipView (
                     
                TBoundary * boundary
    )
{
    FLOAT_32P center = boundary->center;

    FLOAT_32 cx = center [0];
    FLOAT_32 cy = center [1];
    FLOAT_32 cz = center [2];

    FLOAT_32 sphere = boundary->sphere;

    // center distance test
    for (UINT_32 i = 0; i < 4; i ++) {

        // shortcut
        register FLOAT_32P p = &dr_frustum_planes [i][0];

        // nominator
        register FLOAT_32 nom = p[0]*cx + p[1]*cy + p[2]*cz + p[3];

        // sphere test
        if (abs (nom) > sphere) {

            // outside - we can skip all
            if (nom < 0.0) return M_CLIP_OUTSIDE;

        // sphere colliding
        } else {

            while ((++ i) < 4) {

                // shortcut
                p = &dr_frustum_planes [i][0];

                // sphere test
                if ((- p[0] * cx - p[1] * cy - p[2] * cz - p[3]) > sphere)  return M_CLIP_OUTSIDE;
            }

            FLOAT_32P boxmin = boundary->boxmin;
            FLOAT_32P boxmax = boundary->boxmax;

            FLOAT_32 x1 = boxmin [0];
            FLOAT_32 y1 = boxmin [1];
            FLOAT_32 z1 = boxmin [2];

            FLOAT_32 x2 = boxmax [0];
            FLOAT_32 y2 = boxmax [1];
            FLOAT_32 z2 = boxmax [2];

            // testing all planes 
            BOOL colliding = false;     i = 0;

            do {

                register FLOAT_32 A =   dr_frustum_planes [i][0];
                register FLOAT_32 B =   dr_frustum_planes [i][1];
                register FLOAT_32 C =   dr_frustum_planes [i][2];
                register FLOAT_32 D = - dr_frustum_planes [i][3];

                register UINT_8 outstat;

                outstat  = ((A*x1 + B*y1 + C*z1) < D);
                outstat |= ((A*x2 + B*y1 + C*z1) < D) ? 0x02 : 0x00;
                outstat |= ((A*x1 + B*y2 + C*z1) < D) ? 0x04 : 0x00;
                outstat |= ((A*x2 + B*y2 + C*z1) < D) ? 0x08 : 0x00;
                outstat |= ((A*x1 + B*y1 + C*z2) < D) ? 0x10 : 0x00;
                outstat |= ((A*x2 + B*y1 + C*z2) < D) ? 0x20 : 0x00;
                outstat |= ((A*x1 + B*y2 + C*z2) < D) ? 0x40 : 0x00;
                outstat |= ((A*x2 + B*y2 + C*z2) < D) ? 0x80 : 0x00;

                // all point are outside of this plane - we can skip all
                if (outstat == 0xff)   return M_CLIP_OUTSIDE;

                // some points are outside (we know it is not inside)
                if (!colliding) colliding = (outstat != 0x00);

            } while ((++ i) < 4);

            if (colliding)  return (M_CLIP_COLLIDING);   else
                            return (M_CLIP_INSIDE);
        }
    }

    return (M_CLIP_INSIDE);
}

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// dr_ClipShadow
////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if 1

UINT_8 dr_ClipShadow (
                     
                TBoundary * boundary
    )
{
    FLOAT_32P center = boundary->center;

    FLOAT_32 cx = center [0];
    FLOAT_32 cy = center [1];
    FLOAT_32 cz = center [2];

    FLOAT_32 sphere = boundary->sphere;

    UINT_8 r = M_CLIP_INSIDE;

    FLOAT_32P boxmin;
    FLOAT_32P boxmax;

    FLOAT_32 x1, x2;
    FLOAT_32 y1, y2;
    FLOAT_32 z1, z2;

    //////////////////////////////////////////////
    // WHOLE FRUSTUM
    //////////////////////////////////////////////

    INT_32 c = dr_sun_frustum_planesc - 1;
    
    BOOL init = false;

    // somethimes there is no planes so we skip this code, otherwise this code is invalid
    if (c >= 0) {

        // center distance test first
        INT_32 i = c;

        do {

            // shortcut
            register FLOAT_32P p = &dr_sun_frustum_planes [i][0];

            // nominator
            register FLOAT_32 nom = p[0] * cx + p[1] * cy + p[2] * cz + p[3];

            // sphere test
            if (abs (nom) > sphere) {

                // outside - we can skip all
                if (nom < 0.0) return M_CLIP_OUTSIDE;

            // sphere colliding
            } else {

                while ((-- i) >= 0) {

                    // shortcut
                    p = &dr_sun_frustum_planes [i][0];

                    // sphere test
                    if ((- p[0] * cx - p[1] * cy - p[2] * cz - p[3]) > sphere)  return M_CLIP_OUTSIDE;
                }

                ///
                r = M_CLIP_COLLIDING;

                /*
                boxmin = boundary->boxmin;
                boxmax = boundary->boxmax;

                x1 = boxmin [0];    x2 = boxmax [0];
                y1 = boxmin [1];    y2 = boxmax [1];
                z1 = boxmin [2];    z2 = boxmax [2];

                init = true;

                // testing all planes 
                BOOL colliding = false;     i = c;

                do {

                    register FLOAT_32 A =   dr_sun_frustum_planes [i][0];
                    register FLOAT_32 B =   dr_sun_frustum_planes [i][1];
                    register FLOAT_32 C =   dr_sun_frustum_planes [i][2];
                    register FLOAT_32 D = - dr_sun_frustum_planes [i][3];

                    register UINT_8 outstat;

                    outstat  = ((A*x1 + B*y1 + C*z1) < D);
                    outstat |= ((A*x2 + B*y1 + C*z1) < D) ? 0x02 : 0x00;
                    outstat |= ((A*x1 + B*y2 + C*z1) < D) ? 0x04 : 0x00;
                    outstat |= ((A*x2 + B*y2 + C*z1) < D) ? 0x08 : 0x00;
                    outstat |= ((A*x1 + B*y1 + C*z2) < D) ? 0x10 : 0x00;
                    outstat |= ((A*x2 + B*y1 + C*z2) < D) ? 0x20 : 0x00;
                    outstat |= ((A*x1 + B*y2 + C*z2) < D) ? 0x40 : 0x00;
                    outstat |= ((A*x2 + B*y2 + C*z2) < D) ? 0x80 : 0x00;

                    // all point are outside of this plane - we can skip all
                    if (outstat == 0xff)   return M_CLIP_OUTSIDE;

                    // some points are outside (we know it is not inside)
                    if (!colliding) colliding = (outstat != 0x00);

                } while ((-- i) >= 0);

                if (colliding)  r = M_CLIP_COLLIDING;   else
                                r = M_CLIP_INSIDE;
              */
                break;
            }

        }   while ((-- i) >= 0);
    }

    if (!init) {

        boxmin = boundary->boxmin;
        boxmax = boundary->boxmax;

        x1 = boxmin [0];    x2 = boxmax [0];    
        y1 = boxmin [1];    y2 = boxmax [1];
        z1 = boxmin [2];    z2 = boxmax [2];
    }

    //////////////////////////////////////////////
    // SPLITS
    //////////////////////////////////////////////

    UINT_32 passed = 0;

    // split loop
    UINT_32 m = 0;

    do {

        INT_32 c = dr_sun_split_planesc [m] - 1;

        // center distance test first
        INT_32 i = c;

        // NOTE : we assume that each split have minimum 1 plane otherwise this code is invalid

        UINT_32 offset = m * 48;

        do {

            // shortcut
            register FLOAT_32P p = &dr_sun_split_planes [offset + (i << 2)];

            // nominator
            register FLOAT_32 nom = p[0] * cx + p[1] * cy + p[2] * cz + p[3];

            // sphere test
            if (abs (nom) > sphere) {

                // outside
                if (nom < 0.0) {

                    dr_sun_split_clip [m] = M_CLIP_OUTSIDE; 
                    goto next;
                }

            // sphere colliding
            } else {

                // center distance test
                while ((-- i) >= 0) {

                    // shortcut
                    p = &dr_sun_split_planes [offset + (i << 2)];

                    // sphere test
                    if ((- p[0] * cx - p[1] * cy - p[2] * cz - p[3]) > sphere) {
                        
                        dr_sun_split_clip [m] = M_CLIP_OUTSIDE;
                        goto next;
                    }
                }

                /// 
                dr_sun_split_clip [m] = r = M_CLIP_COLLIDING; goto next;

                /*
                // testing all planes 
                BOOL colliding = false;     i = c;

                do {

                    // shortcut
                    FLOAT_32P p = &dr_sun_split_planes [m][i][0];

                    register FLOAT_32 A =   p [0];
                    register FLOAT_32 B =   p [1];
                    register FLOAT_32 C =   p [2];
                    register FLOAT_32 D = - p [3];

                    register UINT_8 outstat;

                    outstat  = ((A*x1 + B*y1 + C*z1) < D);
                    outstat |= ((A*x2 + B*y1 + C*z1) < D) ? 0x02 : 0x00;
                    outstat |= ((A*x1 + B*y2 + C*z1) < D) ? 0x04 : 0x00;
                    outstat |= ((A*x2 + B*y2 + C*z1) < D) ? 0x08 : 0x00;
                    outstat |= ((A*x1 + B*y1 + C*z2) < D) ? 0x10 : 0x00;
                    outstat |= ((A*x2 + B*y1 + C*z2) < D) ? 0x20 : 0x00;
                    outstat |= ((A*x1 + B*y2 + C*z2) < D) ? 0x40 : 0x00;
                    outstat |= ((A*x2 + B*y2 + C*z2) < D) ? 0x80 : 0x00;

                    // all point are outside of this plane
                    if (outstat == 0xff) {

                        dr_sun_split_clip [m] = M_CLIP_OUTSIDE; 
                        goto next;
                    }

                    // some points are outside (we know it is not inside)
                    if (!colliding) colliding = (outstat != 0x00);

                } while ((-- i) >= 0);

                passed ++;

                if (colliding) {

                    // we set result to colliding so we can investigate further
                    dr_sun_split_clip [m] = r = M_CLIP_COLLIDING;
                    goto next;
                }

                // all points are inside of all planes
                dr_sun_split_clip [m] = M_CLIP_INSIDE;
                goto next;
                */
            }

        } while ((-- i) >= 0);

        //  passed ++;

        dr_sun_split_clip [m] = M_CLIP_INSIDE;

        next:;

    } while ((++ m) < dr_control_sun_splits);

    //  if (passed == 0) return (M_CLIP_OUTSIDE);

    return r;
}

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// dr_ClipViewGrabAllRecursive
////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if 1

VOID dr_ClipViewGrabAllRecursive (

                TNode * node
    )
{    
    
    TChain *ch = node->chain;

    while (ch) {

        register UINT_32 count = ch->count;

        for (UINT_32 i = 0; i < count; i ++) {

            register UINT_32 ID = ch->list [i];
            register UINT_16 tags = dr_object_tags [ID];

            if (!(tags &  M_TAG_VIEW)) {
                  tags |= M_TAG_VIEW;

                // distance culling
                M_DISAPPEAR_CULLING (tags);

                // adding to the list
                dr_list_objects_view [dr_list_objects_viewc ++] = ID;
            }
        }

        ch = ch->prev;
    }

    // shortcut
    TNode ** childs = node->childs;

    // just grab all objects
    if (childs [0]) dr_ClipViewGrabAllRecursive (childs [0]);
    if (childs [1]) dr_ClipViewGrabAllRecursive (childs [1]);
    if (childs [2]) dr_ClipViewGrabAllRecursive (childs [2]);
    if (childs [3]) dr_ClipViewGrabAllRecursive (childs [3]);
    if (childs [4]) dr_ClipViewGrabAllRecursive (childs [4]);
    if (childs [5]) dr_ClipViewGrabAllRecursive (childs [5]);
    if (childs [6]) dr_ClipViewGrabAllRecursive (childs [6]);
    if (childs [7]) dr_ClipViewGrabAllRecursive (childs [7]);
}

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// dr_ClipViewRecursive
////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if 1

VOID dr_ClipViewRecursive (

                TNode * node, UINT_8 clip
    )
{    
    switch (clip) {

        case M_CLIP_OUTSIDE:
            break;

        case M_CLIP_INSIDE: {

                // just grab all objects
                dr_ClipViewGrabAllRecursive (node);
            }
            break;

        case M_CLIP_COLLIDING: {

                // shortcut
                TNode ** childs = node->childs;

                if (childs [0]) dr_ClipViewRecursive (childs [0], dr_ClipView (&childs [0]->boundary));
                if (childs [1]) dr_ClipViewRecursive (childs [1], dr_ClipView (&childs [1]->boundary));
                if (childs [2]) dr_ClipViewRecursive (childs [2], dr_ClipView (&childs [2]->boundary));
                if (childs [3]) dr_ClipViewRecursive (childs [3], dr_ClipView (&childs [3]->boundary));
                if (childs [4]) dr_ClipViewRecursive (childs [4], dr_ClipView (&childs [4]->boundary));
                if (childs [5]) dr_ClipViewRecursive (childs [5], dr_ClipView (&childs [5]->boundary));
                if (childs [6]) dr_ClipViewRecursive (childs [6], dr_ClipView (&childs [6]->boundary));
                if (childs [7]) dr_ClipViewRecursive (childs [7], dr_ClipView (&childs [7]->boundary));

                TChain *ch = node->chain;

                while (ch) {

                    register UINT_32 count = ch->count;

                    for (UINT_32 i = 0; i < count; i ++) {

                        register UINT_32 ID = ch->list [i];
                        register UINT_16 tags = dr_object_tags [ID];

                        if (!(tags & M_TAG_VIEW)) {

                            // distance culling
                            M_DISAPPEAR_CULLING (tags);

                            // clipping test - just collision
                            if (dr_ClipView (&dr_object_boundaries [ID]) != M_CLIP_OUTSIDE) {

                                // adding to the list
                                dr_list_objects_view [dr_list_objects_viewc ++] = ID;
                                
                                dr_object_tags [ID] |= M_TAG_VIEW;
                            }
                        }
                    }

                    ch = ch->prev;
                }
            }
            break;
    }
}

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// dr_ClipShadowGrabAllRecursive
////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if 1

VOID dr_ClipShadowGrabAllRecursive (

                TNode * node
    )
{
    TChain *ch = node->chain;

    while (ch) {

        register UINT_32 count = ch->count;

        if (count == M_CHAIN) {

            for (UINT_32 i = 0; i < M_CHAIN; i ++) {

                register UINT_32 ID = ch->list [i];
                register UINT_16 tags = dr_object_tags [ID];

                // distance culling
                M_DISAPPEAR_CULLING (tags);     /// REMOVE THIS ??

                register UINT_16 splitsc = dr_object_disappear_split [ID];

                // adding to the split lists
                for (UINT_32 j = 0; j < splitsc; j ++) {

                    if ((dr_sun_split_clip [j] != M_CLIP_OUTSIDE) && (!(tags &  (1 << j)))) {
                                                                        tags |= (1 << j);

                        dr_list_objects_shadow_split [j][dr_list_objects_shadow_splitc [j] ++] = ID;
                    }
                }

                dr_object_tags [ID] = tags;
            }

        } else {

            for (UINT_32 i = 0; i < count; i ++) {

                register UINT_32 ID = ch->list [i];
                register UINT_16 tags = dr_object_tags [ID];

                // distance culling
                M_DISAPPEAR_CULLING (tags);     /// REMOVE THIS ??

                register UINT_16 splitsc = dr_object_disappear_split [ID];

                // adding to the split lists
                for (UINT_32 j = 0; j < splitsc; j ++) {

                    if ((dr_sun_split_clip [j] != M_CLIP_OUTSIDE) && (!(tags &  (1 << j)))) {
                                                                        tags |= (1 << j);

                        dr_list_objects_shadow_split [j][dr_list_objects_shadow_splitc [j] ++] = ID;
                    }
                }

                dr_object_tags [ID] = tags;
            }
        }

        ch = ch->prev;
    }

    // shortcut
    TNode ** childs = node->childs;

    // just grab all objects
    if (childs [0]) dr_ClipShadowGrabAllRecursive (childs [0]);
    if (childs [1]) dr_ClipShadowGrabAllRecursive (childs [1]);
    if (childs [2]) dr_ClipShadowGrabAllRecursive (childs [2]);
    if (childs [3]) dr_ClipShadowGrabAllRecursive (childs [3]);
    if (childs [4]) dr_ClipShadowGrabAllRecursive (childs [4]);
    if (childs [5]) dr_ClipShadowGrabAllRecursive (childs [5]);
    if (childs [6]) dr_ClipShadowGrabAllRecursive (childs [6]);
    if (childs [7]) dr_ClipShadowGrabAllRecursive (childs [7]);
}

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// dr_ClipShadowRecursive
////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if 1

VOID dr_ClipShadowRecursive (

                TNode * node, UINT_8 clip
    )
{
    switch (clip) {

        case M_CLIP_OUTSIDE:
            break;

        case M_CLIP_INSIDE: {

                // just grab all objects
                dr_ClipShadowGrabAllRecursive (node);
            }
            break;

        case M_CLIP_COLLIDING:  {

                TChain *ch = node->chain;

                while (ch) {

                    register UINT_32 count = ch->count;

                    for (UINT_32 i = 0; i < count; i ++) {

                        register UINT_32 ID = ch->list [i];
                        register UINT_16 tags = dr_object_tags [ID];

                        if (!(tags & M_TAG_SHADOWSPLIT)) {

                            // distance culling
                            M_DISAPPEAR_CULLING (tags);     /// REMOVE THIS ??

                            register UINT_16 splitsc = dr_object_disappear_split [ID];

                            // clipping test - just collision
                            if (dr_ClipShadow (&dr_object_boundaries [ID]) != M_CLIP_OUTSIDE) {

                                // adding to the split lists
                                for (UINT_32 j = 0; j < splitsc; j ++) {

                                    if ((dr_sun_split_clip [j] != M_CLIP_OUTSIDE) && (!(tags &  (1 << j)))) {
                                                                                        tags |= (1 << j);

                                        dr_list_objects_shadow_split [j][dr_list_objects_shadow_splitc [j] ++] = ID;
                                    }
                                }

                                dr_object_tags [ID] = tags | M_TAG_SHADOWSPLIT;
                            }
                        }
                    }

                    ch = ch->prev;
                }

                // shortcut
                TNode ** childs = node->childs;

                if (childs [0]) dr_ClipShadowRecursive (childs [0], dr_ClipShadow (&childs [0]->boundary));
                if (childs [1]) dr_ClipShadowRecursive (childs [1], dr_ClipShadow (&childs [1]->boundary));
                if (childs [2]) dr_ClipShadowRecursive (childs [2], dr_ClipShadow (&childs [2]->boundary));
                if (childs [3]) dr_ClipShadowRecursive (childs [3], dr_ClipShadow (&childs [3]->boundary));
                if (childs [4]) dr_ClipShadowRecursive (childs [4], dr_ClipShadow (&childs [4]->boundary));
                if (childs [5]) dr_ClipShadowRecursive (childs [5], dr_ClipShadow (&childs [5]->boundary));
                if (childs [6]) dr_ClipShadowRecursive (childs [6], dr_ClipShadow (&childs [6]->boundary));
                if (childs [7]) dr_ClipShadowRecursive (childs [7], dr_ClipShadow (&childs [7]->boundary));
            }
            break;
    }
}

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// dr_ClipCombinedGrabAllRecursive
////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if 1

VOID dr_ClipCombinedGrabAllRecursive (

                TNode * node
    )
{
    TChain *ch = node->chain;

    while (ch) {

        register UINT_32 count = ch->count;

        if (count == M_CHAIN) {

            for (UINT_32 i = 0; i < M_CHAIN; i ++) {

                register UINT_32 ID = ch->list [i];
                register UINT_16 tags = dr_object_tags [ID];

                // distance culling
                M_DISAPPEAR_CULLING (tags);

                if (!(tags &  M_TAG_VIEW)) {
                      tags |= M_TAG_VIEW;

                    // adding to the lists
                    dr_list_objects_view [dr_list_objects_viewc ++] = ID;
                }

                register UINT_16 splitsc = dr_object_disappear_split [ID];

                // adding to the split lists
                for (UINT_32 j = 0; j < splitsc; j ++) {

                    if ((dr_sun_split_clip [j] != M_CLIP_OUTSIDE) && (!(tags &  (1 << j)))) {
                                                                        tags |= (1 << j);

                        dr_list_objects_shadow_split [j][dr_list_objects_shadow_splitc [j] ++] = ID;
                    }
                }

                dr_object_tags [ID] = tags;
            }

        } else {

            for (UINT_32 i = 0; i < count; i ++) {

                register UINT_32 ID = ch->list [i];
                register UINT_16 tags = dr_object_tags [ID];

                // distance culling
                M_DISAPPEAR_CULLING (tags);

                if (!(tags &  M_TAG_VIEW)) {
                      tags |= M_TAG_VIEW;

                    // adding to the lists
                    dr_list_objects_view [dr_list_objects_viewc ++] = ID;
                }

                register UINT_16 splitsc = dr_object_disappear_split [ID];

                // adding to the split lists
                for (UINT_32 j = 0; j < splitsc; j ++) {

                    if ((dr_sun_split_clip [j] != M_CLIP_OUTSIDE) && (!(tags &  (1 << j)))) {
                                                                        tags |= (1 << j);

                        dr_list_objects_shadow_split [j][dr_list_objects_shadow_splitc [j] ++] = ID;
                    }
                }

                dr_object_tags [ID] = tags;
            }
        }

        ch = ch->prev;
    }

    // shortcut
    TNode ** childs = node->childs;

    // continues for shadow only
    if (childs [0]) dr_ClipCombinedGrabAllRecursive (childs [0]);
    if (childs [1]) dr_ClipCombinedGrabAllRecursive (childs [1]);
    if (childs [2]) dr_ClipCombinedGrabAllRecursive (childs [2]);
    if (childs [3]) dr_ClipCombinedGrabAllRecursive (childs [3]);
    if (childs [4]) dr_ClipCombinedGrabAllRecursive (childs [4]);
    if (childs [5]) dr_ClipCombinedGrabAllRecursive (childs [5]);
    if (childs [6]) dr_ClipCombinedGrabAllRecursive (childs [6]);
    if (childs [7]) dr_ClipCombinedGrabAllRecursive (childs [7]);
}

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// dr_ClipCombinedRecursive
////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if 1

VOID dr_ClipCombinedRecursive (

                TNode * node, UINT_8 view, UINT_8 shadow
    )
{
    switch (view) {
        case M_CLIP_OUTSIDE:
            switch (shadow) {
                case M_CLIP_OUTSIDE:                                                                    return;     // nothing
                case M_CLIP_INSIDE:         dr_ClipShadowGrabAllRecursive   (node);                     return;     // just grab all for shadows
                case M_CLIP_COLLIDING:      dr_ClipShadowRecursive          (node, M_CLIP_COLLIDING);   return;     // we test further, shadows only
            }
            break;
        case M_CLIP_INSIDE:

            switch (shadow) {
                case M_CLIP_OUTSIDE:        dr_ClipViewGrabAllRecursive     (node);                     return;     // object is behind behind last split
                case M_CLIP_INSIDE:         dr_ClipCombinedGrabAllRecursive (node);                     return;     // take everything
                case M_CLIP_COLLIDING:

                    TChain *ch = node->chain;

                    while (ch) {

                        register UINT_32 count = ch->count;

                        for (UINT_32 i = 0; i < count; i ++) {

                            register UINT_32 ID = ch->list [i];
                            register UINT_16 tags = dr_object_tags [ID];

                            // distance culling
                            M_DISAPPEAR_CULLING (tags);

                            if (!(tags & M_TAG_SHADOWSPLIT)) {

                                // clipping test - just collision
                                if (dr_ClipShadow (&dr_object_boundaries [ID]) != M_CLIP_OUTSIDE) {

                                    register UINT_16 splitsc = dr_object_disappear_split [ID];

                                    // adding to the split lists
                                    for (UINT_32 j = 0; j < splitsc; j ++) {

                                        if ((dr_sun_split_clip [j] != M_CLIP_OUTSIDE) && (!(tags &  (1 << j)))) {
                                                                                            tags |= (1 << j);

                                            dr_list_objects_shadow_split [j][dr_list_objects_shadow_splitc [j] ++] = ID;
                                        }
                                    }

                                    tags |= M_TAG_SHADOWSPLIT;
                                }
                            }

                            if (!(tags &  M_TAG_VIEW)) {
                                  tags |= M_TAG_VIEW;

                                // adding to the list
                                dr_list_objects_view [dr_list_objects_viewc ++] = ID;
                            }

                            dr_object_tags [ID] = tags;
                        }

                        ch = ch->prev;
                    }

                    // shortcut
                    TNode ** childs = node->childs;

                    if (childs [0]) dr_ClipCombinedRecursive (childs [0], M_CLIP_INSIDE, dr_ClipShadow (&childs [0]->boundary));
                    if (childs [1]) dr_ClipCombinedRecursive (childs [1], M_CLIP_INSIDE, dr_ClipShadow (&childs [1]->boundary));
                    if (childs [2]) dr_ClipCombinedRecursive (childs [2], M_CLIP_INSIDE, dr_ClipShadow (&childs [2]->boundary));
                    if (childs [3]) dr_ClipCombinedRecursive (childs [3], M_CLIP_INSIDE, dr_ClipShadow (&childs [3]->boundary));
                    if (childs [4]) dr_ClipCombinedRecursive (childs [4], M_CLIP_INSIDE, dr_ClipShadow (&childs [4]->boundary));
                    if (childs [5]) dr_ClipCombinedRecursive (childs [5], M_CLIP_INSIDE, dr_ClipShadow (&childs [5]->boundary));
                    if (childs [6]) dr_ClipCombinedRecursive (childs [6], M_CLIP_INSIDE, dr_ClipShadow (&childs [6]->boundary));
                    if (childs [7]) dr_ClipCombinedRecursive (childs [7], M_CLIP_INSIDE, dr_ClipShadow (&childs [7]->boundary));

                    return;
            }
            break;
        case M_CLIP_COLLIDING:

            switch (shadow) {
                case M_CLIP_OUTSIDE:    return;     // this should never happen
                case M_CLIP_INSIDE: {

                    TChain *ch = node->chain;

                    while (ch) {

                        register UINT_32 count = ch->count;

                        for (UINT_32 i = 0; i < count; i ++) {

                            register UINT_32 ID = ch->list [i];
                            register UINT_16 tags = dr_object_tags [ID];

                            // distance culling
                            M_DISAPPEAR_CULLING (tags);

                            if (!(tags & M_TAG_VIEW)) {

                                // clipping test - just collision
                                if (dr_ClipView (&dr_object_boundaries [ID]) != M_CLIP_OUTSIDE)   {

                                    // adding to the list
                                    dr_list_objects_view [dr_list_objects_viewc ++] = ID;

                                    tags |= M_TAG_VIEW;
                                }
                            }

                            register UINT_16 splitsc = dr_object_disappear_split [ID];

                            // adding to the split lists
                            for (UINT_32 j = 0; j < splitsc; j ++) {

                                if ((dr_sun_split_clip [j] != M_CLIP_OUTSIDE) && (!(tags &  (1 << j)))) {
                                                                                    tags |= (1 << j);

                                    dr_list_objects_shadow_split [j][dr_list_objects_shadow_splitc [j] ++] = ID;
                                }
                            }

                            dr_object_tags [ID] = tags;
                        }

                        ch = ch->prev;
                    }

                    // shortcut
                    TNode ** childs = node->childs;

                    if (childs [0]) dr_ClipCombinedRecursive (childs [0], dr_ClipView (&childs [0]->boundary), M_CLIP_INSIDE);
                    if (childs [1]) dr_ClipCombinedRecursive (childs [1], dr_ClipView (&childs [1]->boundary), M_CLIP_INSIDE);
                    if (childs [2]) dr_ClipCombinedRecursive (childs [2], dr_ClipView (&childs [2]->boundary), M_CLIP_INSIDE);
                    if (childs [3]) dr_ClipCombinedRecursive (childs [3], dr_ClipView (&childs [3]->boundary), M_CLIP_INSIDE);
                    if (childs [4]) dr_ClipCombinedRecursive (childs [4], dr_ClipView (&childs [4]->boundary), M_CLIP_INSIDE);
                    if (childs [5]) dr_ClipCombinedRecursive (childs [5], dr_ClipView (&childs [5]->boundary), M_CLIP_INSIDE);
                    if (childs [6]) dr_ClipCombinedRecursive (childs [6], dr_ClipView (&childs [6]->boundary), M_CLIP_INSIDE);
                    if (childs [7]) dr_ClipCombinedRecursive (childs [7], dr_ClipView (&childs [7]->boundary), M_CLIP_INSIDE);

                    return;
                }

                case M_CLIP_COLLIDING: {

                    TChain *ch = node->chain;

                    while (ch) {

                        register UINT_32 count = ch->count;

                        for (UINT_32 i = 0; i < count; i ++) {

                            register UINT_32 ID = ch->list [i];
                            register UINT_16 tags = dr_object_tags [ID];

                            // distance culling
                            M_DISAPPEAR_CULLING (tags);

                            if (!(tags & M_TAG_SHADOWSPLIT)) {

                                // clipping test - just collision
                                if (dr_ClipShadow (&dr_object_boundaries [ID]) != M_CLIP_OUTSIDE) {

                                    register UINT_16 splitsc = dr_object_disappear_split [ID];

                                    // adding to the split lists
                                    for (UINT_32 j = 0; j < splitsc; j ++) {

                                        if (dr_sun_split_clip [j] != M_CLIP_OUTSIDE)

                                            dr_list_objects_shadow_split [j][dr_list_objects_shadow_splitc [j] ++] = ID;
                                    }

                                    tags |= M_TAG_SHADOWSPLIT;
                                }
                            }

                            if (!(tags & M_TAG_VIEW)) {

                                // clipping test - just collision
                                if (dr_ClipView (&dr_object_boundaries [ID]) != M_CLIP_OUTSIDE)   {

                                    // adding to the list
                                    dr_list_objects_view [dr_list_objects_viewc ++] = ID;

                                    tags |= M_TAG_VIEW;
                                }
                            }

                            dr_object_tags [ID] = tags;
                        }

                        ch = ch->prev;
                    }

                    // shortcut
                    TNode ** childs = node->childs;

                    // we need to test childs
                    if (childs [0]) dr_ClipCombinedRecursive (childs [0], dr_ClipView (&childs [0]->boundary), dr_ClipShadow (&childs [0]->boundary));
                    if (childs [1]) dr_ClipCombinedRecursive (childs [1], dr_ClipView (&childs [1]->boundary), dr_ClipShadow (&childs [1]->boundary));
                    if (childs [2]) dr_ClipCombinedRecursive (childs [2], dr_ClipView (&childs [2]->boundary), dr_ClipShadow (&childs [2]->boundary));
                    if (childs [3]) dr_ClipCombinedRecursive (childs [3], dr_ClipView (&childs [3]->boundary), dr_ClipShadow (&childs [3]->boundary));
                    if (childs [4]) dr_ClipCombinedRecursive (childs [4], dr_ClipView (&childs [4]->boundary), dr_ClipShadow (&childs [4]->boundary));
                    if (childs [5]) dr_ClipCombinedRecursive (childs [5], dr_ClipView (&childs [5]->boundary), dr_ClipShadow (&childs [5]->boundary));
                    if (childs [6]) dr_ClipCombinedRecursive (childs [6], dr_ClipView (&childs [6]->boundary), dr_ClipShadow (&childs [6]->boundary));
                    if (childs [7]) dr_ClipCombinedRecursive (childs [7], dr_ClipView (&childs [7]->boundary), dr_ClipShadow (&childs [7]->boundary));

                    return;
                }
            }
    }
}

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// dr_ClipUpdateBoundary
////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if 1

VOID dr_ClipUpdateBoundary (

                TBoundary * boundary, FLOAT_32P boxmin, FLOAT_32P boxmax, FLOAT_32P mat
    )
{
    UINT_32 i;

    FLOAT_32 points [8][3];

    // transforming bounding box points and building new one

    points [0][0] = boxmin [0];   points [0][1] = boxmin [1];   points [0][2] = boxmin [2];
    points [1][0] = boxmax [0];   points [1][1] = boxmin [1];   points [1][2] = boxmin [2];
    points [2][0] = boxmin [0];   points [2][1] = boxmax [1];   points [2][2] = boxmin [2];
    points [3][0] = boxmax [0];   points [3][1] = boxmax [1];   points [3][2] = boxmin [2];
    points [4][0] = boxmin [0];   points [4][1] = boxmin [1];   points [4][2] = boxmax [2];
    points [5][0] = boxmax [0];   points [5][1] = boxmin [1];   points [5][2] = boxmax [2];
    points [6][0] = boxmin [0];   points [6][1] = boxmax [1];   points [6][2] = boxmax [2];
    points [7][0] = boxmax [0];   points [7][1] = boxmax [1];   points [7][2] = boxmax [2];

    boundary->boxmin [0] =   FLT_MAX;
    boundary->boxmin [1] =   FLT_MAX;
    boundary->boxmin [2] =   FLT_MAX;
    boundary->boxmax [0] = - FLT_MAX;
    boundary->boxmax [1] = - FLT_MAX;
    boundary->boxmax [2] = - FLT_MAX;

    for (i = 0; i < 8; i ++) {

        FLOAT_32 point [3];

        pTRANSFORM_COL4x4p3 (point, points [i], mat);

        boundary->boxmin [0] = MIN (boundary->boxmin [0], point [0]);
        boundary->boxmin [1] = MIN (boundary->boxmin [1], point [1]);
        boundary->boxmin [2] = MIN (boundary->boxmin [2], point [2]);

        boundary->boxmax [0] = MAX (boundary->boxmax [0], point [0]);
        boundary->boxmax [1] = MAX (boundary->boxmax [1], point [1]);
        boundary->boxmax [2] = MAX (boundary->boxmax [2], point [2]);
    }

    dr_ClipCompleteBoundary (boundary);
}

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// dr_ClipCompleteBoundary
////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if 1

VOID dr_ClipCompleteBoundary (
                                 
                TBoundary * boundary
    )
{
    // center

    vADD3   (boundary->center, boundary->boxmin, boundary->boxmax);
    vSCALE3 (boundary->center, boundary->center, 0.5f);

    // sphere

    FLOAT_32 v [3];  vSUB3 (v, boundary->center, boundary->boxmax);

    boundary->sphere = vLENGTH3 (v);
}

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// dr_ClipUpdate
////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if 1

VOID    dr_ClipUpdate   (

    )
{
    // FRUSTUM PLANES APROXIMATION USING SPHERES

    FLOAT_64 u [3], v [3], uu [3], vv [3], l;
    
    // vertical factor
    dr_factor = tan (dr_fovyr * 0.5);
    
    // near plane frustum quad up vector length
    dr_nearh = dr_planenear * dr_factor;

    // far plane frustum quad up vector length
    dr_farh  = dr_planefar  * dr_factor;

    // near plane frustum quad side vector length
    dr_nearw = dr_nearh * dr_aspect;

    // far plane frustum quad side vector length
    dr_farw  = dr_farh  * dr_aspect;

    // near plane center point
    FLOAT_64 nearp [3] = {  dr_campos [0] + dr_camdir [0] * dr_planenear,
                            dr_campos [1] + dr_camdir [1] * dr_planenear,
                            dr_campos [2] + dr_camdir [2] * dr_planenear };

    // far plane center point
    FLOAT_64 farp  [3] = {  dr_campos [0] + dr_camdir [0] * dr_planefar,
                            dr_campos [1] + dr_camdir [1] * dr_planefar,
                            dr_campos [2] + dr_camdir [2] * dr_planefar };

    // RIGHT PLANE SPHERE APROXIMATION
    
    // near plane right side center point
    u [0] = nearp [0] + dr_cambi [0] * dr_nearw;
    u [1] = nearp [1] + dr_cambi [1] * dr_nearw;
    u [2] = nearp [2] + dr_cambi [2] * dr_nearw;

    // far plane right side center point
    v [0] = farp  [0] + dr_cambi [0] * dr_farw;
    v [1] = farp  [1] + dr_cambi [1] * dr_farw;
    v [2] = farp  [2] + dr_cambi [2] * dr_farw;

    // right side plane horizontal vector
    vSUB3 (uu, u, v);           l = vLENGTH3 (uu); vNORMALIZE3 (uu, l);

    // perpendicular to right side plane
    vCROSS (vv, uu, dr_camup);  l = vLENGTH3 (vv); vNORMALIZE3 (vv, l);

    // right plane
    dr_frustum_planes [0][0] = (FLOAT_32)    vv [0];
    dr_frustum_planes [0][1] = (FLOAT_32)    vv [1];
    dr_frustum_planes [0][2] = (FLOAT_32)    vv [2];
    dr_frustum_planes [0][3] = (FLOAT_32)( - vv [0] * u [0] - vv [1] * u [1] - vv [2] * u [2]);

    // LEFT PLANE SPHERE APROXIMATION

    u [0] = nearp [0] - dr_cambi [0] * dr_nearw;
    u [1] = nearp [1] - dr_cambi [1] * dr_nearw;
    u [2] = nearp [2] - dr_cambi [2] * dr_nearw;

    v [0] = farp  [0] - dr_cambi [0] * dr_farw;
    v [1] = farp  [1] - dr_cambi [1] * dr_farw;
    v [2] = farp  [2] - dr_cambi [2] * dr_farw;

    vSUB3 (uu, v, u);           l = vLENGTH3 (uu); vNORMALIZE3 (uu, l);
    vCROSS (vv, uu, dr_camup);  l = vLENGTH3 (vv); vNORMALIZE3 (vv, l);

    // left plane
    dr_frustum_planes [1][0] = (FLOAT_32)    vv [0];
    dr_frustum_planes [1][1] = (FLOAT_32)    vv [1];
    dr_frustum_planes [1][2] = (FLOAT_32)    vv [2];
    dr_frustum_planes [1][3] = (FLOAT_32)( - vv [0] * u [0] - vv [1] * u [1] - vv [2] * u [2]);

    // BOTTOM PLANE SPHERE APROXIMATION

    u [0] = nearp [0] - dr_camup [0] * dr_nearh;
    u [1] = nearp [1] - dr_camup [1] * dr_nearh;
    u [2] = nearp [2] - dr_camup [2] * dr_nearh;

    v [0] = farp  [0] - dr_camup [0] * dr_farh;
    v [1] = farp  [1] - dr_camup [1] * dr_farh;
    v [2] = farp  [2] - dr_camup [2] * dr_farh;

    vSUB3 (uu, u, v);
    vCROSS (vv, uu, dr_cambi);  l = vLENGTH3 (vv); vNORMALIZE3 (vv, l);

    // bottom plane
    dr_frustum_planes [2][0] = (FLOAT_32)    vv [0];
    dr_frustum_planes [2][1] = (FLOAT_32)    vv [1];
    dr_frustum_planes [2][2] = (FLOAT_32)    vv [2];
    dr_frustum_planes [2][3] = (FLOAT_32)( - vv [0] * u [0] - vv [1] * u [1] - vv [2] * u [2]);

    // TOP PLANE SPHERE APROXIMATION

    u [0] = nearp [0] + dr_camup [0] * dr_nearh;
    u [1] = nearp [1] + dr_camup [1] * dr_nearh;
    u [2] = nearp [2] + dr_camup [2] * dr_nearh;

    v [0] = farp  [0] + dr_camup [0] * dr_farh;
    v [1] = farp  [1] + dr_camup [1] * dr_farh;
    v [2] = farp  [2] + dr_camup [2] * dr_farh;

    vSUB3 (uu, v, u);
    vCROSS (vv, uu, dr_cambi);  l = vLENGTH3 (vv); vNORMALIZE3 (vv, l);

    // top plane
    dr_frustum_planes [3][0] = (FLOAT_32)    vv [0];
    dr_frustum_planes [3][1] = (FLOAT_32)    vv [1];
    dr_frustum_planes [3][2] = (FLOAT_32)    vv [2];
    dr_frustum_planes [3][3] = (FLOAT_32)( - vv [0] * u [0] - vv [1] * u [1] - vv [2] * u [2]);
}

#endif
