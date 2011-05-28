
#include     <stdlib.h>
#include     <string.h>
#include      <stdio.h>
#include	  <float.h>
#include       <math.h>
#include         <io.h>

#include    "toolkit.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DATA STRUCTURES
////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct TImage
{
    #define M_IMAGE_TYPE_RGB    0       // RGB
    #define M_IMAGE_TYPE_RGBA   1       // RGBA

    UINT_8      type;                   // type of image

    UINT_32     width;                  // width of image
    UINT_32     height;                 // height of image
    
    VOIDP       data;                   // image pixel data
};

///////////////////////////////////////////////////////////////////////////////
// IMAGE LOAD
///////////////////////////////////////////////////////////////////////////////

#if 1

INT_32 loadImage (CHARP filename, TImage *image)
{

    FILE *file;

    UINT_16 w, h, depth;

    UINT_32 size;

    if (!(file = fopen (filename, "rb")))   return -1;

    UINT_8  data8;

    // numer of bytes for image identification
    fseek (file,  0, SEEK_SET);
    fread (&data8,  SIZE_UINT_8, 1, file);      if (data8  != 0)    { fclose (file); return -2; }       // 0 - no image identification field

    // color Map Type
    fseek (file,  1, SEEK_SET);
    fread (&data8,  SIZE_UINT_8, 1, file);      if (data8  != 0)    { fclose (file); return -2; }       // 0 - no color map

    // image Type Code
    fseek (file,  2, SEEK_SET);
    fread (&data8,  SIZE_UINT_8, 1, file);      if (data8  != 2)    { fclose (file); return -2; }       // 2 - means image type 10 -> RGB image

    #if 0

        UINT_16 data16;

        // COLOR MAP SPECIFICATION

        // Color Map Origin
        fseek (file,  3, SEEK_SET);
        fread (&data16, SIZE_UINT_16, 1, file);     if (data16 != ) return -2;

        // Color Map Length
        fseek (file,  5, SEEK_SET);
        fread (&data16, SIZE_UINT_16, 1, file);     if (data16 != ) return -2;

        // Color Map Entry Size
        fseek (file,  7, SEEK_SET);
        fread (&data8,  SIZE_UINT_8,  1, file);     if (data8  != ) return -2;

        //  IMAGE SPECIFICATION

        // X Origin of Image
        fseek (file,  8, SEEK_SET);
        fread (&data16, SIZE_UINT_16, 1, file);

        // Y Origin of Image
        fseek (file, 10, SEEK_SET);
        fread (&data16, SIZE_UINT_16, 1, file);

    #endif


    // width of Image
    fseek (file, 12, SEEK_SET);
    fread (&w,      SIZE_UINT_16, 1, file);

    // height of Image
    fseek (file, 14, SEEK_SET);
    fread (&h,      SIZE_UINT_16, 1, file);

    // image Pixel Size
    fseek (file, 16, SEEK_SET);
    fread (&data8,  SIZE_UINT_8,  1, file);

    // depth of pixel data
    switch (data8) {

        case 24:    image->type = M_IMAGE_TYPE_RGB;    depth = 3;  break;
        case 32:    image->type = M_IMAGE_TYPE_RGBA;   depth = 4;  break;

        default:    fclose (file);  return -2;
    }

    #if 0

        // Image Descriptor Byte
        fseek (file, 17, SEEK_SET);
        fread (&data8,  SIZE_UINT_8,  1, file);

    #endif

    // seeking data
    fseek (file, 18, SEEK_SET);

    image->width    = (UINT_32) w;
    image->height   = (UINT_32) h;

    image->data = malloc (size = w * h * depth);

    // reading data
    fread (image->data, 1, size, file);     fclose (file);

    UINT_8P p = (UINT_8P) image->data;

    // TGA is BGR instead of RGB

    UINT_32 i;

    for (i = 0; i < size; i += depth) {

        UINT_8 t =  p [i];
                    p [i] = p [i + 2];
                            p [i + 2] = t;
    }

    return 0;
}

#endif

///////////////////////////////////////////////////////////////////////////////
// IMAGE SAVE
///////////////////////////////////////////////////////////////////////////////

#if 1

INT_32 saveImage (CHARP filename, TImage *image)
{

    FILE *file;

    if (!(file = fopen (filename, "wb")))   return -1;

    UINT_8  data8;

    // numer of bytes for image identification
    data8 = 0;  // 0 - no image identification field
    fwrite (&data8,  SIZE_UINT_8, 1, file);

    // color Map Type
    data8 = 0;  // 0 - no color map
    fwrite (&data8,  SIZE_UINT_8, 1, file);

    // image Type Code
    data8 = 2;  // 2 - means image type 10 -> RGB image
    fwrite (&data8,  SIZE_UINT_8, 1, file);

    UINT_16 data16;

    // COLOR MAP SPECIFICATION

    // color Map Origin
    data16 = 0x00;
    fwrite (&data16, SIZE_UINT_16, 1, file);

    // color Map Length
    data16 = 0x00;
    fwrite (&data16, SIZE_UINT_16, 1, file);

    // color Map Entry Size
    data16 = 0x00;
    fwrite (&data8,  SIZE_UINT_8,  1, file);

    //  IMAGE SPECIFICATION

    // X origin of Image
    data16 = 0x00;
    fwrite (&data16, SIZE_UINT_16, 1, file);

    // Y origin of Image
    data16 = 0x00;
    fwrite (&data16, SIZE_UINT_16, 1, file);

    // width of Image
    data16 = (UINT_16) image->width;
    fwrite (&data16, SIZE_UINT_16, 1, file);

    // height of Image
    data16 = (UINT_16) image->height;
    fwrite (&data16, SIZE_UINT_16, 1, file);

    UINT_32 size = 0;

    // image pixel Size
    switch (image->type) {

        case M_IMAGE_TYPE_RGB:      data8 = 24; size = 3;   break;
        case M_IMAGE_TYPE_RGBA:     data8 = 32; size = 4;   break;
    }

    fwrite (&data8,  SIZE_UINT_8,  1, file);

    // image descriptor byte
    data8 = 0;  // ??
    fwrite (&data8,  SIZE_UINT_8,  1, file);

    // data length
    UINT_32 length = image->width * image->height * SIZE_UINT_8 * size;

    // RGB to BGR

    UINT_8P p = (UINT_8P) image->data;

    for (UINT_32 i = 0; i < length; i += size) {

        UINT_8 t =  p [i];
                    p [i] = p [i + 2];
                            p [i + 2] = t;
    }

    // write data
    fwrite (image->data, 1, length, file);
    
    fclose (file);

    return 0;
}

#endif

///////////////////////////////////////////////////////////////////////////////
// EXPORT FUNCTIONS
///////////////////////////////////////////////////////////////////////////////

extern "C"
{

    TImage errimg;
    
    UINT_32 moverlap, icount, w, h;

    UINT_32 kernela;
    UINT_32 kernels;

    FLOAT_32 tolerancy_face;
    FLOAT_32 tolerancy_texel;

    CHARPP imagefiles;  CHARP errorfile;

    TImage   images      [16]; 
    UINT_8P  imagescopy  [16];

    UINT_32P maskmap;
    UINT_32P masktags;

    ///////////////////////////////////////////////////////////////////////////////
    // PROCESS GET BIT COUNT
    ///////////////////////////////////////////////////////////////////////////////

    DECLDIR UINT_32 _stdcall processBitCount (UINT_32 bits)
    {
        // variable-precision SWAR algorithm

        bits = bits - ((bits >> 1) & 0x55555555);
        bits = (bits & 0x33333333) + ((bits >> 2) & 0x33333333);
        bits = ((bits + (bits >> 4) & 0xf0f0f0f) * 0x1010101) >> 24;

        return (bits);
    }

    /////////////////////////////////////////////////////////////////////////////////
    //// CHECK TEXEl
    /////////////////////////////////////////////////////////////////////////////////

    //inline VOID CkeckTexel (UINT_32 x, UINT_32 y)
    //{
    //    INT_32 y1 = y - 1; y1 = MAX (0, y1);
    //    INT_32 x1 = x - 1; x1 = MAX (0, x1);
    //    INT_32 y2 = y + 1; y2 = MIN ((INT_32) h - 1, y2);
    //    INT_32 x2 = x + 1; x2 = MIN ((INT_32) w - 1, x2);

    //    for (y = y1; y <= (UINT_32) y2; y ++) {
    //        for (x = x1; x <= (UINT_32) x2; x ++) {   UINT_32 i = ((y * w) + x) * 3;

    //            for (UINT_32 n = 0; n < icount; n ++) {

    //                UINT_8P p  = (UINT_8P) images [n].data;

    //                p [i] = 0;
    //            }
    //        }
    //    }
    //}

    ///////////////////////////////////////////////////////////////////////////////
    // CLEAR TEXEl
    ///////////////////////////////////////////////////////////////////////////////

    inline VOID ClearTexel (UINT_32 n, UINT_32 x, UINT_32 y)
    {
        UINT_8P p = (UINT_8P) images [n].data;

        INT_32 y1 = y - 1; y1 = MAX (0, y1);
        INT_32 x1 = x - 1; x1 = MAX (0, x1);
        INT_32 y2 = y + 1; y2 = MIN ((INT_32) h - 1, y2);
        INT_32 x2 = x + 1; x2 = MIN ((INT_32) w - 1, x2);

        for (y = y1; y <= (UINT_32) y2; y ++) {
            for (x = x1; x <= (UINT_32) x2; x ++) {

                UINT_32 i = ((y * w) + x) * 3;

                p [i    ] = 0;
                p [i + 1] = 0;
                p [i + 2] = 0;

                //for (UINT_32 m = 0; m < icount; m ++) {

                //    UINT_8P pp = (UINT_8P) images [m].data;

                //    // we got all zeros, need to fix that
                //    if (pp [i] == pp [i + 1] == pp [i + 2] == 0) {


                //    }
                //}
            }
        }

/*
        UINT_8P p  = (UINT_8P) images [n].data;
        UINT_8P po = imagescopy [n];

        INT_32 y1 = y - 1; y1 = MAX (0, y1);
        INT_32 x1 = x - 1; x1 = MAX (0, x1);
        INT_32 y2 = y + 1; y2 = MIN ((INT_32) h - 1, y2);
        INT_32 x2 = x + 1; x2 = MIN ((INT_32) w - 1, x2);

        INT_32 y11 = y - (1 + kernela); y11 = MAX (0, y11);
        INT_32 x11 = x - (1 + kernela); x11 = MAX (0, x11);
        INT_32 y22 = y + (1 + kernela); y22 = MIN ((INT_32) h - 1, y22);
        INT_32 x22 = x + (1 + kernela); x22 = MIN ((INT_32) w - 1, x22);

        FLOAT_32 factorx;
        FLOAT_32 factory;

        // we sample from image copy
        for (y = y11; y <= (UINT_32) y22; y ++) {

            if (y < (UINT_32) y1) {

                factory = (y1 - y) / ((FLOAT_32) kernela + 1.0f);

                for (x = x11; x <= (UINT_32) x22; x ++) {   UINT_32 i = ((y * w) + x) * 3;

                    if (x < (UINT_32) x1) {

                        factorx = (x1 - x)  / ((FLOAT_32) kernela + 1.0f);

                        FLOAT_32 factor = sqrt (factorx*factorx + factory*factory); factor = MIN (1.0f, factor);

                        UINT_8 val = (UINT_8) floor (factor * po [i]);

                        if (p [i] > val) {  p  [i    ] = val;   p  [i + 1] = val;   p  [i + 2] = val;   }

                    } else {

                        if (x > (UINT_32) x2) {

                            factorx = (x - x2) / ((FLOAT_32) kernela + 1.0f);

                            FLOAT_32 factor = sqrt (factorx*factorx + factory*factory); factor = MIN (1.0f, factor);

                            UINT_8 val = (UINT_8) floor (factor * po [i]);

                            if (p [i] > val) {  p  [i    ] = val;   p  [i + 1] = val;   p  [i + 2] = val;   }

                        } else {

                            UINT_8 val = (UINT_8) floor (factory * po [i]);

                            if (p [i] > val) {  p  [i    ] = val;   p  [i + 1] = val;   p  [i + 2] = val;   }
                        }
                    }
                }

            } else {

                if (y > (UINT_32) y2) {

                    factory = (y  - y2) / ((FLOAT_32) kernela + 1.0f);

                    for (x = x11; x <= (UINT_32) x22; x ++) {   UINT_32 i = ((y * w) + x) * 3;

                        if (x < (UINT_32) x1) {

                            factorx = (x1 - x) / ((FLOAT_32) kernela + 1.0f);

                            FLOAT_32 factor = sqrt (factorx*factorx + factory*factory); factor = MIN (1.0f, factor);

                            UINT_8 val = (UINT_8) floor (factor * po [i]);

                            if (p [i] > val) {  p  [i    ] = val;   p  [i + 1] = val;   p  [i + 2] = val;   }

                        } else {

                            if (x > (UINT_32) x2) {

                                factorx = (x - x2) / ((FLOAT_32) kernela + 1.0f);

                                FLOAT_32 factor = sqrt (factorx*factorx + factory*factory); factor = MIN (1.0f, factor);

                                UINT_8 val = (UINT_8) floor (factor * po [i]);

                                if (p [i] > val) {  p  [i    ] = val;   p  [i + 1] = val;   p  [i + 2] = val;   }

                            } else {

                                UINT_8 val = (UINT_8) floor (factory * po [i]);

                                if (p [i] > val) {  p  [i    ] = val;   p  [i + 1] = val;   p  [i + 2] = val;   }
                            }
                        }
                    }

                } else {

                    for (x = x11; x <= (UINT_32) x22; x ++) {   UINT_32 i = ((y * w) + x) * 3;

                        if (x < (UINT_32) x1) {

                            factorx = (x1 - x) / ((FLOAT_32) kernela + 1.0f);

                            UINT_8 val = (UINT_8) floor (factorx * po [i]);

                            if (p [i] > val) {  p  [i    ] = val;   p  [i + 1] = val;   p  [i + 2] = val;   }

                        } else {

                            if (x > (UINT_32) x2) {

                                factorx = (x - x2) / ((FLOAT_32) kernela + 1.0f);

                                UINT_8 val = (UINT_8) floor (factorx * po [i]);

                                if (p [i] > val) {  p  [i    ] = val;   p  [i + 1] = val;   p  [i + 2] = val;   }

                            } else {

                                p  [i    ] = 0;
                                p  [i + 1] = 0;
                                p  [i + 2] = 0;
                            }
                        }
                    }
                }
            }
        }
*/
    }

    ///////////////////////////////////////////////////////////////////////////////
    // OVERLAPPING TEXEL
    ///////////////////////////////////////////////////////////////////////////////

    inline UINT_32 OverlappingTexel (UINT_32 r, UINT_32 x, UINT_32 y, UINT_32P sums, UINT_32P maxs)
    {

        INT_32 y1 = y - 1; y1 = MAX (0, y1);
        INT_32 x1 = x - 1; x1 = MAX (0, x1);
        INT_32 y2 = y + 1; y2 = MIN ((INT_32) h - 1, y2);
        INT_32 x2 = x + 1; x2 = MIN ((INT_32) w - 1, x2);

        ///
        UINT_32 mask = 0;


        UINT_32 c = 0;

        for (UINT_32 n = 0; n < icount; n ++) {

            UINT_8P p = (UINT_8P) images [n].data;  ///imagescopy [n];

            BOOL inc = false;

            for (UINT_32 yy = y1; yy <= (UINT_32) y2; yy ++) {
                for (UINT_32 xx = x1; xx <= (UINT_32) x2; xx ++) {


                    UINT_32 ii = (yy * w) + xx;

                    mask |= maskmap [ii];



                    // data offset
                    UINT_32 i = ((yy * w) + xx) * 3;

                    if (p [i] > maxs [n]) maxs [n] = p [i];

                    if (p [i] > 0) {

                        sums [n] += p [i];

                        inc = true;
                    }
                }
            }

            if (inc) c ++;
        }



        UINT_32 cc = 0;
        for (UINT_32 n = 0; n < icount; n ++) {

            if (mask & (1 << n)) cc ++;
        }

        if (cc > moverlap) {

            cc = 0;

            for (UINT_32 n = 0; n < icount; n ++) {
                for (UINT_32 yy = y1; yy <= (UINT_32) y2; yy ++) {
                    for (UINT_32 xx = x1; xx <= (UINT_32) x2; xx ++) {

                        UINT_32 ii = (yy * w) + xx;

                        mask |= maskmap [ii];
                    }
                }
            }
        }


        return c;


/*
        // data offset
        UINT_32 i = ((y * w) + x) * 3;

        UINT_32 c = 0;
        for (UINT_32 n = 0; n < icount; n ++) {

            sums [n] += imagescopy [n][i]; if (imagescopy [n][i] > 0) c ++;
        }

        return c;
*/
/*


        INT_32 y1 = y - r; y1 = MAX (0, y1);
        INT_32 x1 = x - r; x1 = MAX (0, x1);
        INT_32 y2 = y + r; y2 = MIN ((INT_32) h - 1, y2);
        INT_32 x2 = x + r; x2 = MIN ((INT_32) w - 1, x2);

        UINT_32 c = 0;

        for (UINT_32 n = 0; n < icount; n ++) {

            UINT_8P p = imagescopy [n];

            BOOL inc = false;

            for (UINT_32 yy = y1; yy <= (UINT_32) y2; yy ++) {
                for (UINT_32 xx = x1; xx <= (UINT_32) x2; xx ++) {

                    UINT_32 xsqr = xx - x1;  xsqr *= xsqr;
                    UINT_32 ysqr = yy - y1;  ysqr *= ysqr;

                    // radius check
                    if (xsqr + ysqr < r*r) {

                        // data offset
                        UINT_32 i = ((yy * w) + xx) * 3;

                        if (p [i] > 0) {

                            sums [n] += p [i];

                            inc = true;
                        }
                    }
                }
            }

            if (inc) c ++;
        }

        return c;
*/
    }

    ///////////////////////////////////////////////////////////////////////////////
    // CHECK TEXEL
    ///////////////////////////////////////////////////////////////////////////////

    inline BOOL CheckTexel (UINT_32 mask, UINT_32 x, UINT_32 y)
    {
        INT_32 y1 = y - 1;    y1 = MAX (0, y1);
        INT_32 x1 = x - 1;    x1 = MAX (0, x1);
        INT_32 y2 = y + 1;    y2 = MIN ((INT_32) h - 1, y2);
        INT_32 x2 = x + 1;    x2 = MIN ((INT_32) w - 1, x2);

        UINT_32 c = 0;
        for (UINT_32 yy = y1; yy <= (UINT_32) y2; yy ++) {
            for (UINT_32 xx = x1; xx <= (UINT_32) x2; xx ++) {

                UINT_32 i = (yy * w) + xx;

                if ((masktags [i] > 0) && ((maskmap [i] & mask) == 0)) c ++;
            }
        }

        if (c > 0)  return true;    else 
                    return false;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // MASK TEXEL
    ///////////////////////////////////////////////////////////////////////////////

    inline VOID MaskTexel (UINT_32 mask, UINT_32 x, UINT_32 y)
    {
        INT_32 y1 = y - 1;    y1 = MAX (0, y1);
        INT_32 x1 = x - 1;    x1 = MAX (0, x1);
        INT_32 y2 = y + 1;    y2 = MIN ((INT_32) h - 1, y2);
        INT_32 x2 = x + 1;    x2 = MIN ((INT_32) w - 1, x2);

        //UINT_32 c = 0;
        //for (UINT_32 n = 0; n < icount; n ++) {
        //    if (mask & (1 << n)) c ++;
        //}

        //if (c > moverlap) {

        //    c = 0;
        //}

        for (UINT_32 yy = y1; yy <= (UINT_32) y2; yy ++) {
            for (UINT_32 xx = x1; xx <= (UINT_32) x2; xx ++) {

                UINT_32 i = (yy * w) + xx;

                if (masktags [i] > 0) {

                    maskmap [i] &= mask; 

                    //// we got problem
                    //if ((maskmap [i] & mask) == 0) {

                    //    /// FUCKING PROBLEM
                    //    /// WE HAVE TO REMOVE ONE MORE LAYER FROM TRIANGLE
                    //    /// PLUS WE HAVE TO ADD NEW LAYER TO TRIANGLE
                    //    maskmap [i] &= mask; 

                    //} else {
                    //    maskmap [i] &= mask; 
                    //}

                } else {    maskmap [i] = mask;
                }

                masktags [i] ++;

                //UINT_32 c = 0;
                //for (UINT_32 n = 0; n < icount; n ++) {
                //    if (maskmap [i] & (1 << n)) c ++;
                //}

                //if (c > moverlap) {

                //    c = 0;
                //}
            }
        }

        //mask = maskmap [y * w + x];

        //for (UINT_32 yy = y1; yy <= (UINT_32) y2; yy ++) {
        //    for (UINT_32 xx = x1; xx <= (UINT_32) x2; xx ++) {

        //        mask &= maskmap [y * w + x];
        //    }
        //}
    }

    ///////////////////////////////////////////////////////////////////////////////
    // POLISH TEXEL
    ///////////////////////////////////////////////////////////////////////////////

    inline VOID PolishTexel (UINT_32 x, UINT_32 y, UINT_32 threshold)
    {

        UINT_32 max = 0;

        INT_32 y1 = y - 1;    y1 = MAX (0, y1);
        INT_32 x1 = x - 1;    x1 = MAX (0, x1);
        INT_32 y2 = y + 1;    y2 = MIN ((INT_32) h - 1, y2);
        INT_32 x2 = x + 1;    x2 = MIN ((INT_32) w - 1, x2);

        //for (UINT_32 yy = y1; yy <= (UINT_32) y2; yy ++) {
        //    for (UINT_32 xx = x1; xx <= (UINT_32) x2; xx ++) {

        //        UINT_32 i = ((yy * w) + xx) * 3;

        //        UINT_32 mask = maskmap [((yy * w) + xx)];

        //        for (UINT_32 n = 0; n < icount; n ++) {

        //            if (mask & (1 << n)) {

        //                UINT_8P p = (UINT_8P) images [n].data;

        //                UINT_32 val = p [i];

        //                if (val > max) max = val;
        //            }
        //        }
        //    }
        //}

        ///if (max < threshold) {

            for (UINT_32 yy = y1; yy <= (UINT_32) y2; yy ++) {
                for (UINT_32 xx = x1; xx <= (UINT_32) x2; xx ++) {

                    UINT_32 i = ((yy * w) + xx) * 3;

                    UINT_32 mask = maskmap [((yy * w) + xx)];

                    if (mask == 0) {

                        INT_32 yy1 = yy - 1;    yy1 = MAX (0, yy1);
                        INT_32 xx1 = xx - 1;    xx1 = MAX (0, xx1);
                        INT_32 yy2 = yy + 1;    yy2 = MIN ((INT_32) h - 1, yy2);
                        INT_32 xx2 = xx + 1;    xx2 = MIN ((INT_32) w - 1, xx2);

                        for (UINT_32 yyy = yy1; yyy <= (UINT_32) yy2; yyy ++) {
                            for (UINT_32 xxx = xx1; xxx <= (UINT_32) xx2; xxx ++) {

                                if ((yy == yyy) && (xx == xxx)) continue;

                                if (mask == 0)  mask  = maskmap [((yyy * w) + xxx)];
                                else            mask &= maskmap [((yyy * w) + xxx)];
                            }
                        }
                    }

                    for (UINT_32 n = 0; n < icount; n ++) {

                        UINT_8P p = (UINT_8P) images [n].data;

                        if (mask & (1 << n)) {

                            //UINT_32 avr = 0;

                            //for (UINT_32 yy = y1; yy <= (UINT_32) y2; yy ++) {
                            //    for (UINT_32 xx = x1; xx <= (UINT_32) x2; xx ++) {

                            //        avr += p [((yy * w) + xx) * 3];
                            //    }
                            //}

                            //avr /= 9;   avr = MAX (1, avr);

                            p [i    ] = 150; ///avr;
                            p [i + 1] = 150; ///avr;
                            p [i + 2] = 150; ///avr;

                        } else {

                            p [i    ] = 0; ///avr;
                            p [i + 1] = 0; ///avr;
                            p [i + 2] = 0; ///avr;
                        }
                    }
                }
            }
        ///}

/*
        INT_32 y1 = y - 1; y1 = MAX (0, y1);
        INT_32 x1 = x - 1; x1 = MAX (0, x1);
        INT_32 y2 = y + 1; y2 = MIN ((INT_32) h - 1, y2);
        INT_32 x2 = x + 1; x2 = MIN ((INT_32) w - 1, x2);

        for (UINT_32 yy = y1; yy <= (UINT_32) y2; yy ++) {
            for (UINT_32 xx = x1; xx <= (UINT_32) x2; xx ++) {

                UINT_32 max = 0;

                for (UINT_32 n = 0; n < icount; n ++) {

                    if (mask & (1 << n)) {

                        UINT_8P p = (UINT_8P) images [n].data;

                        UINT_32 i = ((yy * w) + xx) * 3;

                        UINT_32 val = p [i];

                        if (val > max) max = val;
                    }
                }
    
                if (max < 1) {

                    for (UINT_32 n = 0; n < icount; n ++) {

                        if (mask & (1 << n)) {

                            UINT_8P p = (UINT_8P) images [n].data;

                            UINT_32 i = ((yy * w) + xx) * 3;

                            INT_32 y11 = yy - 1;    y11 = MAX (0, y11);
                            INT_32 x11 = xx - 1;    x11 = MAX (0, x11);
                            INT_32 y22 = yy + 1;    y22 = MIN ((INT_32) h - 1, y22);
                            INT_32 x22 = xx + 1;    x22 = MIN ((INT_32) w - 1, x22);

                            UINT_32 avr = 0;

                            for (UINT_32 yyy = y1; yyy <= (UINT_32) y2; yyy ++) {
                                for (UINT_32 xxx = x1; xxx <= (UINT_32) x2; xxx ++) {

                                    avr += p [((yyy * w) + xxx) * 3];
                                }
                            }

                            p [i    ] = avr / 9;
                            p [i + 1] = avr / 9;
                            p [i + 2] = avr / 9;
                        }
                    }
                }
            }
        }
*/
    }

    ///////////////////////////////////////////////////////////////////////////////
    // UNVEIL TEXEL
    ///////////////////////////////////////////////////////////////////////////////

    inline UINT_32 UnveilTexel (UINT_32 x, UINT_32 y, INT_32 l)
    {

        INT_32 y1 = y - kernels;    y1 = MAX (0, y1);
        INT_32 x1 = x - kernels;    x1 = MAX (0, x1);
        INT_32 y2 = y + kernels;    y2 = MIN ((INT_32) h - 1, y2);
        INT_32 x2 = x + kernels;    x2 = MIN ((INT_32) w - 1, x2);

        UINT_32 sums [16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

        INT_32 y11 = y - 1; y11 = MAX (0, y11);
        INT_32 x11 = x - 1; x11 = MAX (0, x11);
        INT_32 y22 = y + 1; y22 = MIN ((INT_32) h - 1, y22);
        INT_32 x22 = x + 1; x22 = MIN ((INT_32) w - 1, x22);

        UINT_32 c = 0;
        UINT_32 mask = 0;
        for (UINT_32 n = 0; n < icount; n ++) {

            UINT_8P p = imagescopy [n];

            for (UINT_32 yy = y11; yy <= (UINT_32) y22; yy ++) {
                for (UINT_32 xx = x11; xx <= (UINT_32) x22; xx ++) {

                    if (p [((yy * w) + xx) * 3] > 0) {

                        mask |= 1 << n;     c ++;
                        goto end;  
                    }
                }
            }

            end:;
        }

        if (c  > moverlap) {
            c -= moverlap;

            for (UINT_32 n = 0; n < icount; n ++) {

                UINT_8P p = imagescopy [n];

                for (UINT_32 yy = y1; yy <= (UINT_32) y2; yy ++) {
                    for (UINT_32 xx = x1; xx <= (UINT_32) x2; xx ++) {

                        UINT_32 xsqr = xx - x1;  xsqr *= xsqr;
                        UINT_32 ysqr = yy - y1;  ysqr *= ysqr;

                        // radius check
                        if (xsqr + ysqr < kernels*kernels) {

                            sums [n] += p [((yy * w) + xx) * 3];
                        }
                    }
                }
            }

            for (UINT_32 i = 0; i < c; i ++) {

                UINT_32 min = 0xffffffff;   UINT_32 min_index = 0xffffffff;

                for (UINT_32 n = 0; n < icount; n ++) {

                    if ((mask & (1 << n)) && (sums [n] > 0) && (sums [n] < sums [min_index]))    min_index = n;
                }

                sums [min_index] = 0;

                if ((l < 0) || (min_index == l)) ClearTexel (min_index, x, y);
            }
        }

        return c;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // PROCESS TEX UVEIL TEXEL TRIANGLE
    ///////////////////////////////////////////////////////////////////////////////

    DECLDIR VOID _stdcall processTexUnveilTexelTriangle  (FLOAT_32 u1, FLOAT_32 v1, 
                                                          FLOAT_32 u2, FLOAT_32 v2, 
                                                          FLOAT_32 u3, FLOAT_32 v3, INT_32 l)
    {

        UINT_32 order [3];

        FLOAT_32 uv [3][2] = { {u1, v1}, {u2, v2}, {u3, v3} };

        FLOAT_32 d1, d0, add, x, y;

        FLOAT_32 ystart;
        FLOAT_32 yend;

        // find the lowest Y
        if (uv [0][1] < uv [1][1]) {

            if (uv [0][1] < uv [2][1]) order [0] = 0; else
                                       order [0] = 2;        } else {
            if (uv [1][1] < uv [2][1]) order [0] = 1; else
                                       order [0] = 2;
        }

        // find the highest Y
        if (uv [0][1] > uv [1][1]) {

            if (uv [0][1] > uv [2][1]) order [2] = 0; else
                                       order [2] = 2;        } else {
            if (uv [1][1] > uv [2][1]) order [2] = 1; else
                                       order [2] = 2;
        }

        // and the middle one is a matter of deduction
        order [1] = 3 - (order [0] + order [2]);  

        // degenerated triangle
        if ((uv [order [0]][1] - uv [order [2]][1]) == 0.0) return;

        // pixel size
        add = 1.0f / (FLOAT_32) h;

        // main edge
        d1 = (uv [order [2]][0] - uv [order [0]][0]) / (uv [order [2]][1] - uv [order [0]][1]);

        //------------------------------------------------------
        // UNVEIL OVERLAPPING PER TEXEL
        //------------------------------------------------------

        // is there top half ?
        if ((uv [order [1]][1] - uv [order [0]][1]) != 0.0) {

            // slopes for first half
            d0 = (uv [order [1]][0] - uv [order [0]][0]) / (uv [order [1]][1] - uv [order [0]][1]);

            ystart = uv [order [0]][1]; ystart = MIN (1.0f, ystart); ystart = MAX (0.0f, ystart);
            yend   = uv [order [1]][1]; yend   = MIN (1.0f, yend);   yend   = MAX (0.0f, yend);

            for (y = ystart; y <= yend; y += add) {

                FLOAT_32 x1 = uv [order [0]][0] + d0 * (y - ystart);    x1 = MIN (1.0f, x1); x1 = MAX (0.0f, x1);
                FLOAT_32 x2 = uv [order [0]][0] + d1 * (y - ystart);    x2 = MIN (1.0f, x2); x2 = MAX (0.0f, x2);

                if (x1 < x2) {
                    for (x = x1; x <= x2; x += add) UnveilTexel ((UINT_32) floor (x * (w - 1)), (UINT_32) floor (y * (h - 1)), l);   } else {
                    for (x = x2; x <= x1; x += add) UnveilTexel ((UINT_32) floor (x * (w - 1)), (UINT_32) floor (y * (h - 1)), l);
                }
            }
        }

        // is there bottom half ?
        if ((uv [order [2]][1] - uv [order [1]][1]) != 0.0) {

            // slope for second half
            d0 = (uv [order [2]][0] - uv [order [1]][0]) / (uv [order [2]][1] - uv [order [1]][1]);

            ystart = uv [order [1]][1]; ystart = MIN (1.0f, ystart); ystart = MAX (0.0f, ystart);
            yend   = uv [order [2]][1]; yend   = MIN (1.0f, yend);   yend   = MAX (0.0f, yend);

            for (y = ystart; y <= yend; y += add) {

                FLOAT_32 x1 = uv [order [1]][0] + d0 * (y - ystart);                x1 = MIN (1.0f, x1); x1 = MAX (0.0f, x1);
                FLOAT_32 x2 = uv [order [0]][0] + d1 * (y - uv [order [0]][1]);     x2 = MIN (1.0f, x2); x2 = MAX (0.0f, x2);

                if (x1 < x2) {
                    for (x = x1; x <= x2; x += add) UnveilTexel ((UINT_32) floor (x * (w - 1)), (UINT_32) floor (y * (h - 1)), l);   } else {
                    for (x = x2; x <= x1; x += add) UnveilTexel ((UINT_32) floor (x * (w - 1)), (UINT_32) floor (y * (h - 1)), l);
                }
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////
    // PROCESS TEX UVEIL FACE TRIANGLE
    ///////////////////////////////////////////////////////////////////////////////
/*
    DECLDIR VOID _stdcall processTexUnveilFaceTriangle  (FLOAT_32 u1, FLOAT_32 v1, 
                                                         FLOAT_32 u2, FLOAT_32 v2, 
                                                         FLOAT_32 u3, FLOAT_32 v3, INT_32 l)
    {

        UINT_32 order [3];

        FLOAT_32 uv [3][2] = { {u1, v1}, {u2, v2}, {u3, v3} };

        FLOAT_32 d1, d0, add, x, y;

        FLOAT_32 ystart;
        FLOAT_32 yend;

        UINT_32 sums [16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};


        // find the lowest Y
        if (uv [0][1] < uv [1][1]) {

            if (uv [0][1] < uv [2][1]) order [0] = 0; else
                                       order [0] = 2;        } else {
            if (uv [1][1] < uv [2][1]) order [0] = 1; else
                                       order [0] = 2;
        }

        // find the highest Y
        if (uv [0][1] > uv [1][1]) {

            if (uv [0][1] > uv [2][1]) order [2] = 0; else
                                       order [2] = 2;        } else {
            if (uv [1][1] > uv [2][1]) order [2] = 1; else
                                       order [2] = 2;
        }

        // and the middle one is a matter of deduction
        order [1] = 3 - (order [0] + order [2]);  

        // degenerated triangle
        if ((uv [order [0]][1] - uv [order [2]][1]) == 0.0) return;

        // pixel size
        add = 1.0f / (FLOAT_32) h;

        // main edge
        d1 = (uv [order [2]][0] - uv [order [0]][0]) / (uv [order [2]][1] - uv [order [0]][1]);

        //------------------------------------------------------
        // CHECK LAYER OVERLAPPING PER FACE
        //------------------------------------------------------
        
        // is there top half ?
        if ((uv [order [1]][1] - uv [order [0]][1]) != 0.0) {

            // slopes for first half
            d0 = (uv [order [1]][0] - uv [order [0]][0]) / (uv [order [1]][1] - uv [order [0]][1]);

            ystart = uv [order [0]][1]; ystart = MIN (1.0f, ystart); ystart = MAX (0.0f, ystart);
            yend   = uv [order [1]][1]; yend   = MIN (1.0f, yend);   yend   = MAX (0.0f, yend);

            for (y = ystart; y <= yend; y += add) {

                FLOAT_32 x1 = uv [order [0]][0] + d0 * (y - ystart);    x1 = MIN (1.0f, x1); x1 = MAX (0.0f, x1);
                FLOAT_32 x2 = uv [order [0]][0] + d1 * (y - ystart);    x2 = MIN (1.0f, x2); x2 = MAX (0.0f, x2);

                if (x1 < x2) {
                    for (x = x1; x <= x2; x += add) OverlappingTexel (2, (UINT_32) floor (x * (w - 1)), (UINT_32) floor (y * (h - 1)), sums);   } else {
                    for (x = x2; x <= x1; x += add) OverlappingTexel (2, (UINT_32) floor (x * (w - 1)), (UINT_32) floor (y * (h - 1)), sums);
                }
            }
        }

        // is there bottom half ?
        if ((uv [order [2]][1] - uv [order [1]][1]) != 0.0) {

            // slope for second half
            d0 = (uv [order [2]][0] - uv [order [1]][0]) / (uv [order [2]][1] - uv [order [1]][1]);

            ystart = uv [order [1]][1]; ystart = MIN (1.0f, ystart); ystart = MAX (0.0f, ystart);
            yend   = uv [order [2]][1]; yend   = MIN (1.0f, yend);   yend   = MAX (0.0f, yend);

            for (y = ystart; y <= yend; y += add) {

                FLOAT_32 x1 = uv [order [1]][0] + d0 * (y - ystart);                x1 = MIN (1.0f, x1); x1 = MAX (0.0f, x1);
                FLOAT_32 x2 = uv [order [0]][0] + d1 * (y - uv [order [0]][1]);     x2 = MIN (1.0f, x2); x2 = MAX (0.0f, x2);

                if (x1 < x2) {
                    for (x = x1; x <= x2; x += add) OverlappingTexel (2, (UINT_32) floor (x * (w - 1)), (UINT_32) floor (y * (h - 1)), sums);   } else {
                    for (x = x2; x <= x1; x += add) OverlappingTexel (2, (UINT_32) floor (x * (w - 1)), (UINT_32) floor (y * (h - 1)), sums);
                }
            }
        }

        UINT_32 mask = 0x00000000;  UINT_32 c = 0;

        // we construct bit mask and check if there is not too many layers per whole face
        for (UINT_32 i = 0; i < icount; i ++) { 

            if (sums [i] > 0) {
                mask |= 1 << i; c ++;
            }
        }

        //------------------------------------------------------
        // UNVEIL OVERLAPPING PER FACE
        //------------------------------------------------------

        // we have to unveil problematic overlapped parts
        if (c > moverlap) {

            FLOAT_32 centeru = (u1 + u2 + u3) / 3.0f;
            FLOAT_32 centerv = (v1 + v2 + v3) / 3.0f;

            FLOAT_32 r1 = sqrt ((u1 - centeru)*(u1 - centeru) + (v1 - centerv)*(v1 - centerv));
            FLOAT_32 r2 = sqrt ((u2 - centeru)*(u2 - centeru) + (v2 - centerv)*(v2 - centerv));
            FLOAT_32 r3 = sqrt ((u3 - centeru)*(u3 - centeru) + (v3 - centerv)*(v3 - centerv));

            FLOAT_32 r = r1;
            if (r2 > r) r = r2;
            if (r3 > r) r = r3;

            UINT_32 k = (UINT_32) floor (r * (w - 1) + kernels);

            UINT_32 cx = (UINT_32) floor (centeru * (w - 1));
            UINT_32 cy = (UINT_32) floor (centerv * (h - 1));

            INT_32 y1 = cy - k;  y1 = MAX (0, y1);
            INT_32 x1 = cx - k;  x1 = MAX (0, x1);
            INT_32 y2 = cy + k;  y2 = MIN ((INT_32) h - 1, y2);
            INT_32 x2 = cx + k;  x2 = MIN ((INT_32) w - 1, x2);

            UINT_32 sums [16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

            for (UINT_32 n = 0; n < icount; n ++) {

                UINT_8P p = imagescopy [n];

                for (UINT_32 yy = y1; yy <= (UINT_32) y2; yy ++) {
                    for (UINT_32 xx = x1; xx <= (UINT_32) x2; xx ++) {

                        UINT_32 xsqr = xx - x1;  xsqr *= xsqr;
                        UINT_32 ysqr = yy - y1;  ysqr *= ysqr;

                        // radius check
                        if (xsqr + ysqr < k*k) {

                            sums [n] += p [((yy * w) + xx) * 3];
                        }
                    }
                }
            }

            // difference
            c -= moverlap;

            for (UINT_32 i = 0; i < c; i ++) {

                UINT_32 min = 0xffffffff;   UINT_32 min_index = 0;

                for (UINT_32 n = 0; n < icount; n ++) {

                    if (mask & (1 << n)) {

                        if ((sums [n] > 0) && (sums [n] < min)) {

                            min = sums [n];    min_index = n;
                        }
                    }
                }

                sums [min_index] = 0;

                mask &= ~(1 << min_index);

                if ((l < 0) || (l == min_index)) {

                    // is there top half ?
                    if ((uv [order [1]][1] - uv [order [0]][1]) != 0.0) {

                        // slopes for first half
                        d0 = (uv [order [1]][0] - uv [order [0]][0]) / (uv [order [1]][1] - uv [order [0]][1]);

                        ystart = uv [order [0]][1]; ystart = MIN (1.0f, ystart); ystart = MAX (0.0f, ystart);
                        yend   = uv [order [1]][1]; yend   = MIN (1.0f, yend);   yend   = MAX (0.0f, yend);

                        for (y = ystart; y <= yend; y += add) {

                            FLOAT_32 x1 = uv [order [0]][0] + d0 * (y - ystart);
                            FLOAT_32 x2 = uv [order [0]][0] + d1 * (y - ystart);

                            x1 = MIN (1.0f, x1); x1 = MAX (0.0f, x1);
                            x2 = MIN (1.0f, x2); x2 = MAX (0.0f, x2);

                            if (x1 < x2) {  for (x = x1; x <= x2; x += add) ClearTexel (min_index, (UINT_32) floor (x * (w - 1)), (UINT_32) floor (y * (h - 1)));
                            } else {        for (x = x2; x <= x1; x += add) ClearTexel (min_index, (UINT_32) floor (x * (w - 1)), (UINT_32) floor (y * (h - 1)));
                            }
                        }
                    }

                    // is there bottom half ?
                    if ((uv [order [2]][1] - uv [order [1]][1]) != 0.0) {

                        // slope for second half
                        d0 = (uv [order [2]][0] - uv [order [1]][0]) / (uv [order [2]][1] - uv [order [1]][1]);

                        ystart = uv [order [1]][1]; ystart = MIN (1.0f, ystart); ystart = MAX (0.0f, ystart);
                        yend   = uv [order [2]][1]; yend   = MIN (1.0f, yend);   yend   = MAX (0.0f, yend);

                        for (y = ystart; y <= yend; y += add) {

                            FLOAT_32 x1 = uv [order [1]][0] + d0 * (y - ystart);
                            FLOAT_32 x2 = uv [order [0]][0] + d1 * (y - uv [order [0]][1]);

                            x1 = MIN (1.0f, x1); x1 = MAX (0.0f, x1);
                            x2 = MIN (1.0f, x2); x2 = MAX (0.0f, x2);

                            if (x1 < x2) {  for (x = x1; x <= x2; x += add) ClearTexel (min_index, (UINT_32) floor (x * (w - 1)), (UINT_32) floor (y * (h - 1)));
                            } else {        for (x = x2; x <= x1; x += add) ClearTexel (min_index, (UINT_32) floor (x * (w - 1)), (UINT_32) floor (y * (h - 1)));
                            }
                        }
                    }

                    // remove bit
                    mask &= ~(1 << min_index);
                }
            }

            //------------------------------------------------------
            // POLISH "WEAK" TEXELS
            //------------------------------------------------------

            ///if (l < 0) return;

            // is there top half ?
            if ((uv [order [1]][1] - uv [order [0]][1]) != 0.0) {

                // slopes for first half
                d0 = (uv [order [1]][0] - uv [order [0]][0]) / (uv [order [1]][1] - uv [order [0]][1]);

                ystart = uv [order [0]][1]; ystart = MIN (1.0f, ystart); ystart = MAX (0.0f, ystart);
                yend   = uv [order [1]][1]; yend   = MIN (1.0f, yend);   yend   = MAX (0.0f, yend);

                for (y = ystart; y <= yend; y += add) {

                    FLOAT_32 x1 = uv [order [0]][0] + d0 * (y - ystart);
                    FLOAT_32 x2 = uv [order [0]][0] + d1 * (y - ystart);

                    x1 = MIN (1.0f, x1); x1 = MAX (0.0f, x1);
                    x2 = MIN (1.0f, x2); x2 = MAX (0.0f, x2);

                    if (x1 < x2) {  for (x = x1; x <= x2; x += add) PolishTexel (mask, (UINT_32) floor (x * (w - 1)), (UINT_32) floor (y * (h - 1)));
                    } else {        for (x = x2; x <= x1; x += add) PolishTexel (mask, (UINT_32) floor (x * (w - 1)), (UINT_32) floor (y * (h - 1)));
                    }
                }
            }

            // is there bottom half ?
            if ((uv [order [2]][1] - uv [order [1]][1]) != 0.0) {

                // slope for second half
                d0 = (uv [order [2]][0] - uv [order [1]][0]) / (uv [order [2]][1] - uv [order [1]][1]);

                ystart = uv [order [1]][1]; ystart = MIN (1.0f, ystart); ystart = MAX (0.0f, ystart);
                yend   = uv [order [2]][1]; yend   = MIN (1.0f, yend);   yend   = MAX (0.0f, yend);

                for (y = ystart; y <= yend; y += add) {

                    FLOAT_32 x1 = uv [order [1]][0] + d0 * (y - ystart);
                    FLOAT_32 x2 = uv [order [0]][0] + d1 * (y - uv [order [0]][1]);

                    x1 = MIN (1.0f, x1); x1 = MAX (0.0f, x1);
                    x2 = MIN (1.0f, x2); x2 = MAX (0.0f, x2);

                    if (x1 < x2) {  for (x = x1; x <= x2; x += add) PolishTexel (mask, (UINT_32) floor (x * (w - 1)), (UINT_32) floor (y * (h - 1)));
                    } else {        for (x = x2; x <= x1; x += add) PolishTexel (mask, (UINT_32) floor (x * (w - 1)), (UINT_32) floor (y * (h - 1)));
                    }
                }
            }
        }
    }
*/
    ///////////////////////////////////////////////////////////////////////////////
    // PROCESS TEX UVEIL FACE TRIANGLE
    ///////////////////////////////////////////////////////////////////////////////

    DECLDIR VOID _stdcall processTexPolishFaceTriangle  (FLOAT_32 u1, FLOAT_32 v1, 
                                                         FLOAT_32 u2, FLOAT_32 v2, 
                                                         FLOAT_32 u3, FLOAT_32 v3, UINT_32 threshold)
    {
        UINT_32 order [3];

        FLOAT_32 uv [3][2] = { {u1, v1}, {u2, v2}, {u3, v3} };

        FLOAT_32 d1, d0, add, x, y;

        FLOAT_32 ystart;
        FLOAT_32 yend;

        UINT_32 sums [16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
        UINT_32 maxs [16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};


        // find the lowest Y
        if (uv [0][1] < uv [1][1]) {

            if (uv [0][1] < uv [2][1]) order [0] = 0; else
                                       order [0] = 2;        } else {
            if (uv [1][1] < uv [2][1]) order [0] = 1; else
                                       order [0] = 2;
        }

        // find the highest Y
        if (uv [0][1] > uv [1][1]) {

            if (uv [0][1] > uv [2][1]) order [2] = 0; else
                                       order [2] = 2;        } else {
            if (uv [1][1] > uv [2][1]) order [2] = 1; else
                                       order [2] = 2;
        }

        // and the middle one is a matter of deduction
        order [1] = 3 - (order [0] + order [2]);  

        // degenerated triangle
        if ((uv [order [0]][1] - uv [order [2]][1]) == 0.0) return;

        // pixel size
        add = 1.0f / (FLOAT_32) h;

        // main edge
        d1 = (uv [order [2]][0] - uv [order [0]][0]) / (uv [order [2]][1] - uv [order [0]][1]);

        //------------------------------------------------------
        // POLISH "DAMAGED" TEXELS
        //------------------------------------------------------
        
        // is there top half ?
        if ((uv [order [1]][1] - uv [order [0]][1]) != 0.0) {

            // slopes for first half
            d0 = (uv [order [1]][0] - uv [order [0]][0]) / (uv [order [1]][1] - uv [order [0]][1]);

            ystart = uv [order [0]][1]; ystart = MIN (1.0f, ystart); ystart = MAX (0.0f, ystart);
            yend   = uv [order [1]][1]; yend   = MIN (1.0f, yend);   yend   = MAX (0.0f, yend);

            for (y = ystart; y <= yend; y += add) {

                FLOAT_32 x1 = uv [order [0]][0] + d0 * (y - ystart);    x1 = MIN (1.0f, x1); x1 = MAX (0.0f, x1);
                FLOAT_32 x2 = uv [order [0]][0] + d1 * (y - ystart);    x2 = MIN (1.0f, x2); x2 = MAX (0.0f, x2);

                if (x1 < x2) {
                    for (x = x1; x <= x2; x += add) PolishTexel ((UINT_32) floor (x * (w - 1)), (UINT_32) floor (y * (h - 1)), threshold);   } else {
                    for (x = x2; x <= x1; x += add) PolishTexel ((UINT_32) floor (x * (w - 1)), (UINT_32) floor (y * (h - 1)), threshold);
                }

                //if (x1 < x2) {
                //    for (x = x1; x <= x2; x += add) PolishTexel (mask, (UINT_32) floor (x * (w - 1)), (UINT_32) floor (y * (h - 1)), threshold);   } else {
                //    for (x = x2; x <= x1; x += add) PolishTexel (mask, (UINT_32) floor (x * (w - 1)), (UINT_32) floor (y * (h - 1)), threshold);
                //}
            }
        }

        // is there bottom half ?
        if ((uv [order [2]][1] - uv [order [1]][1]) != 0.0) {

            // slope for second half
            d0 = (uv [order [2]][0] - uv [order [1]][0]) / (uv [order [2]][1] - uv [order [1]][1]);

            ystart = uv [order [1]][1]; ystart = MIN (1.0f, ystart); ystart = MAX (0.0f, ystart);
            yend   = uv [order [2]][1]; yend   = MIN (1.0f, yend);   yend   = MAX (0.0f, yend);

            for (y = ystart; y <= yend; y += add) {

                FLOAT_32 x1 = uv [order [1]][0] + d0 * (y - ystart);                x1 = MIN (1.0f, x1); x1 = MAX (0.0f, x1);
                FLOAT_32 x2 = uv [order [0]][0] + d1 * (y - uv [order [0]][1]);     x2 = MIN (1.0f, x2); x2 = MAX (0.0f, x2);

                if (x1 < x2) {
                    for (x = x1; x <= x2; x += add) PolishTexel ((UINT_32) floor (x * (w - 1)), (UINT_32) floor (y * (h - 1)), threshold);   } else {
                    for (x = x2; x <= x1; x += add) PolishTexel ((UINT_32) floor (x * (w - 1)), (UINT_32) floor (y * (h - 1)), threshold);
                }

                //if (x1 < x2) {
                //    for (x = x1; x <= x2; x += add) PolishTexel (mask, (UINT_32) floor (x * (w - 1)), (UINT_32) floor (y * (h - 1)), threshold);   } else {
                //    for (x = x2; x <= x1; x += add) PolishTexel (mask, (UINT_32) floor (x * (w - 1)), (UINT_32) floor (y * (h - 1)), threshold);
                //}
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////
    // PROCESS TEX UVEIL FACE TRIANGLE
    ///////////////////////////////////////////////////////////////////////////////

    DECLDIR VOID _stdcall processTexUnveilFaceTriangle  (FLOAT_32 u1, FLOAT_32 v1, 
                                                         FLOAT_32 u2, FLOAT_32 v2, 
                                                         FLOAT_32 u3, FLOAT_32 v3, INT_32 l)
    {
        UINT_32 order [3];

        FLOAT_32 uv [3][2] = { {u1, v1}, {u2, v2}, {u3, v3} };

        FLOAT_32 d1, d0, add, x, y;

        FLOAT_32 ystart;
        FLOAT_32 yend;

        UINT_32 sums [16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
        UINT_32 maxs [16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

        // find the lowest Y
        if (uv [0][1] < uv [1][1]) {

            if (uv [0][1] < uv [2][1]) order [0] = 0; else
                                       order [0] = 2;        } else {
            if (uv [1][1] < uv [2][1]) order [0] = 1; else
                                       order [0] = 2;
        }

        // find the highest Y
        if (uv [0][1] > uv [1][1]) {

            if (uv [0][1] > uv [2][1]) order [2] = 0; else
                                       order [2] = 2;        } else {
            if (uv [1][1] > uv [2][1]) order [2] = 1; else
                                       order [2] = 2;
        }

        // and the middle one is a matter of deduction
        order [1] = 3 - (order [0] + order [2]);  

        // degenerated triangle
        if ((uv [order [0]][1] - uv [order [2]][1]) == 0.0) return;

        // pixel size
        add = 1.0f / (FLOAT_32) h;

        // main edge
        d1 = (uv [order [2]][0] - uv [order [0]][0]) / (uv [order [2]][1] - uv [order [0]][1]);

        //------------------------------------------------------
        // CHECK LAYER OVERLAPPING PER FACE
        //------------------------------------------------------
        
        // is there top half ?
        if ((uv [order [1]][1] - uv [order [0]][1]) != 0.0) {

            // slopes for first half
            d0 = (uv [order [1]][0] - uv [order [0]][0]) / (uv [order [1]][1] - uv [order [0]][1]);

            ystart = uv [order [0]][1]; ystart = MIN (1.0f, ystart); ystart = MAX (0.0f, ystart);
            yend   = uv [order [1]][1]; yend   = MIN (1.0f, yend);   yend   = MAX (0.0f, yend);

            for (y = ystart; y <= yend; y += add) {

                FLOAT_32 x1 = uv [order [0]][0] + d0 * (y - ystart);    x1 = MIN (1.0f, x1); x1 = MAX (0.0f, x1);
                FLOAT_32 x2 = uv [order [0]][0] + d1 * (y - ystart);    x2 = MIN (1.0f, x2); x2 = MAX (0.0f, x2);

                if (x1 < x2) {
                    for (x = x1; x <= x2; x += add) OverlappingTexel (1, (UINT_32) floor (x * (w - 1)), (UINT_32) floor (y * (h - 1)), sums, maxs); } else {
                    for (x = x2; x <= x1; x += add) OverlappingTexel (1, (UINT_32) floor (x * (w - 1)), (UINT_32) floor (y * (h - 1)), sums, maxs);
                }
            }
        }

        // is there bottom half ?
        if ((uv [order [2]][1] - uv [order [1]][1]) != 0.0) {

            // slope for second half
            d0 = (uv [order [2]][0] - uv [order [1]][0]) / (uv [order [2]][1] - uv [order [1]][1]);

            ystart = uv [order [1]][1]; ystart = MIN (1.0f, ystart); ystart = MAX (0.0f, ystart);
            yend   = uv [order [2]][1]; yend   = MIN (1.0f, yend);   yend   = MAX (0.0f, yend);

            for (y = ystart; y <= yend; y += add) {

                FLOAT_32 x1 = uv [order [1]][0] + d0 * (y - ystart);                x1 = MIN (1.0f, x1); x1 = MAX (0.0f, x1);
                FLOAT_32 x2 = uv [order [0]][0] + d1 * (y - uv [order [0]][1]);     x2 = MIN (1.0f, x2); x2 = MAX (0.0f, x2);

                if (x1 < x2) {
                    for (x = x1; x <= x2; x += add) OverlappingTexel (1, (UINT_32) floor (x * (w - 1)), (UINT_32) floor (y * (h - 1)), sums, maxs); } else {
                    for (x = x2; x <= x1; x += add) OverlappingTexel (1, (UINT_32) floor (x * (w - 1)), (UINT_32) floor (y * (h - 1)), sums, maxs);
                }
            }
        }

        UINT_32 mask = 0x00000000;  UINT_32 c = 0;

        // we construct bit mask and check if there is not too many layers per whole face
        for (UINT_32 i = 0; i < icount; i ++) { 

            if (sums [i] > 0) {
                mask |= 1 << i; c ++;
            }
        }

        if (c > moverlap) {

            // difference
            c -= moverlap;

            for (UINT_32 i = 0; i < c; i ++) {

                UINT_32 min = 0;    for (; (maxs [min] == 0) && (min < icount); min ++);

                /// ALL ZEROS,  SHIT HAPPENDS !!
                if (min == icount) break;

                for (UINT_32 n = 0; n < icount; n ++) {
                    if ((maxs [n] > 0) && (maxs [n] < maxs [min]))  min = n;
                }

                maxs [min] = 0;

                UINT_32 maskbk = mask;  mask &= ~(1 << min);

                ////------------------------------------------------------
                //// CHECK IF REMOVING THIS LAYER DOES NOT CREATES HOLES
                ////------------------------------------------------------
                //
                //// is there top half ?
                //if ((uv [order [1]][1] - uv [order [0]][1]) != 0.0) {

                //    // slopes for first half
                //    d0 = (uv [order [1]][0] - uv [order [0]][0]) / (uv [order [1]][1] - uv [order [0]][1]);

                //    ystart = uv [order [0]][1]; ystart = MIN (1.0f, ystart); ystart = MAX (0.0f, ystart);
                //    yend   = uv [order [1]][1]; yend   = MIN (1.0f, yend);   yend   = MAX (0.0f, yend);

                //    for (y = ystart; y <= yend; y += add) {

                //        FLOAT_32 x1 = uv [order [0]][0] + d0 * (y - ystart);    x1 = MIN (1.0f, x1); x1 = MAX (0.0f, x1);
                //        FLOAT_32 x2 = uv [order [0]][0] + d1 * (y - ystart);    x2 = MIN (1.0f, x2); x2 = MAX (0.0f, x2);

                //        if (x1 < x2) {
                //            for (x = x1; x <= x2; x += add) if (CheckTexel (mask, (UINT_32) floor (x * (w - 1)), (UINT_32) floor (y * (h - 1)))) goto error; } else {
                //            for (x = x2; x <= x1; x += add) if (CheckTexel (mask, (UINT_32) floor (x * (w - 1)), (UINT_32) floor (y * (h - 1)))) goto error;
                //        }
                //    }
                //}

                //// is there bottom half ?
                //if ((uv [order [2]][1] - uv [order [1]][1]) != 0.0) {

                //    // slope for second half
                //    d0 = (uv [order [2]][0] - uv [order [1]][0]) / (uv [order [2]][1] - uv [order [1]][1]);

                //    ystart = uv [order [1]][1]; ystart = MIN (1.0f, ystart); ystart = MAX (0.0f, ystart);
                //    yend   = uv [order [2]][1]; yend   = MIN (1.0f, yend);   yend   = MAX (0.0f, yend);

                //    for (y = ystart; y <= yend; y += add) {

                //        FLOAT_32 x1 = uv [order [1]][0] + d0 * (y - ystart);                x1 = MIN (1.0f, x1); x1 = MAX (0.0f, x1);
                //        FLOAT_32 x2 = uv [order [0]][0] + d1 * (y - uv [order [0]][1]);     x2 = MIN (1.0f, x2); x2 = MAX (0.0f, x2);

                //        if (x1 < x2) {
                //            for (x = x1; x <= x2; x += add) if (CheckTexel (mask, (UINT_32) floor (x * (w - 1)), (UINT_32) floor (y * (h - 1)))) goto error; } else {
                //            for (x = x2; x <= x1; x += add) if (CheckTexel (mask, (UINT_32) floor (x * (w - 1)), (UINT_32) floor (y * (h - 1)))) goto error;
                //        }
                //    }
                //}

                // advance
                //i ++;   continue;

                //// we skip advance
                //error:;

                //mask = maskbk;

                UINT_8P p = (UINT_8P) images [min].data;

                //------------------------------------------------------
                // UNVEIL LAYER WITH THE LEAST CONTRIBUTION
                //------------------------------------------------------
                
                // is there top half ?
                if ((uv [order [1]][1] - uv [order [0]][1]) != 0.0) {

                    // slopes for first half
                    d0 = (uv [order [1]][0] - uv [order [0]][0]) / (uv [order [1]][1] - uv [order [0]][1]);

                    ystart = uv [order [0]][1]; ystart = MIN (1.0f, ystart); ystart = MAX (0.0f, ystart);
                    yend   = uv [order [1]][1]; yend   = MIN (1.0f, yend);   yend   = MAX (0.0f, yend);

                    for (y = ystart; y <= yend; y += add) {

                        FLOAT_32 x1 = uv [order [0]][0] + d0 * (y - ystart);    x1 = MIN (1.0f, x1); x1 = MAX (0.0f, x1);
                        FLOAT_32 x2 = uv [order [0]][0] + d1 * (y - ystart);    x2 = MIN (1.0f, x2); x2 = MAX (0.0f, x2);

                        if (x1 < x2) {
                            for (x = x1; x <= x2; x += add) ClearTexel (min, (UINT_32) floor (x * (w - 1)), (UINT_32) floor (y * (h - 1))); } else {
                            for (x = x2; x <= x1; x += add) ClearTexel (min, (UINT_32) floor (x * (w - 1)), (UINT_32) floor (y * (h - 1)));
                        }
                    }
                }

                // is there bottom half ?
                if ((uv [order [2]][1] - uv [order [1]][1]) != 0.0) {

                    // slope for second half
                    d0 = (uv [order [2]][0] - uv [order [1]][0]) / (uv [order [2]][1] - uv [order [1]][1]);

                    ystart = uv [order [1]][1]; ystart = MIN (1.0f, ystart); ystart = MAX (0.0f, ystart);
                    yend   = uv [order [2]][1]; yend   = MIN (1.0f, yend);   yend   = MAX (0.0f, yend);

                    for (y = ystart; y <= yend; y += add) {

                        FLOAT_32 x1 = uv [order [1]][0] + d0 * (y - ystart);                x1 = MIN (1.0f, x1); x1 = MAX (0.0f, x1);
                        FLOAT_32 x2 = uv [order [0]][0] + d1 * (y - uv [order [0]][1]);     x2 = MIN (1.0f, x2); x2 = MAX (0.0f, x2);

                        if (x1 < x2) {
                            for (x = x1; x <= x2; x += add) ClearTexel (min, (UINT_32) floor (x * (w - 1)), (UINT_32) floor (y * (h - 1))); } else {
                            for (x = x2; x <= x1; x += add) ClearTexel (min, (UINT_32) floor (x * (w - 1)), (UINT_32) floor (y * (h - 1)));
                        }
                    }
                }
            }
        }

        //------------------------------------------------------
        // SAVE MASK FOR EACH TEXEL
        //------------------------------------------------------
        
        // is there top half ?
        if ((uv [order [1]][1] - uv [order [0]][1]) != 0.0) {

            // slopes for first half
            d0 = (uv [order [1]][0] - uv [order [0]][0]) / (uv [order [1]][1] - uv [order [0]][1]);

            ystart = uv [order [0]][1]; ystart = MIN (1.0f, ystart); ystart = MAX (0.0f, ystart);
            yend   = uv [order [1]][1]; yend   = MIN (1.0f, yend);   yend   = MAX (0.0f, yend);

            for (y = ystart; y <= yend; y += add) {

                FLOAT_32 x1 = uv [order [0]][0] + d0 * (y - ystart);    x1 = MIN (1.0f, x1); x1 = MAX (0.0f, x1);
                FLOAT_32 x2 = uv [order [0]][0] + d1 * (y - ystart);    x2 = MIN (1.0f, x2); x2 = MAX (0.0f, x2);

                if (x1 < x2) {
                    for (x = x1; x <= x2; x += add) MaskTexel (mask, (UINT_32) floor (x * (w - 1)), (UINT_32) floor (y * (h - 1))); } else {
                    for (x = x2; x <= x1; x += add) MaskTexel (mask, (UINT_32) floor (x * (w - 1)), (UINT_32) floor (y * (h - 1)));
                }
            }
        }

        // is there bottom half ?
        if ((uv [order [2]][1] - uv [order [1]][1]) != 0.0) {

            // slope for second half
            d0 = (uv [order [2]][0] - uv [order [1]][0]) / (uv [order [2]][1] - uv [order [1]][1]);

            ystart = uv [order [1]][1]; ystart = MIN (1.0f, ystart); ystart = MAX (0.0f, ystart);
            yend   = uv [order [2]][1]; yend   = MIN (1.0f, yend);   yend   = MAX (0.0f, yend);

            for (y = ystart; y <= yend; y += add) {

                FLOAT_32 x1 = uv [order [1]][0] + d0 * (y - ystart);                x1 = MIN (1.0f, x1); x1 = MAX (0.0f, x1);
                FLOAT_32 x2 = uv [order [0]][0] + d1 * (y - uv [order [0]][1]);     x2 = MIN (1.0f, x2); x2 = MAX (0.0f, x2);

                if (x1 < x2) {
                    for (x = x1; x <= x2; x += add) MaskTexel (mask, (UINT_32) floor (x * (w - 1)), (UINT_32) floor (y * (h - 1))); } else {
                    for (x = x2; x <= x1; x += add) MaskTexel (mask, (UINT_32) floor (x * (w - 1)), (UINT_32) floor (y * (h - 1)));
                }
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////
    // PROCESS TEX SPLIT TRIANGLE
    ///////////////////////////////////////////////////////////////////////////////

    DECLDIR UINT_32  _stdcall processTexMaskTriangle   (FLOAT_32 u1, FLOAT_32 v1, 
                                                        FLOAT_32 u2, FLOAT_32 v2, 
                                                        FLOAT_32 u3, FLOAT_32 v3)
    {

        UINT_32 order [3];

        FLOAT_32 uv [3][2] = { {u1, v1}, {u2, v2}, {u3, v3} };

        FLOAT_32 d1, d0, add, x, y;

        FLOAT_32 ystart;
        FLOAT_32 yend;

        UINT_32 sums [16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
        UINT_32 maxs [16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

        UINT_8P errp = (UINT_8P) errimg.data;


        // find the lowest Y
        if (uv [0][1] < uv [1][1]) {

            if (uv [0][1] < uv [2][1]) order [0] = 0; else
                                       order [0] = 2;        } else {
            if (uv [1][1] < uv [2][1]) order [0] = 1; else
                                       order [0] = 2;
        }

        // find the highest Y
        if (uv [0][1] > uv [1][1]) {

            if (uv [0][1] > uv [2][1]) order [2] = 0; else
                                       order [2] = 2;        } else {
            if (uv [1][1] > uv [2][1]) order [2] = 1; else
                                       order [2] = 2;
        }

        // and the middle one is a matter of deduction
        order [1] = 3 - (order [0] + order [2]);  

        // degenerated triangle
        if ((uv [order [0]][1] - uv [order [2]][1]) == 0.0) return 0;

        // pixel size
        add = 1.0f / (FLOAT_32) h;

        // main edge
        d1 = (uv [order [2]][0] - uv [order [0]][0]) / (uv [order [2]][1] - uv [order [0]][1]);

        //------------------------------------------------------
        // CHECK LAYER OVERLAPPING PER TEXEL
        //------------------------------------------------------

        BOOL error = false;

        // is there top half ?
        if ((uv [order [1]][1] - uv [order [0]][1]) != 0.0) {

            // slopes for first half
            d0 = (uv [order [1]][0] - uv [order [0]][0]) / (uv [order [1]][1] - uv [order [0]][1]);

            ystart = uv [order [0]][1]; ystart = MIN (1.0f, ystart); ystart = MAX (0.0f, ystart);
            yend   = uv [order [1]][1]; yend   = MIN (1.0f, yend);   yend   = MAX (0.0f, yend);

            for (y = ystart; y <= yend; y += add) {

                FLOAT_32 x1 = uv [order [0]][0] + d0 * (y - ystart);    x1 = MIN (1.0f, x1); x1 = MAX (0.0f, x1);
                FLOAT_32 x2 = uv [order [0]][0] + d1 * (y - ystart);    x2 = MIN (1.0f, x2); x2 = MAX (0.0f, x2);

                UINT_32 offset = (UINT_32) floor (y * (h - 1)) * w;

                if (x1 < x2) { 
                    for (x = x1; x <= x2; x += add) OverlappingTexel (1, (UINT_32) floor (x * (w - 1)), (UINT_32) floor (y * (h - 1)), sums, maxs); } else {
                    for (x = x2; x <= x1; x += add) OverlappingTexel (1, (UINT_32) floor (x * (w - 1)), (UINT_32) floor (y * (h - 1)), sums, maxs);
                }
            }
        }

        // is there bottom half ?
        if ((uv [order [2]][1] - uv [order [1]][1]) != 0.0) {

            // slope for second half
            d0 = (uv [order [2]][0] - uv [order [1]][0]) / (uv [order [2]][1] - uv [order [1]][1]);

            ystart = uv [order [1]][1]; ystart = MIN (1.0f, ystart); ystart = MAX (0.0f, ystart);
            yend   = uv [order [2]][1]; yend   = MIN (1.0f, yend);   yend   = MAX (0.0f, yend);

            for (y = ystart; y <= yend; y += add) {

                FLOAT_32 x1 = uv [order [1]][0] + d0 * (y - ystart);                x1 = MIN (1.0f, x1); x1 = MAX (0.0f, x1);
                FLOAT_32 x2 = uv [order [0]][0] + d1 * (y - uv [order [0]][1]);     x2 = MIN (1.0f, x2); x2 = MAX (0.0f, x2);

                UINT_32 offset = (UINT_32) floor (y * (h - 1)) * w;

                if (x1 < x2) { 
                    for (x = x1; x <= x2; x += add) OverlappingTexel (1, (UINT_32) floor (x * (w - 1)), (UINT_32) floor (y * (h - 1)), sums, maxs); } else {
                    for (x = x2; x <= x1; x += add) OverlappingTexel (1, (UINT_32) floor (x * (w - 1)), (UINT_32) floor (y * (h - 1)), sums, maxs);
                }
            }
        }

        //------------------------------------------------------
        // LAYER OVERLAPPING PER FACE
        //------------------------------------------------------
        
        UINT_32 mask = 0x00000000;  UINT_32 c = 0;

        // we construct bit mask and check if there is not too many layers per whole face
        for (UINT_32 i = 0; i < icount; i ++) {

            if (sums [i] > 0) {

                mask |= 1 << i; c ++;
            }
        }

        //------------------------------------------------------
        // SAVE MASK FOR EACH TEXEL
        //------------------------------------------------------
        
        // is there top half ?
        if ((uv [order [1]][1] - uv [order [0]][1]) != 0.0) {

            // slopes for first half
            d0 = (uv [order [1]][0] - uv [order [0]][0]) / (uv [order [1]][1] - uv [order [0]][1]);

            ystart = uv [order [0]][1]; ystart = MIN (1.0f, ystart); ystart = MAX (0.0f, ystart);
            yend   = uv [order [1]][1]; yend   = MIN (1.0f, yend);   yend   = MAX (0.0f, yend);

            for (y = ystart; y <= yend; y += add) {

                FLOAT_32 x1 = uv [order [0]][0] + d0 * (y - ystart);    x1 = MIN (1.0f, x1); x1 = MAX (0.0f, x1);
                FLOAT_32 x2 = uv [order [0]][0] + d1 * (y - ystart);    x2 = MIN (1.0f, x2); x2 = MAX (0.0f, x2);

                if (x1 < x2) {
                    for (x = x1; x <= x2; x += add) MaskTexel (mask, (UINT_32) floor (x * (w - 1)), (UINT_32) floor (y * (h - 1))); } else {
                    for (x = x2; x <= x1; x += add) MaskTexel (mask, (UINT_32) floor (x * (w - 1)), (UINT_32) floor (y * (h - 1)));
                }
            }
        }

        // is there bottom half ?
        if ((uv [order [2]][1] - uv [order [1]][1]) != 0.0) {

            // slope for second half
            d0 = (uv [order [2]][0] - uv [order [1]][0]) / (uv [order [2]][1] - uv [order [1]][1]);

            ystart = uv [order [1]][1]; ystart = MIN (1.0f, ystart); ystart = MAX (0.0f, ystart);
            yend   = uv [order [2]][1]; yend   = MIN (1.0f, yend);   yend   = MAX (0.0f, yend);

            for (y = ystart; y <= yend; y += add) {

                FLOAT_32 x1 = uv [order [1]][0] + d0 * (y - ystart);                x1 = MIN (1.0f, x1); x1 = MAX (0.0f, x1);
                FLOAT_32 x2 = uv [order [0]][0] + d1 * (y - uv [order [0]][1]);     x2 = MIN (1.0f, x2); x2 = MAX (0.0f, x2);

                if (x1 < x2) {
                    for (x = x1; x <= x2; x += add) MaskTexel (mask, (UINT_32) floor (x * (w - 1)), (UINT_32) floor (y * (h - 1))); } else {
                    for (x = x2; x <= x1; x += add) MaskTexel (mask, (UINT_32) floor (x * (w - 1)), (UINT_32) floor (y * (h - 1)));
                }
            }
        }

        return mask;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // PROCESS TEX SPLIT TRIANGLE
    ///////////////////////////////////////////////////////////////////////////////

    DECLDIR UINT_32  _stdcall processTexSplitTriangle  (FLOAT_32 u1, FLOAT_32 v1, 
                                                        FLOAT_32 u2, FLOAT_32 v2, 
                                                        FLOAT_32 u3, FLOAT_32 v3)
    {

        UINT_32 order [3];

        FLOAT_32 uv [3][2] = { {u1, v1}, {u2, v2}, {u3, v3} };

        FLOAT_32 d1, d0, add, x, y;

        FLOAT_32 ystart;
        FLOAT_32 yend;

        UINT_32 sums [16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
        UINT_32 maxs [16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

        UINT_8P errp = (UINT_8P) errimg.data;


        // find the lowest Y
        if (uv [0][1] < uv [1][1]) {

            if (uv [0][1] < uv [2][1]) order [0] = 0; else
                                       order [0] = 2;        } else {
            if (uv [1][1] < uv [2][1]) order [0] = 1; else
                                       order [0] = 2;
        }

        // find the highest Y
        if (uv [0][1] > uv [1][1]) {

            if (uv [0][1] > uv [2][1]) order [2] = 0; else
                                       order [2] = 2;        } else {
            if (uv [1][1] > uv [2][1]) order [2] = 1; else
                                       order [2] = 2;
        }

        // and the middle one is a matter of deduction
        order [1] = 3 - (order [0] + order [2]);  

        // degenerated triangle
        if ((uv [order [0]][1] - uv [order [2]][1]) == 0.0) return 0;

        // pixel size
        add = 1.0f / (FLOAT_32) h;

        // main edge
        d1 = (uv [order [2]][0] - uv [order [0]][0]) / (uv [order [2]][1] - uv [order [0]][1]);

        //------------------------------------------------------
        // CHECK LAYER OVERLAPPING PER TEXEL
        //------------------------------------------------------

        BOOL error = false;

        // is there top half ?
        if ((uv [order [1]][1] - uv [order [0]][1]) != 0.0) {

            // slopes for first half
            d0 = (uv [order [1]][0] - uv [order [0]][0]) / (uv [order [1]][1] - uv [order [0]][1]);

            ystart = uv [order [0]][1]; ystart = MIN (1.0f, ystart); ystart = MAX (0.0f, ystart);
            yend   = uv [order [1]][1]; yend   = MIN (1.0f, yend);   yend   = MAX (0.0f, yend);

            for (y = ystart; y <= yend; y += add) {

                FLOAT_32 x1 = uv [order [0]][0] + d0 * (y - ystart);    x1 = MIN (1.0f, x1); x1 = MAX (0.0f, x1);
                FLOAT_32 x2 = uv [order [0]][0] + d1 * (y - ystart);    x2 = MIN (1.0f, x2); x2 = MAX (0.0f, x2);

                UINT_32 offset = (UINT_32) floor (y * (h - 1)) * w;

                if (x1 < x2) {
                    for (x = x1; x <= x2; x += add) {
                        if (OverlappingTexel (1, (UINT_32) floor (x * (w - 1)), (UINT_32) floor (y * (h - 1)), sums, maxs) > moverlap) { error = true;

                            UINT_32 index = 3 * (offset + (UINT_32) floor (x * (w - 1)));
                            errp [index    ] = 255;
                            errp [index + 1] = 0;
                            errp [index + 2] = 0;
                        }
                    }
                } else {
                    for (x = x2; x <= x1; x += add) {
                        if (OverlappingTexel (1, (UINT_32) floor (x * (w - 1)), (UINT_32) floor (y * (h - 1)), sums, maxs) > moverlap) { error = true;

                            UINT_32 index = 3 * (offset + (UINT_32) floor (x * (w - 1)));
                            errp [index    ] = 255;
                            errp [index + 1] = 0;
                            errp [index + 2] = 0;
                        }
                    }
                }
            }
        }

        // is there bottom half ?
        if ((uv [order [2]][1] - uv [order [1]][1]) != 0.0) {

            // slope for second half
            d0 = (uv [order [2]][0] - uv [order [1]][0]) / (uv [order [2]][1] - uv [order [1]][1]);

            ystart = uv [order [1]][1]; ystart = MIN (1.0f, ystart); ystart = MAX (0.0f, ystart);
            yend   = uv [order [2]][1]; yend   = MIN (1.0f, yend);   yend   = MAX (0.0f, yend);

            for (y = ystart; y <= yend; y += add) {

                FLOAT_32 x1 = uv [order [1]][0] + d0 * (y - ystart);                x1 = MIN (1.0f, x1); x1 = MAX (0.0f, x1);
                FLOAT_32 x2 = uv [order [0]][0] + d1 * (y - uv [order [0]][1]);     x2 = MIN (1.0f, x2); x2 = MAX (0.0f, x2);

                UINT_32 offset = (UINT_32) floor (y * (h - 1)) * w;

                if (x1 < x2) {
                    for (x = x1; x <= x2; x += add) {
                        if (OverlappingTexel (1, (UINT_32) floor (x * (w - 1)), (UINT_32) floor (y * (h - 1)), sums, maxs) > moverlap) { error = true;

                            UINT_32 index = 3 * (offset + (UINT_32) floor (x * (w - 1)));
                            errp [index    ] = 255;
                            errp [index + 1] = 0;
                            errp [index + 2] = 0;
                        }
                    }
                } else {
                    for (x = x2; x <= x1; x += add) {
                        if (OverlappingTexel (1, (UINT_32) floor (x * (w - 1)), (UINT_32) floor (y * (h - 1)), sums, maxs) > moverlap) { error = true;

                            UINT_32 index = 3 * (offset + (UINT_32) floor (x * (w - 1)));
                            errp [index    ] = 255;
                            errp [index + 1] = 0;
                            errp [index + 2] = 0;
                        }
                    }
                }
            }
        }

        //------------------------------------------------------
        // CHECK LAYER OVERLAPPING PER FACE
        //------------------------------------------------------
        
        UINT_32 mask = 0x00000000;  UINT_32 c = 0;

        // we construct bit mask and check if there is not too many layers per whole face
        for (UINT_32 i = 0; i < icount; i ++) {

            if (sums [i] > 0) {

                mask |= 1 << i; c ++;
            }
        }

        // too many layers per face
        if (c > moverlap)   return 0;

        return mask;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // PROCESS TEX SPLIT BEGIN
    ///////////////////////////////////////////////////////////////////////////////

    DECLDIR INT_32 _stdcall processTexSplitBegin (CHARPP files, CHARP errfile, UINT_32 count, UINT_32 overlap, UINT_32 kernel_a, UINT_32 kernel_s, FLOAT_32 texel_tolerancy, FLOAT_32 face_tolerancy)
    {
        // maximum image count
        if (count > 16) return -10;

        // loading images
        for (UINT_32 n = 0; n < count; n ++) {

            if (loadImage (files [n], &images [n]) < 0)     return -1;
        }

        // check dimensions
        for (UINT_32 n = 0; n < count; n ++) {

            if ((images [n].width  != images [0].width) || 
                (images [n].height != images [0].height))   return -2;
        }

        // check type
        for (UINT_32 n = 0; n < count; n ++) {

            if (images [n].type != M_IMAGE_TYPE_RGB)        return -3;
        }

        w = images [0].width;
        h = images [0].height;
        
        UINT_32 size = sizeof (UINT_8) * w * h * 3;

        for (UINT_32 n = 0; n < count; n ++) {

            imagescopy [n] = (UINT_8P) malloc (size);

            memcpy ((VOIDP) imagescopy [n], images [n].data, size);
        }

        masktags = (UINT_32P) calloc (w * h, sizeof (UINT_32));
        maskmap  = (UINT_32P) calloc (w * h, sizeof (UINT_32));

        // saving tolerancy parameters
        tolerancy_face  = face_tolerancy;
        tolerancy_texel = texel_tolerancy;

        // saving overlapping parameter
        moverlap = overlap;

        // saving count of images
        icount = count;

        // save file names
        imagefiles = files; errorfile = errfile;

        // save kernel parameters
        kernela = kernel_a;
        kernels = kernel_s;

        // image with error info
        errimg.data     = calloc (w * h * 3, SIZE_UINT_8);
        errimg.width    = w;
        errimg.height   = h;
        errimg.type     = M_IMAGE_TYPE_RGB;
        
        return 0;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // PROCESS TEX SPLIT END
    ///////////////////////////////////////////////////////////////////////////////

    DECLDIR INT_32 _stdcall processTexSplitEnd (UINT_32 save, UINT_32 err)
    {
        if (save) {

            // saving images
            for (UINT_32 n = 0; n < icount; n ++) {

                if (saveImage (imagefiles [n], &images [n]) < 0) return -1;
            }
        }

        if (err) {

            // save error image
            saveImage (errorfile, &errimg);
        }

        // unloading images
        for (UINT_32 n = 0; n < icount; n ++) {

            free (images [n].data); free (imagescopy [n]);
        }

        // free image data
        free (errimg.data);

        free (masktags);
        free (maskmap);

        return 0;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // PROCESS TEX UNVEIL UPDATE
    ///////////////////////////////////////////////////////////////////////////////

    DECLDIR VOID _stdcall processTexUnveilUpdate ()
    {
        UINT_32 size = sizeof (UINT_8) * w * h * 3;

        for (UINT_32 n = 0; n < icount; n ++) {

            memcpy ((VOIDP) imagescopy [n], images [n].data, size);
        }
    }

    ///////////////////////////////////////////////////////////////////////////////
    // PROCESS TEX SPLIT GENERATE
    ///////////////////////////////////////////////////////////////////////////////

    DECLDIR INT_32  _stdcall processTexSplitGenerate  (CHARPP filesin, CHARP fileout, UINT_8P indices, UINT_32 count)
    {
        // list of images
        TImage * images = (TImage *) malloc (sizeof (TImage) * count);

        printf ("Output file :  %s \n", fileout);

        // loading images
        for (UINT_32 n = 0; n < count; n ++) {

            printf ("Loading file : %i -> %s  \n", indices [n], filesin [indices [n]]);

            if (loadImage (filesin [indices [n]], &images [n]) < 0) return -1;
        }

        printf ("\n");

        // check dimensions
        for (UINT_32 n = 0; n < count; n ++) {

            if ((images [n].width  != images [0].width) || 
                (images [n].height != images [0].height))   return -2;
        }

        // check type
        for (UINT_32 n = 0; n < count; n ++) {

            if (images [n].type != M_IMAGE_TYPE_RGB)    return -3;
        }

        UINT_32 w = images [0].width;
        UINT_32 h = images [0].height;

        TImage outimage;

        outimage.width    = w;
        outimage.height   = h;

        UINT_32 index, x, y;       UINT_8P p;

        switch (count) {
            case 2: {
                outimage.data   = (UINT_8P) malloc (SIZE_UINT_8 * w * h * 3);   UINT_8P outp    = (UINT_8P) outimage.data;
                outimage.type   = M_IMAGE_TYPE_RGB;

                for (y = 0; y < h; y ++) {
                    for (x = 0; x < w; x ++) {  index = ((y * w) + x) * 3;

                        p = (UINT_8P) images [0].data;  *outp = p [index];   outp ++;
                        p = (UINT_8P) images [1].data;  *outp = p [index];   outp ++;
                        *outp = 0; outp ++;
                    }
                }

                break;
            }
            case 3: {
                outimage.data   = (UINT_8P) malloc (SIZE_UINT_8 * w * h * 3);   UINT_8P outp    = (UINT_8P) outimage.data;
                outimage.type   = M_IMAGE_TYPE_RGB;
                    
                for (y = 0; y < h; y ++) {
                    for (x = 0; x < w; x ++) {  index = ((y * w) + x) * 3;

                        p = (UINT_8P) images [0].data;  *outp = p [index];   outp ++;
                        p = (UINT_8P) images [1].data;  *outp = p [index];   outp ++;
                        p = (UINT_8P) images [2].data;  *outp = p [index];   outp ++;
                    }
                }

                break;
            }
            case 4: {
                outimage.data   = (UINT_8P) malloc (SIZE_UINT_8 * w * h * 4);   UINT_8P outp    = (UINT_8P) outimage.data;
                outimage.type   = M_IMAGE_TYPE_RGBA;

                for (y = 0; y < h; y ++) {
                    for (x = 0; x < w; x ++) {  index = ((y * w) + x) * 3;

                        p = (UINT_8P) images [0].data;  *outp = p [index];   outp ++;
                        p = (UINT_8P) images [1].data;  *outp = p [index];   outp ++;
                        p = (UINT_8P) images [2].data;  *outp = p [index];   outp ++;
                        p = (UINT_8P) images [3].data;  *outp = p [index];   outp ++;
                    }
                }

                break;
            }
        }
        
        // save image
        saveImage (fileout, &outimage);

        // free images
        for (UINT_32 n = 0; n < count; n ++) {
        
            free (images [n].data);
        }

        free (images);

        return 0;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // PROCESS PARTICLE CUT
    ///////////////////////////////////////////////////////////////////////////////

    DECLDIR UINT_32 _stdcall processParticlesCut (UINT_32P indices, FLOAT_32P points, FLOAT_32P faces, UINT_32 pcount, UINT_32 fcount)
    {
        UINT_32 i, j, k, c;

        FLOAT_32 minv [3] = {  99999999.0f,  99999999.0f,  99999999.0f };
        FLOAT_32 maxv [3] = { -99999999.0f, -99999999.0f, -99999999.0f };

        // evaluate bounding box
        for (i = 0; i < fcount; i += 3) {
            
            FLOAT_32 x = faces [i    ];
            FLOAT_32 y = faces [i + 1];
            FLOAT_32 z = faces [i + 2];

            if (x < minv [0])   minv [0] = x;
            if (y < minv [1])   minv [1] = y;
            if (z < minv [2])   minv [2] = z;

            if (x > maxv [0])   maxv [0] = x;
            if (y > maxv [1])   maxv [1] = y;
            if (z > maxv [2])   maxv [2] = z;
        }

        UINT_32P tags = (UINT_32P) calloc (pcount / 3, SIZE_UINT_32);

        for (c = i = 0; i < fcount; i += 9) {

            FLOAT_32P p1 = &(faces [i    ]);
            FLOAT_32P p2 = &(faces [i + 3]);
            FLOAT_32P p3 = &(faces [i + 6]);

            FLOAT_32 v1 [3] = { p1 [0] - p2 [0], p1 [1] - p2 [1], p1 [2] - p2 [2] };
            FLOAT_32 v2 [3] = { p1 [0] - p3 [0], p1 [1] - p3 [1], p1 [2] - p3 [2] };
            FLOAT_32 v3 [3] = { p2 [0] - p3 [0], p2 [1] - p3 [1], p2 [2] - p3 [2] };

            FLOAT_32 n [3]; vCROSS (n, v1, v2);

            FLOAT_32 planevec1 [3]; vCROSS (planevec1, n, v1);
            FLOAT_32 planevec2 [3]; vCROSS (planevec2, n, v2);
            FLOAT_32 planevec3 [3]; vCROSS (planevec3, n, v3);

            if (vDOT (planevec1, v2) > 0.0) {   vINVERT (planevec1);    }
            if (vDOT (planevec2, v3) < 0.0) {   vINVERT (planevec2);    }
            if (vDOT (planevec3, v1) < 0.0) {   vINVERT (planevec3);    }

            FLOAT_32 d1 = - vDOT (planevec1, p1);
            FLOAT_32 d2 = - vDOT (planevec2, p1);
            FLOAT_32 d3 = - vDOT (planevec3, p3);

            for (k = j = 0; j < pcount; j += 3) {

                FLOAT_32P p = &(points [j]);

                if (!tags [k]) {

                    // bounding box test
                    if (((p [0] >= minv [0]) && (p [0] <= maxv [0])) && \
                        ((p [1] >= minv [1]) && (p [1] <= maxv [1])) && \
                        ((p [2] >= minv [2]) && (p [2] <= maxv [2]))) {

                        // is inside of face boundaries ?
                        if (((planevec1[0]*p[0] + planevec1[1]*p[1] + planevec1[2]*p[2] + d1) > 0.0) && \
                            ((planevec2[0]*p[0] + planevec2[1]*p[1] + planevec2[2]*p[2] + d2) > 0.0) && \
                            ((planevec3[0]*p[0] + planevec3[1]*p[1] + planevec3[2]*p[2] + d3) > 0.0)) {
                                
                            indices [c ++] = k; tags [k] = true;
                        }
                    }
                }

                k ++;
            }
        }

        free (tags);

        return c;
    }
}