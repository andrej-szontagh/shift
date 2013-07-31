
///////////////////////////////////////////////////////////////////////////////
// lo_LoadImage
///////////////////////////////////////////////////////////////////////////////

#if 1

INT_32 lo_LoadImage (CHARP filename, TImage *image)
{

	INT_32 l = (INT_32) strlen (filename), r = - 1;

    // TGA

	if	((l > 4) &&
		 (filename [l - 4] == '.') && 
		 (filename [l - 3] == 't') && 
		 (filename [l - 2] == 'g') &&
		 (filename [l - 1] == 'a')) {

        FILE *file;

        UINT_16 w, h, depth;

        UINT_32 size;

        if (!(file = fopen (filename, "rb")))   return -1;

        UINT_8  data8;

        // Numer of bytes for image identification
        fseek (file,  0, SEEK_SET);
        fread (&data8,  SIZE_UINT_8, 1, file);      if (data8  != 0)    { fclose (file); return -2; }       // 0 - no image identification field

        // Color Map Type
        fseek (file,  1, SEEK_SET);
        fread (&data8,  SIZE_UINT_8, 1, file);      if (data8  != 0)    { fclose (file); return -2; }       // 0 - no color map

        // Image Type Code
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


        // Width of Image
        fseek (file, 12, SEEK_SET);
        fread (&w,      SIZE_UINT_16, 1, file);

        // Height of Image
        fseek (file, 14, SEEK_SET);
        fread (&h,      SIZE_UINT_16, 1, file);

        // Image Pixel Size
        fseek (file, 16, SEEK_SET);
        fread (&data8,  SIZE_UINT_8,  1, file);

        // Depth of pixel data
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

        // Seeking data
        fseek (file, 18, SEEK_SET);

        image->width    = (UINT_32) w;
        image->height   = (UINT_32) h;

        image->mipmaps  = 0;

        image->data = malloc (size = w * h * depth);

        // Reading data
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

    // DDS

	if	((l > 4) &&
		 (filename [l - 4] == '.') && 
		 (filename [l - 3] == 'd') && 
		 (filename [l - 2] == 'd') &&
		 (filename [l - 1] == 's')) {

        FILE * file;

        if (!(file = fopen (filename, "rb")))   return -1;

        CHAR filecode [4];

        // Verify the file is a true .dds file
        fread (filecode, 1, 4, file);

        if (strncmp (filecode, "DDS ", 4) != 0) return -2;

        UINT_32 code;
        UINT_32 size;
        UINT_32 sizel;

        // size of descriptor
        fread (&size, 4, 1, file);

        // dimensions
        fseek (file, 4, SEEK_CUR);

        fread (&image->width,  4, 1, file);
        fread (&image->height, 4, 1, file);

        // linear size
        fread (&sizel, 4, 1, file);

        // valid size ?
        if (sizel == 0) return -4;

        // mipmaps
        fseek (file, 4, SEEK_CUR);
        fread (&image->mipmaps, 4, 1, file);

        // compression code
        fseek (file, 52, SEEK_CUR);
        fread (&code, 4, 1, file);

        UINT_32 factor;

        #define FOURCC_DXT1 827611204
        #define FOURCC_DXT3 861165636
        #define FOURCC_DXT5 894720068

        switch (code) {

            // DXT1's compression ratio is 8:1
            case FOURCC_DXT1:
                image->type = M_IMAGE_TYPE_DXT1;
                factor = 2;
                break;

            // DXT3's compression ratio is 4:1
            case FOURCC_DXT3:
                image->type = M_IMAGE_TYPE_DXT3;
                factor = 4;
                break;

            // DXT5's compression ratio is 4:1
            case FOURCC_DXT5:
                image->type = M_IMAGE_TYPE_DXT5;
                factor = 4;
                break;

            default:

                return -3;
        }

        if (image->mipmaps > 1)    sizel *= factor;

        image->data = malloc (sizel * SIZE_UINT_8);

        // data
        fseek (file, size + 4, SEEK_SET);
        fread (image->data, 1, sizel, file);
        
        /// VERTICALY FLIP IMAGE AFTER DDS LOAD (http://users.telenet.be/tfautre/softdev/ddsload/explanation.htm)

        return 0;
	}

    return -10;
}

#endif
