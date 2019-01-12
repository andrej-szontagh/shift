
#ifndef _RENDERER_INTERNALS_H
#define _RENDERER_INTERNALS_H

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// GLOBALS
////////////////////////////////////////////////////////////////////////////////////////////////////////////
;

// CAMREA

#ifdef M_DR_BODY

    FLOAT_32    dr_campos   [3] =   {   89.403015f,       13.763757f,      121.25739f     };
    FLOAT_32    dr_camdir   [3] =   {    0.87422132f,     -0.33092949f,     -0.35527843f  };
    FLOAT_32    dr_cambi    [3] =   {    0.37649167f,      0.00000000f,      0.92642003f  };
    FLOAT_32    dr_camup    [3] =   {    0.30657971f,      0.94365549f,     -0.12459219f  };
    FLOAT_32    dr_worldup  [3] =   {    0.0f,             1.0f,             0.0f         };

#else

    extern FLOAT_32    dr_campos   [3];
    extern FLOAT_32    dr_camdir   [3];
    extern FLOAT_32    dr_cambi    [3];
    extern FLOAT_32    dr_camup    [3];
    extern FLOAT_32    dr_worldup  [3];

#endif

#ifndef M_DR_BODY
#define M_DR_BODY extern
#endif

// TIMING
M_DR_BODY UINT_32               dr_frame;
M_DR_BODY FLOAT_32              dr_delta;
M_DR_BODY FLOAT_32              dr_swing;

// GAMMA CONTROL
M_DR_BODY BOOL                  dr_control_gammacorrection;

// AUTOEXPOSURE
M_DR_BODY BOOL                  dr_control_autoexposure;

// FOG CONTROL
M_DR_BODY BOOL                  dr_control_fog;

// AA CONTROL
M_DR_BODY BOOL                  dr_control_aa;

// ANISOTROPHY CONTROL
M_DR_BODY INT_32                dr_control_anisotrophy;

// CULLING CONTROL
M_DR_BODY UINT_32               dr_control_culling;

// SSAO CONTROL
M_DR_BODY BOOL                  dr_control_ssao_enable;
M_DR_BODY UINT_32               dr_control_ssao_res;

// MIP CONTROL
M_DR_BODY UINT_32               dr_control_mip;

// BOOM CONTROL
M_DR_BODY BOOL                  dr_control_bloom_enable;
M_DR_BODY BOOL                  dr_control_bloom_enable_vblur;
M_DR_BODY BOOL                  dr_control_bloom_enable_hblur;
M_DR_BODY UINT_32               dr_control_bloom_res;
M_DR_BODY FLOAT_32              dr_control_bloom_strength;

// SHADOWING CONTROL
M_DR_BODY UINT_32               dr_control_sun_res;
M_DR_BODY UINT_32               dr_control_sun_splits;
M_DR_BODY FLOAT_32              dr_control_sun_offset;
M_DR_BODY FLOAT_32              dr_control_sun_transition;
M_DR_BODY FLOAT_32              dr_control_sun_distribution;
M_DR_BODY FLOAT_32              dr_control_sun_scheme;
M_DR_BODY FLOAT_32              dr_control_sun_zoom;
M_DR_BODY BOOL                  dr_control_sun_debug;

// IMAGE CONTROL
M_DR_BODY FLOAT_32              dr_control_image_desaturation;
M_DR_BODY FLOAT_32              dr_control_image_brightness;
M_DR_BODY FLOAT_32              dr_control_image_contrast;

// SKY CONTROL
M_DR_BODY FLOAT_32              dr_control_sky_desaturation;
M_DR_BODY FLOAT_32              dr_control_sky_brightness;
M_DR_BODY FLOAT_32              dr_control_sky_contrast;

// HDR CONTROL
M_DR_BODY FLOAT_32              dr_control_hdr_exposure;			                    // exposure balanced level
M_DR_BODY FLOAT_32              dr_control_hdr_exposure_scale_min;	                    // exposure scale minimum boundary
M_DR_BODY FLOAT_32              dr_control_hdr_exposure_scale_max;	                    // exposure scale maximum boundary
M_DR_BODY FLOAT_32              dr_control_hdr_exposure_speed;		                    // speed of exposure adaptation

// DETAIL CONTROL
M_DR_BODY UINT_32               dr_detailculling;

// DEBUG

M_DR_BODY UINT_32               dr_debug_view;
M_DR_BODY UINT_32               dr_debug_shadow;
M_DR_BODY UINT_32               dr_debug_missed;
M_DR_BODY UINT_32               dr_debug_culled;

#ifdef M_DEBUG

M_DR_BODY FLOAT_64              dr_debug_profile_sort;
M_DR_BODY FLOAT_64              dr_debug_profile_sortg;

M_DR_BODY FLOAT_64              dr_debug_profile_clip;
M_DR_BODY FLOAT_64              dr_debug_profile_clipg;

M_DR_BODY FLOAT_64              dr_debug_profile_occlusion;
M_DR_BODY FLOAT_64              dr_debug_profile_occlusiong;

M_DR_BODY FLOAT_64              dr_debug_profile_detail;
M_DR_BODY FLOAT_64              dr_debug_profile_detailg;

M_DR_BODY FLOAT_64              dr_debug_profile_shadows;
M_DR_BODY FLOAT_64              dr_debug_profile_shadowsg;

M_DR_BODY FLOAT_64              dr_debug_profile_gbuffers;
M_DR_BODY FLOAT_64              dr_debug_profile_gbuffersg;

M_DR_BODY FLOAT_64              dr_debug_profile_depth;
M_DR_BODY FLOAT_64              dr_debug_profile_depthg;

M_DR_BODY FLOAT_64              dr_debug_profile_sun;
M_DR_BODY FLOAT_64              dr_debug_profile_sung;

M_DR_BODY FLOAT_64              dr_debug_profile_aa;
M_DR_BODY FLOAT_64              dr_debug_profile_aag;

M_DR_BODY FLOAT_64              dr_debug_profile_fog;
M_DR_BODY FLOAT_64              dr_debug_profile_fogg;

M_DR_BODY FLOAT_64              dr_debug_profile_ssao;
M_DR_BODY FLOAT_64              dr_debug_profile_ssaog;

M_DR_BODY FLOAT_64              dr_debug_profile_tonemap;
M_DR_BODY FLOAT_64              dr_debug_profile_tonemapg;

M_DR_BODY FLOAT_64              dr_debug_profile_enviroment;
M_DR_BODY FLOAT_64              dr_debug_profile_enviromentg;

#endif

// ENVIROMENT

M_DR_BODY UINT_32P              dr_w;
M_DR_BODY UINT_32P              dr_h;

M_DR_BODY UINT_32               dr_width;
M_DR_BODY UINT_32               dr_height;

M_DR_BODY UINT_32               dr_framebuffer;
M_DR_BODY UINT_32               dr_renderbuffer;

M_DR_BODY UINT_32               dr_framebuffer_sun;
M_DR_BODY UINT_32               dr_renderbuffer_sun;

M_DR_BODY FLOAT_32              dr_matrixp [16];
M_DR_BODY FLOAT_32              dr_matrixv [16];

M_DR_BODY UINT_32               dr_fullscreen;

// WORLD BBOX

M_DR_BODY FLOAT_32              dr_world_min [3];
M_DR_BODY FLOAT_32              dr_world_max [3];

// FRUSTUM

M_DR_BODY INT_32                dr_maxplanes;

M_DR_BODY FLOAT_32              dr_planenear;
M_DR_BODY FLOAT_32              dr_planefar;

M_DR_BODY FLOAT_64              dr_nearw;
M_DR_BODY FLOAT_64              dr_nearh;

M_DR_BODY FLOAT_64              dr_farw;
M_DR_BODY FLOAT_64              dr_farh;

M_DR_BODY FLOAT_64              dr_fovy;
M_DR_BODY FLOAT_64              dr_fovyr;

M_DR_BODY FLOAT_64              dr_factor;
M_DR_BODY FLOAT_64              dr_aspect;

M_DR_BODY FLOAT_64              dr_avertical;

M_DR_BODY FLOAT_64              dr_origin [3];

// SUN

M_DR_BODY FLOAT_64P             dr_sun_split_nearh;
M_DR_BODY FLOAT_64P             dr_sun_split_nearw;
M_DR_BODY FLOAT_64P             dr_sun_split_farh;
M_DR_BODY FLOAT_64P             dr_sun_split_farw;

M_DR_BODY UINT_32               dr_sun_frustum_planesc;
M_DR_BODY FLOAT_64              dr_sun_frustum_planesd  [12][4];
M_DR_BODY FLOAT_32              dr_sun_frustum_planes   [12][4];

M_DR_BODY UINT_32P              dr_sun_split_planesc;
M_DR_BODY FLOAT_64P             dr_sun_split_planesd;
M_DR_BODY FLOAT_32P             dr_sun_split_planes;
M_DR_BODY FLOAT_64P             dr_sun_split_points;
M_DR_BODY UINT_8P               dr_sun_split_clip;

M_DR_BODY FLOAT_32              dr_sun_split_finish;                                    // end of all shadows (depth)
M_DR_BODY FLOAT_64P             dr_sun_split_start;                                     // projection centers distances
M_DR_BODY FLOAT_64P             dr_sun_split_end;                                       // projection centers distances

M_DR_BODY FLOAT_64              dr_sun_planefar;                                        // defines ortographics projection
M_DR_BODY FLOAT_64P             dr_sun_planesside;                                      // defines ortographics projection

M_DR_BODY FLOAT_64P             dr_sun_view;                                            // view matrices for each split
M_DR_BODY FLOAT_64P             dr_sun_projection;                                      // projection matrices for each split
M_DR_BODY FLOAT_64P             dr_sun_transitions;                                     // transition size for each split

M_DR_BODY FLOAT_32P             dr_sun_matrices;                                        // matrices for backward transformation in shader
M_DR_BODY FLOAT_32P             dr_sun_depthmin;                                        // minimum linear depth of split
M_DR_BODY FLOAT_32P             dr_sun_depthmax;                                        // maximum linear depth of split
M_DR_BODY FLOAT_32P             dr_sun_offsets;                                         // sampling depth offsets (shadowmapping)

M_DR_BODY FLOAT_64              dr_sun_distance;                                        // sun distance from 0,0,0

M_DR_BODY FLOAT_64              dr_sun_directionx  [3];                                 // view space of the sun, x axis vector
M_DR_BODY FLOAT_64              dr_sun_directiony  [3];                                 // view space of the sun, y axis vector
M_DR_BODY FLOAT_64              dr_sun_directionz  [3];                                 // view space of the sun, z axis vector

M_DR_BODY FLOAT_32              dr_sun_intensity;                                       // sun intensity
M_DR_BODY FLOAT_32              dr_sun_ambient;                                         // sun ambient intensity
M_DR_BODY FLOAT_32              dr_sun_color       [3];                                 // sun color

// SPATIAL STRUCTURE

M_DR_BODY TTree                 dr_tree;

// CLIPPING PLANES

M_DR_BODY FLOAT_32              dr_frustum_planes [4][4];                               // only righ, left, top, bottom planes

// TIME STAMP

M_DR_BODY UINT_32               dr_stamp;

// DRAW LISTS

M_DR_BODY UINT_32               dr_quadpot;
M_DR_BODY UINT_32P              dr_quads;
M_DR_BODY UINT_32               dr_quad;

// TEXTURES

M_DR_BODY UINT_32P              dr_sunshadows;
M_DR_BODY UINT_32               dr_sunshadowtmp;

M_DR_BODY UINT_32               dr_auxhdr1;
M_DR_BODY UINT_32               dr_auxhdr2;
M_DR_BODY UINT_32P              dr_auxA;
M_DR_BODY UINT_32P              dr_auxB;

M_DR_BODY UINT_32               dr_depth;
M_DR_BODY UINT_32               dr_rand;
M_DR_BODY UINT_32               dr_text;
M_DR_BODY UINT_32               dr_G1;
M_DR_BODY UINT_32               dr_G2;
M_DR_BODY UINT_32               dr_G3;

// GENERAL SHADERS

M_DR_BODY UINT_32               dr_program_enviroment;
M_DR_BODY UINT_32               dr_program_enviromentv;
M_DR_BODY UINT_32               dr_program_enviromentf;

M_DR_BODY UINT_32               dr_program_sun;
M_DR_BODY UINT_32               dr_program_sunv;
M_DR_BODY UINT_32               dr_program_sunf;
M_DR_BODY UINT_32               dr_program_sun_matrix;

M_DR_BODY UINT_32               dr_program_fog;
M_DR_BODY UINT_32               dr_program_fogv;
M_DR_BODY UINT_32               dr_program_fogf;

// DEBUG SHADERS

M_DR_BODY UINT_32               dr_program_debug_shadow;
M_DR_BODY UINT_32               dr_program_debug_shadowv;
M_DR_BODY UINT_32               dr_program_debug_shadowf;

// SPECIFIC SHADERS

// SOLID

M_DR_BODY UINT_32               dr_program_solid [256];

// TERRAIN

M_DR_BODY UINT_32               dr_program_terrain_gbuffers_normal;
M_DR_BODY UINT_32               dr_program_terrain_gbuffers_normalv;
M_DR_BODY UINT_32               dr_program_terrain_gbuffers_normalf;

M_DR_BODY UINT_32               dr_program_terrain_gbuffers_morph;
M_DR_BODY UINT_32               dr_program_terrain_gbuffers_morphv;
M_DR_BODY UINT_32               dr_program_terrain_gbuffers_morphf;
M_DR_BODY UINT_32               dr_program_terrain_gbuffers_morph_gloss;
M_DR_BODY UINT_32               dr_program_terrain_gbuffers_morph_shininess;

M_DR_BODY UINT_32               dr_program_terrain_depth_normal;
M_DR_BODY UINT_32               dr_program_terrain_depth_normalv;
M_DR_BODY UINT_32               dr_program_terrain_depth_normalf;

M_DR_BODY UINT_32               dr_program_terrain_depth_morph;
M_DR_BODY UINT_32               dr_program_terrain_depth_morphv;
M_DR_BODY UINT_32               dr_program_terrain_depth_morphf;

// FOLIAGE

M_DR_BODY UINT_32				dr_program_foliage [256];

// GRASS

M_DR_BODY UINT_32               dr_program_grass_gbuffers_normal;
M_DR_BODY UINT_32               dr_program_grass_gbuffers_normalv;
M_DR_BODY UINT_32               dr_program_grass_gbuffers_normalf;

M_DR_BODY UINT_32               dr_program_grass_depth_normal;
M_DR_BODY UINT_32               dr_program_grass_depth_normalv;
M_DR_BODY UINT_32               dr_program_grass_depth_normalf;

// AA SHADERS

M_DR_BODY UINT_32               dr_program_aa_blurvert;
M_DR_BODY UINT_32               dr_program_aa_blurvertv;
M_DR_BODY UINT_32               dr_program_aa_blurvertf;

M_DR_BODY UINT_32               dr_program_aa_blurhoriz;
M_DR_BODY UINT_32               dr_program_aa_blurhorizv;
M_DR_BODY UINT_32               dr_program_aa_blurhorizf;

// HDR SHADERS

M_DR_BODY UINT_32               dr_program_hdr;
M_DR_BODY UINT_32               dr_program_hdrv;
M_DR_BODY UINT_32               dr_program_hdrf;

M_DR_BODY UINT_32               dr_program_hdr_blurvert;
M_DR_BODY UINT_32               dr_program_hdr_blurvertv;
M_DR_BODY UINT_32               dr_program_hdr_blurvertf;

M_DR_BODY UINT_32               dr_program_hdr_blurhoriz;
M_DR_BODY UINT_32               dr_program_hdr_blurhorizv;
M_DR_BODY UINT_32               dr_program_hdr_blurhorizf;

M_DR_BODY UINT_32               dr_program_hdr_hipass;
M_DR_BODY UINT_32               dr_program_hdr_hipassv;
M_DR_BODY UINT_32               dr_program_hdr_hipassf;

// SSAO SHADERS

M_DR_BODY UINT_32               dr_program_ssao;
M_DR_BODY UINT_32               dr_program_ssaov;
M_DR_BODY UINT_32               dr_program_ssaof;

M_DR_BODY UINT_32               dr_program_ssao_blurvert;
M_DR_BODY UINT_32               dr_program_ssao_blurvertv;
M_DR_BODY UINT_32               dr_program_ssao_blurvertf;

M_DR_BODY UINT_32               dr_program_ssao_blurhblend;
M_DR_BODY UINT_32               dr_program_ssao_blurhblendv;
M_DR_BODY UINT_32               dr_program_ssao_blurhblendf;

// STATES

M_DR_BODY UINT_32               dr_state_tex0;
M_DR_BODY UINT_32               dr_state_tex1;
M_DR_BODY UINT_32               dr_state_tex2;
M_DR_BODY UINT_32               dr_state_tex3;
M_DR_BODY UINT_32               dr_state_tex4;
M_DR_BODY UINT_32               dr_state_tex5;
M_DR_BODY UINT_32               dr_state_tex6;
M_DR_BODY UINT_32               dr_state_tex7;
M_DR_BODY UINT_32               dr_state_tex8;
M_DR_BODY UINT_32               dr_state_tex9;
M_DR_BODY UINT_32               dr_state_tex10;
M_DR_BODY UINT_32               dr_state_tex11;
M_DR_BODY UINT_32               dr_state_tex12;
M_DR_BODY UINT_32               dr_state_tex13;
M_DR_BODY UINT_32               dr_state_tex14;
M_DR_BODY UINT_32               dr_state_tex15;
M_DR_BODY UINT_32               dr_state_tex16;

M_DR_BODY UINT_32               dr_state_tex0_rect;
M_DR_BODY UINT_32               dr_state_tex1_rect;
M_DR_BODY UINT_32               dr_state_tex2_rect;
M_DR_BODY UINT_32               dr_state_tex3_rect;

M_DR_BODY UINT_32               dr_state_tex0_coord;
M_DR_BODY UINT_32               dr_state_tex1_coord;
M_DR_BODY UINT_32               dr_state_tex2_coord;

M_DR_BODY UINT_32               dr_state_planes [M_DR_MAX_CLIP_PLANES];

M_DR_BODY UINT_32               dr_state_color;
M_DR_BODY UINT_32               dr_state_colors;
M_DR_BODY UINT_32               dr_state_normal;
M_DR_BODY UINT_32               dr_state_vertex;
M_DR_BODY UINT_32               dr_state_indices;
M_DR_BODY UINT_32               dr_state_cullface;
M_DR_BODY UINT_32               dr_state_depthtest;
M_DR_BODY UINT_32               dr_state_depthmask;
M_DR_BODY UINT_32               dr_state_matrixmode;
M_DR_BODY UINT_32               dr_state_shader;

// AUTOEXPOSURE

M_DR_BODY FLOAT_32              dr_intensityscale;
M_DR_BODY UINT_32               dr_intensitysize;
M_DR_BODY UINT_8P               dr_intensitydata;

// RADIX

M_DR_BODY TRadix                dr_radix;
M_DR_BODY TRadix                dr_radix_instanced;

// CONTEXT

M_DR_BODY TContext              dr_context;
M_DR_BODY TContextModel       * dr_context_models;
M_DR_BODY TContextMaterial    * dr_context_materials;

// CONTENT

M_DR_BODY UINT_32               dr_materialsc;
M_DR_BODY UINT_32               dr_objectsc;
M_DR_BODY UINT_32               dr_modelsc;

M_DR_BODY TMaterial           * dr_materials;
M_DR_BODY TObject             * dr_objects;
M_DR_BODY TModel              * dr_models;

// ENVIROMENT

M_DR_BODY TMaterial *           dr_enviroment_material;
M_DR_BODY TModel    *           dr_enviroment_model;

// FOG

M_DR_BODY FLOAT_32              dr_fog_color [3];

// NODES

M_DR_BODY UINT_32               dr_nodesc;
M_DR_BODY TNode               * dr_nodes;
M_DR_BODY TNode               * dr_nodestmp;

// CHAINS

M_DR_BODY UINT_32               dr_chainsc;
M_DR_BODY TChain              * dr_chains;
M_DR_BODY TChain              * dr_chainstmp;

// OBJECT PROPERTIES

// tags

M_DR_BODY UINT_16P              dr_object_tags;

/// RECYCLE TEMPORARY DATA (UINT_32 ..)
///#	M_DR_BODY UINT_32P              dr_object_stamps
///#	M_DR_BODY UINT_32P              dr_object_distances_sort
///#	M_DR_BODY UINT_32P              dr_list_objects1
///#	M_DR_BODY UINT_32P              dr_list_objects2
///#	M_DR_BODY UINT_32P              dr_list_objects3
///#	M_DR_BODY UINT_32P              dr_list_objects4
///#	M_DR_BODY UINT_32P              dr_list_objects_view
///#	M_DR_BODY UINT_32P              dr_list_objects_shadow_split    [M_DR_SUN_SPLITS];

// disappear distances

M_DR_BODY FLOAT_32P             dr_object_disappear_end;                                // dissapearing distances (end)
M_DR_BODY FLOAT_32P             dr_object_disappear_start;                              // dissapearing distances (start)
M_DR_BODY UINT_8P               dr_object_disappear_split;                              // shadow dissapearing split index
M_DR_BODY FLOAT_32P             dr_object_disappear_shadow;                             // shadow dissapearing distances

// distances

M_DR_BODY FLOAT_32P             dr_object_distances;                                    // linear distances from camera
M_DR_BODY FLOAT_32P             dr_object_distancesq;                                   // quadratic distances from camera
M_DR_BODY UINT_32P              dr_object_distances_sort;                               /// temporary array for sorting

// other

M_DR_BODY UINT_16P              dr_object_flags;                                        // flags
M_DR_BODY TDetail             * dr_object_details;                                      // details
M_DR_BODY TBoundary           * dr_object_boundaries;                                   // boundaries
M_DR_BODY UINT_16P              dr_object_queries;                                      // occlusion queries
M_DR_BODY UINT_16P              dr_object_models;                                       // object models
M_DR_BODY UINT_16P              dr_object_materials;                                    // object materuals
M_DR_BODY FLOAT_32P             dr_object_morph1;                                       // morphing param1
M_DR_BODY FLOAT_32P             dr_object_morph2;                                       // morphing param2
M_DR_BODY FLOAT_32P             dr_object_transforms;                                   // transformations
M_DR_BODY FLOAT_32P             dr_object_centers;                                      // centers
M_DR_BODY UINT_8P               dr_object_depths;                                       // depths


// instances

M_DR_BODY UINT_16P              dr_object_instances;		                            // count of instances per object
M_DR_BODY FLOAT_32PP            dr_object_instances_rand;	                            // random factors per instance
M_DR_BODY FLOAT_32PP            dr_object_instances_transforms;	                        // pointers to transforms array

// MODEL PROPERTIES

M_DR_BODY UINT_16P              dr_model_occlusions;                                    // occlusion models
M_DR_BODY UINT_8P               dr_model_shaders;                                       // shader types
M_DR_BODY UINT_32P              dr_model_stamps;                                        // stamps
M_DR_BODY UINT_16P              dr_model_flags;                                         // flags

// MATERIAL PROPERTIES

M_DR_BODY UINT_16P              dr_material_counters;
M_DR_BODY UINT_32P              dr_material_stamps;

// OBJECT LISTS

M_DR_BODY UINT_32P              dr_list_objects1;
M_DR_BODY UINT_32P              dr_list_objects2;
M_DR_BODY UINT_32P              dr_list_objects3;
M_DR_BODY UINT_32P              dr_list_objects4;

M_DR_BODY UINT_32P              dr_list_objects_view;
M_DR_BODY UINT_32               dr_list_objects_viewc;

M_DR_BODY UINT_32PP             dr_list_objects_shadow_split;
M_DR_BODY UINT_32P              dr_list_objects_shadow_splitc;

#endif