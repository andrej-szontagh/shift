
#ifndef _RENDERER_CLIP_H
#define _RENDERER_CLIP_H

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FUNCTIONS
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

VOID dr_ClipInit            ();
VOID dr_ClipAdd             (UINT_32 object);
VOID dr_ClipAddRecursive    (TNode * node, UINT_32 object);

VOID dr_ClipViewRecursive           (TNode * node, UINT_8 clip);
VOID dr_ClipViewNodesRecursive      (TNode * node, UINT_8 clip);
VOID dr_ClipShadowRecursive         (TNode * node, UINT_8 clip);
VOID dr_ClipShadowNodesRecursive    (TNode * node, UINT_8 clip);
VOID dr_ClipCombinedRecursive       (TNode * node, UINT_8 view, UINT_8 shadow);

VOID dr_ClipUpdateBoundary      (TBoundary * boundary, FLOAT_32P boxmin, FLOAT_32P boxmax, FLOAT_32P mat);
VOID dr_ClipCompleteBoundary    (TBoundary * boundary);

VOID dr_ClipUpdate ();

VOID dr_ClipInitNode (
                TNode * node, 
                TNode * parent, 
                                    FLOAT_32 xmin, FLOAT_32 ymin, FLOAT_32 zmin, 
                                    FLOAT_32 xmax, FLOAT_32 ymax, FLOAT_32 zmax
    );

VOID  dr_ClipDefrag          ();
VOIDP dr_ClipDefragRecursive (TNode * node, UINT_32 &index);

#endif