
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

    DECLDIR UINT_32 _stdcall processSaveInstances (CHARP filename, UINT_32 count, FLOAT_32P data, UINT_32P datak, FLOAT_32P bbox, UINT_32 divx, UINT_32 divy, UINT_32 divz, FLOAT_32 rotx, FLOAT_32 roty, FLOAT_32 rotz, FLOAT_32 randrad, UINT_32 rotorder, FLOAT_32 cancell)
    {
        UINT_16 groups = divx * divy * divz;

        FLOAT_32 minco [3] = { 99999999.0f, 99999999.0f, 99999999.0f};
        FLOAT_32 maxco [3] = {-99999999.0f,-99999999.0f,-99999999.0f};

        UINT_32 index  = 0;
        UINT_32 indexk = 0;
        for (UINT_32 n = 0; n < count; n ++) {

			// fixing problem in blender that is unable to retieve starting position

            FLOAT_32 v [3] = {  data [index    ] - data [index + 3],
                                data [index + 1] - data [index + 4],
								data [index + 2] - data [index + 5]	};

			FLOAT_32 c = datak [indexk ++] - 1.0f;

			data [index    ] += v [0] * c;
			data [index + 1] += v [1] * c;
			data [index + 2] += v [2] * c;
			data [index + 3] += v [0] * c;
			data [index + 4] += v [1] * c;
			data [index + 5] += v [2] * c;

            if (data [index    ] < minco [0]) minco [0] = data [index    ];
            if (data [index + 1] < minco [1]) minco [1] = data [index + 1];
            if (data [index + 2] < minco [2]) minco [2] = data [index + 2];
            if (data [index    ] > maxco [0]) maxco [0] = data [index    ];
            if (data [index + 1] > maxco [1]) maxco [1] = data [index + 1];
            if (data [index + 2] > maxco [2]) maxco [2] = data [index + 2];

            index += 7;
        }

        FLOAT_32 sizex = abs (minco [0] - maxco [0]);
        FLOAT_32 sizey = abs (minco [1] - maxco [1]);
        FLOAT_32 sizez = abs (minco [2] - maxco [2]);

        UINT_32P particles   = (UINT_32P) malloc (count * SIZE_UINT_32);
        UINT_32P groupc      = (UINT_32P) calloc (groups, SIZE_UINT_32);

        index = 0;
        for (UINT_32 n = 0; n < count; n ++) {

			FLOAT_32 rx = (rand () / (FLOAT_32) RAND_MAX) - 0.5f;
			FLOAT_32 ry = (rand () / (FLOAT_32) RAND_MAX) - 0.5f;
			FLOAT_32 rz = (rand () / (FLOAT_32) RAND_MAX) - 0.5f;

			FLOAT_32 factor = randrad / sqrt (rx*rx + ry*ry + rz*rz);

			// normalization and scale
			rx *= factor;
			ry *= factor;
			rz *= factor;

            FLOAT_32 x = data [index    ] + rx;	if (x < minco [0]) x = minco [0];	if (x > maxco [0]) x = maxco [0];
            FLOAT_32 y = data [index + 1] + ry;	if (y < minco [1]) y = minco [1];	if (y > maxco [1]) y = maxco [1];
            FLOAT_32 z = data [index + 2] + rz;	if (z < minco [2]) z = minco [2];	if (z > maxco [2]) z = maxco [2];

            x = floor (divx * (x - minco [0]) / sizex); x = MIN (divx - 1, x);
            y = floor (divy * (y - minco [1]) / sizey); y = MIN (divy - 1, y);
            z = floor (divz * (z - minco [2]) / sizez); z = MIN (divz - 1, z);

            UINT_32 g = (UINT_32) ((z * divy * divx) + (y * divx) + x);

            particles [n] = g;  groupc [g] ++;

            index += 7;
        }

		if (cancell > 0.0) {

			cancell *= cancell;

	        index = 0;
			for (UINT_32 n = 0; n < count; n ++) {

				FLOAT_32 x = data [index    ];
				FLOAT_32 y = data [index + 1];
				FLOAT_32 z = data [index + 2];

				for (UINT_32 m = 0; m < count; m ++) {

					if ((m != n) && (particles [m] != 0xffffffff)) {

						UINT_32 i = m * 7;

						FLOAT_32 v [3] = {	x - data [i    ],
											y - data [i + 1],
											z - data [i + 2] };

						if ((v[0]*v[0] + v[1]*v[1] + v[2]*v[2]) < cancell) {

							groupc [particles [n]] --;	particles [n] = 0xffffffff;	break;
						}
					}
				}

	            index += 7;
			}
		}

		UINT_16 groups_count = 0;

        for (UINT_32 n = 0; n < groups; n ++) {

            if (groupc [n] > 0xffff) {

                /// PROBLEM
				printf ("ERROR - group size is exceeding limit 65535. Use higher subdivision levels.\n");

				return -2;
            }

			if (groupc [n] > 0) groups_count ++;
		}

		if (groups_count == 0) {

			printf ("REMOVED - there is no particles to export.\n");

			remove (filename);

			return 0;
		}

        FILE *file;

        if (!(file = fopen (filename, "wb")))   return -1;

        // write count of groups		
		fwrite ((VOIDP) &groups_count, SIZE_UINT_16, 1, file);

		// fix the coordinate system 
		FLOAT_32 tmp;
		
		tmp = bbox [ 1];	bbox [ 1] = bbox [ 2];	bbox [ 2] = - tmp;
		tmp = bbox [ 4];	bbox [ 4] = bbox [ 5];	bbox [ 5] = - tmp;
		tmp = bbox [ 7];	bbox [ 7] = bbox [ 8];	bbox [ 8] = - tmp;
		tmp = bbox [10];	bbox [10] = bbox [11];	bbox [11] = - tmp;
		tmp = bbox [13];	bbox [13] = bbox [14];	bbox [14] = - tmp;
		tmp = bbox [16];	bbox [16] = bbox [17];	bbox [17] = - tmp;
		tmp = bbox [19];	bbox [19] = bbox [20];	bbox [20] = - tmp;
		tmp = bbox [22];	bbox [22] = bbox [23];	bbox [23] = - tmp;

        for (UINT_32 n = 0; n < groups; n ++) {

			if (groupc [n] > 0) {

				FLOAT_32 wminco [3] = { 99999999.0f, 99999999.0f, 99999999.0f};
				FLOAT_32 wmaxco [3] = {-99999999.0f,-99999999.0f,-99999999.0f};

				UINT_16 gc = (UINT_16) groupc [n];

				// write count of instances in group
				fwrite ((VOIDP) &gc, SIZE_UINT_16, 1, file);

				index = 0;
				for (UINT_32 m = 0; m < count; m ++) {

					if (particles [m] == n) {

						FLOAT_32 v [3] = {  data [index    ] - data [index + 3],
											data [index + 2] - data [index + 5],
											data [index + 4] - data [index + 1]	};

						FLOAT_32 scale = sqrt (v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);

						v [0] /= scale;
						v [1] /= scale;
						v [2] /= scale;
												
						scale *= (datak [m] - 1.0f) * data [index + 6];

						FLOAT_32 translation [3] = {    data [index    ], 
														data [index + 2], 
													  - data [index + 1]    };

						fwrite ((VOIDP) &translation [0], SIZE_FLOAT_32, 1, file);
						fwrite ((VOIDP) &translation [1], SIZE_FLOAT_32, 1, file);
						fwrite ((VOIDP) &translation [2], SIZE_FLOAT_32, 1, file);

						// rotation matrix

						FLOAT_32 a = 0;
						FLOAT_32 b = 0;
						FLOAT_32 c = 0;

						if (v [1] > 0) {

							if (v [0] < 0)	c -= acos (v [1] / sqrt (v[1]*v[1] + v[0]*v[0])) - (FLOAT_32) PI*0.5f; else
											c += acos (v [1] / sqrt (v[1]*v[1] + v[0]*v[0])) + (FLOAT_32) PI*0.5f;

							if (v [2] < 0)  a -= acos (v [1] / sqrt (v[1]*v[1] + v[2]*v[2])) - (FLOAT_32) PI*0.5f; else
											a += acos (v [1] / sqrt (v[1]*v[1] + v[2]*v[2])) + (FLOAT_32) PI*0.5f;

						} else {

							if (v [0] < 0)	c -= acos (v [1] / sqrt (v[1]*v[1] + v[0]*v[0])); else
											c += acos (v [1] / sqrt (v[1]*v[1] + v[0]*v[0]));

							if (v [2] < 0)  a -= acos (v [1] / sqrt (v[1]*v[1] + v[2]*v[2])); else
											a += acos (v [1] / sqrt (v[1]*v[1] + v[2]*v[2]));
						}

						FLOAT_32 c1 = cos (a * 0.5f);
						FLOAT_32 c2 = cos (b * 0.5f);
						FLOAT_32 c3 = cos (c * 0.5f);
						FLOAT_32 s1 = sin (a * 0.5f);
						FLOAT_32 s2 = sin (b * 0.5f);
						FLOAT_32 s3 = sin (c * 0.5f);

						FLOAT_32 qw = c1*c2*c3 - s1*s2*s3;
						FLOAT_32 qx = s1*s2*c3 + c1*c2*s3;
						FLOAT_32 qy = s1*c2*c3 + c1*s2*s3;
						FLOAT_32 qz = c1*s2*c3 - s1*c2*s3;

						FLOAT_32 denom = sqrt (qx*qx + qy*qy + qz*qz + qw*qw);

						qw /= denom;
						qx /= denom;
						qy /= denom;
						qz /= denom;

						// hack rotation

						c1 = cos ((FLOAT_32) (PI * 0.5) * 0.5f);
						s1 = sin ((FLOAT_32) (PI * 0.5) * 0.5f);

						FLOAT_32 qwh = c1;
						FLOAT_32 qxh = 0.0f;
						FLOAT_32 qyh = s1;
						FLOAT_32 qzh = 0.0f;

						denom = sqrt (qyh*qyh + qwh*qwh);

						qwh /= denom;
						qyh /= denom;
					
						// random rotation

						FLOAT_32 ar = (FLOAT_32) (rotx * rand () / (FLOAT_32) RAND_MAX);
						FLOAT_32 br = (FLOAT_32) (rotz * rand () / (FLOAT_32) RAND_MAX);
						FLOAT_32 cr = (FLOAT_32) (roty * rand () / (FLOAT_32) RAND_MAX);

						// X axis

						FLOAT_32 qwrX = cos (ar * 0.5f);
						FLOAT_32 qxrX = 0.0f;
						FLOAT_32 qyrX = sin (ar * 0.5f);
						FLOAT_32 qzrX = 0.0f;

						denom = sqrt (qyrX*qyrX + qwrX*qwrX);

						qwrX /= denom;
						qyrX /= denom;

						// Y axis

						FLOAT_32 qwrY = cos (br * 0.5f);
						FLOAT_32 qxrY = 0.0f;
						FLOAT_32 qyrY = 0.0f;
						FLOAT_32 qzrY = sin (br * 0.5f);

						denom = sqrt (qzrY*qzrY + qwrY*qwrY);

						qwrY /= denom;
						qzrY /= denom;

						// Z axis

						FLOAT_32 qwrZ = cos (cr * 0.5f);
						FLOAT_32 qxrZ = sin (cr * 0.5f);
						FLOAT_32 qyrZ = 0.0f;
						FLOAT_32 qzrZ = 0.0f;

						denom = sqrt (qxrZ*qxrZ + qwrZ*qwrZ);

						qwrZ /= denom;
						qxrZ /= denom;

						// COMBINE RANDOM ROTATIONS IN ORDER

						FLOAT_32 qwr;	FLOAT_32 qwrC;
						FLOAT_32 qxr;	FLOAT_32 qxrC;
						FLOAT_32 qyr;	FLOAT_32 qyrC;
						FLOAT_32 qzr;	FLOAT_32 qzrC;

						switch (rotorder) {

							// XYZ
							case 1:
								qwrC = qwrY*qwrZ - qxrY*qxrZ - qyrY*qyrZ - qzrY*qzrZ;
								qxrC = qwrY*qxrZ + qxrY*qwrZ + qyrY*qzrZ - qzrY*qyrZ;
								qyrC = qwrY*qyrZ + qyrY*qwrZ + qzrY*qxrZ - qxrY*qzrZ;
								qzrC = qwrY*qzrZ + qzrY*qwrZ + qxrY*qyrZ - qyrY*qxrZ;

								denom = sqrt (qxrC*qxrC + qyrC*qyrC + qzrC*qzrC + qwrC*qwrC);

								qwrC /= denom;
								qxrC /= denom;
								qyrC /= denom;
								qzrC /= denom;

								qwr  = qwrX*qwrC - qxrX*qxrC - qyrX*qyrC - qzrX*qzrC;
								qxr  = qwrX*qxrC + qxrX*qwrC + qyrX*qzrC - qzrX*qyrC;
								qyr  = qwrX*qyrC + qyrX*qwrC + qzrX*qxrC - qxrX*qzrC;
								qzr  = qwrX*qzrC + qzrX*qwrC + qxrX*qyrC - qyrX*qxrC;
								break;
							// YXZ
							case 2:
								qwrC = qwrX*qwrZ - qxrX*qxrZ - qyrX*qyrZ - qzrX*qzrZ;
								qxrC = qwrX*qxrZ + qxrX*qwrZ + qyrX*qzrZ - qzrX*qyrZ;
								qyrC = qwrX*qyrZ + qyrX*qwrZ + qzrX*qxrZ - qxrX*qzrZ;
								qzrC = qwrX*qzrZ + qzrX*qwrZ + qxrX*qyrZ - qyrX*qxrZ;

								denom = sqrt (qxrC*qxrC + qyrC*qyrC + qzrC*qzrC + qwrC*qwrC);

								qwrC /= denom;
								qxrC /= denom;
								qyrC /= denom;
								qzrC /= denom;

								qwr  = qwrY*qwrC - qxrY*qxrC - qyrY*qyrC - qzrY*qzrC;
								qxr  = qwrY*qxrC + qxrY*qwrC + qyrY*qzrC - qzrY*qyrC;
								qyr  = qwrY*qyrC + qyrY*qwrC + qzrY*qxrC - qxrY*qzrC;
								qzr  = qwrY*qzrC + qzrY*qwrC + qxrY*qyrC - qyrY*qxrC;
								break;
							// ZYX
							case 3:
								qwrC = qwrY*qwrX - qxrY*qxrX - qyrY*qyrX - qzrY*qzrX;
								qxrC = qwrY*qxrX + qxrY*qwrX + qyrY*qzrX - qzrY*qyrX;
								qyrC = qwrY*qyrX + qyrY*qwrX + qzrY*qxrX - qxrY*qzrX;
								qzrC = qwrY*qzrX + qzrY*qwrX + qxrY*qyrX - qyrY*qxrX;

								denom = sqrt (qxrC*qxrC + qyrC*qyrC + qzrC*qzrC + qwrC*qwrC);

								qwrC /= denom;
								qxrC /= denom;
								qyrC /= denom;
								qzrC /= denom;

								qwr  = qwrZ*qwrC - qxrZ*qxrC - qyrZ*qyrC - qzrZ*qzrC;
								qxr  = qwrZ*qxrC + qxrZ*qwrC + qyrZ*qzrC - qzrZ*qyrC;
								qyr  = qwrZ*qyrC + qyrZ*qwrC + qzrZ*qxrC - qxrZ*qzrC;
								qzr  = qwrZ*qzrC + qzrZ*qwrC + qxrZ*qyrC - qyrZ*qxrC;
								break;
							// XZY
							case 4:
								qwrC = qwrZ*qwrY - qxrZ*qxrY - qyrZ*qyrY - qzrZ*qzrY;
								qxrC = qwrZ*qxrY + qxrZ*qwrY + qyrZ*qzrY - qzrZ*qyrY;
								qyrC = qwrZ*qyrY + qyrZ*qwrY + qzrZ*qxrY - qxrZ*qzrY;
								qzrC = qwrZ*qzrY + qzrZ*qwrY + qxrZ*qyrY - qyrZ*qxrY;

								denom = sqrt (qxrC*qxrC + qyrC*qyrC + qzrC*qzrC + qwrC*qwrC);

								qwrC /= denom;
								qxrC /= denom;
								qyrC /= denom;
								qzrC /= denom;

								qwr  = qwrX*qwrC - qxrX*qxrC - qyrX*qyrC - qzrX*qzrC;
								qxr  = qwrX*qxrC + qxrX*qwrC + qyrX*qzrC - qzrX*qyrC;
								qyr  = qwrX*qyrC + qyrX*qwrC + qzrX*qxrC - qxrX*qzrC;
								qzr  = qwrX*qzrC + qzrX*qwrC + qxrX*qyrC - qyrX*qxrC;
								break;
							// YZX
							case 5:
								qwrC = qwrZ*qwrX - qxrZ*qxrX - qyrZ*qyrX - qzrZ*qzrX;
								qxrC = qwrZ*qxrX + qxrZ*qwrX + qyrZ*qzrX - qzrZ*qyrX;
								qyrC = qwrZ*qyrX + qyrZ*qwrX + qzrZ*qxrX - qxrZ*qzrX;
								qzrC = qwrZ*qzrX + qzrZ*qwrX + qxrZ*qyrX - qyrZ*qxrX;

								denom = sqrt (qxrC*qxrC + qyrC*qyrC + qzrC*qzrC + qwrC*qwrC);

								qwrC /= denom;
								qxrC /= denom;
								qyrC /= denom;
								qzrC /= denom;

								qwr  = qwrY*qwrC - qxrY*qxrC - qyrY*qyrC - qzrY*qzrC;
								qxr  = qwrY*qxrC + qxrY*qwrC + qyrY*qzrC - qzrY*qyrC;
								qyr  = qwrY*qyrC + qyrY*qwrC + qzrY*qxrC - qxrY*qzrC;
								qzr  = qwrY*qzrC + qzrY*qwrC + qxrY*qyrC - qyrY*qxrC;
								break;
							// ZXY
							case 6:
								qwrC = qwrX*qwrY - qxrX*qxrY - qyrX*qyrY - qzrX*qzrY;
								qxrC = qwrX*qxrY + qxrX*qwrY + qyrX*qzrY - qzrX*qyrY;
								qyrC = qwrX*qyrY + qyrX*qwrY + qzrX*qxrY - qxrX*qzrY;
								qzrC = qwrX*qzrY + qzrX*qwrY + qxrX*qyrY - qyrX*qxrY;

								denom = sqrt (qxrC*qxrC + qyrC*qyrC + qzrC*qzrC + qwrC*qwrC);

								qwrC /= denom;
								qxrC /= denom;
								qyrC /= denom;
								qzrC /= denom;

								qwr  = qwrZ*qwrC - qxrZ*qxrC - qyrZ*qyrC - qzrZ*qzrC;
								qxr  = qwrZ*qxrC + qxrZ*qwrC + qyrZ*qzrC - qzrZ*qyrC;
								qyr  = qwrZ*qyrC + qyrZ*qwrC + qzrZ*qxrC - qxrZ*qzrC;
								qzr  = qwrZ*qzrC + qzrZ*qwrC + qxrZ*qyrC - qyrZ*qxrC;
								break;
						}

						// combining

						denom = sqrt (qxr*qxr + qyr*qyr + qzr*qzr + qwr*qwr);

						qwr /= denom;
						qxr /= denom;
						qyr /= denom;
						qzr /= denom;

						FLOAT_32 qtw = qwh*qwr - qxh*qxr - qyh*qyr - qzh*qzr;
						FLOAT_32 qtx = qwh*qxr + qxh*qwr + qyh*qzr - qzh*qyr;
						FLOAT_32 qty = qwh*qyr + qyh*qwr + qzh*qxr - qxh*qzr;
						FLOAT_32 qtz = qwh*qzr + qzh*qwr + qxh*qyr - qyh*qxr;

						denom = sqrt (qtx*qtx + qty*qty + qtz*qtz + qtw*qtw);

						qtw /= denom;
						qtx /= denom;
						qty /= denom;
						qtz /= denom;

						FLOAT_32 qqw = qtw*qw - qtx*qx - qty*qy - qtz*qz;
						FLOAT_32 qqx = qtw*qx + qtx*qw + qty*qz - qtz*qy;
						FLOAT_32 qqy = qtw*qy + qty*qw + qtz*qx - qtx*qz;
						FLOAT_32 qqz = qtw*qz + qtz*qw + qtx*qy - qty*qx;

						denom = sqrt (qqx*qqx + qqy*qqy + qqz*qqz + qqw*qqw);

						qqw /= denom;
						qqx /= denom;
						qqy /= denom;
						qqz /= denom;

						// quaterion to euler

						FLOAT_32 test = qqx*qqy + qqz*qqw;

						if (test > 0.499) {		// singularity at north pole

							a = (FLOAT_32) 2 * atan2 (qqx, qqw);
							b = (FLOAT_32) PI/2;
							c = (FLOAT_32) 0;

						} else {

							if (test < -0.499) {	// singularity at south pole

								a = (FLOAT_32) -2 * atan2 (qqx, qqw);
								b = (FLOAT_32) - PI/2;
								c = (FLOAT_32) 0;

							} else {

								FLOAT_32 sqx = qqx*qqx;
								FLOAT_32 sqy = qqy*qqy;
								FLOAT_32 sqz = qqz*qqz;

								a = (FLOAT_32) atan2 (2*qqy*qqw-2*qqx*qqz, 1 - 2*sqy - 2*sqz);
								b = (FLOAT_32) asin  (2*test);
								c = (FLOAT_32) atan2 (2*qqx*qqw-2*qqy*qqz, 1 - 2*sqx - 2*sqz);
							}
						}

						FLOAT_32 sina = sin (a);
						FLOAT_32 sinb = sin (b);
						FLOAT_32 sinc = sin (c);

						FLOAT_32 cosa = cos (a);
						FLOAT_32 cosb = cos (b);
						FLOAT_32 cosc = cos (c);

						FLOAT_32 rotation [3] = { a, b, c }; 

						fwrite ((VOIDP) &rotation [0], SIZE_FLOAT_32, 1, file);
						fwrite ((VOIDP) &rotation [1], SIZE_FLOAT_32, 1, file);
						fwrite ((VOIDP) &rotation [2], SIZE_FLOAT_32, 1, file);

						fwrite ((VOIDP) &scale, SIZE_FLOAT_32, 1, file);

						FLOAT_32 matrix [16] = {    cosc, 		-	sinc*cosb,						sinc*sinb,						translation [0],
													cosa*sinc,	-	sina*sinb + cosc*cosb*cosa,	-	sina*cosb - cosc*sinb*cosa,		translation [1],
													sina*sinc,		cosa*sinb + cosc*cosb*sina,		cosa*cosb - cosc*sinb*sina,		translation [2],
													0,				0,								0,								1    };

						matrix [ 0] *= scale;   matrix [ 1] *= scale;   matrix [ 2] *= scale;
						matrix [ 4] *= scale;   matrix [ 5] *= scale;   matrix [ 6] *= scale;
						matrix [ 8] *= scale;   matrix [ 9] *= scale;   matrix [10] *= scale;

						UINT_32 j = 0;
						for (UINT_32 i = 0; i < 8; i ++) {

							FLOAT_32 point [3];

							FLOAT_32P p = &bbox [j];    j += 3;

							pTRANSFORM_ROW4x4p3 (point, p, matrix)

							if (point [0] < wminco [0])  wminco [0] = point [0];
							if (point [1] < wminco [1])  wminco [1] = point [1];
							if (point [2] < wminco [2])  wminco [2] = point [2];
							if (point [0] > wmaxco [0])  wmaxco [0] = point [0];
							if (point [1] > wmaxco [1])  wmaxco [1] = point [1];
							if (point [2] > wmaxco [2])  wmaxco [2] = point [2];
						}
					}

					index += 7;
				}

				// write boundary
				fwrite ((VOIDP) &wminco [0], SIZE_FLOAT_32, 1, file);
				fwrite ((VOIDP) &wminco [1], SIZE_FLOAT_32, 1, file);
				fwrite ((VOIDP) &wminco [2], SIZE_FLOAT_32, 1, file);

				fwrite ((VOIDP) &wmaxco [0], SIZE_FLOAT_32, 1, file);
				fwrite ((VOIDP) &wmaxco [1], SIZE_FLOAT_32, 1, file);
				fwrite ((VOIDP) &wmaxco [2], SIZE_FLOAT_32, 1, file);
			}
        }
                    
        fclose (file);

        return 0;
    }

    DECLDIR UINT_32 _stdcall processDecompose (CHARP composite, CHARP destination)
    {
        TImage icomposite;

        printf ("Loading file : %s", composite);

        if (loadImage (composite, &icomposite) < 0) {

            printf (" - FAILED - Supported is only uncompressed RGB 'tga' format. \n");
            return -1;
        }

        if (icomposite.type != M_IMAGE_TYPE_RGB) {

            printf (" - FAILED - Supported is only uncompressed RGB 'tga' format. \n");
            return -2;
        }

        printf (" - SUCCEED\n");

        TImage iout [32];
        
        UINT_32 w = icomposite.width;
        UINT_32 h = icomposite.height;

        for (UINT_32 n = 0; n < 32; n ++) {

            iout [n].width  = w;
            iout [n].height = h;

            iout [n].type = M_IMAGE_TYPE_RGB;

            iout [n].data = NULL;
        }

        UINT_32 colcount = 0;

        UINT_8 colors [32][3] = {{0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, 
                                 {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, 
                                 {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, 
                                 {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}};

        UINT_8P pcomposite = (UINT_8P) icomposite.data;

        for (UINT_32 x = 0; x < w; x ++) {
            for (UINT_32 y = 0; y < h; y ++) {

                UINT_32 index = (y*w + x) * 3;

                UINT_32 r = pcomposite [index    ];
                UINT_32 g = pcomposite [index + 1]; 
                UINT_32 b = pcomposite [index + 2];

                UINT_32 n;
                for (n = 0; n < colcount; n ++) {

                    if ((colors [n][0] == r) && (colors [n][1] == g) && (colors [n][2] == b)) break;
                }

                if (n == colcount) {
                    if (colcount <= 32) {

                        colors [colcount][0] = r;
                        colors [colcount][1] = g;
                        colors [colcount][2] = b;   colcount ++;

                        iout [n].data = calloc (w * h * 3, SIZE_UINT_8);

                    } else {

                        printf (" ERROR - Supported number of layers (different colors) is up to 32. \n");
                        return -3;
                    }

                }

                UINT_8P p = (UINT_8P) iout [n].data;

                p [index    ] = 255;
                p [index + 1] = 255;
                p [index + 2] = 255;
            }
        }

        UINT_32 l = (UINT_32) strlen (composite);   composite [l - 4] = 0;  l += 10;

        CHARP fullname = (CHARP) malloc (l);

        for (UINT_32 n = 0; n < colcount; n ++) {

            if (iout [n].data != NULL) {

                sprintf (fullname, "%s.%i_%i_%i.tga", composite, colors [n][0], colors [n][1], colors [n][2]);

                if (saveImage (fullname, &iout [n]) < 0) {

                    printf ("Unable to save file : %s. \n", fullname);
                    return -4;
                }

                printf ("Out. file : %s \n", fullname);

                free (iout [n].data);
            }
        }

        free (icomposite.data);
        free (fullname);

        return 0;
    }

    DECLDIR UINT_32 _stdcall processWeightsGenerate (CHARPP files, CHARP filename)
    {
        // list of images
        TImage * images = (TImage *) malloc (sizeof (TImage) * 16);

        printf ("Output file 1 :  %s1.tga \n", filename);
        printf ("Output file 2 :  %s2.tga \n", filename);
        printf ("Output file 3 :  %s3.tga \n", filename);
        printf ("Output file 4 :  %s4.tga \n", filename);
        printf ("\n");

        UINT_32 w = 0;
        UINT_32 h = 0;

        // loading images
        for (UINT_32 n = 0; n < 16; n ++) {

            if (files [n][0] != 0) {

                printf ("Loading file : %i -> %s", n, files [n]);

                if (loadImage (files [n], &images [n]) < 0) {

                    printf (" - FAILED - Supported is only uncompressed RGB 'tga' format. \n");
                    return -1;
                }

                if (images [n].type != M_IMAGE_TYPE_RGB) {

                    printf (" - FAILED - Supported is only uncompressed RGB 'tga' format. \n");
                    return -2;
                }

                if (w == 0) {

                    w = images [n].width;
                    h = images [n].height;

                } else {

                    if ((images [n].width  != w) || 
                        (images [n].height != h))   {

                        printf ("ERROR - Inconsistent image dimensions. \n");
                        return -3;
                    }
                }

                printf (" - SUCCEED\n");
            }
        }

        TImage image1;
        TImage image2;
        TImage image3;
        TImage image4;

        image1.width  = w;
        image2.width  = w;
        image3.width  = w;
        image4.width  = w;

        image1.height = h;
        image2.height = h;
        image3.height = h;
        image4.height = h;

        image1.data   = (UINT_8P) calloc (w * h * 4, SIZE_UINT_8);   UINT_8P p1 = (UINT_8P) image1.data;
        image1.type   = M_IMAGE_TYPE_RGBA;
        image2.data   = (UINT_8P) calloc (w * h * 4, SIZE_UINT_8);   UINT_8P p2 = (UINT_8P) image2.data;
        image2.type   = M_IMAGE_TYPE_RGBA;
        image3.data   = (UINT_8P) calloc (w * h * 4, SIZE_UINT_8);   UINT_8P p3 = (UINT_8P) image3.data;
        image3.type   = M_IMAGE_TYPE_RGBA;
        image4.data   = (UINT_8P) calloc (w * h * 4, SIZE_UINT_8);   UINT_8P p4 = (UINT_8P) image4.data;
        image4.type   = M_IMAGE_TYPE_RGBA;

        UINT_32 x, y;

        for (y = 0; y < h; y ++) {
            for (x = 0; x < w; x ++) {
                
                UINT_32 index1 = ((y * w) + x) * 3;
                UINT_32 index2 = ((y * w) + x) * 4;

                for (UINT_32 n = 0; n < 16; n ++) {

                    if (files [n][0] != 0) {

                        UINT_8 val = ((UINT_8P) images [n].data) [index1];

                        UINT_32 m = n % 4;

                        if (n < 4) {

                            p1 [index2 + m] = val;    continue;
                        }
                        if (n < 8) {

                            p2 [index2 + m] = val;    continue;
                        }
                        if (n < 12) {

                            p3 [index2 + m] = val;    continue;
                        }
                        if (n < 16) {

                            p4 [index2 + m] = val;    continue;
                        }
                    }
                }
            }
        }

        UINT_32 l = (UINT_32) strlen (filename) + 5; 

        CHARP fullname = (CHARP) malloc (l);
        
        // save image 1
        sprintf (fullname, "%s1.tga", filename);

        if (saveImage (fullname, &image1) < 0) {

            printf ("Unable to save weight file. \n");
            return -5;
        }

        free (image1.data);

        // save image 2
        sprintf (fullname, "%s2.tga", filename);

        if (saveImage (fullname, &image2) < 0) {

            printf ("Unable to save weight file. \n");
            return -5;
        }

        free (image2.data);

        // save image 3
        sprintf (fullname, "%s3.tga", filename);

        if (saveImage (fullname, &image3) < 0) {

            printf ("Unable to save weight file. \n");
            return -5;
        }

        free (image3.data);

        // save image 4
        sprintf (fullname, "%s4.tga", filename);

        if (saveImage (fullname, &image4) < 0) {

            printf ("Unable to save weight file. \n");
            return -5;
        }

        free (image4.data);

        // free images
        for (UINT_32 n = 0; n < 16; n ++) {
        
            if (files [n][0] != 0) {

                free (images [n].data);
            }
        }

        free (images);

        return 0;
    }

    DECLDIR UINT_32 _stdcall processGenerateComposite (CHARP filename, CHARP translation, CHARP diffuse, CHARP normal, CHARP gloss, CHARP shininess, FLOAT_32 factord, FLOAT_32 factorn, FLOAT_32 factorg, FLOAT_32 factors)
    {
        TImage icomposite;
        TImage idiffuse;
        TImage inormal;
        TImage igloss;
        TImage ishininess;

        BOOL tdiffuse    = false;
        BOOL tnormal     = false;
        BOOL tgloss      = false;
        BOOL tshininess  = false;

        UINT_32 w = 0;
        UINT_32 h = 0;

        if ((translation [0] == 'A') || (translation [0] == 'B') || (translation [0] == 'C') ||
            (translation [1] == 'A') || (translation [1] == 'B') || (translation [1] == 'C') ||
            (translation [2] == 'A') || (translation [2] == 'B') || (translation [2] == 'C') ||
            (translation [3] == 'A') || (translation [3] == 'B') || (translation [3] == 'C')) {

            tdiffuse = true;

            printf ("Loading diffuse map file : \n\t%s", diffuse);

            if (loadImage (diffuse, &idiffuse) < 0) {

                printf (" - FAILED - Supported is only uncompressed RGB 'tga' format. \n");
                return -1;
            }

            if (idiffuse.type != M_IMAGE_TYPE_RGB) {

                printf (" - FAILED - Supported is only uncompressed RGB 'tga' format. \n");
                return -2;
            }

            if (w > 0) {

                if ((w != idiffuse.width) || (h != idiffuse.height)) {

                    printf ("ERROR - Inconsistent image dimensions. \n");
                    return -3;
                }

            } else {

                w = idiffuse.width;
                h = idiffuse.height;
            }

            printf (" - SUCCEED\n");
        }

        if ((translation [0] == 'D') || (translation [0] == 'E') || (translation [0] == 'F') ||
            (translation [1] == 'D') || (translation [1] == 'E') || (translation [1] == 'F') ||
            (translation [2] == 'D') || (translation [2] == 'E') || (translation [2] == 'F') ||
            (translation [3] == 'D') || (translation [3] == 'E') || (translation [3] == 'F')) {

            tnormal = true;

            printf ("Loading normal map file : \n\t%s", normal);

            if (loadImage (normal, &inormal) < 0) {

                printf (" - FAILED - Supported is only uncompressed RGB 'tga' format. \n");
                return -1;
            }

            if (inormal.type != M_IMAGE_TYPE_RGB) {

                printf (" - FAILED - Supported is only uncompressed RGB 'tga' format. \n");
                return -2;
            }

            if (w > 0) {

                if ((w != inormal.width) || (h != inormal.height)) {

                    printf ("ERROR - Inconsistent image dimensions. \n");
                    return -3;
                }

            } else {

                w = inormal.width;
                h = inormal.height;
            }

            printf (" - SUCCEED\n");
        }

        if ((translation [0] == 'G') || (translation [0] == 'H') || (translation [0] == 'I') ||
            (translation [1] == 'G') || (translation [1] == 'H') || (translation [1] == 'I') ||
            (translation [2] == 'G') || (translation [2] == 'H') || (translation [2] == 'I') ||
            (translation [3] == 'G') || (translation [3] == 'H') || (translation [3] == 'I')) {

            tgloss = true;

            printf ("Loading gloss map file : \n\t%s", gloss);

            if (loadImage (gloss, &igloss) < 0) {

                printf (" - FAILED - Supported is only uncompressed RGB 'tga' format. \n");
                return -1;
            }

            if (igloss.type != M_IMAGE_TYPE_RGB) {

                printf (" - FAILED - Supported is only uncompressed RGB 'tga' format. \n");
                return -2;
            }

            if (w > 0) {

                if ((w != igloss.width) || (h != igloss.height)) {

                    printf ("ERROR - Inconsistent image dimensions. \n");
                    return -3;
                }

            } else {

                w = igloss.width;
                h = igloss.height;
            }

            printf (" - SUCCEED\n");
        }

        if ((translation [0] == 'J') || (translation [0] == 'K') || (translation [0] == 'L') ||
            (translation [1] == 'J') || (translation [1] == 'K') || (translation [1] == 'L') ||
            (translation [2] == 'J') || (translation [2] == 'K') || (translation [2] == 'L') ||
            (translation [3] == 'J') || (translation [3] == 'K') || (translation [3] == 'L')) {

            tshininess = true;

            printf ("Loading shininess map file : \n\t%s", shininess);

            if (loadImage (shininess, &ishininess) < 0) {

                printf (" - FAILED - Supported is only uncompressed RGB 'tga' format. \n");
                return -1;
            }

            if (ishininess.type != M_IMAGE_TYPE_RGB) {

                printf (" - FAILED - Supported is only uncompressed RGB 'tga' format. \n");
                return -2;
            }

            if (w > 0) {

                if ((w != ishininess.width) || (h != ishininess.height)) {

                    printf ("ERROR - Inconsistent image dimensions. \n");
                    return -3;
                }

            } else {

                w = ishininess.width;
                h = ishininess.height;
            }

            printf (" - SUCCEED\n");
        }

        // using alpha ?
        if (translation [3] == 'X') {

            icomposite.data   = (UINT_8P) calloc (w * h * 3, SIZE_UINT_8);   UINT_8P p = (UINT_8P) icomposite.data;
            icomposite.type   = M_IMAGE_TYPE_RGB;
            icomposite.width  = w;
            icomposite.height = h;

            UINT_32 x, y;

            for (y = 0; y < h; y ++) {
                for (x = 0; x < w; x ++) {
                
                    UINT_32 index1 = ((y * w) + x) * 3;
                    UINT_32 index2 = ((y * w) + x) * 3;

                    switch (translation [0]) {
                        case 'A': p [index2    ] = (UINT_8) floor (((UINT_8P) idiffuse      .data) [index1    ]   * factord);   break;
                        case 'B': p [index2    ] = (UINT_8) floor (((UINT_8P) idiffuse      .data) [index1 + 1]   * factord);   break;
                        case 'C': p [index2    ] = (UINT_8) floor (((UINT_8P) idiffuse      .data) [index1 + 2]   * factord);   break;
                        case 'D': p [index2    ] = (UINT_8) floor (((UINT_8P) inormal       .data) [index1    ]   * factorn);   break;
                        case 'E': p [index2    ] = (UINT_8) floor (((UINT_8P) inormal       .data) [index1 + 1]   * factorn);   break;
                        case 'F': p [index2    ] = (UINT_8) floor (((UINT_8P) inormal       .data) [index1 + 2]   * factorn);   break;
                        case 'G': p [index2    ] = (UINT_8) floor (((UINT_8P) igloss        .data) [index1    ]   * factorg);   break;
                        case 'H': p [index2    ] = (UINT_8) floor (((UINT_8P) igloss        .data) [index1 + 1]   * factorg);   break;
                        case 'I': p [index2    ] = (UINT_8) floor (((UINT_8P) igloss        .data) [index1 + 2]   * factorg);   break;
                        case 'J': p [index2    ] = (UINT_8) floor (((UINT_8P) ishininess    .data) [index1    ]   * factors);   break;
                        case 'K': p [index2    ] = (UINT_8) floor (((UINT_8P) ishininess    .data) [index1 + 1]   * factors);   break;
                        case 'L': p [index2    ] = (UINT_8) floor (((UINT_8P) ishininess    .data) [index1 + 2]   * factors);   break;
                    }

                    switch (translation [1]) {
                        case 'A': p [index2 + 1] = (UINT_8) floor (((UINT_8P) idiffuse      .data) [index1    ]   * factord);   break;
                        case 'B': p [index2 + 1] = (UINT_8) floor (((UINT_8P) idiffuse      .data) [index1 + 1]   * factord);   break;
                        case 'C': p [index2 + 1] = (UINT_8) floor (((UINT_8P) idiffuse      .data) [index1 + 2]   * factord);   break;
                        case 'D': p [index2 + 1] = (UINT_8) floor (((UINT_8P) inormal       .data) [index1    ]   * factorn);   break;
                        case 'E': p [index2 + 1] = (UINT_8) floor (((UINT_8P) inormal       .data) [index1 + 1]   * factorn);   break;
                        case 'F': p [index2 + 1] = (UINT_8) floor (((UINT_8P) inormal       .data) [index1 + 2]   * factorn);   break;
                        case 'G': p [index2 + 1] = (UINT_8) floor (((UINT_8P) igloss        .data) [index1    ]   * factorg);   break;
                        case 'H': p [index2 + 1] = (UINT_8) floor (((UINT_8P) igloss        .data) [index1 + 1]   * factorg);   break;
                        case 'I': p [index2 + 1] = (UINT_8) floor (((UINT_8P) igloss        .data) [index1 + 2]   * factorg);   break;
                        case 'J': p [index2 + 1] = (UINT_8) floor (((UINT_8P) ishininess    .data) [index1    ]   * factors);   break;
                        case 'K': p [index2 + 1] = (UINT_8) floor (((UINT_8P) ishininess    .data) [index1 + 1]   * factors);   break;
                        case 'L': p [index2 + 1] = (UINT_8) floor (((UINT_8P) ishininess    .data) [index1 + 2]   * factors);   break;
                    }

                    switch (translation [2]) {
                        case 'A': p [index2 + 2] = (UINT_8) floor (((UINT_8P) idiffuse      .data) [index1    ]   * factord);   break;
                        case 'B': p [index2 + 2] = (UINT_8) floor (((UINT_8P) idiffuse      .data) [index1 + 1]   * factord);   break;
                        case 'C': p [index2 + 2] = (UINT_8) floor (((UINT_8P) idiffuse      .data) [index1 + 2]   * factord);   break;
                        case 'D': p [index2 + 2] = (UINT_8) floor (((UINT_8P) inormal       .data) [index1    ]   * factorn);   break;
                        case 'E': p [index2 + 2] = (UINT_8) floor (((UINT_8P) inormal       .data) [index1 + 1]   * factorn);   break;
                        case 'F': p [index2 + 2] = (UINT_8) floor (((UINT_8P) inormal       .data) [index1 + 2]   * factorn);   break;
                        case 'G': p [index2 + 2] = (UINT_8) floor (((UINT_8P) igloss        .data) [index1    ]   * factorg);   break;
                        case 'H': p [index2 + 2] = (UINT_8) floor (((UINT_8P) igloss        .data) [index1 + 1]   * factorg);   break;
                        case 'I': p [index2 + 2] = (UINT_8) floor (((UINT_8P) igloss        .data) [index1 + 2]   * factorg);   break;
                        case 'J': p [index2 + 2] = (UINT_8) floor (((UINT_8P) ishininess    .data) [index1    ]   * factors);   break;
                        case 'K': p [index2 + 2] = (UINT_8) floor (((UINT_8P) ishininess    .data) [index1 + 1]   * factors);   break;
                        case 'L': p [index2 + 2] = (UINT_8) floor (((UINT_8P) ishininess    .data) [index1 + 2]   * factors);   break;
                    }
                }
            }

        } else {

            icomposite.data   = (UINT_8P) calloc (w * h * 4, SIZE_UINT_8);   UINT_8P p = (UINT_8P) icomposite.data;
            icomposite.type   = M_IMAGE_TYPE_RGBA;
            icomposite.width  = w;
            icomposite.height = h;

            UINT_32 x, y;

            for (y = 0; y < h; y ++) {
                for (x = 0; x < w; x ++) {
                
                    UINT_32 index1 = ((y * w) + x) * 3;
                    UINT_32 index2 = ((y * w) + x) * 4;

                    switch (translation [0]) {
                        case 'A': p [index2    ] = (UINT_8) floor (((UINT_8P) idiffuse      .data) [index1    ]   * factord);   break;
                        case 'B': p [index2    ] = (UINT_8) floor (((UINT_8P) idiffuse      .data) [index1 + 1]   * factord);   break;
                        case 'C': p [index2    ] = (UINT_8) floor (((UINT_8P) idiffuse      .data) [index1 + 2]   * factord);   break;
                        case 'D': p [index2    ] = (UINT_8) floor (((UINT_8P) inormal       .data) [index1    ]   * factorn);   break;
                        case 'E': p [index2    ] = (UINT_8) floor (((UINT_8P) inormal       .data) [index1 + 1]   * factorn);   break;
                        case 'F': p [index2    ] = (UINT_8) floor (((UINT_8P) inormal       .data) [index1 + 2]   * factorn);   break;
                        case 'G': p [index2    ] = (UINT_8) floor (((UINT_8P) igloss        .data) [index1    ]   * factorg);   break;
                        case 'H': p [index2    ] = (UINT_8) floor (((UINT_8P) igloss        .data) [index1 + 1]   * factorg);   break;
                        case 'I': p [index2    ] = (UINT_8) floor (((UINT_8P) igloss        .data) [index1 + 2]   * factorg);   break;
                        case 'J': p [index2    ] = (UINT_8) floor (((UINT_8P) ishininess    .data) [index1    ]   * factors);   break;
                        case 'K': p [index2    ] = (UINT_8) floor (((UINT_8P) ishininess    .data) [index1 + 1]   * factors);   break;
                        case 'L': p [index2    ] = (UINT_8) floor (((UINT_8P) ishininess    .data) [index1 + 2]   * factors);   break;
                    }

                    switch (translation [1]) {
                        case 'A': p [index2 + 1] = (UINT_8) floor (((UINT_8P) idiffuse      .data) [index1    ]   * factord);   break;
                        case 'B': p [index2 + 1] = (UINT_8) floor (((UINT_8P) idiffuse      .data) [index1 + 1]   * factord);   break;
                        case 'C': p [index2 + 1] = (UINT_8) floor (((UINT_8P) idiffuse      .data) [index1 + 2]   * factord);   break;
                        case 'D': p [index2 + 1] = (UINT_8) floor (((UINT_8P) inormal       .data) [index1    ]   * factorn);   break;
                        case 'E': p [index2 + 1] = (UINT_8) floor (((UINT_8P) inormal       .data) [index1 + 1]   * factorn);   break;
                        case 'F': p [index2 + 1] = (UINT_8) floor (((UINT_8P) inormal       .data) [index1 + 2]   * factorn);   break;
                        case 'G': p [index2 + 1] = (UINT_8) floor (((UINT_8P) igloss        .data) [index1    ]   * factorg);   break;
                        case 'H': p [index2 + 1] = (UINT_8) floor (((UINT_8P) igloss        .data) [index1 + 1]   * factorg);   break;
                        case 'I': p [index2 + 1] = (UINT_8) floor (((UINT_8P) igloss        .data) [index1 + 2]   * factorg);   break;
                        case 'J': p [index2 + 1] = (UINT_8) floor (((UINT_8P) ishininess    .data) [index1    ]   * factors);   break;
                        case 'K': p [index2 + 1] = (UINT_8) floor (((UINT_8P) ishininess    .data) [index1 + 1]   * factors);   break;
                        case 'L': p [index2 + 1] = (UINT_8) floor (((UINT_8P) ishininess    .data) [index1 + 2]   * factors);   break;
                    }

                    switch (translation [2]) {
                        case 'A': p [index2 + 2] = (UINT_8) floor (((UINT_8P) idiffuse      .data) [index1    ]   * factord);   break;
                        case 'B': p [index2 + 2] = (UINT_8) floor (((UINT_8P) idiffuse      .data) [index1 + 1]   * factord);   break;
                        case 'C': p [index2 + 2] = (UINT_8) floor (((UINT_8P) idiffuse      .data) [index1 + 2]   * factord);   break;
                        case 'D': p [index2 + 2] = (UINT_8) floor (((UINT_8P) inormal       .data) [index1    ]   * factorn);   break;
                        case 'E': p [index2 + 2] = (UINT_8) floor (((UINT_8P) inormal       .data) [index1 + 1]   * factorn);   break;
                        case 'F': p [index2 + 2] = (UINT_8) floor (((UINT_8P) inormal       .data) [index1 + 2]   * factorn);   break;
                        case 'G': p [index2 + 2] = (UINT_8) floor (((UINT_8P) igloss        .data) [index1    ]   * factorg);   break;
                        case 'H': p [index2 + 2] = (UINT_8) floor (((UINT_8P) igloss        .data) [index1 + 1]   * factorg);   break;
                        case 'I': p [index2 + 2] = (UINT_8) floor (((UINT_8P) igloss        .data) [index1 + 2]   * factorg);   break;
                        case 'J': p [index2 + 2] = (UINT_8) floor (((UINT_8P) ishininess    .data) [index1    ]   * factors);   break;
                        case 'K': p [index2 + 2] = (UINT_8) floor (((UINT_8P) ishininess    .data) [index1 + 1]   * factors);   break;
                        case 'L': p [index2 + 2] = (UINT_8) floor (((UINT_8P) ishininess    .data) [index1 + 2]   * factors);   break;
                    }

                    switch (translation [3]) {
                        case 'A': p [index2 + 3] = (UINT_8) floor (((UINT_8P) idiffuse      .data) [index1    ]   * factord);   break;
                        case 'B': p [index2 + 3] = (UINT_8) floor (((UINT_8P) idiffuse      .data) [index1 + 1]   * factord);   break;
                        case 'C': p [index2 + 3] = (UINT_8) floor (((UINT_8P) idiffuse      .data) [index1 + 2]   * factord);   break;
                        case 'D': p [index2 + 3] = (UINT_8) floor (((UINT_8P) inormal       .data) [index1    ]   * factorn);   break;
                        case 'E': p [index2 + 3] = (UINT_8) floor (((UINT_8P) inormal       .data) [index1 + 1]   * factorn);   break;
                        case 'F': p [index2 + 3] = (UINT_8) floor (((UINT_8P) inormal       .data) [index1 + 2]   * factorn);   break;
                        case 'G': p [index2 + 3] = (UINT_8) floor (((UINT_8P) igloss        .data) [index1    ]   * factorg);   break;
                        case 'H': p [index2 + 3] = (UINT_8) floor (((UINT_8P) igloss        .data) [index1 + 1]   * factorg);   break;
                        case 'I': p [index2 + 3] = (UINT_8) floor (((UINT_8P) igloss        .data) [index1 + 2]   * factorg);   break;
                        case 'J': p [index2 + 3] = (UINT_8) floor (((UINT_8P) ishininess    .data) [index1    ]   * factors);   break;
                        case 'K': p [index2 + 3] = (UINT_8) floor (((UINT_8P) ishininess    .data) [index1 + 1]   * factors);   break;
                        case 'L': p [index2 + 3] = (UINT_8) floor (((UINT_8P) ishininess    .data) [index1 + 2]   * factors);   break;
                    }
                }
            }
        }

        // save composite image
        if (saveImage (filename, &icomposite) < 0) {

            printf ("Unable to save composite file. \n");
            return -5;
        }

        free (icomposite.data);

        if (tdiffuse)    free (idiffuse  .data);
        if (tnormal)     free (inormal   .data);
        if (tgloss)      free (igloss    .data);
        if (tshininess)  free (ishininess.data);

        return 0;
    }

    DECLDIR UINT_32 _stdcall processGenerateCompositeAtlas (CHARP filename, CHARP translation, CHARPP diffuse, CHARPP normal, CHARPP gloss, CHARPP shininess, FLOAT_32P factord, FLOAT_32P factorn, FLOAT_32P factorg, FLOAT_32P factors, UINT_32 mode, UINT_32 count)
    {

        TImage icomposite;

        TImage * idiffuse   = (TImage *) malloc (sizeof (TImage) * count);
        TImage * inormal    = (TImage *) malloc (sizeof (TImage) * count);
        TImage * igloss     = (TImage *) malloc (sizeof (TImage) * count);
        TImage * ishininess = (TImage *) malloc (sizeof (TImage) * count);

        UINT_32 w = 0;
        UINT_32 h = 0;

        BOOL tdiffuse    = false;
        BOOL tnormal     = false;
        BOOL tgloss      = false;
        BOOL tshininess  = false;

        if ((translation [0] == 'A') || (translation [0] == 'B') || (translation [0] == 'C') ||
            (translation [1] == 'A') || (translation [1] == 'B') || (translation [1] == 'C') ||
            (translation [2] == 'A') || (translation [2] == 'B') || (translation [2] == 'C') ||
            (translation [3] == 'A') || (translation [3] == 'B') || (translation [3] == 'C')) {

            tdiffuse = true;

            for (UINT_32 n = 0; n < count; n ++) {

                printf ("Loading : %s", diffuse [n]);

                if (loadImage (diffuse [n], &idiffuse [n]) < 0) {

                    printf (" - FAILED - Supported is only uncompressed RGB 'tga' format. \n");
                    return -1;
                }

                if (idiffuse [n].type != M_IMAGE_TYPE_RGB) {

                    printf (" - FAILED - Supported is only uncompressed RGB 'tga' format. \n");
                    return -2;
                }

                if (w > 0) {

                    if ((w != idiffuse [n].width) || (h != idiffuse [n].height)) {

                        printf (" - FAILED - Inconsistent image dimensions. \n");
                        return -3;
                    }

                } else {

                    w = idiffuse [n].width;
                    h = idiffuse [n].height;
                }

                printf (" - SUCCEED\n");
            }
        }

        if ((translation [0] == 'D') || (translation [0] == 'E') || (translation [0] == 'F') ||
            (translation [1] == 'D') || (translation [1] == 'E') || (translation [1] == 'F') ||
            (translation [2] == 'D') || (translation [2] == 'E') || (translation [2] == 'F') ||
            (translation [3] == 'D') || (translation [3] == 'E') || (translation [3] == 'F')) {

            tnormal = true;

            for (UINT_32 n = 0; n < count; n ++) {

                printf ("Loading : %s", normal [n]);

                if (loadImage (normal [n], &inormal [n]) < 0) {

                    printf (" - FAILED - Supported is only uncompressed RGB 'tga' format. \n");
                    return -1;
                }

                if (inormal [n].type != M_IMAGE_TYPE_RGB) {

                    printf (" - FAILED - Supported is only uncompressed RGB 'tga' format. \n");
                    return -2;
                }

                if (w > 0) {

                    if ((w != inormal [n].width) || (h != inormal [n].height)) {

                        printf (" - FAILED - Inconsistent image dimensions. \n");
                        return -3;
                    }

                } else {

                    w = inormal [n].width;
                    h = inormal [n].height;
                }

                printf (" - SUCCEED\n");
            }
        }

        if ((translation [0] == 'G') || (translation [0] == 'H') || (translation [0] == 'I') ||
            (translation [1] == 'G') || (translation [1] == 'H') || (translation [1] == 'I') ||
            (translation [2] == 'G') || (translation [2] == 'H') || (translation [2] == 'I') ||
            (translation [3] == 'G') || (translation [3] == 'H') || (translation [3] == 'I')) {

            tgloss = true;

            for (UINT_32 n = 0; n < count; n ++) {

                printf ("Loading : %s", gloss [n]);

                if (loadImage (gloss [n], &igloss [n]) < 0) {

                    printf (" - FAILED - Supported is only uncompressed RGB 'tga' format. \n");
                    return -1;
                }

                if (igloss [n].type != M_IMAGE_TYPE_RGB) {

                    printf (" - FAILED - Supported is only uncompressed RGB 'tga' format. \n");
                    return -2;
                }

                if (w > 0) {

                    if ((w != igloss [n].width) || (h != igloss [n].height)) {

                        printf (" - FAILED - Inconsistent image dimensions. \n");
                        return -3;
                    }

                } else {

                    w = igloss [n].width;
                    h = igloss [n].height;
                }

                printf (" - SUCCEED\n");
            }
        }

        if ((translation [0] == 'J') || (translation [0] == 'K') || (translation [0] == 'L') ||
            (translation [1] == 'J') || (translation [1] == 'K') || (translation [1] == 'L') ||
            (translation [2] == 'J') || (translation [2] == 'K') || (translation [2] == 'L') ||
            (translation [3] == 'J') || (translation [3] == 'K') || (translation [3] == 'L')) {

            tshininess = true;

            for (UINT_32 n = 0; n < count; n ++) {

                printf ("Loading : %s", shininess [n]);

                if (loadImage (shininess [n], &ishininess [n]) < 0) {

                    printf (" - FAILED - Supported is only uncompressed RGB 'tga' format. \n");
                    return -1;
                }

                if (ishininess [n].type != M_IMAGE_TYPE_RGB) {

                    printf (" - FAILED - Supported is only uncompressed RGB 'tga' format. \n");
                    return -2;
                }

                if (w > 0) {

                    if ((w != ishininess [n].width) || (h != ishininess [n].height)) {

                        printf (" - FAILED - Inconsistent image dimensions. \n");
                        return -3;
                    }

                } else {

                    w = ishininess [n].width;
                    h = ishininess [n].height;
                }

                printf (" - SUCCEED\n");
            }
        }

        UINT_32 ww = w * mode;
        UINT_32 hh = h * mode;

        icomposite.width  = ww;
        icomposite.height = hh;

        // using alpha ?
        if (translation [3] == 'X') {

            icomposite.data   = (UINT_8P) calloc (ww * hh * 3, SIZE_UINT_8);   UINT_8P p = (UINT_8P) icomposite.data;
            icomposite.type   = M_IMAGE_TYPE_RGB;

            for (UINT_32 yy = 0; yy < hh; yy ++) {
                for (UINT_32 xx = 0; xx < ww; xx ++) {
                
                    UINT_32 x = xx % w;
                    UINT_32 y = yy % h;

                    UINT_32 indexi = ((y  *  w) +  x) * 3;
                    UINT_32 indexo = ((yy * ww) + xx) * 3;

                    UINT_32 indext = (yy / h)*mode + (xx / w);

                    if (indext < count) {

                        switch (translation [0]) {
                            case 'A': p [indexo    ] = (UINT_8) floor (((UINT_8P) idiffuse      [indext].data) [indexi    ]   * factord [indext]);   break;
                            case 'B': p [indexo    ] = (UINT_8) floor (((UINT_8P) idiffuse      [indext].data) [indexi + 1]   * factord [indext]);   break;
                            case 'C': p [indexo    ] = (UINT_8) floor (((UINT_8P) idiffuse      [indext].data) [indexi + 2]   * factord [indext]);   break;
                            case 'D': p [indexo    ] = (UINT_8) floor (((UINT_8P) inormal       [indext].data) [indexi    ]   * factorn [indext]);   break;
                            case 'E': p [indexo    ] = (UINT_8) floor (((UINT_8P) inormal       [indext].data) [indexi + 1]   * factorn [indext]);   break;
                            case 'F': p [indexo    ] = (UINT_8) floor (((UINT_8P) inormal       [indext].data) [indexi + 2]   * factorn [indext]);   break;
                            case 'G': p [indexo    ] = (UINT_8) floor (((UINT_8P) igloss        [indext].data) [indexi    ]   * factorg [indext]);   break;
                            case 'H': p [indexo    ] = (UINT_8) floor (((UINT_8P) igloss        [indext].data) [indexi + 1]   * factorg [indext]);   break;
                            case 'I': p [indexo    ] = (UINT_8) floor (((UINT_8P) igloss        [indext].data) [indexi + 2]   * factorg [indext]);   break;
                            case 'J': p [indexo    ] = (UINT_8) floor (((UINT_8P) ishininess    [indext].data) [indexi    ]   * factors [indext]);   break;
                            case 'K': p [indexo    ] = (UINT_8) floor (((UINT_8P) ishininess    [indext].data) [indexi + 1]   * factors [indext]);   break;
                            case 'L': p [indexo    ] = (UINT_8) floor (((UINT_8P) ishininess    [indext].data) [indexi + 2]   * factors [indext]);   break;
                        }

                        switch (translation [1]) {
                            case 'A': p [indexo + 1] = (UINT_8) floor (((UINT_8P) idiffuse      [indext].data) [indexi    ]   * factord [indext]);   break;
                            case 'B': p [indexo + 1] = (UINT_8) floor (((UINT_8P) idiffuse      [indext].data) [indexi + 1]   * factord [indext]);   break;
                            case 'C': p [indexo + 1] = (UINT_8) floor (((UINT_8P) idiffuse      [indext].data) [indexi + 2]   * factord [indext]);   break;
                            case 'D': p [indexo + 1] = (UINT_8) floor (((UINT_8P) inormal       [indext].data) [indexi    ]   * factorn [indext]);   break;
                            case 'E': p [indexo + 1] = (UINT_8) floor (((UINT_8P) inormal       [indext].data) [indexi + 1]   * factorn [indext]);   break;
                            case 'F': p [indexo + 1] = (UINT_8) floor (((UINT_8P) inormal       [indext].data) [indexi + 2]   * factorn [indext]);   break;
                            case 'G': p [indexo + 1] = (UINT_8) floor (((UINT_8P) igloss        [indext].data) [indexi    ]   * factorg [indext]);   break;
                            case 'H': p [indexo + 1] = (UINT_8) floor (((UINT_8P) igloss        [indext].data) [indexi + 1]   * factorg [indext]);   break;
                            case 'I': p [indexo + 1] = (UINT_8) floor (((UINT_8P) igloss        [indext].data) [indexi + 2]   * factorg [indext]);   break;
                            case 'J': p [indexo + 1] = (UINT_8) floor (((UINT_8P) ishininess    [indext].data) [indexi    ]   * factors [indext]);   break;
                            case 'K': p [indexo + 1] = (UINT_8) floor (((UINT_8P) ishininess    [indext].data) [indexi + 1]   * factors [indext]);   break;
                            case 'L': p [indexo + 1] = (UINT_8) floor (((UINT_8P) ishininess    [indext].data) [indexi + 2]   * factors [indext]);   break;
                        }

                        switch (translation [2]) {
                            case 'A': p [indexo + 2] = (UINT_8) floor (((UINT_8P) idiffuse      [indext].data) [indexi    ]   * factord [indext]);   break;
                            case 'B': p [indexo + 2] = (UINT_8) floor (((UINT_8P) idiffuse      [indext].data) [indexi + 1]   * factord [indext]);   break;
                            case 'C': p [indexo + 2] = (UINT_8) floor (((UINT_8P) idiffuse      [indext].data) [indexi + 2]   * factord [indext]);   break;
                            case 'D': p [indexo + 2] = (UINT_8) floor (((UINT_8P) inormal       [indext].data) [indexi    ]   * factorn [indext]);   break;
                            case 'E': p [indexo + 2] = (UINT_8) floor (((UINT_8P) inormal       [indext].data) [indexi + 1]   * factorn [indext]);   break;
                            case 'F': p [indexo + 2] = (UINT_8) floor (((UINT_8P) inormal       [indext].data) [indexi + 2]   * factorn [indext]);   break;
                            case 'G': p [indexo + 2] = (UINT_8) floor (((UINT_8P) igloss        [indext].data) [indexi    ]   * factorg [indext]);   break;
                            case 'H': p [indexo + 2] = (UINT_8) floor (((UINT_8P) igloss        [indext].data) [indexi + 1]   * factorg [indext]);   break;
                            case 'I': p [indexo + 2] = (UINT_8) floor (((UINT_8P) igloss        [indext].data) [indexi + 2]   * factorg [indext]);   break;
                            case 'J': p [indexo + 2] = (UINT_8) floor (((UINT_8P) ishininess    [indext].data) [indexi    ]   * factors [indext]);   break;
                            case 'K': p [indexo + 2] = (UINT_8) floor (((UINT_8P) ishininess    [indext].data) [indexi + 1]   * factors [indext]);   break;
                            case 'L': p [indexo + 2] = (UINT_8) floor (((UINT_8P) ishininess    [indext].data) [indexi + 2]   * factors [indext]);   break;
                        }

                    } else {

                        p [indexo    ] = 0;
                        p [indexo + 1] = 0;
                        p [indexo + 2] = 0;
                        p [indexo + 3] = 0;
                    }
                }
            }

        } else {

            icomposite.data   = (UINT_8P) calloc (ww * hh * 4, SIZE_UINT_8);   UINT_8P p = (UINT_8P) icomposite.data;
            icomposite.type   = M_IMAGE_TYPE_RGBA;

            for (UINT_32 yy = 0; yy < hh; yy ++) {
                for (UINT_32 xx = 0; xx < ww; xx ++) {
                
                    UINT_32 x = xx % w;
                    UINT_32 y = yy % h;

                    UINT_32 indexi = ((y  *  w) +  x) * 3;
                    UINT_32 indexo = ((yy * ww) + xx) * 4;

                    UINT_32 indext = (yy / h)*mode + (xx / w);

                    if (indext < count) {

                        switch (translation [0]) {
                            case 'A': p [indexo    ] = (UINT_8) floor (((UINT_8P) idiffuse      [indext].data) [indexi    ]   * factord [indext]);   break;
                            case 'B': p [indexo    ] = (UINT_8) floor (((UINT_8P) idiffuse      [indext].data) [indexi + 1]   * factord [indext]);   break;
                            case 'C': p [indexo    ] = (UINT_8) floor (((UINT_8P) idiffuse      [indext].data) [indexi + 2]   * factord [indext]);   break;
                            case 'D': p [indexo    ] = (UINT_8) floor (((UINT_8P) inormal       [indext].data) [indexi    ]   * factorn [indext]);   break;
                            case 'E': p [indexo    ] = (UINT_8) floor (((UINT_8P) inormal       [indext].data) [indexi + 1]   * factorn [indext]);   break;
                            case 'F': p [indexo    ] = (UINT_8) floor (((UINT_8P) inormal       [indext].data) [indexi + 2]   * factorn [indext]);   break;
                            case 'G': p [indexo    ] = (UINT_8) floor (((UINT_8P) igloss        [indext].data) [indexi    ]   * factorg [indext]);   break;
                            case 'H': p [indexo    ] = (UINT_8) floor (((UINT_8P) igloss        [indext].data) [indexi + 1]   * factorg [indext]);   break;
                            case 'I': p [indexo    ] = (UINT_8) floor (((UINT_8P) igloss        [indext].data) [indexi + 2]   * factorg [indext]);   break;
                            case 'J': p [indexo    ] = (UINT_8) floor (((UINT_8P) ishininess    [indext].data) [indexi    ]   * factors [indext]);   break;
                            case 'K': p [indexo    ] = (UINT_8) floor (((UINT_8P) ishininess    [indext].data) [indexi + 1]   * factors [indext]);   break;
                            case 'L': p [indexo    ] = (UINT_8) floor (((UINT_8P) ishininess    [indext].data) [indexi + 2]   * factors [indext]);   break;
                        }

                        switch (translation [1]) {
                            case 'A': p [indexo + 1] = (UINT_8) floor (((UINT_8P) idiffuse      [indext].data) [indexi    ]   * factord [indext]);   break;
                            case 'B': p [indexo + 1] = (UINT_8) floor (((UINT_8P) idiffuse      [indext].data) [indexi + 1]   * factord [indext]);   break;
                            case 'C': p [indexo + 1] = (UINT_8) floor (((UINT_8P) idiffuse      [indext].data) [indexi + 2]   * factord [indext]);   break;
                            case 'D': p [indexo + 1] = (UINT_8) floor (((UINT_8P) inormal       [indext].data) [indexi    ]   * factorn [indext]);   break;
                            case 'E': p [indexo + 1] = (UINT_8) floor (((UINT_8P) inormal       [indext].data) [indexi + 1]   * factorn [indext]);   break;
                            case 'F': p [indexo + 1] = (UINT_8) floor (((UINT_8P) inormal       [indext].data) [indexi + 2]   * factorn [indext]);   break;
                            case 'G': p [indexo + 1] = (UINT_8) floor (((UINT_8P) igloss        [indext].data) [indexi    ]   * factorg [indext]);   break;
                            case 'H': p [indexo + 1] = (UINT_8) floor (((UINT_8P) igloss        [indext].data) [indexi + 1]   * factorg [indext]);   break;
                            case 'I': p [indexo + 1] = (UINT_8) floor (((UINT_8P) igloss        [indext].data) [indexi + 2]   * factorg [indext]);   break;
                            case 'J': p [indexo + 1] = (UINT_8) floor (((UINT_8P) ishininess    [indext].data) [indexi    ]   * factors [indext]);   break;
                            case 'K': p [indexo + 1] = (UINT_8) floor (((UINT_8P) ishininess    [indext].data) [indexi + 1]   * factors [indext]);   break;
                            case 'L': p [indexo + 1] = (UINT_8) floor (((UINT_8P) ishininess    [indext].data) [indexi + 2]   * factors [indext]);   break;
                        }

                        switch (translation [2]) {
                            case 'A': p [indexo + 2] = (UINT_8) floor (((UINT_8P) idiffuse      [indext].data) [indexi    ]   * factord [indext]);   break;
                            case 'B': p [indexo + 2] = (UINT_8) floor (((UINT_8P) idiffuse      [indext].data) [indexi + 1]   * factord [indext]);   break;
                            case 'C': p [indexo + 2] = (UINT_8) floor (((UINT_8P) idiffuse      [indext].data) [indexi + 2]   * factord [indext]);   break;
                            case 'D': p [indexo + 2] = (UINT_8) floor (((UINT_8P) inormal       [indext].data) [indexi    ]   * factorn [indext]);   break;
                            case 'E': p [indexo + 2] = (UINT_8) floor (((UINT_8P) inormal       [indext].data) [indexi + 1]   * factorn [indext]);   break;
                            case 'F': p [indexo + 2] = (UINT_8) floor (((UINT_8P) inormal       [indext].data) [indexi + 2]   * factorn [indext]);   break;
                            case 'G': p [indexo + 2] = (UINT_8) floor (((UINT_8P) igloss        [indext].data) [indexi    ]   * factorg [indext]);   break;
                            case 'H': p [indexo + 2] = (UINT_8) floor (((UINT_8P) igloss        [indext].data) [indexi + 1]   * factorg [indext]);   break;
                            case 'I': p [indexo + 2] = (UINT_8) floor (((UINT_8P) igloss        [indext].data) [indexi + 2]   * factorg [indext]);   break;
                            case 'J': p [indexo + 2] = (UINT_8) floor (((UINT_8P) ishininess    [indext].data) [indexi    ]   * factors [indext]);   break;
                            case 'K': p [indexo + 2] = (UINT_8) floor (((UINT_8P) ishininess    [indext].data) [indexi + 1]   * factors [indext]);   break;
                            case 'L': p [indexo + 2] = (UINT_8) floor (((UINT_8P) ishininess    [indext].data) [indexi + 2]   * factors [indext]);   break;
                        }

                        switch (translation [3]) {
                            case 'A': p [indexo + 3] = (UINT_8) floor (((UINT_8P) idiffuse      [indext].data) [indexi    ]   * factord [indext]);   break;
                            case 'B': p [indexo + 3] = (UINT_8) floor (((UINT_8P) idiffuse      [indext].data) [indexi + 1]   * factord [indext]);   break;
                            case 'C': p [indexo + 3] = (UINT_8) floor (((UINT_8P) idiffuse      [indext].data) [indexi + 2]   * factord [indext]);   break;
                            case 'D': p [indexo + 3] = (UINT_8) floor (((UINT_8P) inormal       [indext].data) [indexi    ]   * factorn [indext]);   break;
                            case 'E': p [indexo + 3] = (UINT_8) floor (((UINT_8P) inormal       [indext].data) [indexi + 1]   * factorn [indext]);   break;
                            case 'F': p [indexo + 3] = (UINT_8) floor (((UINT_8P) inormal       [indext].data) [indexi + 2]   * factorn [indext]);   break;
                            case 'G': p [indexo + 3] = (UINT_8) floor (((UINT_8P) igloss        [indext].data) [indexi    ]   * factorg [indext]);   break;
                            case 'H': p [indexo + 3] = (UINT_8) floor (((UINT_8P) igloss        [indext].data) [indexi + 1]   * factorg [indext]);   break;
                            case 'I': p [indexo + 3] = (UINT_8) floor (((UINT_8P) igloss        [indext].data) [indexi + 2]   * factorg [indext]);   break;
                            case 'J': p [indexo + 3] = (UINT_8) floor (((UINT_8P) ishininess    [indext].data) [indexi    ]   * factors [indext]);   break;
                            case 'K': p [indexo + 3] = (UINT_8) floor (((UINT_8P) ishininess    [indext].data) [indexi + 1]   * factors [indext]);   break;
                            case 'L': p [indexo + 3] = (UINT_8) floor (((UINT_8P) ishininess    [indext].data) [indexi + 2]   * factors [indext]);   break;
                        }
                        
                    } else {

                        p [indexo    ] = 0;
                        p [indexo + 1] = 0;
                        p [indexo + 2] = 0;
                        p [indexo + 3] = 0;
                    }
                }
            }

        }

        // save composite image
        if (saveImage (filename, &icomposite) < 0) {

            printf ("Unable to save composite file. \n");
            return -5;
        }

        for (UINT_32 n = 0; n < count; n ++) {

            if (tdiffuse)       free (idiffuse   [n].data);
            if (tnormal)        free (inormal    [n].data);
            if (tgloss)         free (igloss     [n].data);
            if (tshininess)     free (ishininess [n].data);
        }

        free (icomposite.data);

        return 0;
    }


















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