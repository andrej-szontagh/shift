
#ifndef _SKELETON_H
#define _SKELETON_H

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ENUMS
////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define M_RESERVED_NODES  	        10000           // specifies reserved memory for all nodes
#define M_RESERVED_CHAINS  	        10000           // specifies reserved memory for all chains
#define M_RESERVED_MODELS  	        64              // specifies reserved memory for dynamic or redundant data
#define M_RESERVED_OBJECTS   	    1024            // specifies reserved memory for dynamic or redundant data
#define M_RESERVED_MATERIALS   	    32              // specifies reserved memory for dynamic or redundant data

#define M_INT_SCALE                 1000            // scale before conversion floating values into integer to preserve some precision
#define M_INT_SCALE_INV             0.001f          // inverse of above

#define M_FLAG_OBJECT_CLEAR         0x0000          // clear all flags
#define M_FLAG_OBJECT_OCCLUDER      0x0001          // object can be occluder

#define M_FLAG_MODEL_CLEAR          0x0000          // clear all flags
#define M_FLAG_MODEL_LIST           0x0001          // uses display list
#define M_FLAG_MODEL_OCCLUSION      0x0002          // model can be occluded

#define M_MODEL_SHADER_NORMAL       0x00            // normal shader
#define M_MODEL_SHADER_GROW         0x01            // model using growing
#define M_MODEL_SHADER_MORPH        0x02            // model using morphing
#define M_MODEL_SHADER_SHRINK       0x03            // model using shrinking
#define M_MODEL_SHADER_INSTANCED	0x08            // model using instancing	(can combine with other shaders)

#define M_SHADER_DEPTH				0x10            // depth shader				(can combine with other shaders)
#define M_SHADER_SHADOW				0x20            // shadow shader			(can combine with other shaders)

#define M_TAG_CLEAR                 0x0000          // clear all tags
#define M_TAG_SHADOWSPLIT1          0x0001          // object is in shadow split 1
#define M_TAG_SHADOWSPLIT2          0x0002          // object is in shadow split 2
#define M_TAG_SHADOWSPLIT3          0x0004          // object is in shadow split 3
#define M_TAG_SHADOWSPLIT4          0x0008          // object is in shadow split 4
#define M_TAG_SHADOWSPLIT5          0x0010          // object is in shadow split 5
#define M_TAG_SHADOWSPLIT6          0x0020          // object is in shadow split 6
#define M_TAG_SHADOWSPLIT7          0x0040          // object is in shadow split 7
#define M_TAG_SHADOWSPLIT8          0x0080          // object is in shadow split 8
#define M_TAG_SHADOWSPLIT           0x0100          // object is in shadow split
#define M_TAG_VIEW                  0x0200          // object is in view
#define M_TAG_DISTANCE              0x0400          // object has valid linear distance
#define M_TAG_DISTANCEQ             0x0800          // object has valid quadtatic distance
#define M_TAG_OCCLUDER              0x1000          // object passed occluder estimation
#define M_TAG_OCCLUDERSEL           0x2000          // object is selected occluder
#define M_TAG_DISAPPEAR             0x4000          // object is distance culled

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DATA STRUCTURES
////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct TImage
{
    #define M_IMAGE_TYPE_RGB    0               // RGB
    #define M_IMAGE_TYPE_RGBA   1               // RGBA
    #define M_IMAGE_TYPE_DXT1   2               // compressed DXT1
    #define M_IMAGE_TYPE_DXT3   3               // compressed DXT3
    #define M_IMAGE_TYPE_DXT5   4               // compressed DXT5

    UINT_8      type;                           // type of image
    UINT_8      mipmaps;                        // count of mipmaps

    UINT_32     width;                          // width of image
    UINT_32     height;                         // height of image
    
    VOIDP       data;                           // image pixel data
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// RUNTIME STRUCTURES
////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct TMaterial
{
    #define M_MATERIAL_TYPE_SOLID         1     // solid
    #define M_MATERIAL_TYPE_TERRAIN       2     // terrain
    #define M_MATERIAL_TYPE_ENVIROMENT    3     // enviroment
    #define M_MATERIAL_TYPE_FOLIAGE       4     // foliage
    #define M_MATERIAL_TYPE_GRASS         5     // grass

    UINT_32     type;                           // one of above

    CHARP       name;                           // material name

    UINT_32     diffuse;                        // diffuse texture id
    UINT_32     composite;                      // composite texture id

    UINT_32     weights1;                       // weights 1 texture id
    UINT_32     weights2;                       // weights 2 texture id
    UINT_32     weights3;                       // weights 3 texture id
    UINT_32     weights4;                       // weights 4 texture id

    UINT_32     width;                          // width of rectangular texture (shader parameter)
    UINT_32     height;                         // height of rectangular texture (shader parameter)

	FLOAT_32    damping;						// scale of dissolve effect to match with object dissapear
	FLOAT_32    threshold;						// threshold for alpha tested texturing
};

struct TModel
{
    UINT_32     count;                          // count of indices
    UINT_32     mode;                           // primitive mode

    UINT_32     list;                           // render list

    UINT_32     vbo_vertices;                   // vetices     VBO id
    UINT_32     vbo_tangents;                   // tangents    VBO id
    UINT_32     vbo_normals;                    // normals     VBO id
    UINT_32     vbo_colors;                     // colors      VBO id
    UINT_32     ebo;                            // indices     EBO id

    UINT_32     vbo_uv1;                        // uv1 VBO id
    UINT_32     vbo_uv2;                        // uv2 VBO id
    UINT_32     vbo_uv3;                        // uv3 VBO id

    UINT_32     type_uv1;                       // data type of uv1
    UINT_32     type_uv2;                       // data type of uv2
    UINT_32     type_uv3;                       // data type of uv3

    UINT_32     size_uv1;                       // data size of uv1
    UINT_32     size_uv2;                       // data size of uv2
    UINT_32     size_uv3;                       // data size of uv3

    FLOAT_32    param1;                         // special parameter 1
    FLOAT_32    param2;                         // special parameter 2
    FLOAT_32    param3;                         // special parameter 3

    FLOAT_32    sphere;                         // bounding sphere radius

    FLOAT_32    boxmin [3];                     // bounding box minimums
    FLOAT_32    boxmax [3];                     // bounding box maximums
};

struct TBoundary
{    
    #define M_CLIP_INSIDE       0
    #define M_CLIP_OUTSIDE      1
    #define M_CLIP_COLLIDING    2

    FLOAT_32    center  [3];                    // center of bounding box/sphere
    FLOAT_32    boxmin  [3];                    // AA bounding box min values
    FLOAT_32    boxmax  [3];                    // AA bounding box max values

    FLOAT_32    sphere;                         // radius of bounding sphere
};

struct TObject;
struct TChain
{
    #define M_CHAIN      32                     // size of linear array

    UINT_32     count;                          // count of references
    UINT_32     list    [M_CHAIN];              // linear array of object IDs

    TChain    * prev;                           // previous chain
    TChain    * next;                           // next chain
};

struct TNode
{
    UINT_32     depth;                          // depth of this node in tree
    TBoundary   boundary;                       // bounding volume

    TNode     * childs [8];                     // eight child nodes

    TChain    * chain;                          // previous chain
};

struct TTree
{
    TNode       root;                           // root node

    FLOAT_32    size;                           // size of bounding cube
    FLOAT_32    sizeh;                          // half size of bounding cube
};

struct TObject
{
    UINT_32         ID;                         // object ID

    CHARP           name;                       // name of object

    UINT_32         nodesc;                     // counr of nodes
    TNode         * nodes   [8];                // 8 possible nodes
};

struct TDetail
{
    UINT_16     lodbase;                        // LOD model ID
    UINT_16     lod1;                           // LOD model ID
    UINT_16     lod2;                           // LOD model ID
    UINT_16     lod3;                           // LOD model ID

    FLOAT_32    lod1_distance;                  // distance when LOD switches to lod1
    FLOAT_32    lod2_distance;                  // distance when LOD switches to lod2
    FLOAT_32    lod3_distance;                  // distance when LOD switches to lod3
};

struct TContextModel
{
    UINT_32 count;                              // count of objects

    UINT_32P          list;                     // list of object IDs
    UINT_32           model;                    // model of this node
};

struct TContextMaterial
{
    UINT_32 count;                              // count of model nodes

    TContextModel   * list;                     // list of models nodes
    UINT_32           material;                 // material of this node
};

struct TContext
{
    UINT_32 count;                              // count of material nodes

    TContextMaterial * list;                    // list of material nodes
};

#endif