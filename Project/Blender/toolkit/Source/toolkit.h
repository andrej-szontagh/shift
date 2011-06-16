
#ifndef _TOOLKIT_H_
#define _TOOLKIT_H_

#define DECLDIR __declspec (dllexport)

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// BASIC TYPES
////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define INT_8			signed char
#define INT_8P			signed char *
#define INT_8PP			signed char **
#define SIZE_INT_8		sizeof (signed char)
#define SIZE_INT_8P		sizeof (signed char *)

#define INT_16			signed short
#define INT_16P			signed short *
#define INT_16PP		signed short **
#define SIZE_INT_16		sizeof (signed short)
#define SIZE_INT_16P	sizeof (signed short *)

#define INT_32			signed int
#define INT_32P			signed int *
#define INT_32PP		signed int **
#define SIZE_INT_32		sizeof (signed int)
#define SIZE_INT_32P	sizeof (signed int *)

#define INT_64			signed long long
#define INT_64P			signed long long *
#define INT_64PP		signed long long **
#define SIZE_INT_64		sizeof (signed long long)
#define SIZE_INT_64P	sizeof (signed long long *)

#define UINT_8			unsigned char
#define UINT_8P			unsigned char *
#define UINT_8PP		unsigned char **
#define SIZE_UINT_8		sizeof (unsigned char)
#define SIZE_UINT_8P	sizeof (unsigned char *)

#define UINT_16			unsigned short
#define UINT_16P		unsigned short *
#define UINT_16PP		unsigned short **
#define SIZE_UINT_16	sizeof (unsigned short)
#define SIZE_UINT_16P	sizeof (unsigned short *)

#define UINT_32			unsigned int
#define UINT_32P		unsigned int *
#define UINT_32PP		unsigned int **
#define SIZE_UINT_32	sizeof (unsigned int)
#define SIZE_UINT_32P	sizeof (unsigned int *)

#define UINT_64			unsigned long long
#define UINT_64P		unsigned long long *
#define UINT_64PP		unsigned long long **
#define SIZE_UINT_64	sizeof (unsigned long long)
#define SIZE_UINT_64P	sizeof (unsigned long long *)

#define FLOAT_32		float
#define FLOAT_32P		float *
#define FLOAT_32PP		float **
#define SIZE_FLOAT_32	sizeof (float)
#define SIZE_FLOAT_32P	sizeof (float *)

#define FLOAT_64		double
#define FLOAT_64P		double *
#define FLOAT_64PP		double **
#define SIZE_FLOAT_64	sizeof (double)
#define SIZE_FLOAT_64P	sizeof (double *)

#define BOOL			bool
#define BOOLP			bool *
#define BOOLPP			bool **
#define SIZE_BOOL		sizeof (bool)
#define SIZE_BOOLP		sizeof (bool *)

#define VOID			void
#define VOIDP			void *
#define VOIDPP			void **
#define SIZE_VOID		sizeof (void)
#define SIZE_VOIDP		sizeof (void *)

#define CHAR			char
#define CHARP			char *
#define CHARPP			char **
#define SIZE_CHAR		sizeof (char)
#define SIZE_CHARP		sizeof (char *)

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// MACRO
////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define PI  3.1415926535897932384626433832795

#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

#define vDOT(v1, v2) \
	(v1 [0] * v2 [0] + v1 [1] * v2 [1] + v1 [2] * v2 [2])

#define vCROSS(dest, v1, v2)	\
	dest [0] = v1 [1] * v2 [2] - v1 [2] * v2 [1]; \
	dest [1] = v1 [2] * v2 [0] - v1 [0] * v2 [2]; \
	dest [2] = v1 [0] * v2 [1] - v1 [1] * v2 [0];

#define vLENGTH(v) \
	sqrt (vDOT (v, v))

#define vNORMALIZE(v, mag) \
	v [0] /= mag; \
	v [1] /= mag; \
	v [2] /= mag;

#define vINVERT(v) \
	v [0] = - v [0]; \
	v [1] = - v [1]; \
	v [2] = - v [2];

#define pTRANSFORM_ROW4x4p3(dest, src, mat) { \
    dest [0] = mat[ 0]*src[0] + mat[ 1]*src[1] + mat[ 2]*src[2] + mat[ 3]; \
    dest [1] = mat[ 4]*src[0] + mat[ 5]*src[1] + mat[ 6]*src[2] + mat[ 7]; \
    dest [2] = mat[ 8]*src[0] + mat[ 9]*src[1] + mat[10]*src[2] + mat[11]; \
}

#define pTRANSFORM_COL4x4p3(dest, src, mat) { \
    dest [0] = mat[ 0]*src[0] + mat[ 4]*src[1] + mat[ 8]*src[2] + mat[12]; \
    dest [1] = mat[ 1]*src[0] + mat[ 5]*src[1] + mat[ 9]*src[2] + mat[13]; \
    dest [2] = mat[ 2]*src[0] + mat[ 6]*src[1] + mat[10]*src[2] + mat[14]; \
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EXPORT FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern "C"
{

    DECLDIR UINT_32 _stdcall processSaveInstances           (CHARP filename, UINT_32 count, FLOAT_32P data, UINT_32P datak, FLOAT_32P bbox, UINT_32 divx, UINT_32 divy, UINT_32 divz, FLOAT_32 rotx, FLOAT_32 roty, FLOAT_32 rotz, FLOAT_32 randrad, UINT_32 rotorder, FLOAT_32 cancell);

    DECLDIR UINT_32 _stdcall processDecompose               (CHARP composite, CHARP destination);

    DECLDIR UINT_32 _stdcall processWeightsGenerate         (CHARPP files, CHARP filename);

    DECLDIR UINT_32 _stdcall processGenerateComposite       (CHARP filename, CHARP translation, CHARP  diffuse, CHARP  normal, CHARP  gloss, CHARP  shininess, FLOAT_32  factord, FLOAT_32  factorn, FLOAT_32  factorg, FLOAT_32  factors);
    DECLDIR UINT_32 _stdcall processGenerateCompositeAtlas  (CHARP filename, CHARP translation, CHARPP diffuse, CHARPP normal, CHARPP gloss, CHARPP shininess, FLOAT_32P factord, FLOAT_32P factorn, FLOAT_32P factorg, FLOAT_32P factors, UINT_32 mode, UINT_32 count);



    DECLDIR VOID    _stdcall processTexUnveilUpdate         ();

    DECLDIR UINT_32 _stdcall processBitCount                (UINT_32 bits);

    DECLDIR INT_32  _stdcall processTexSplitBegin           (CHARPP files, CHARP errfile, UINT_32 count, UINT_32 overlap, UINT_32 kernel_a, UINT_32 kernel_s, FLOAT_32 texel_tolerancy, FLOAT_32 face_tolerancy);
    DECLDIR INT_32  _stdcall processTexSplitEnd             (UINT_32 save, UINT_32 err);

    DECLDIR UINT_32 _stdcall processTexSplitTriangle        (FLOAT_32 u1, FLOAT_32 v1, 
                                                             FLOAT_32 u2, FLOAT_32 v2, 
                                                             FLOAT_32 u3, FLOAT_32 v3);

    DECLDIR UINT_32 _stdcall processTexMaskTriangle         (FLOAT_32 u1, FLOAT_32 v1, 
                                                             FLOAT_32 u2, FLOAT_32 v2, 
                                                             FLOAT_32 u3, FLOAT_32 v3);

    DECLDIR VOID    _stdcall processTexUnveilTexelTriangle  (FLOAT_32 u1, FLOAT_32 v1, 
                                                             FLOAT_32 u2, FLOAT_32 v2, 
                                                             FLOAT_32 u3, FLOAT_32 v3, INT_32 l);

    DECLDIR VOID    _stdcall processTexUnveilFaceTriangle   (FLOAT_32 u1, FLOAT_32 v1, 
                                                             FLOAT_32 u2, FLOAT_32 v2, 
                                                             FLOAT_32 u3, FLOAT_32 v3, INT_32 l);

    DECLDIR VOID    _stdcall processTexPolishFaceTriangle   (FLOAT_32 u1, FLOAT_32 v1, 
                                                             FLOAT_32 u2, FLOAT_32 v2, 
                                                             FLOAT_32 u3, FLOAT_32 v3, UINT_32 threshold);

    DECLDIR INT_32  _stdcall processTexSplitGenerate        (CHARPP filesin, CHARP fileout, UINT_8P indices, UINT_32 count);

    DECLDIR UINT_32 _stdcall processParticlesCut            (UINT_32P indices, FLOAT_32P points, FLOAT_32P faces, UINT_32 pcount, UINT_32 fcount);
}

#endif