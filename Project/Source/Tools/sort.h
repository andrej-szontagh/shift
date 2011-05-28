
#ifndef _SORT_H
#define _SORT_H

struct TRadix
{
    UINT_32P histogram;     // counters for each byte
    UINT_32P offset;        // offsets (nearly a cumulative distribution function)

    UINT_32P indices;       // two lists, swapped each pass
    UINT_32P indicestmp;    // two lists, swapped each pass
};

UINT_32P so_RadixSort       (TRadix * radix, UINT_32P input, UINT_32 count);

VOID     so_RadixCreate     (TRadix * radix, UINT_32  size);
VOID     so_RadixFree       (TRadix * radix);

#endif
