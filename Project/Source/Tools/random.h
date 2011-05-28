
#ifndef _RANDOM_H
#define _RANDOM_H


UINT_32		randInt		();
FLOAT_64	randDouble	();

VOID randSeed (UINT_32  oneSeed);
VOID randSeed (UINT_32P bigSeed, UINT_32 seedLength);
VOID randSeed ();


#endif
