
///////////////////////////////////////////////////////////////////////////////
// lo_LoadWorld
///////////////////////////////////////////////////////////////////////////////

#if 1

INT_32 lo_LoadWorld (CHARP filename)
{
    INT_32 i, j;

    CHAR fname [255];   UINT_32 length;

    // open file
    FILE * f = fopen (filename, "rb");

    // file not fount
    if (f == NULL) {

        debug_Print ("ERROR : Cannot open file : ", filename, true);  
        return -1;
    }

    // counters

    UINT_32 texturesc;
    UINT_32 instancedc;

    fread (&dr_objectsc,    SIZE_UINT_32, 1, f);
    fread (&dr_modelsc,     SIZE_UINT_32, 1, f);
    fread (&dr_materialsc,  SIZE_UINT_32, 1, f);
    fread (&texturesc,      SIZE_UINT_32, 1, f);
    fread (&instancedc,     SIZE_UINT_32, 1, f);

    // WORLD PARAMS

    fread ((VOIDP) &dr_planenear, SIZE_FLOAT_32, 1, f);
    fread ((VOIDP) &dr_planefar,  SIZE_FLOAT_32, 1, f);

    // LIST OF TEXTURE REFERENCES

    UINT_32P list_textures = (UINT_32P) malloc (sizeof (VOIDP) * texturesc);

    // ---------------------------------------------------------------------------------------
    // TEXTURES

    debug_Print ("Loading Textures ..\n");  

    for (i = 0; i < (INT_32) texturesc; i ++) {

        TImage image;

        CHARP name;

        // image
        fread ((VOIDP) &length, SIZE_UINT_32, 1, f);    name = (CHARP) malloc (SIZE_CHAR * length);
        fread ((VOIDP) name, SIZE_CHAR, length, f);

		sprintf (fname, "%s%s", M_LOADER_IMAGEPATH, name);

		// log
        debug_PrintIntend ("Loading ", fname);  

        if (lo_LoadImage (fname, &image) < 0) {

            debug_Print (" .. ERROR");  fclose (f);
            return - 1;
        }

        free (name);

        // texture properties
		UINT_32 gamma;	fread ((VOIDP) &gamma,	SIZE_UINT_32,	1, f);
        UINT_32 wraps;  fread ((VOIDP) &wraps,	SIZE_UINT_32,   1, f);
        UINT_32 wrapt;	fread ((VOIDP) &wrapt,	SIZE_UINT_32,   1, f);

        switch (wraps) {
            case 0: wraps = GL_REPEAT;          break;
            case 1: wraps = GL_CLAMP;           break;
            case 2: wraps = GL_CLAMP_TO_EDGE;   break;
        }
        switch (wrapt) {
            case 0: wrapt = GL_REPEAT;          break;
            case 1: wrapt = GL_CLAMP;           break;
            case 2: wrapt = GL_CLAMP_TO_EDGE;   break;
        }

        BOOL pow = false;

        // is power of 2 ?
        if (image.width == image.height) {

            switch (image.width) {

                case 16:
                case 32:
                case 64:
                case 128:
                case 256:
                case 512:
                case 1024:
                case 2048:
                case 4096:  pow = true;
            }
        }

        UINT_32 id;

        // Is this texture power of two ?
        if (pow) {

            glPixelStorei (GL_PACK_ALIGNMENT, 1);

            glGenTextures    (1, &id);
            glBindTexture    (GL_TEXTURE_2D, id);

            glTexParameteri  (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wraps);
            glTexParameteri  (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapt);

            glTexParameteri  (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            glTexParameteri  (GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, dr_control_anisotrophy);

            glTexParameteri  (GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
            glTexParameteri  (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

            BOOL compressed = false;

            UINT_32 format;
            UINT_32 blocksize;

			// gamma correction
			if (dr_control_gammacorrection && GLEE_EXT_texture_sRGB && GLEE_EXT_framebuffer_sRGB && gamma) {

				switch (image.type) {
					case M_IMAGE_TYPE_RGB:      glTexImage2D (GL_TEXTURE_2D, 0, GL_SRGB8_EXT,			image.width, image.height, 0, GL_RGB,   GL_UNSIGNED_BYTE, image.data);	break;
					case M_IMAGE_TYPE_RGBA:     glTexImage2D (GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8_EXT,	image.width, image.height, 0, GL_RGBA,  GL_UNSIGNED_BYTE, image.data);	break;

					case M_IMAGE_TYPE_DXT1:     compressed = true;  format = GL_COMPRESSED_SRGB_S3TC_DXT1_EXT;			blocksize = 8;  break;
					case M_IMAGE_TYPE_DXT3:     compressed = true;  format = GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT;	blocksize = 16; break;
					case M_IMAGE_TYPE_DXT5:     compressed = true;  format = GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT;	blocksize = 16; break;
				}

			} else {

				switch (image.type) {
					case M_IMAGE_TYPE_RGB:      glTexImage2D (GL_TEXTURE_2D, 0, GL_RGB8,   image.width, image.height, 0, GL_RGB,   GL_UNSIGNED_BYTE, image.data);	break;
					case M_IMAGE_TYPE_RGBA:     glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA8,  image.width, image.height, 0, GL_RGBA,  GL_UNSIGNED_BYTE, image.data);	break;

					case M_IMAGE_TYPE_DXT1:     compressed = true;  format = GL_COMPRESSED_RGB_S3TC_DXT1_EXT;   blocksize = 8;  break;
					case M_IMAGE_TYPE_DXT3:     compressed = true;  format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;  blocksize = 16; break;
					case M_IMAGE_TYPE_DXT5:     compressed = true;  format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;  blocksize = 16; break;
				}
			}

            if (compressed) {

				if (image.mipmaps <= 1) {

                    debug_Print (" .. ERROR - missing mipmaps in dds.");  fclose (f);
		            return - 1;
				}

                glTexParameteri  (GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE);

                UINT_32 w = image.width;
                UINT_32 h = image.height;

                UINT_32 offset = 0;

                for (j = 0; j < image.mipmaps; j ++) {

                    if (w == 0) w = 1;
                    if (h == 0) h = 1;

                    UINT_32 size = ((w + 3) / 4) * ((h + 3) / 4) * blocksize;

                    glCompressedTexImage2DARB (GL_TEXTURE_2D, j, format, w, h, 0, size, (VOIDP) ((UINT_8P) image.data + offset));

                    offset += size;

                    // Half the image size for the next mip-map level...
                    w /= 2;     h /= 2;
                }
            }

        // Using rectangular texture
        } else {

            glPixelStorei (GL_PACK_ALIGNMENT, 1);

            glGenTextures    (1, &id);
            glBindTexture    (GL_TEXTURE_RECTANGLE_ARB, id);

            //glTexParameteri  (GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, wraps);
            //glTexParameteri  (GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, wrapt);

            glTexParameteri  (GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri  (GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			// gamma correction
			if (dr_control_gammacorrection && GLEE_EXT_texture_sRGB && GLEE_EXT_framebuffer_sRGB && gamma)

				glTexImage2D     (GL_TEXTURE_RECTANGLE_ARB, 0, GL_SRGB8_EXT, image.width, image.height, 0, GL_RGB, GL_UNSIGNED_BYTE, image.data);	else
				glTexImage2D     (GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGB8,		 image.width, image.height, 0, GL_RGB, GL_UNSIGNED_BYTE, image.data);
        }

        // removing image data
        free (image.data);

        // saving texture id
        list_textures [i] = id;

		// log
        debug_Print (" .. OK\n");

		// check OpenGL error
		GLenum err = glGetError ();	if (err != GL_NO_ERROR) {	
			
            debug_Print ("ERROR OpenGL code : ", err, true);    return -1;
		};
    }

    // ---------------------------------------------------------------------------------------
    // MATERIALS

    debug_Print ("Loading Materials ..\n");

    dr_materials    = (TMaterial *) malloc (sizeof (TMaterial) * (dr_materialsc + M_RESERVED_MATERIALS));

    // material properites

    dr_material_counters    = (UINT_16P) calloc ((dr_materialsc + M_RESERVED_MATERIALS), SIZE_UINT_16);
    dr_material_stamps      = (UINT_32P) calloc ((dr_materialsc + M_RESERVED_MATERIALS), SIZE_UINT_32);

    for (i = 0; i < (INT_32) dr_materialsc; i ++) {

        CHARP name;

        // name
        fread ((VOIDP) &length, SIZE_UINT_32, 1, f);    name = (CHARP) malloc (SIZE_CHAR * length);
        fread ((VOIDP) name, SIZE_CHAR, length, f);

        debug_PrintIntend ("Loading ", name);

        UINT_32 type;

        // type 
        fread ((VOIDP) &type, SIZE_UINT_32,   1, f);

        // shortcut
        TMaterial * material = &dr_materials [i];

		// init
		material->diffuse   = 0;
        material->weights1  = 0;
        material->weights2  = 0;
        material->weights3  = 0;
        material->weights4  = 0;
        material->composite = 0;

        UINT_32 tex;

        switch (type) {

            case M_MATERIAL_TYPE_SOLID: {
                    fread ((VOIDP) &tex,                    SIZE_UINT_32,   1, f); material->diffuse   = list_textures [tex];
                    fread ((VOIDP) &tex,                    SIZE_UINT_32,   1, f); material->composite = list_textures [tex];
                    break;
                }
            case M_MATERIAL_TYPE_TERRAIN: {
                    fread ((VOIDP) &tex,					SIZE_UINT_32,   1, f); material->diffuse   = list_textures [tex];
                    fread ((VOIDP) &tex,                    SIZE_UINT_32,   1, f); material->composite = list_textures [tex];
                    fread ((VOIDP) &tex,                    SIZE_UINT_32,   1, f); material->weights1  = list_textures [tex];
                    fread ((VOIDP) &tex,                    SIZE_UINT_32,   1, f); material->weights2  = list_textures [tex];
                    fread ((VOIDP) &tex,                    SIZE_UINT_32,   1, f); material->weights3  = list_textures [tex];
                    fread ((VOIDP) &tex,                    SIZE_UINT_32,   1, f); material->weights4  = list_textures [tex];
                    break;
                }
            case M_MATERIAL_TYPE_ENVIROMENT: {

                    fread ((VOIDP) &tex, SIZE_UINT_32,   1, f); material->diffuse = list_textures [tex];

                    glGetError ();

                    glGetTexLevelParameteriv (GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH,  (GLint *) &material->width);
                    glGetTexLevelParameteriv (GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, (GLint *) &material->height);

                    if (glGetError != GL_NO_ERROR) {

                        glGetTexLevelParameteriv (GL_TEXTURE_RECTANGLE_ARB, 0, GL_TEXTURE_WIDTH,  (GLint *) &material->width);
                        glGetTexLevelParameteriv (GL_TEXTURE_RECTANGLE_ARB, 0, GL_TEXTURE_HEIGHT, (GLint *) &material->height);
                    }
                    break;
                }
            case M_MATERIAL_TYPE_GRASS: {
                    fread ((VOIDP) &tex,					SIZE_UINT_32,   1, f); material->diffuse   = list_textures [tex];
					fread ((VOIDP) &material->damping,		SIZE_FLOAT_32,  1, f);
					fread ((VOIDP) &material->threshold,	SIZE_FLOAT_32,  1, f);
                    break;
                }
            case M_MATERIAL_TYPE_FOLIAGE: {
                    fread ((VOIDP) &tex,					SIZE_UINT_32,   1, f); material->diffuse   = list_textures [tex];
                    fread ((VOIDP) &tex,					SIZE_UINT_32,   1, f); material->composite = list_textures [tex];
                    break;
                }
        }

        material->name = name;
        material->type = type;

		// log
        debug_Print (" .. OK\n");

		// check OpenGL error
		GLenum err = glGetError ();	if (err != GL_NO_ERROR) {	
			
            debug_Print ("ERROR OpenGL code : ", err, true);    return -1;
		};
    }

    // release
    free (list_textures);

    // ---------------------------------------------------------------------------------------
    // MODELS

    debug_Print ("Loading Models ..\n");

    dr_models   = (TModel *) malloc (sizeof (TModel) * (dr_modelsc + M_RESERVED_MODELS));

    // model properites

    dr_model_occlusions     = (UINT_16P) malloc ((dr_modelsc + M_RESERVED_MODELS) * SIZE_UINT_16);
    dr_model_shaders        = (UINT_8P)  malloc ((dr_modelsc + M_RESERVED_MODELS) * SIZE_UINT_8);
    dr_model_stamps         = (UINT_32P) calloc ((dr_modelsc + M_RESERVED_MODELS),  SIZE_UINT_32);
    dr_model_flags          = (UINT_16P) malloc ((dr_modelsc + M_RESERVED_MODELS) * SIZE_UINT_16);

    // load meshes
    for (i = 0; i < (INT_32) dr_modelsc; i ++) {

        CHARP name;

        // name
        fread ((VOIDP) &length, SIZE_UINT_32, 1, f);    name = (CHARP) malloc (SIZE_CHAR * length);
        fread ((VOIDP) name, SIZE_CHAR, length, f);

        // file & path
        CHAR filepath [255];

        sprintf (filepath, "%s%s.mesh", M_LOADER_MESHPATH, name);

		// log
        debug_PrintIntend ("Loading ", filepath);

        // shortcut
        TModel * model = &dr_models [i];

        // occlusion model
        fread ((VOIDP) &dr_model_occlusions [i], SIZE_UINT_16, 1, f);

        // params
        fread ((VOIDP) &model->param1, SIZE_FLOAT_32, 1, f);
        fread ((VOIDP) &model->param2, SIZE_FLOAT_32, 1, f);
        fread ((VOIDP) &model->param3, SIZE_FLOAT_32, 1, f);

        // shader
        fread ((VOIDP) &dr_model_shaders    [i], SIZE_UINT_8,  1, f);

        // flags
        fread ((VOIDP) &dr_model_flags      [i], SIZE_UINT_16, 1, f);

        // open mesh file
        FILE * fm = fopen (filepath, "rb");     
        
        if (fm == NULL) {

            debug_Print (" .. ERROR");  fclose (f);
            return -1;
        }

        // primitive mode
        fread ((VOIDP) &model->mode, SIZE_UINT_32, 1, fm);

        switch (model->mode) {
            case 0: model->mode = GL_TRIANGLE_STRIP;    break;
            case 1: model->mode = GL_TRIANGLES;         break;
            case 2: model->mode = GL_QUADS;             break;
        }

        UINT_32 cindices;   fread ((VOIDP) &cindices,     SIZE_UINT_32,   1, fm);
        UINT_32 cvertices;  fread ((VOIDP) &cvertices,    SIZE_UINT_32,   1, fm);
        UINT_32 cpolygons;  fread ((VOIDP) &cpolygons,    SIZE_UINT_32,   1, fm);

        model->count = cindices;

        // boundary

        fread ((VOIDP) &model->boxmin [0],  SIZE_FLOAT_32,  1, fm);
        fread ((VOIDP) &model->boxmin [1],  SIZE_FLOAT_32,  1, fm);
        fread ((VOIDP) &model->boxmin [2],  SIZE_FLOAT_32,  1, fm);

        fread ((VOIDP) &model->boxmax [0],  SIZE_FLOAT_32,  1, fm);
        fread ((VOIDP) &model->boxmax [1],  SIZE_FLOAT_32,  1, fm);
        fread ((VOIDP) &model->boxmax [2],  SIZE_FLOAT_32,  1, fm);

        FLOAT_32 v [3]; vSUB3 (v, model->boxmin, model->boxmax)

        model->sphere = vLENGTH3 (v);

        UINT_32 presence, size;

        // use display list
        if (dr_model_flags [i] & M_FLAG_MODEL_LIST) {

            VOIDP indices   = NULL;
            VOIDP vertices  = NULL;
            VOIDP tangents  = NULL;
            VOIDP normals   = NULL;
            VOIDP colors    = NULL;
            VOIDP uv1       = NULL;
            VOIDP uv2       = NULL;
            VOIDP uv3       = NULL;

            // push states

            glPushClientAttrib (GL_CLIENT_VERTEX_ARRAY_BIT);

            // reset bindings

            glBindBufferARB (GL_ARRAY_BUFFER_ARB, 0);
            glBindBufferARB (GL_ELEMENT_ARRAY_BUFFER_ARB, 0);

            // vertices

            vertices = malloc (SIZE_FLOAT_32 * cvertices * 3);
            fread (vertices, SIZE_FLOAT_32,  cvertices * 3, fm);

            glEnableClientState (GL_VERTEX_ARRAY);
            glVertexPointer (3, GL_FLOAT, 0, vertices);

            // tangents

            fread ((VOIDP) &presence, SIZE_UINT_32, 1, fm);
            if (presence) { 

                tangents = malloc (SIZE_UINT_8 * cvertices * 3);
                fread (tangents, SIZE_UINT_8,  cvertices * 3, fm);

                glEnableClientState (GL_SECONDARY_COLOR_ARRAY);
                glSecondaryColorPointer (3, GL_UNSIGNED_BYTE, 0, tangents);
            }

            // normals

            fread ((VOIDP) &presence, SIZE_UINT_32, 1, fm);
            if (presence) { 

                normals = malloc (SIZE_INT_16 * cvertices * 3);
                fread (normals, SIZE_INT_16,  cvertices * 3, fm);

                glEnableClientState (GL_NORMAL_ARRAY);
                glNormalPointer (GL_SHORT, 0, normals);
            }

            // colors

            fread ((VOIDP) &presence, SIZE_UINT_32, 1, fm);
            if (presence) {

                colors = malloc (SIZE_UINT_8 * cvertices * 3);
                fread (colors, SIZE_UINT_8,  cvertices * 3, fm);

                glEnableClientState (GL_COLOR_ARRAY);
                glColorPointer (3, GL_UNSIGNED_BYTE, 0, colors);
            }

            // UV 1

            fread ((VOIDP) &presence, SIZE_UINT_32, 1, fm);
            if (presence) {

                fread ((VOIDP) &size, SIZE_UINT_32, 1, fm);

                if (presence == 16) {

                    uv1 = malloc (SIZE_INT_16 * cvertices * size);
                    fread (uv1, SIZE_INT_16,  cvertices * size, fm);

                    glClientActiveTexture (GL_TEXTURE0);
                    glEnableClientState (GL_TEXTURE_COORD_ARRAY);
                    glTexCoordPointer (size, GL_SHORT, 0, uv1);

                    model->type_uv1 = GL_SHORT;
                    model->size_uv1 = size;

                } else

                if (presence == 32) {

                    uv1 = malloc (SIZE_FLOAT_32 * cvertices * size);
                    fread (uv1, SIZE_FLOAT_32,  cvertices * size, fm);

                    glClientActiveTexture (GL_TEXTURE0);
                    glEnableClientState (GL_TEXTURE_COORD_ARRAY);
                    glTexCoordPointer (size, GL_FLOAT, 0, uv1);

                    model->type_uv1 = GL_FLOAT;
                    model->size_uv1 = size;
                }
            }

            // UV 2

            fread ((VOIDP) &presence, SIZE_UINT_32, 1, fm);
            if (presence) {

                fread ((VOIDP) &size, SIZE_UINT_32, 1, fm);

                if (presence == 16) {

                    uv2 = malloc (SIZE_INT_16 * cvertices * size);
                    fread (uv2, SIZE_INT_16,  cvertices * size, fm);

                    glClientActiveTexture (GL_TEXTURE1);
                    glEnableClientState (GL_TEXTURE_COORD_ARRAY);
                    glTexCoordPointer (size, GL_SHORT, 0, uv1);

                    model->type_uv2 = GL_SHORT;
                    model->size_uv2 = size;

                } else 

                if (presence == 32) {

                    uv2 = malloc (SIZE_FLOAT_32 * cvertices * size);
                    fread (uv2, SIZE_FLOAT_32,  cvertices * size, fm);

                    glClientActiveTexture (GL_TEXTURE1);
                    glEnableClientState (GL_TEXTURE_COORD_ARRAY);
                    glTexCoordPointer (size, GL_FLOAT, 0, uv1);

                    model->type_uv2 = GL_FLOAT;
                    model->size_uv2 = size;
                }
            }

            // UV 3

            fread ((VOIDP) &presence, SIZE_UINT_32, 1, fm);
            if (presence) {

                fread ((VOIDP) &size, SIZE_UINT_32, 1, fm);

                if (presence == 16) {

                    uv3 = malloc (SIZE_INT_16 * cvertices * size);
                    fread (uv3, SIZE_INT_16,  cvertices * size, fm);

                    glClientActiveTexture (GL_TEXTURE2);
                    glEnableClientState (GL_TEXTURE_COORD_ARRAY);
                    glTexCoordPointer (size, GL_SHORT, 0, uv1);

                    model->type_uv3 = GL_SHORT;
                    model->size_uv3 = size;

                } else 

                if (presence == 32) {

                    uv3 = malloc (SIZE_FLOAT_32 * cvertices * size);
                    fread (uv3, SIZE_FLOAT_32,  cvertices * size, fm);

                    glClientActiveTexture (GL_TEXTURE2);
                    glEnableClientState (GL_TEXTURE_COORD_ARRAY);
                    glTexCoordPointer (size, GL_FLOAT, 0, uv1);

                    model->type_uv3 = GL_FLOAT;
                    model->size_uv3 = size;
                }
            }

            // indices

            indices = malloc (SIZE_UINT_16 * cindices);
            fread (indices, SIZE_UINT_16,  cindices, fm);

            // new list

            model->list = glGenLists (1);

            // compile list

            glNewList (model->list, GL_COMPILE);
            glDrawElements (model->mode, model->count, GL_UNSIGNED_SHORT, indices);
            glEndList ();

            // pop states

            glPopClientAttrib ();

            // clean up

            if (indices)    free (indices);
            if (vertices)   free (vertices);
            if (tangents)   free (tangents);
            if (normals)    free (normals);
            if (colors)     free (colors);
            if (uv1)        free (uv1);
            if (uv2)        free (uv2);
            if (uv3)        free (uv3);

        // use VBO
        } else {

            // vertices

            FLOAT_32P vertices = (FLOAT_32P) malloc (SIZE_FLOAT_32 * cvertices * 3);
            fread ((VOIDP) vertices, SIZE_FLOAT_32,  cvertices * 3, fm);

            glGenBuffersARB (1, &model->vbo_vertices);
            glBindBufferARB (GL_ARRAY_BUFFER_ARB, model->vbo_vertices);
            glBufferDataARB (GL_ARRAY_BUFFER_ARB, cvertices * 3 * SIZE_FLOAT_32, vertices, GL_STATIC_DRAW_ARB);

            free (vertices);

            // tangents

            fread ((VOIDP) &presence, SIZE_UINT_32, 1, fm);
            if (presence) { 

                UINT_8P tangents = (UINT_8P) malloc (SIZE_UINT_8 * cvertices * 3);
                fread ((VOIDP) tangents, SIZE_UINT_8,  cvertices * 3, fm);

                glGenBuffersARB (1, &model->vbo_tangents);
                glBindBufferARB (GL_ARRAY_BUFFER_ARB, model->vbo_tangents);
                glBufferDataARB (GL_ARRAY_BUFFER_ARB, cvertices * 3 * SIZE_UINT_8, tangents, GL_STATIC_DRAW_ARB);

                free (tangents);

            } else model->vbo_tangents	= 0;

            // normals

            fread ((VOIDP) &presence, SIZE_UINT_32, 1, fm);
            if (presence) { 

                INT_16P normals = (INT_16P) malloc (SIZE_INT_16 * cvertices * 3);
                fread ((VOIDP) normals, SIZE_INT_16,  cvertices * 3, fm);

                glGenBuffersARB (1, &model->vbo_normals);
                glBindBufferARB (GL_ARRAY_BUFFER_ARB, model->vbo_normals);
                glBufferDataARB (GL_ARRAY_BUFFER_ARB, cvertices * 3 * SIZE_INT_16, normals, GL_STATIC_DRAW_ARB);

                free (normals);

            } else model->vbo_normals	= 0;

            // colors

            fread ((VOIDP) &presence, SIZE_UINT_32, 1, fm);
            if (presence) {

                UINT_8P colors = (UINT_8P) malloc (SIZE_UINT_8 * cvertices * 3);
                fread ((VOIDP) colors, SIZE_UINT_8,  cvertices * 3, fm);

                glGenBuffersARB (1, &model->vbo_colors);
                glBindBufferARB (GL_ARRAY_BUFFER_ARB, model->vbo_colors);
                glBufferDataARB (GL_ARRAY_BUFFER_ARB, cvertices * 3 * SIZE_UINT_8, colors, GL_STATIC_DRAW_ARB);

                free (colors);

            } else model->vbo_colors   = 0;

            // UV 1

            fread ((VOIDP) &presence, SIZE_UINT_32, 1, fm);
            if (presence) {

                fread ((VOIDP) &size, SIZE_UINT_32, 1, fm);

                if (presence == 16) {

                    INT_16P uv1 = (INT_16P) malloc (SIZE_INT_16 * cvertices * size);
                    fread ((VOIDP) uv1, SIZE_INT_16,  cvertices * size, fm);

                    glGenBuffersARB (1, &model->vbo_uv1);
                    glBindBufferARB (GL_ARRAY_BUFFER_ARB, model->vbo_uv1);
                    glBufferDataARB (GL_ARRAY_BUFFER_ARB, cvertices * size * SIZE_INT_16, uv1, GL_STATIC_DRAW_ARB);

                    model->type_uv1 = GL_SHORT;
                    model->size_uv1 = size;

                    free (uv1);

                } else

                if (presence == 32) {

                    FLOAT_32P uv1 = (FLOAT_32P) malloc (SIZE_FLOAT_32 * cvertices * size);
                    fread ((VOIDP) uv1, SIZE_FLOAT_32,  cvertices * size, fm);

                    glGenBuffersARB (1, &model->vbo_uv1);
                    glBindBufferARB (GL_ARRAY_BUFFER_ARB, model->vbo_uv1);
                    glBufferDataARB (GL_ARRAY_BUFFER_ARB, cvertices * size * SIZE_FLOAT_32, uv1, GL_STATIC_DRAW_ARB);

                    model->type_uv1 = GL_FLOAT;
                    model->size_uv1 = size;

                    free (uv1);
                }

            } else model->vbo_uv1      = 0;

            // UV 2

            fread ((VOIDP) &presence, SIZE_UINT_32, 1, fm);
            if (presence) {

                fread ((VOIDP) &size, SIZE_UINT_32, 1, fm);

                if (presence == 16) {

                    INT_16P uv2 = (INT_16P) malloc (SIZE_INT_16 * cvertices * size);
                    fread ((VOIDP) uv2, SIZE_INT_16,  cvertices * size, fm);

                    glGenBuffersARB (1, &model->vbo_uv2);
                    glBindBufferARB (GL_ARRAY_BUFFER_ARB, model->vbo_uv2);
                    glBufferDataARB (GL_ARRAY_BUFFER_ARB, cvertices * size * SIZE_INT_16, uv2, GL_STATIC_DRAW_ARB);

                    model->type_uv2 = GL_SHORT;
                    model->size_uv2 = size;

                    free (uv2);
                } else 

                if (presence == 32) {

                    FLOAT_32P uv2 = (FLOAT_32P) malloc (SIZE_FLOAT_32 * cvertices * size);
                    fread ((VOIDP) uv2, SIZE_FLOAT_32,  cvertices * size, fm);

                    glGenBuffersARB (1, &model->vbo_uv2);
                    glBindBufferARB (GL_ARRAY_BUFFER_ARB, model->vbo_uv2);
                    glBufferDataARB (GL_ARRAY_BUFFER_ARB, cvertices * size * SIZE_FLOAT_32, uv2, GL_STATIC_DRAW_ARB);

                    model->type_uv2 = GL_FLOAT;
                    model->size_uv2 = size;

                    free (uv2);
                }

            } else model->vbo_uv2      = 0;

            // UV 3

            fread ((VOIDP) &presence, SIZE_UINT_32, 1, fm);
            if (presence) {

                fread ((VOIDP) &size, SIZE_UINT_32, 1, fm);

                if (presence == 16) {

                    INT_16P uv3 = (INT_16P) malloc (SIZE_INT_16 * cvertices * size);
                    fread ((VOIDP) uv3, SIZE_INT_16,  cvertices * size, fm);

                    glGenBuffersARB (1, &model->vbo_uv3);
                    glBindBufferARB (GL_ARRAY_BUFFER_ARB, model->vbo_uv3);
                    glBufferDataARB (GL_ARRAY_BUFFER_ARB, cvertices * size * SIZE_INT_16, uv3, GL_STATIC_DRAW_ARB);

                    model->type_uv3 = GL_SHORT;
                    model->size_uv3 = size;

                    free (uv3);
                } else 

                if (presence == 32) {

                    FLOAT_32P uv3 = (FLOAT_32P) malloc (SIZE_FLOAT_32 * cvertices * size);
                    fread ((VOIDP) uv3, SIZE_FLOAT_32,  cvertices * size, fm);

                    glGenBuffersARB (1, &model->vbo_uv3);
                    glBindBufferARB (GL_ARRAY_BUFFER_ARB, model->vbo_uv3);
                    glBufferDataARB (GL_ARRAY_BUFFER_ARB, cvertices * size * SIZE_FLOAT_32, uv3, GL_STATIC_DRAW_ARB);

                    model->type_uv3 = GL_FLOAT;
                    model->size_uv3 = size;

                    free (uv3);
                }

            } else model->vbo_uv3      = 0;

            // indices

            UINT_16P data = (UINT_16P) malloc (SIZE_UINT_16 * cindices);
            fread ((VOIDP) data, SIZE_UINT_16,  cindices, fm);

            glGenBuffersARB (1, &model->ebo);
            glBindBufferARB (GL_ELEMENT_ARRAY_BUFFER_ARB, model->ebo);
            glBufferDataARB (GL_ELEMENT_ARRAY_BUFFER_ARB, cindices * SIZE_UINT_16, data, GL_STATIC_DRAW_ARB);

            free (data);

            // init

            model->list = 0;
        }

        // close mesh file
        fclose (fm);

		// log
        debug_Print (" .. OK\n");

		// check OpenGL error
		GLenum err = glGetError ();	if (err != GL_NO_ERROR) {	
			
            debug_Print ("ERROR OpenGL code : ", err, true);    return -1;
		};
    }

    // back
    glBindBufferARB (GL_ARRAY_BUFFER_ARB, 0);
    glBindBufferARB (GL_ELEMENT_ARRAY_BUFFER_ARB, 0);

    // ---------------------------------------------------------------------------------------
    // INSTANCED

    debug_Print ("Loading Instanced ..\n");

	// save number of records
	UINT_32 ocount = dr_objectsc;

	// preloaded data
    VOIDPP  instanced = (VOIDPP) malloc (SIZE_VOIDP * instancedc);
    
	// We preload instanced data before objects loading to evaluate object count and 
	// select object that are to be skipped if they have no instances
	// External instances file data structure can describe more than one object with instances, so
	// without prereading objects counts we don't know how to allocate static object data.

    for (i = 0; i < (INT_32) instancedc; i ++) {
		
        CHAR filepath [255];

        // name
        fread ((VOIDP) &length, SIZE_UINT_32, 1, f);    CHARP name = (CHARP) malloc (SIZE_CHAR * length);
        fread ((VOIDP) name, SIZE_CHAR, length, f);

        // reconstruct file name
        sprintf (filepath, "%s%s.inst", M_LOADER_INSTANCESPATH, name);

        debug_PrintIntend ("Loading ", filepath);
        
        // open file of intances
        FILE * fp = fopen (filepath, "rb");     
		
		if (fp != NULL) {

			// get file length
			fseek (fp, 0L, SEEK_END);   UINT_32 l = ftell (fp); instanced [i] = malloc (l);
			fseek (fp, 0L, SEEK_SET);

			// cacheing data
			fread ((VOIDP) instanced [i], SIZE_UINT_8, l, fp);

			// accumulate count
			dr_objectsc += (UINT_32) (*((UINT_16P) instanced [i])) - 1;

			// close file
			fclose (fp);

		} else {

            debug_Print (" .. SKIPPED\n");

			// file do not exist so we remove object associated with it
			instanced [i] = NULL;	dr_objectsc --;

			continue;
		}

        debug_Print (" .. OK\n");
    }

    // ---------------------------------------------------------------------------------------
    // OBJECTS

    dr_objects					= (TObject   *)		malloc (sizeof (TObject)	* (dr_objectsc + M_RESERVED_OBJECTS));
    dr_object_details           = (TDetail   *)     malloc (sizeof (TDetail)    * (dr_objectsc + M_RESERVED_OBJECTS));
    dr_object_boundaries        = (TBoundary *)     malloc (sizeof (TBoundary)  * (dr_objectsc + M_RESERVED_OBJECTS));

    dr_object_disappear_end     = (FLOAT_32P)       malloc (SIZE_FLOAT_32       * (dr_objectsc + M_RESERVED_OBJECTS));
    dr_object_disappear_start   = (FLOAT_32P)       malloc (SIZE_FLOAT_32       * (dr_objectsc + M_RESERVED_OBJECTS));
    dr_object_disappear_shadow  = (FLOAT_32P)       malloc (SIZE_FLOAT_32       * (dr_objectsc + M_RESERVED_OBJECTS));
    dr_object_disappear_split   = (UINT_8P)         malloc (SIZE_UINT_8         * (dr_objectsc + M_RESERVED_OBJECTS));

    dr_object_flags             = (UINT_16P)        malloc (SIZE_UINT_16        * (dr_objectsc + M_RESERVED_OBJECTS));
    dr_object_models            = (UINT_16P)        malloc (SIZE_UINT_16        * (dr_objectsc + M_RESERVED_OBJECTS));
    dr_object_queries           = (UINT_16P)        malloc (SIZE_UINT_16        * (dr_objectsc + M_RESERVED_OBJECTS));
    dr_object_materials         = (UINT_16P)        malloc (SIZE_UINT_16        * (dr_objectsc + M_RESERVED_OBJECTS));

    dr_object_centers           = (FLOAT_32P)       malloc (SIZE_FLOAT_32       * (dr_objectsc + M_RESERVED_OBJECTS)    * 3);
    dr_object_transforms        = (FLOAT_32P)       malloc (SIZE_FLOAT_32       * (dr_objectsc + M_RESERVED_OBJECTS)    * 16);

    dr_object_morph1            = (FLOAT_32P)       malloc (SIZE_FLOAT_32       * (dr_objectsc + M_RESERVED_OBJECTS));
    dr_object_morph2            = (FLOAT_32P)       malloc (SIZE_FLOAT_32       * (dr_objectsc + M_RESERVED_OBJECTS));

    // object instances

    dr_object_instances             = (UINT_16P)    malloc ((dr_objectsc + M_RESERVED_OBJECTS) * SIZE_UINT_16);
    dr_object_instances_rand        = (FLOAT_32PP)  calloc ((dr_objectsc + M_RESERVED_OBJECTS),  SIZE_FLOAT_32P);
    dr_object_instances_transforms  = (FLOAT_32PP)  calloc ((dr_objectsc + M_RESERVED_OBJECTS),  SIZE_FLOAT_32P);

    // world boundary init.

    dr_world_min [0] = FLT_MAX;     dr_world_max [0] = - FLT_MAX;
    dr_world_min [1] = FLT_MAX;     dr_world_max [1] = - FLT_MAX;
    dr_world_min [2] = FLT_MAX;     dr_world_max [2] = - FLT_MAX;

	INT_32 index = 0;

    for (i = 0; i < (INT_32) ocount; i ++) {

		INT_32 pindex;

        UINT_32 presence;

        // instances
        fread ((VOIDP) &pindex, SIZE_INT_32, 1, f);

		// is instanced object, but have no instances
		if ((pindex >= 0) && (instanced [pindex] == NULL)) {

			// skipping file data 

			fread ((VOIDP) &length, SIZE_UINT_32, 1, f);

			fseek (f, length, SEEK_CUR);

			// fread ((VOIDP) &dr_object_models            [index], SIZE_UINT_16,  1, f);
			// fread ((VOIDP) &dr_object_materials         [index], SIZE_UINT_16,  1, f);
			// fread ((VOIDP) &dr_object_disappear_end     [index], SIZE_FLOAT_32, 1, f);
			// fread ((VOIDP) &dr_object_disappear_start   [index], SIZE_FLOAT_32, 1, f);
			// fread ((VOIDP) &dr_object_disappear_shadow  [index], SIZE_FLOAT_32, 1, f);
			// fread ((VOIDP) &dr_object_flags             [index], SIZE_UINT_16,  1, f);

			fseek (f, SIZE_UINT_16 * 3 + SIZE_FLOAT_32 * 3, SEEK_CUR);

			fread ((VOIDP) &presence, SIZE_UINT_32, 1, f);
	        if (presence) {

				// fread ((VOIDP) &detail->lod1, SIZE_UINT_16,  1, f);
				// fread ((VOIDP) &detail->lod1_distance,  SIZE_FLOAT_32, 1, f);
				// fread ((VOIDP) &detail->lod2, SIZE_UINT_16,  1, f);
				// fread ((VOIDP) &detail->lod2_distance,  SIZE_FLOAT_32, 1, f);
				// fread ((VOIDP) &detail->lod3, SIZE_UINT_16,  1, f);
				// fread ((VOIDP) &detail->lod3_distance,  SIZE_FLOAT_32, 1, f);

				fseek (f, SIZE_UINT_16 * 3 + SIZE_FLOAT_32 * 3, SEEK_CUR);
			}

			// we skip this object 
			continue;
		}

        // shortcut
        TObject * object = &dr_objects [index];

        // name
        fread ((VOIDP) &length, SIZE_UINT_32, 1, f);            object->name = (CHARP) malloc (SIZE_CHAR * length);
        fread ((VOIDP) object->name, SIZE_CHAR, length, f);

        fread ((VOIDP) &dr_object_models            [index], SIZE_UINT_16,  1, f);
        fread ((VOIDP) &dr_object_materials         [index], SIZE_UINT_16,  1, f);
        fread ((VOIDP) &dr_object_disappear_end     [index], SIZE_FLOAT_32, 1, f);
        fread ((VOIDP) &dr_object_disappear_start   [index], SIZE_FLOAT_32, 1, f);
        fread ((VOIDP) &dr_object_disappear_shadow  [index], SIZE_FLOAT_32, 1, f);
        fread ((VOIDP) &dr_object_flags             [index], SIZE_UINT_16,  1, f);

        // LOD description
        fread ((VOIDP) &presence, SIZE_UINT_32, 1, f);
        if (presence) {

            TDetail * detail = &dr_object_details [index];

            fread ((VOIDP) &detail->lod1, SIZE_UINT_16,  1, f);
            fread ((VOIDP) &detail->lod1_distance,  SIZE_FLOAT_32, 1, f);
            fread ((VOIDP) &detail->lod2, SIZE_UINT_16,  1, f);
            fread ((VOIDP) &detail->lod2_distance,  SIZE_FLOAT_32, 1, f);
            fread ((VOIDP) &detail->lod3, SIZE_UINT_16,  1, f);
            fread ((VOIDP) &detail->lod3_distance,  SIZE_FLOAT_32, 1, f);

            detail->lodbase = dr_object_models [index];

            // creating occlusion query
            if ((dr_model_flags [detail->lodbase] & M_FLAG_MODEL_OCCLUSION)    ||
                ((detail->lod1 != 0xffff) && (dr_model_flags [detail->lod1   ] & M_FLAG_MODEL_OCCLUSION))   || 
                ((detail->lod2 != 0xffff) && (dr_model_flags [detail->lod2   ] & M_FLAG_MODEL_OCCLUSION))   ||
                ((detail->lod3 != 0xffff) && (dr_model_flags [detail->lod3   ] & M_FLAG_MODEL_OCCLUSION))) {

                UINT_32 query;
                glGenQueriesARB (1, &query);    dr_object_queries [index] = (UINT_16) query;

            } else  dr_object_queries [index] = 0;

        } else {

            TDetail * detail = &dr_object_details [index];

            detail->lod1    = 0xffff;
            detail->lod2    = 0xffff;
            detail->lod3    = 0xffff;
            detail->lodbase = 0xffff;

            // init morphing parameters
            dr_object_morph1 [index] =												dr_object_disappear_start [index];
            dr_object_morph2 [index] = 1.0f / (dr_object_disappear_end [index] -	dr_object_disappear_start [index]);

            // creating occlusion query
            if (dr_model_flags [dr_object_models [index]] & M_FLAG_MODEL_OCCLUSION) {
                
                UINT_32 query;
                glGenQueriesARB (1, &query);    dr_object_queries [index] = (UINT_16) query;

            } else  dr_object_queries [index] = 0;
        }

		// single object
        if (pindex < 0) {

            // single object
            dr_object_instances             [index] = 1;
            dr_object_instances_transforms  [index] = NULL;

            // shortcut
            TModel * model = &dr_models	[dr_object_models	[index]];

            // shortcut
            TBoundary * boundary	= &dr_object_boundaries [index];

            // shortcut
            FLOAT_32P transform		= &dr_object_transforms	[index * 16];

            // matrix
            fread ((VOIDP) transform, SIZE_FLOAT_32, 16, f);

            // updating object boundary
            dr_ClipUpdateBoundary (boundary, model->boxmin, model->boxmax, transform);

            // update object center
            FLOAT_32P center = &dr_object_centers [index * 3];

            center [0] = boundary->center [0];
            center [1] = boundary->center [1];
            center [2] = boundary->center [2];

            // collecting world bounding box
            dr_world_min [0] = MIN (boundary->boxmin [0], dr_world_min [0]);
            dr_world_min [1] = MIN (boundary->boxmin [1], dr_world_min [1]);
            dr_world_min [2] = MIN (boundary->boxmin [2], dr_world_min [2]);

            dr_world_max [0] = MAX (boundary->boxmax [0], dr_world_max [0]);
            dr_world_max [1] = MAX (boundary->boxmax [1], dr_world_max [1]);
            dr_world_max [2] = MAX (boundary->boxmax [2], dr_world_max [2]);

            // ID
            object->ID = index;

			// advance
			index ++;

        } else {

			// shortcut
            TDetail * detailo = &dr_object_details [index];

			// mark object to use instanced shader
			dr_model_shaders [dr_object_models	[index]] |= M_MODEL_SHADER_INSTANCED;

			if (detailo->lod1 != 0xffff)	dr_model_shaders [detailo->lod1] |= M_MODEL_SHADER_INSTANCED;
			if (detailo->lod2 != 0xffff)	dr_model_shaders [detailo->lod2] |= M_MODEL_SHADER_INSTANCED;
			if (detailo->lod3 != 0xffff)	dr_model_shaders [detailo->lod3] |= M_MODEL_SHADER_INSTANCED;

			// count of instances
            UINT_32 count = (UINT_32) (*((UINT_16P) (instanced [pindex])));

			// shortcut
            CHARP pinstanced = ((CHARP) (instanced [pindex])) + SIZE_UINT_16;

			// origin index
            UINT_32 indexo = index;

			// create sub-objects
            for (UINT_32 n = 0; n < count; n ++) {

                // we make a copy of base object
                if (n > 0) {

                    // name
                    sprintf (dr_objects [index].name = (CHARP) malloc (SIZE_CHAR * (strlen (dr_objects [indexo].name) + 6)), "%s_%i", dr_objects [indexo].name, n);

                    dr_object_models            [index] = dr_object_models              [indexo];
                    dr_object_materials         [index] = dr_object_materials           [indexo];
                    dr_object_disappear_end     [index] = dr_object_disappear_end       [indexo];
                    dr_object_disappear_start   [index] = dr_object_disappear_start     [indexo];
                    dr_object_disappear_shadow  [index] = dr_object_disappear_shadow    [indexo];
                    dr_object_flags             [index] = dr_object_flags               [indexo];

                    TDetail * detail = &dr_object_details [index];

					detail->lod1            = detailo->lod1;
					detail->lod2            = detailo->lod2;
					detail->lod3            = detailo->lod3;
					detail->lod1_distance   = detailo->lod1_distance;
					detail->lod2_distance   = detailo->lod2_distance;
					detail->lod3_distance   = detailo->lod3_distance;
					detail->lodbase         = detailo->lodbase;

                    if (dr_object_queries [indexo]) {

                        UINT_32 query;
                        glGenQueriesARB (1, &query);    dr_object_queries [index] = (UINT_16) query;
                    }

                    if (detailo->lod1 == 0xffff) {

                        // init morphing parameters
						dr_object_morph1 [index] =												dr_object_disappear_start [index];
						dr_object_morph2 [index] = 1.0f / (dr_object_disappear_end [index] -	dr_object_disappear_start [index]);
                    }
                }

                // count of instances
                UINT_32 c = dr_object_instances [index] = * ((UINT_16P) pinstanced);     pinstanced += SIZE_UINT_16;

                // transformations
                UINT_32 l = SIZE_FLOAT_32 * 7 * dr_object_instances [index];

				// copy values from pre-readed data block
                memcpy ((VOIDP) (dr_object_instances_transforms [index] = (FLOAT_32P) malloc (l)), (VOIDP) pinstanced, l);  pinstanced += l;

                // random factors
                dr_object_instances_rand [index] = (FLOAT_32P) malloc (SIZE_FLOAT_32 * dr_object_instances [index]);

                // generate factors
                FLOAT_32P p = dr_object_instances_rand [index];

                /// SLOW !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                for (UINT_32 m = 0; m < c; m ++)  p [m] = (rand () / (FLOAT_32) RAND_MAX);

                // shortcut
                TBoundary * boundary = &dr_object_boundaries [index];

                // boundary
				boundary->boxmin [0] = * ((FLOAT_32P) pinstanced);  pinstanced += SIZE_FLOAT_32;
				boundary->boxmin [1] = * ((FLOAT_32P) pinstanced);  pinstanced += SIZE_FLOAT_32;
				boundary->boxmin [2] = * ((FLOAT_32P) pinstanced);  pinstanced += SIZE_FLOAT_32;

				boundary->boxmax [0] = * ((FLOAT_32P) pinstanced);  pinstanced += SIZE_FLOAT_32;
				boundary->boxmax [1] = * ((FLOAT_32P) pinstanced);  pinstanced += SIZE_FLOAT_32;
				boundary->boxmax [2] = * ((FLOAT_32P) pinstanced);  pinstanced += SIZE_FLOAT_32;

                // complete boundary
                dr_ClipCompleteBoundary (boundary);

                // update object center
                FLOAT_32P center = &dr_object_centers [index * 3];

                center [0] = boundary->center [0];
                center [1] = boundary->center [1];
                center [2] = boundary->center [2];

                // collecting world bounding box
                dr_world_min [0] = MIN (boundary->boxmin [0], dr_world_min [0]);
                dr_world_min [1] = MIN (boundary->boxmin [1], dr_world_min [1]);
                dr_world_min [2] = MIN (boundary->boxmin [2], dr_world_min [2]);

                dr_world_max [0] = MAX (boundary->boxmax [0], dr_world_max [0]);
                dr_world_max [1] = MAX (boundary->boxmax [1], dr_world_max [1]);
                dr_world_max [2] = MAX (boundary->boxmax [2], dr_world_max [2]);

                // ID
                object->ID = index;

                // advance
                index ++;
            }
        }
    }

	// free temorary array
    for (UINT_32 n = 0; n < instancedc; n ++)
        if (instanced [n])
			free (instanced [n]);

    free (instanced);

    // ---------------------------------------------------------------------------------------
    // ENVIROMENT

    // model
    fread ((VOIDP) &index, SIZE_UINT_32, 1, f);  dr_enviroment_model = &dr_models [index];

    // material
    fread ((VOIDP) &index, SIZE_UINT_32, 1, f);  dr_enviroment_material = &dr_materials [index];

    // ---------------------------------------------------------------------------------------
    // SUN

    FLOAT_32 tmp [3], d;

    // direction
    fread ((VOIDP) &tmp [0], SIZE_FLOAT_32, 1, f);
    fread ((VOIDP) &tmp [1], SIZE_FLOAT_32, 1, f);
    fread ((VOIDP) &tmp [2], SIZE_FLOAT_32, 1, f);

    dr_sun_directionz [0] = (FLOAT_64) tmp [0];
    dr_sun_directionz [1] = (FLOAT_64) tmp [1];
    dr_sun_directionz [2] = (FLOAT_64) tmp [2];

    // distance
    fread ((VOIDP) &d, SIZE_FLOAT_32, 1, f);

    dr_sun_distance = (FLOAT_64) d;

    // color
    fread ((VOIDP) &dr_sun_color [0], SIZE_FLOAT_32, 1, f);
    fread ((VOIDP) &dr_sun_color [1], SIZE_FLOAT_32, 1, f);
    fread ((VOIDP) &dr_sun_color [2], SIZE_FLOAT_32, 1, f);

    // intensity
    fread ((VOIDP) &dr_sun_intensity, SIZE_FLOAT_32, 1, f);

    // ambient intensity
    fread ((VOIDP) &dr_sun_ambient,   SIZE_FLOAT_32, 1, f);
        
    // far plane
    fread ((VOIDP) &d,                SIZE_FLOAT_32, 1, f);     dr_sun_planefar = (FLOAT_64) d;

    // ---------------------------------------------------------------------------------------
    // FOG

    // color
    fread ((VOIDP) &dr_fog_color [0], SIZE_FLOAT_32, 1, f);
    fread ((VOIDP) &dr_fog_color [1], SIZE_FLOAT_32, 1, f);
    fread ((VOIDP) &dr_fog_color [2], SIZE_FLOAT_32, 1, f);

    // done
    fclose (f);

    // ---------------------------------------------------------------------------------------
    // BUILDING OCT-TREE

    // nodes

    dr_nodes   = (TNode  *) malloc (sizeof (TNode)  * M_RESERVED_NODES);
    dr_nodesc  = 0;

    // chains

    dr_chains  = (TChain *) malloc (sizeof (TChain) * M_RESERVED_CHAINS);
    dr_chainsc = 0;

    // depths in oct-tree

    dr_object_depths = (UINT_8P) malloc (SIZE_UINT_8 * (dr_objectsc + M_RESERVED_OBJECTS));

    // initialization
    dr_ClipInit ();

    // adding objects into oct-tree
    for (i = 0; i < (INT_32) dr_objectsc; i ++)

        dr_ClipAdd  (i);

    // oct-tree defragmentation
    dr_ClipDefrag ();

    // ---------------------------------------------------------------------------------------
    // INTERNAL DATA

    // RENDERING CONTEXT NODES

    dr_context_models        = (TContextModel    *) malloc (sizeof (TContextModel)    * (dr_modelsc    + M_RESERVED_MODELS));
    dr_context_materials     = (TContextMaterial *) malloc (sizeof (TContextMaterial) * (dr_materialsc + M_RESERVED_MATERIALS));

    dr_context.list          = dr_context_materials;

    // OBJECT TAGS

    dr_object_tags      = (UINT_16P) calloc ((dr_objectsc + M_RESERVED_OBJECTS), SIZE_UINT_16);

    // OBJECT LISTS

    dr_list_objects1        = (UINT_32P) malloc (SIZE_UINT_32 * (dr_objectsc + M_RESERVED_OBJECTS));
    dr_list_objects2        = (UINT_32P) malloc (SIZE_UINT_32 * (dr_objectsc + M_RESERVED_OBJECTS));
    dr_list_objects3        = (UINT_32P) malloc (SIZE_UINT_32 * (dr_objectsc + M_RESERVED_OBJECTS));
    dr_list_objects4        = (UINT_32P) malloc (SIZE_UINT_32 * (dr_objectsc + M_RESERVED_OBJECTS));
    dr_list_objects_view    = (UINT_32P) malloc (SIZE_UINT_32 * (dr_objectsc + M_RESERVED_OBJECTS));

    // RADIX SORTER

    so_RadixCreate	(&dr_radix,	(dr_objectsc + M_RESERVED_OBJECTS));

    so_RadixCreate	(&dr_radix_instanced, 65535);

    // OBJECT INTERNAL 

    dr_object_distances         = (FLOAT_32P)   malloc (SIZE_FLOAT_32   * (dr_objectsc + M_RESERVED_OBJECTS));
    dr_object_distancesq        = (FLOAT_32P)   malloc (SIZE_FLOAT_32   * (dr_objectsc + M_RESERVED_OBJECTS));
    dr_object_distances_sort    = (UINT_32P)    malloc (SIZE_UINT_32    * (dr_objectsc + M_RESERVED_OBJECTS));

    return 0;
}

#endif

///////////////////////////////////////////////////////////////////////////////
// lo_UnloadWorld
///////////////////////////////////////////////////////////////////////////////

#if 1

VOID lo_UnloadWorld ()
{
    /// UNLOAD VIDEO MEMORY !!!

    // main data structures

    if (dr_models)					free (dr_models);
    if (dr_objects)					free (dr_objects);
    if (dr_materials)				free (dr_materials);

    // model properites

    if (dr_model_flags)				free (dr_model_flags);
    if (dr_model_stamps)			free (dr_model_stamps);
    if (dr_model_occlusions)		free (dr_model_occlusions);

    // material properites

    if (dr_material_counters)		free (dr_material_counters);
    if (dr_material_stamps)			free (dr_material_stamps);

    // object properties

    if (dr_object_details)			free (dr_object_details);
    if (dr_object_boundaries)		free (dr_object_boundaries);

    if (dr_object_distances)		free (dr_object_distances);
    if (dr_object_distancesq)		free (dr_object_distancesq);
    if (dr_object_distances_sort)	free (dr_object_distances_sort);
    if (dr_object_disappear_end)	free (dr_object_disappear_end);
    if (dr_object_disappear_start)	free (dr_object_disappear_start);
    if (dr_object_disappear_split)	free (dr_object_disappear_split);
    if (dr_object_disappear_shadow)	free (dr_object_disappear_shadow);
    if (dr_object_queries)			free (dr_object_queries);
    if (dr_object_depths)			free (dr_object_depths);
    if (dr_object_flags)			free (dr_object_flags);
    if (dr_object_models)			free (dr_object_models);
    if (dr_object_materials)		free (dr_object_materials);
    if (dr_object_morph1)			free (dr_object_morph1);
    if (dr_object_morph2)			free (dr_object_morph2);
    if (dr_object_transforms)		free (dr_object_transforms);
    if (dr_object_centers)			free (dr_object_centers);

    // object instances

    if (dr_object_instances)		free (dr_object_instances);

    UINT_32 c = dr_objectsc + M_RESERVED_OBJECTS;

    for (UINT_32 i = 0; i < c; i ++) {

        if (dr_object_instances_transforms [i]) {

            free (dr_object_instances_transforms [i]);
        }
    }

    if (dr_object_instances_transforms)	free (dr_object_instances_transforms);
   
    // object tags

    if (dr_object_tags)	free (dr_object_tags);

    // object lists

    if (dr_list_objects1)				free (dr_list_objects1);
    if (dr_list_objects2)				free (dr_list_objects2);
    if (dr_list_objects3)				free (dr_list_objects3);
    if (dr_list_objects_view)			free (dr_list_objects_view);
    if (dr_list_objects_shadow_split)	free (dr_list_objects_shadow_split [0]);
    if (dr_list_objects_shadow_split)	free (dr_list_objects_shadow_split);
    if (dr_list_objects_shadow_splitc)	free (dr_list_objects_shadow_splitc);

    // nodes

    if (dr_nodes)	free (dr_nodes);

    // chains

    if (dr_chains)	free (dr_chains);

    // radix sorter

    so_RadixFree (&dr_radix);

    // rendering context nodes

    if (dr_context_materials)	free (dr_context_materials);
    if (dr_context_models)		free (dr_context_models);
}

#endif
