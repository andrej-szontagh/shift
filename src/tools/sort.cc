
////////////////////////////////////////////////////////////////////////////////////////////////////
// Include
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "types.h"
#include "sort.h"

#include <Stdio.h>
#include <Stdlib.h>
#include <String.h>

////////////////////////////////////////////////////////////////////////////////////////////////////
// so_RadixCreate
////////////////////////////////////////////////////////////////////////////////////////////////////

#if 1

VOID so_RadixCreate (TRadix *radix, UINT_32 size)
{
    // Allocate input-independent ram
    radix->histogram    = (UINT_32P) malloc (SIZE_UINT_32 * 256 * 4);
    radix->offset       = (UINT_32P) malloc (SIZE_UINT_32 * 256);

    // Allocate indices list
    radix->indices      = (UINT_32P) malloc (SIZE_UINT_32 * size);
    radix->indicestmp   = (UINT_32P) malloc (SIZE_UINT_32 * size);
}

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////
// so_RadixFree
////////////////////////////////////////////////////////////////////////////////////////////////////

#if 1

VOID so_RadixFree   (TRadix *radix)
{
    if (radix->offset       != NULL) free (radix->offset);      radix->offset       = NULL;
    if (radix->histogram    != NULL) free (radix->histogram);   radix->histogram    = NULL;
    if (radix->indicestmp   != NULL) free (radix->indicestmp);  radix->indicestmp   = NULL;
    if (radix->indices      != NULL) free (radix->indices);     radix->indices      = NULL;
}

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////
// so_RadixSortInt
////////////////////////////////////////////////////////////////////////////////////////////////////

#if 1

UINT_32P so_RadixSort (TRadix *radix, UINT_32P input, UINT_32 count)
{
    UINT_32 i, j;

    // shortcuts
    UINT_32P offset     = radix->offset;
    UINT_32P indices    = radix->indices;
    UINT_32P indicestmp = radix->indicestmp;
    UINT_32P histogram  = radix->histogram;

    // initialize indices so that the input buffer is read in sequential order
    for (i = 0; i < count; i ++) {

        indices     [i] = i;
        indicestmp  [i] = i;
    }

    // clear counters
    memset (histogram, 0, 256 * 4 * SIZE_UINT_32);

    // create histograms (counters). counters for all passes are created in one run.
    // pros:   read input buffer once instead of four times
    // cons:   histogram is 4kb instead of 1kb

    // temporal coherence
    BOOL alreadysorted = true;

    // prepare to count
    UINT_8P p = (UINT_8P) input;    UINT_8P pe = &p [count * 4];

    UINT_32P h0 = &histogram [  0];     // histogram for first pass (LSB)
    UINT_32P h1 = &histogram [256];     // histogram for second pass
    UINT_32P h2 = &histogram [512];     // histogram for third pass
    UINT_32P h3 = &histogram [768];     // histogram for last pass (MSB)

    // temporal coherence
    INT_32 prevval = input [radix->indices [0]];

    while (p != pe) {

        // temporal coherence
        INT_32 val = input [* indices ++];      // read input buffer in previous sorted order

        if (val < prevval) { alreadysorted = false; break; }        // check whether already sorted or not
                  prevval = val;                                    // update for next iteration

        // create histograms
        h0 [* p++] ++;
        h1 [* p++] ++;
        h2 [* p++] ++;
        h3 [* p++] ++;
    }

    // if all input values are already sorted, we just have to return and leave the previous list unchanged.
    // that way the routine may take advantage of temporal coherence, for example when used to sort transparent faces.
    if (alreadysorted) return (radix->indices);

	/* else there has been an early out and we must finish computing the histograms */
    while (p != pe) {

		/* create histograms without the previous overhead */	
        h0 [* p++] ++;
        h1 [* p++] ++;
        h2 [* p++] ++;
        h3 [* p++] ++;
	}

    // radix sort, j is the pass number (0=LSB, 3=MSB)
    for (j = 0; j < 4; j ++) {

        // shortcut to current counters
        UINT_32P ccount = &histogram [j << 8];

        // reset flag. the sorting pass is supposed to be performed. (default)
        BOOL performpass = true;

        // check pass validity [some cycles are lost there in the generic case, but that's ok, just a little loop]
        for (i = 0; i < 256; i++) {

            // if all values have the same byte, sorting is useless. it may happen when sorting bytes or words instead of dwords.
            // this routine actually sorts words faster than dwords, and bytes faster than words. standard running time (o(4*n))is
            // reduced to o(2*n) for words and o(n) for bytes. running time for floats depends on actual values...
            if (ccount [i] == count) {

                performpass = false;
                break;
            }

            // if at least one count is not null, we suppose the pass must be done. hence, this test takes very few cpu time in the generic case.
            if (ccount [i]) break;
        }

        // sometimes the fourth (negative) pass is skipped because all numbers are negative and the msb is 0xff (for example). this is
        // not a problem, numbers are correctly sorted anyway.
        if (performpass) {

            // create offsets
            offset [0] = 0;

            for (i = 1; i < 256; i ++) offset [i] = offset [i - 1] + ccount [i - 1];

            // perform radix sort
            register UINT_8P inputbytes = (UINT_8P) input;

            register UINT_32P indices    =  radix->indices;
            register UINT_32P indicestmp =  radix->indicestmp;
            register UINT_32P indicesend = &radix->indices   [count];        inputbytes += j;

            while (indices != indicesend) {

                register UINT_32 id = * indices ++;

                indicestmp [offset [inputbytes [id << 2]] ++] = id;
            }

            // swap pointers for next pass
            UINT_32P tmp =  radix->indices;
                            radix->indices = radix->indicestmp;
                                             radix->indicestmp = tmp;
            indices     = radix->indices;
            indicestmp  = radix->indicestmp;
        }
    }

    return (radix->indices);
}

#endif
