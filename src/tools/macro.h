
#ifndef _MACRO_H
#define _MACRO_H

#define EPSILON		0.00000001
#define PI			3.1415926535897932384626433832795

#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

#define vDOT2(v1, v2) \
    (v1 [0] * v2 [0] + v1 [1] * v2 [1])

#define vDOT3(v1, v2) \
    (v1 [0] * v2 [0] + v1 [1] * v2 [1] + v1 [2] * v2 [2])

#define vCROSS(dest, v1, v2) {	\
	dest [0] = v1 [1] * v2 [2] - v1 [2] * v2 [1]; \
	dest [1] = v1 [2] * v2 [0] - v1 [0] * v2 [2]; \
	dest [2] = v1 [0] * v2 [1] - v1 [1] * v2 [0]; \
}

#define vADD2(dest, v1, v2) { \
	dest [0] = v1 [0] + v2 [0]; \
	dest [1] = v1 [1] + v2 [1]; \
}

#define vADD3(dest, v1, v2) { \
	dest [0] = v1 [0] + v2 [0]; \
	dest [1] = v1 [1] + v2 [1]; \
	dest [2] = v1 [2] + v2 [2]; \
}

#define vSUB2(dest, v1, v2) { \
	dest [0] = v1 [0] - v2 [0]; \
	dest [1] = v1 [1] - v2 [1]; \
}

#define vSUB3(dest, v1, v2) { \
	dest [0] = v1 [0] - v2 [0]; \
	dest [1] = v1 [1] - v2 [1]; \
	dest [2] = v1 [2] - v2 [2]; \
}

#define vMUL2(dest, v1, v2) { \
	dest [0] = v1 [0] * v2 [0]; \
	dest [1] = v1 [1] * v2 [1]; \
}

#define vMUL3(dest, v1, v2) { \
	dest [0] = v1 [0] * v2 [0]; \
	dest [1] = v1 [1] * v2 [1]; \
	dest [2] = v1 [2] * v2 [2]; \
}

#define vSCALE2(dest, v, m) { \
	dest [0] = v  [0] * m; \
	dest [1] = v  [1] * m; \
}

#define vSCALE3(dest, v, m) { \
	dest [0] = v  [0] * m; \
	dest [1] = v  [1] * m; \
	dest [2] = v  [2] * m; \
}

#define vASSIGN2(dest, src) { \
	dest [0] = src [0]; \
	dest [1] = src [1]; \
}

#define vASSIGN3(dest, src) { \
	dest [0] = src [0]; \
	dest [1] = src [1]; \
	dest [2] = src [2]; \
}

#define vSET2(dest, x, y) { \
	dest [0] = x; \
	dest [1] = y; \
}

#define vSET3(dest, x, y, z) { \
	dest [0] = x; \
	dest [1] = y; \
	dest [2] = z; \
}

#define vAVERAGE3v2(dest, v1, v2) { \
	dest [0] = (v1 [0] + v2 [0]) * 0.5f; \
	dest [1] = (v1 [1] + v2 [1]) * 0.5f; \
	dest [2] = (v1 [2] + v2 [2]) * 0.5f; \
}

#define vAVERAGE3v3(dest, v1, v2, v3) { \
	dest [0] = (v1 [0] + v2 [0] + v3 [0]) * 0.3333333333f; \
	dest [1] = (v1 [1] + v2 [1] + v3 [1]) * 0.3333333333f; \
	dest [2] = (v1 [2] + v2 [2] + v3 [2]) * 0.3333333333f; \
}

#define vAVERAGE3v4(dest, v1, v2, v3, v4) { \
	dest [0] = (v1 [0] + v2 [0] + v3 [0] + v4 [0]) * 0.25f; \
	dest [1] = (v1 [1] + v2 [1] + v3 [1] + v4 [1]) * 0.25f; \
	dest [2] = (v1 [2] + v2 [2] + v3 [2] + v4 [2]) * 0.25f; \
}

#define vINVERT2(v) { \
	v [0] = - v [0]; \
	v [1] = - v [1]; \
}

#define vINVERT3(v) { \
	v [0] = - v [0]; \
	v [1] = - v [1]; \
	v [2] = - v [2]; \
}

// nonzero vector !
#define vNORMALIZE3(v, mag) { \
	v [0] /= mag; \
	v [1] /= mag; \
	v [2] /= mag; \
}

// nonzero vector !
#define vNORMALIZE4(v, mag) { \
	v [0] /= mag; \
	v [1] /= mag; \
	v [2] /= mag; \
	v [3] /= mag; \
}

// nonzero vector !
#define vANGLE(dot, mag1, mag2) \
	(dot / mag1 * mag2)

#define vLENGTH2(v) \
	sqrt (vDOT2 (v, v))

#define vLENGTH3(v) \
	sqrt (vDOT3 (v, v))

#define vLENGTHQ2(v) \
	vDOT2 (v, v)

#define vLENGTHQ3(v) \
	vDOT3 (v, v)

// dot forget to normalize after !
#define vFLY(dir, up, bi, x, y, z) { \
	dir [0] += x * bi [0] + y * up [0]; up  [0] += z * bi [0]; \
	dir [1] += x * bi [1] + y * up [1]; up  [1] += z * bi [1]; \
	dir [2] += x * bi [2] + y * up [2]; up  [2] += z * bi [2]; \
	vCROSS (bi, dir, up); \
}

#define vTRANSFORM_ROW4x4v3(dest, src, mat) { \
    dest [0] = mat[ 0]*src[0] + mat[ 1]*src[1] + mat[ 2]*src[2]; \
    dest [1] = mat[ 4]*src[0] + mat[ 5]*src[1] + mat[ 6]*src[2]; \
    dest [2] = mat[ 8]*src[0] + mat[ 9]*src[1] + mat[10]*src[2]; \
}

#define vTRANSFORM_COL4x4v3(dest, src, mat) { \
    dest [0] = mat[ 0]*src[0] + mat[ 4]*src[1] + mat[ 8]*src[2]; \
    dest [1] = mat[ 1]*src[0] + mat[ 5]*src[1] + mat[ 9]*src[2]; \
    dest [2] = mat[ 2]*src[0] + mat[ 6]*src[1] + mat[10]*src[2]; \
}

#define pTRANSFORM_ROW4x4p2(dest, src, mat) { \
    dest [0] = mat[ 0]*src[0] + mat[ 1]*src[1] + mat[ 2]*src[2] + mat[ 3]; \
    dest [1] = mat[ 4]*src[0] + mat[ 5]*src[1] + mat[ 6]*src[2] + mat[ 7]; \
}

#define pTRANSFORM_COL4x4p2(dest, src, mat) { \
    dest [0] = mat[ 0]*src[0] + mat[ 4]*src[1] + mat[ 8]*src[2] + mat[12]; \
    dest [1] = mat[ 1]*src[0] + mat[ 5]*src[1] + mat[ 9]*src[2] + mat[13]; \
}

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

#define pTRANSFORM_ROW4x4p4(dest, src, mat) { \
    dest [0] = mat[ 0]*src[0] + mat[ 1]*src[1] + mat[ 2]*src[2] + mat[ 3]; \
    dest [1] = mat[ 4]*src[0] + mat[ 5]*src[1] + mat[ 6]*src[2] + mat[ 7]; \
    dest [2] = mat[ 8]*src[0] + mat[ 9]*src[1] + mat[10]*src[2] + mat[11]; \
    dest [3] = mat[12]*src[0] + mat[13]*src[1] + mat[14]*src[2] + mat[15]; \
}

#define pTRANSFORM_COL4x4p4(dest, src, mat) { \
    dest [0] = mat[ 0]*src[0] + mat[ 4]*src[1] + mat[ 8]*src[2] + mat[12]; \
    dest [1] = mat[ 1]*src[0] + mat[ 5]*src[1] + mat[ 9]*src[2] + mat[13]; \
    dest [2] = mat[ 2]*src[0] + mat[ 6]*src[1] + mat[10]*src[2] + mat[14]; \
    dest [3] = mat[ 3]*src[0] + mat[ 7]*src[1] + mat[11]*src[2] + mat[15]; \
}

#define mIDENTITY(mat) { \
	mat [ 0] = 1; mat [ 1] = 0; mat [ 2] = 0; mat [ 3] = 0; \
	mat [ 4] = 0; mat [ 5] = 1; mat [ 6] = 0; mat [ 7] = 0; \
	mat [ 8] = 0; mat [ 9] = 0; mat [10] = 1; mat [11] = 0; \
	mat [12] = 0; mat [13] = 0; mat [14] = 0; mat [15] = 1; \
}

#define mTRANSPOSE(dest, mat) { \
	dest [ 0] = mat [0]; dest [ 1] = mat [4]; dest [ 2] = mat [ 8]; dest [ 3] = mat [12]; \
	dest [ 4] = mat [1]; dest [ 5] = mat [5]; dest [ 6] = mat [ 9]; dest [ 7] = mat [13]; \
	dest [ 8] = mat [2]; dest [ 9] = mat [6]; dest [10] = mat [10]; dest [11] = mat [14]; \
	dest [12] = mat [3]; dest [13] = mat [7]; dest [14] = mat [11]; dest [15] = mat [15]; \
}

#define mTRANSLATE(mat, x, y, z) { \
	mat [ 0] = 1; mat [ 1] = 0; mat [ 2] = 0; mat [ 3] = x; \
	mat [ 4] = 0; mat [ 5] = 1; mat [ 6] = 0; mat [ 7] = y; \
	mat [ 8] = 0; mat [ 9] = 0; mat [10] = 1; mat [11] = z; \
	mat [12] = 0; mat [13] = 0; mat [14] = 0; mat [15] = 1; \
}

#define mSCALE(mat, x, y, z) { \
	mat [ 0] = x; mat [ 1] = 0; mat [ 2] = 0; mat [ 3] = 0; \
	mat [ 4] = 0; mat [ 5] = y; mat [ 6] = 0; mat [ 7] = 0; \
	mat [ 8] = 0; mat [ 9] = 0; mat [10] = z; mat [11] = 0; \
	mat [12] = 0; mat [13] = 0; mat [14] = 0; mat [15] = 1; \
}

// dest != mat1 && dest != mat2		!!!!
#define mMUL(dest, mat1, mat2) { \
	dest [ 0] = mat1 [ 0]*mat2 [ 0] + mat1 [ 1]*mat2 [ 4] + mat1 [ 2]*mat2 [ 8] + mat1 [ 3]*mat2 [12]; \
	dest [ 1] = mat1 [ 0]*mat2 [ 1] + mat1 [ 1]*mat2 [ 5] + mat1 [ 2]*mat2 [ 9] + mat1 [ 3]*mat2 [13]; \
	dest [ 2] = mat1 [ 0]*mat2 [ 2] + mat1 [ 1]*mat2 [ 6] + mat1 [ 2]*mat2 [10] + mat1 [ 3]*mat2 [14]; \
	dest [ 3] = mat1 [ 0]*mat2 [ 3] + mat1 [ 1]*mat2 [ 7] + mat1 [ 2]*mat2 [11] + mat1 [ 3]*mat2 [15]; \
	dest [ 4] = mat1 [ 4]*mat2 [ 0] + mat1 [ 5]*mat2 [ 4] + mat1 [ 6]*mat2 [ 8] + mat1 [ 7]*mat2 [12]; \
	dest [ 5] = mat1 [ 4]*mat2 [ 1] + mat1 [ 5]*mat2 [ 5] + mat1 [ 6]*mat2 [ 9] + mat1 [ 7]*mat2 [13]; \
	dest [ 6] = mat1 [ 4]*mat2 [ 2] + mat1 [ 5]*mat2 [ 6] + mat1 [ 6]*mat2 [10] + mat1 [ 7]*mat2 [14]; \
	dest [ 7] = mat1 [ 4]*mat2 [ 3] + mat1 [ 5]*mat2 [ 7] + mat1 [ 6]*mat2 [11] + mat1 [ 7]*mat2 [15]; \
	dest [ 8] = mat1 [ 8]*mat2 [ 0] + mat1 [ 9]*mat2 [ 4] + mat1 [10]*mat2 [ 8] + mat1 [11]*mat2 [12]; \
	dest [ 9] = mat1 [ 8]*mat2 [ 1] + mat1 [ 9]*mat2 [ 5] + mat1 [10]*mat2 [ 9] + mat1 [11]*mat2 [13]; \
	dest [10] = mat1 [ 8]*mat2 [ 2] + mat1 [ 9]*mat2 [ 6] + mat1 [10]*mat2 [10] + mat1 [11]*mat2 [14]; \
	dest [11] = mat1 [ 8]*mat2 [ 3] + mat1 [ 9]*mat2 [ 7] + mat1 [10]*mat2 [11] + mat1 [11]*mat2 [15]; \
	dest [12] = mat1 [12]*mat2 [ 0] + mat1 [13]*mat2 [ 4] + mat1 [14]*mat2 [ 8] + mat1 [15]*mat2 [12]; \
	dest [13] = mat1 [12]*mat2 [ 1] + mat1 [13]*mat2 [ 5] + mat1 [14]*mat2 [ 9] + mat1 [15]*mat2 [13]; \
	dest [14] = mat1 [12]*mat2 [ 2] + mat1 [13]*mat2 [ 6] + mat1 [14]*mat2 [10] + mat1 [15]*mat2 [14]; \
	dest [15] = mat1 [12]*mat2 [ 3] + mat1 [13]*mat2 [ 7] + mat1 [14]*mat2 [11] + mat1 [15]*mat2 [15]; \
}

#define mASSIGN(dest, src) { \
	dest [ 0] = src [ 0]; dest [ 1] = src [ 1]; dest [ 2] = src [ 2]; dest [ 3] = src [ 3]; \
	dest [ 4] = src [ 4]; dest [ 5] = src [ 5]; dest [ 6] = src [ 6]; dest [ 7] = src [ 7]; \
	dest [ 8] = src [ 8]; dest [ 9] = src [ 9]; dest [10] = src [10]; dest [11] = src [11]; \
	dest [12] = src [12]; dest [13] = src [13]; dest [14] = src [14]; dest [15] = src [15]; \
}

#define mROTATE4(mat, sina, cosa, sinb, cosb, sinc, cosc) { \
	mat [ 0] =		cosa*cosb - cosc*sinb*sina; \
	mat [ 1] =		cosa*sinb + cosc*cosb*sina; \
	mat [ 2] =		sina*sinc;						mat [ 3] = 0; \
	mat [ 4] = -	sina*cosb - cosc*sinb*cosa; \
	mat [ 5] = -	sina*sinb + cosc*cosb*cosa; \
	mat [ 6] =		cosa*sinc;						mat [ 7] = 0; \
	mat [ 8] =		sinc*sinb; \
	mat [ 9] = -	sinc*cosb; \
	mat [10] =		cosc;							mat [11] = 0; \
	mat [12] = 0; \
	mat [13] = 0; \
	mat [14] = 0; \
	mat [15] = 1; \
}

#define mROTATE3(mat, sina, cosa, sinb, cosb, sinc, cosc) { \
	mat [0] =	cosa*cosb - cosc*sinb*sina; \
	mat [1] =	cosa*sinb + cosc*cosb*sina; \
	mat [2] =	sina*sinc; \
	mat [3] = -	sina*cosb - cosc*sinb*cosa; \
	mat [4] = -	sina*sinb + cosc*cosb*cosa; \
	mat [5] =	cosa*sinc; \
	mat [6] =	sinc*sinb; \
	mat [7] = -	sinc*cosb; \
	mat [8] =	cosc; \
}

// need normalized vector !
#define mROTATEA4(mat, sina, cosa, oneminuscosa, v) { \
	mat [ 0] = v[0]*v[0]*oneminuscosa +		cosa; \
	mat [ 1] = v[0]*v[1]*oneminuscosa - v[2]*	sina; \
	mat [ 2] = v[0]*v[2]*oneminuscosa + v[1]*	sina; mat [ 3] = 0; \
	mat [ 4] = v[1]*v[0]*oneminuscosa + v[2]*	sina; \
	mat [ 5] = v[1]*v[1]*oneminuscosa +		cosa; \
	mat [ 6] = v[1]*v[2]*oneminuscosa - v[0]*	sina; mat [ 7] = 0; \
	mat [ 8] = v[2]*v[0]*oneminuscosa - v[1]*	sina; \
	mat [ 9] = v[2]*v[1]*oneminuscosa + v[0]*	sina; \
	mat [10] = v[2]*v[2]*oneminuscosa +		cosa; mat [11] = 0; \
	mat [12] = 0; \
	mat [13] = 0; \
	mat [14] = 0; \
	mat [15] = 1; \
}

// need normalized vector !
#define mROTATEA3(mat, sina, cosa, oneminuscosa, v) { \
	mat [ 0] = v[0]*v[0]*oneminuscosa +		cosa; \
	mat [ 1] = v[0]*v[1]*oneminuscosa - v[2]*	sina; \
	mat [ 2] = v[0]*v[2]*oneminuscosa + v[1]*	sina; \
	mat [ 4] = v[1]*v[0]*oneminuscosa + v[2]*	sina; \
	mat [ 5] = v[1]*v[1]*oneminuscosa +		cosa; \
	mat [ 6] = v[1]*v[2]*oneminuscosa - v[0]*	sina; \
	mat [ 8] = v[2]*v[0]*oneminuscosa - v[1]*	sina; \
	mat [ 9] = v[2]*v[1]*oneminuscosa + v[0]*	sina; \
	mat [10] = v[2]*v[2]*oneminuscosa +		cosa; \
}

#define mREDUCTION(dest, mat) { \
	dest [0] = mat [ 0]; \
	dest [1] = mat [ 1]; \
	dest [2] = mat [ 2]; \
	dest [3] = mat [ 4]; \
	dest [4] = mat [ 5]; \
	dest [5] = mat [ 6]; \
	dest [6] = mat [ 8]; \
	dest [7] = mat [ 9]; \
	dest [8] = mat [10]; \
}

#define mDETERMINANT3x3(mat) { \
(	mat [0] * mat [4] * mat [8] + \
	mat [1] * mat [5] * mat [6] + \
	mat [2] * mat [3] * mat [7] - \
	mat [8] * mat [3] * mat [1] - \
	mat [7] * mat [5] * mat [0] - \
	mat [6] * mat [4] * mat [2]) \
}

#define mTRANSPOSE3x3(dest, mat) { \
	dest [0] = mat [0]; dest [1] = mat [3]; dest [2] = mat [6]; \
	dest [3] = mat [1]; dest [4] = mat [4]; dest [5] = mat [7]; \
	dest [6] = mat [2]; dest [7] = mat [5]; dest [8] = mat [8]; \
}

#define mINVERSE3x3(mat, transposed, invdet) { \
	mat [0] = invdet *			(transposed[4]*transposed[8] - transposed[7]*transposed[5]); \
	mat [1] = invdet * (- 1) *	(transposed[3]*transposed[8] - transposed[6]*transposed[5]); \
	mat [2] = invdet *			(transposed[3]*transposed[7] - transposed[6]*transposed[4]); \
	mat [3] = invdet * (- 1) *	(transposed[1]*transposed[8] - transposed[7]*transposed[2]); \
	mat [4] = invdet *			(transposed[0]*transposed[8] - transposed[6]*transposed[2]); \
	mat [5] = invdet * (- 1) *	(transposed[0]*transposed[7] - transposed[6]*transposed[1]); \
	mat [6] = invdet *			(transposed[1]*transposed[5] - transposed[4]*transposed[2]); \
	mat [7] = invdet * (- 1) *	(transposed[0]*transposed[5] - transposed[3]*transposed[2]); \
	mat [8] = invdet *			(transposed[0]*transposed[4] - transposed[3]*transposed[1]); \
}

#endif
