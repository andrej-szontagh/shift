
////////////////////////////////////////////////////////////////////////////////////////////////////////////
// dr_GrabSame
////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if 1

VOID dr_GrabSame (
                                        
                    UINT_32     &propn, 
                    UINT_32     &prop, 
                    UINT_32P    &stamps, 
                    UINT_16P    &listprop, 
                    UINT_32P    &listout, 
                    UINT_32P    &listin, 
                    UINT_32     &listinc, 
                    UINT_32     &search, 
                    UINT_32     &count, 
                    BOOL        &next
    )
{

    // GRAB OBJECTS WITH THE SAME MATERIAL

    propn = prop;   next = false;   count = 0;

    // traverse thru all visible objects and store those with selected material
    for (UINT_32 n = search; n < listinc; n ++) {

        register UINT_32 ID = listin [n];   propn = listprop [ID];

        if (propn == prop) {

            // put objects with same material together
            listout [count ++] = ID;

        } else {

            if (stamps [propn] != dr_stamp) {

                // we have found new material
                stamps [propn]  = dr_stamp; search = n;    next = true;

                // continue without overhead
                for (; n < listinc; n ++) {

                    register UINT_32 ID = listin [n];

                    if (listprop [ID] == prop) {

                        // put objects with same material together
                        listout [count ++] = ID;
                    }
                }

                break;
            }
        }
    }
}

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// dr_GrabSameStop
////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if 1

VOID dr_GrabSameStop (
                                        
                    UINT_32     &propn, 
                    UINT_32     &prop, 
                    UINT_32P    &stamps, 
                    UINT_16P    &listprop, 
                    UINT_32P    &listout, 
                    UINT_32P    &listin, 
                    UINT_32     &listinc, 
                    UINT_32     &search, 
                    UINT_32     &countmax,
                    UINT_32     &count, 
                    BOOL        &next
    )
{

    // GRAB OBJECTS WITH THE SAME MATERIAL

    propn = prop;   next = false;   count = 0;

    // traverse thru all visible objects and store those with selected material
    for (UINT_32 n = search; n < listinc; n ++) {

        register UINT_32 ID = listin [n];   propn = listprop [ID];

        if (propn == prop) {

            // put objects with same material together
            listout [count ++] = ID;

        } else {

            if (stamps [propn] != dr_stamp) {

                // we have found new material
                stamps [propn]  = dr_stamp; search = n;    next = true;

                // we know we are done
                if (count == countmax) return;

                // continue without overhead
                for (; n < listinc; n ++) {

                    register UINT_32 ID = listin [n];

                    if (listprop [ID] == prop) {

                        // put objects with same material together
                        listout [count ++] = ID;

                        // we know we are done
                        if (count == countmax) return;
                    }
                }

                break;
            }
        }
    }
}

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// dr_GrabSameCall
////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if 1

VOID dr_GrabSameCall (
                                        
                    VOID       (&func) (UINT_32&, UINT_32&),
                    UINT_32     &param1,
                    UINT_32     &propn, 
                    UINT_32     &prop, 
                    UINT_32P    &stamps, 
                    UINT_16P    &listprop, 
                    UINT_32P    &listin, 
                    UINT_32     &listinc, 
                    UINT_32     &search, 
                    BOOL        &next
    )
{

    // GRAB OBJECTS WITH THE SAME MATERIAL

    propn = prop;   next = false;

    // traverse thru all visible objects and store those with selected material
    for (UINT_32 n = search; n < listinc; n ++) {

        register UINT_32 ID = listin [n];   propn = listprop [ID];

        if (propn == prop) {

            // call
            func (ID, param1);

        } else {

            if (stamps [propn] != dr_stamp) {

                // we have found new material
                stamps [propn]  = dr_stamp; search = n;    next = true;

                // continue without overhead
                for (; n < listinc; n ++) {

                    register UINT_32 ID = listin [n];

                    if (listprop [ID] == prop) {

                        // call
                        func (ID, param1);
                    }
                }

                break;
            }
        }
    }
}

#define M_DR_GRAB_SAME_DRAW1(func_prepare, func_draw, param1, propn, prop, stamps, listprop, listin, listinc, search, next) {   \
    \
    /* GRAB OBJECTS WITH THE SAME MATERIAL */   \
                                                \
    propn = prop;   next = false;               \
                                                \
    /* traverse thru all visible objects and store those with selected material */  \
    for (UINT_32 n = search; n < listinc; n ++) {                                   \
                                                                                    \
        register UINT_32 ID = listin [n];   propn = listprop [ID];                  \
                                                                                    \
        if (propn == prop) {                \
                                            \
            /* draw */                      \
            func_prepare    (ID);           \
            func_draw       (ID, param1);   \
                                            \
        } else {                            \
                                            \
            if (stamps [propn] != dr_stamp) {                                   \
                                                                                \
                /* we have found new material */                                \
                stamps [propn]  = dr_stamp; search = n;    next = true;         \
                                                                                \
                /* continue without overhead */                                 \
                for (; n < listinc; n ++) {             \
                                                        \
                    register UINT_32 ID = listin [n];   \
                                                        \
                    if (listprop [ID] == prop) {        \
                                                        \
                        /* draw */                      \
                        func_prepare    (ID);           \
                        func_draw       (ID, param1);   \
                    }                                   \
                }                                       \
                                \
                break;          \
            }                   \
        }                       \
    }                           \
}

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// dr_GrabSameCall
////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if 1

VOID dr_GrabSameCall (
                                        
                    VOID       (&func) (UINT_32&, UINT_32&, UINT_32&),
                    UINT_32     &param1,
                    UINT_32     &param2,
                    UINT_32     &propn, 
                    UINT_32     &prop, 
                    UINT_32P    &stamps, 
                    UINT_16P    &listprop, 
                    UINT_32P    &listin, 
                    UINT_32     &listinc, 
                    UINT_32     &search, 
                    BOOL        &next
    )
{

    // GRAB OBJECTS WITH THE SAME MATERIAL

    propn = prop;   next = false;

    // traverse thru all visible objects and store those with selected material
    for (UINT_32 n = search; n < listinc; n ++) {

        register UINT_32 ID = listin [n];   propn = listprop [ID];

        if (propn == prop) {

            // call
            func (ID, param1, param2);

        } else {

            if (stamps [propn] != dr_stamp) {

                // we have found new material
                stamps [propn]  = dr_stamp; search = n;    next = true;

                // continue without overhead
                for (; n < listinc; n ++) {

                    register UINT_32 ID = listin [n];

                    if (listprop [ID] == prop) {

                        // call
                        func (ID, param1, param2);
                    }
                }

                break;
            }
        }
    }
}

#define M_DR_GRAB_SAME_DRAW2(func_prepare, func_draw, param1, param2, propn, prop, stamps, listprop, listin, listinc, search, next) {   \
    \
    /* GRAB OBJECTS WITH THE SAME MATERIAL */   \
                                                \
    propn = prop;   next = false;               \
                                                \
    /* traverse thru all visible objects and store those with selected material */  \
    for (UINT_32 n = search; n < listinc; n ++) {                                   \
                                                                                    \
        register UINT_32 ID = listin [n];   propn = listprop [ID];                  \
                                                                                    \
        if (propn == prop) {                \
                                            \
            /* draw */                              \
            func_prepare    (ID);                   \
            func_draw       (ID, param1, param2);   \
                                                    \
        } else {                                    \
                                                    \
            if (stamps [propn] != dr_stamp) {                                   \
                                                                                \
                /* we have found new material */                                \
                stamps [propn]  = dr_stamp; search = n;    next = true;         \
                                                                                \
                /* continue without overhead */                                 \
                for (; n < listinc; n ++) {             \
                                                        \
                    register UINT_32 ID = listin [n];   \
                                                        \
                    if (listprop [ID] == prop) {        \
                                                        \
                        /* draw */                              \
                        func_prepare    (ID);                   \
                        func_draw       (ID, param1, param2);   \
                    }                                           \
                }                                               \
                                \
                break;          \
            }                   \
        }                       \
    }                           \
}

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// dr_SortDistanceInsert
////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if 1

VOID dr_SortDistanceInsert (

              UINT_32P &list,
              UINT_32  &count
    )
{
    // INSERT SORT

    register UINT_32 index = 0;

    for (UINT_32 n = 1; n < count; n ++) {

        register UINT_32 ID = list [n];

        register UINT_32 dist = (UINT_32) (dr_object_distances [ID] * M_INT_SCALE);
        
        register INT_32 m;
        for (m = index; (m >= 0) && (dr_object_distances [list [m]] > dist); m --) {

            list [m + 1] = list [m];
        }

        list [m + 1] = ID;

        index ++;
    }
}

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// dr_SortDistanceInsertMove
////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if 1

VOID dr_SortDistanceInsertMove (

            UINT_32P    &listin,
            UINT_32P    &listout,
            UINT_32     &count
    )
{
    // INSERT SORT

    listout [0] = listin [0];

    if (count > 1) {

        register UINT_32 index = 0;

        for (UINT_32 n = 1; n < count; n ++) {

            register UINT_32 ID = listin [n];

            register UINT_32 dist = (UINT_32) (dr_object_distances [ID] * M_INT_SCALE);

            register INT_32 m;
            for (m = index; (m >= 0) && (dr_object_distances [listout [m]] > dist); m --) {

                listout [m + 1] = listout [m];
            }

            listout [m + 1] = ID;

            index ++;
        }
    }
}

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// dr_SortDistanceInsertMoveCall
////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if 1

VOID dr_SortDistanceInsertMoveCall (

            BOOL       (&func) (UINT_32&),
            UINT_32P    &listin,
            UINT_32P    &listout,
            UINT_32     &count
    )
{
    // INSERT SORT

    register UINT_32 index  = 0;

    for (UINT_32 n = 0; n < count; n ++) {

        register UINT_32 ID = listin [n];

        // call
        if (func (ID)) continue;

        listout [0] = ID;   index ++;

        for (++ n; n < count; n ++) {

            register UINT_32 ID = listin [n];

            // call
            if (func (ID)) continue;

            register UINT_32 dist = (UINT_32) (dr_object_distances [ID] * M_INT_SCALE);

            register INT_32 m;
            for (m = index - 1; (m >= 0) && (dr_object_distances [listout [m]] > dist); m --) {

                listout [m + 1] = listout [m];
            }

            listout [m + 1] = ID;   index ++;
        }
    }

    count = index;
}

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////
// dr_RadixSort
////////////////////////////////////////////////////////////////////////////////////////////////////

#if 1

UINT_32P dr_RadixSort (UINT_32P input, UINT_32 count)
{

    // shortcuts
    UINT_32P offset     = dr_radix.offset;
    UINT_32P indices    = dr_radix.indices;
    UINT_32P indicestmp = dr_radix.indicestmp;
    UINT_32P histogram  = dr_radix.histogram;

    // initialize indices so that the input buffer is read in sequential order
    for (UINT_32 i = 0; i < count; i ++) {

        indices     [i] = i;
        indicestmp  [i] = i;
    }

    // clear counters
    memset (histogram, 0, 256 * 4 * SIZE_UINT_32);

    // create histograms (counters). counters for all passes are created in one run.
    // pros:   read input buffer once instead of four times
    // cons:   histogram is 4kb instead of 1kb

    // prepare to count
    UINT_8P p = (UINT_8P) input;

    UINT_8P pe = &p [count * 4];

    UINT_32P h0 = &histogram [  0];     // histogram for first pass (LSB)
    UINT_32P h1 = &histogram [256];     // histogram for second pass
    UINT_32P h2 = &histogram [512];     // histogram for third pass
    UINT_32P h3 = &histogram [768];     // histogram for last pass (MSB)

    while (p != pe) {

        // create histograms
        h0 [* p++] ++;
        h1 [* p++] ++;
        h2 [* p++] ++;
        h3 [* p++] ++;
    }

    // radix sort, j is the pass number (0=LSB, 3=MSB)
    for (UINT_32 j = 0; j < 4; j ++) {

        // shortcut to current counters
        UINT_32P ccount = &histogram [j << 8];

        // create offsets
        offset [0] = 0;

        for (UINT_32 i = 1; i < 256; i ++) offset [i] = offset [i - 1] + ccount [i - 1];

        // perform radix sort
        register UINT_8P inputbytes = (UINT_8P) input;

        register UINT_32P indices    =  dr_radix.indices;
        register UINT_32P indicestmp =  dr_radix.indicestmp;
        register UINT_32P indicesend = &dr_radix.indices   [count];        inputbytes += j;

        while (indices != indicesend) {

            register UINT_32 id = * indices ++;

            indicestmp [offset [inputbytes [id << 2]] ++] = id;
        }

        // swap pointers for next pass
        UINT_32P tmp =  dr_radix.indices;
                        dr_radix.indices = dr_radix.indicestmp;
                                           dr_radix.indicestmp = tmp;
        indices     = dr_radix.indices;
        indicestmp  = dr_radix.indicestmp;
    }

    return (dr_radix.indices);
}

#endif
