
#ifndef _TYPES_H
#define _TYPES_H

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// BASIC TYPES
////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define INT_8           signed char
#define INT_8P          signed char *
#define INT_8PP         signed char **
#define SIZE_INT_8      sizeof (signed char)
#define SIZE_INT_8P     sizeof (signed char *)

#define INT_16          signed short
#define INT_16P         signed short *
#define INT_16PP        signed short **
#define SIZE_INT_16     sizeof (signed short)
#define SIZE_INT_16P    sizeof (signed short *)

#define INT_32          signed int
#define INT_32P         signed int *
#define INT_32PP        signed int **
#define SIZE_INT_32     sizeof (signed int)
#define SIZE_INT_32P    sizeof (signed int *)

#define INT_64          signed long long
#define INT_64P         signed long long *
#define INT_64PP        signed long long **
#define SIZE_INT_64     sizeof (signed long long)
#define SIZE_INT_64P    sizeof (signed long long *)

#define UINT_8          unsigned char
#define UINT_8P         unsigned char *
#define UINT_8PP        unsigned char **
#define SIZE_UINT_8     sizeof (unsigned char)
#define SIZE_UINT_8P    sizeof (unsigned char *)

#define UINT_16         unsigned short
#define UINT_16P        unsigned short *
#define UINT_16PP       unsigned short **
#define SIZE_UINT_16    sizeof (unsigned short)
#define SIZE_UINT_16P   sizeof (unsigned short *)

#define UINT_32         unsigned int
#define UINT_32P        unsigned int *
#define UINT_32PP       unsigned int **
#define SIZE_UINT_32    sizeof (unsigned int)
#define SIZE_UINT_32P   sizeof (unsigned int *)

#define UINT_64         unsigned long long
#define UINT_64P        unsigned long long *
#define UINT_64PP       unsigned long long **
#define SIZE_UINT_64    sizeof (unsigned long long)
#define SIZE_UINT_64P   sizeof (unsigned long long *)

#define FLOAT_32        float
#define FLOAT_32P       float *
#define FLOAT_32PP      float **
#define SIZE_FLOAT_32   sizeof (float)
#define SIZE_FLOAT_32P  sizeof (float *)

#define FLOAT_64        double
#define FLOAT_64P       double *
#define FLOAT_64PP      double **
#define SIZE_FLOAT_64   sizeof (double)
#define SIZE_FLOAT_64P  sizeof (double *)

#define BOOL            bool
#define BOOLP           bool *
#define BOOLPP          bool **
#define SIZE_BOOL       sizeof (bool)
#define SIZE_BOOLP      sizeof (bool *)

#define VOID            void
#define VOIDP           void *
#define VOIDPP          void **
#define SIZE_VOID       sizeof (void)
#define SIZE_VOIDP      sizeof (void *)

#define CHAR            char
#define CHARP           char *
#define CHARPP          char **
#define SIZE_CHAR       sizeof (char)
#define SIZE_CHARP      sizeof (char *)

#define INLINE          inline
#define INLINEF         __forceinline

#endif
