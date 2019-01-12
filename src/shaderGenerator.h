
#ifndef _RENDERER_SHADER_GENERATOR_H
#define _RENDERER_SHADER_GENERATOR_H

#include "types.h"

#define M_SHFLAG_DEPTH                  0x00000001
#define M_SHFLAG_INSTANCED              0x00000002
#define M_SHFLAG_DISSOLVE               0x00000008
#define M_SHFLAG_TRANSPARENT            0x00000010
#define M_SHFLAG_TRANSLUCENT            0x00000020
#define M_SHFLAG_MULTILAYER             0x00000040
#define M_SHFLAG_ANIM_WIND1             0x00000080
#define M_SHFLAG_ANIM_WIND2             0x00000100
#define M_SHFLAG_DEFORM_SHRINK          0x00000200
#define M_SHFLAG_DEFORM_GROW            0x00000400
#define M_SHFLAG_DEFORM_MORPH           0x00000800
#define M_SHFLAG_VERTEX_COLORS          0x00001000
#define M_SHFLAG_DOUBLESIDE             0x00002000
#define M_SHFLAG_COMPOSITE              0x00004000
#define M_SHFLAG_UV1_PRECISION_HI       0x00008000
#define M_SHFLAG_UV2_PRECISION_HI       0x00010000
#define M_SHFLAG_SINGLE_NORMAL          0x00020000

VOID dr_GenerateShader (UINT_32 flags, CHARPP sh_vertex, CHARPP sh_fragment);

#endif