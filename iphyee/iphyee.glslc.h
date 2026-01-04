#ifndef _iphyee_glsl_h_
#define _iphyee_glsl_h_

#include <stdint.h>
#include "iphyee.mat4x4.h"

typedef uint64_t iphyee_glslc_pointer_t;
// model
typedef struct iphyee_glslc_model_vec3xyz_t iphyee_glslc_model_vec3xyz_t;
// bvh
typedef struct iphyee_glslc_bvh_pos_t iphyee_glslc_bvh_pos_t;
typedef struct iphyee_glslc_bvh_box_t iphyee_glslc_bvh_box_t;
typedef struct iphyee_glslc_bvh_refer_t iphyee_glslc_bvh_refer_t;

// bvh
typedef iphyee_glslc_pointer_t iphyee_glslc_pointer_bvh_refer_t;

// model
struct iphyee_glslc_model_vec3xyz_t {
	float x;
	float y;
	float z;
};

// bvh
struct iphyee_glslc_bvh_pos_t {
	float x;
	float y;
	float z;
};
struct iphyee_glslc_bvh_box_t {
	iphyee_glslc_bvh_pos_t p1;
	iphyee_glslc_bvh_pos_t p2;
};
struct iphyee_glslc_bvh_refer_t {
	iphyee_glslc_pointer_bvh_refer_t parent;
	iphyee_glslc_pointer_bvh_refer_t next;
	iphyee_glslc_pointer_bvh_refer_t child;
	iphyee_glslc_bvh_box_t box;
	iphyee_glslc_pointer_t target;
	uint32_t tri3index;
	int32_t layer;
};

/// TODO: need remove old version

typedef struct iphyee_glslc_vec3_xyz_t iphyee_glslc_vec3_xyz_t;
typedef struct iphyee_glslc_vec2_uv_t iphyee_glslc_vec2_uv_t;
typedef struct iphyee_glslc_vec3_bgr_t iphyee_glslc_vec3_bgr_t;
typedef struct iphyee_glslc_joint_weight_t iphyee_glslc_joint_weight_t;
typedef struct iphyee_glslc_joint_effect_t iphyee_glslc_joint_effect_t;
typedef struct iphyee_glslc_index_fusion_t iphyee_glslc_index_fusion_t;
typedef struct iphyee_glslc_tri3_fusion_t iphyee_glslc_tri3_fusion_t;
typedef struct iphyee_glslc_array_pixel_t iphyee_glslc_array_pixel_t;
typedef struct iphyee_glslc_array_depth_t iphyee_glslc_array_depth_t;
typedef struct iphyee_glslc_array_vertex_t iphyee_glslc_array_vertex_t;
typedef struct iphyee_glslc_array_texture_t iphyee_glslc_array_texture_t;
typedef struct iphyee_glslc_array_normal_t iphyee_glslc_array_normal_t;
typedef struct iphyee_glslc_array_weight_t iphyee_glslc_array_weight_t;
typedef struct iphyee_glslc_array_effect_t iphyee_glslc_array_effect_t;
typedef struct iphyee_glslc_array_fusion_t iphyee_glslc_array_fusion_t;
typedef struct iphyee_glslc_array_tri3_fusion_t iphyee_glslc_array_tri3_fusion_t;
typedef struct iphyee_glslc_array_index_t iphyee_glslc_array_index_t;
typedef struct iphyee_glslc_image_t iphyee_glslc_image_t;
typedef struct iphyee_glslc_texture_pool_t iphyee_glslc_texture_pool_t;
typedef struct iphyee_glslc_model_data_t iphyee_glslc_model_data_t;
typedef struct iphyee_glslc_model_count_t iphyee_glslc_model_count_t;
typedef struct iphyee_glslc_model_inst_t iphyee_glslc_model_inst_t;
typedef struct iphyee_glslc_model_t iphyee_glslc_model_t;
typedef struct iphyee_glslc_tri3_box_t iphyee_glslc_tri3_box_t;
typedef struct iphyee_glslc_tri3_inside_t iphyee_glslc_tri3_inside_t;
typedef struct iphyee_glslc_tri3_depth_t iphyee_glslc_tri3_depth_t;
typedef struct iphyee_glslc_tri3_index_t iphyee_glslc_tri3_index_t;
typedef struct iphyee_glslc_tri3_render_t iphyee_glslc_tri3_render_t;
typedef struct iphyee_glslc_array_tri3_render_t iphyee_glslc_array_tri3_render_t;
typedef struct iphyee_glslc_index_list_t iphyee_glslc_index_list_t;
typedef struct iphyee_glslc_array_index_list_t iphyee_glslc_array_index_list_t;
typedef struct iphyee_glslc_rasterize_block_t iphyee_glslc_rasterize_block_t;
typedef struct iphyee_glslc_rasterize_t iphyee_glslc_rasterize_t;

typedef iphyee_glslc_pointer_t iphyee_glslc_pointer_array_pixel_t;
typedef iphyee_glslc_pointer_t iphyee_glslc_pointer_array_depth_t;
typedef iphyee_glslc_pointer_t iphyee_glslc_pointer_array_vertex_t;
typedef iphyee_glslc_pointer_t iphyee_glslc_pointer_array_texture_t;
typedef iphyee_glslc_pointer_t iphyee_glslc_pointer_array_normal_t;
typedef iphyee_glslc_pointer_t iphyee_glslc_pointer_array_weight_t;
typedef iphyee_glslc_pointer_t iphyee_glslc_pointer_array_effect_t;
typedef iphyee_glslc_pointer_t iphyee_glslc_pointer_array_fusion_t;
typedef iphyee_glslc_pointer_t iphyee_glslc_pointer_array_tri3_fusion_t;
typedef iphyee_glslc_pointer_t iphyee_glslc_pointer_array_index_t;
typedef iphyee_glslc_pointer_t iphyee_glslc_pointer_texture_pool_t;
typedef iphyee_glslc_pointer_t iphyee_glslc_pointer_model_t;
typedef iphyee_glslc_pointer_t iphyee_glslc_pointer_array_tri3_render_t;
typedef iphyee_glslc_pointer_t iphyee_glslc_pointer_array_index_list_t;
typedef iphyee_glslc_pointer_t iphyee_glslc_pointer_rasterize_block_t;
typedef iphyee_glslc_pointer_t iphyee_glslc_pointer_rasterize_t;

struct iphyee_glslc_vec3_xyz_t {
	float x;
	float y;
	float z;
};

struct iphyee_glslc_vec2_uv_t {
	float u;
	float v;
};

struct iphyee_glslc_vec3_bgr_t {
	float b;
	float g;
	float r;
};

struct iphyee_glslc_joint_weight_t {
	uint32_t joint_index;
	float joint_weight;
};

struct iphyee_glslc_joint_effect_t {
	uint32_t weight_offset;
	uint32_t weight_number;
};

struct iphyee_glslc_index_fusion_t {
	uint32_t vertex;
	uint32_t texture;
	uint32_t normal;
	uint32_t effect;
};

struct iphyee_glslc_tri3_fusion_t {
	iphyee_glslc_vec3_xyz_t vertex_0;
	iphyee_glslc_vec3_xyz_t vertex_1;
	iphyee_glslc_vec3_xyz_t vertex_2;
	iphyee_glslc_vec3_xyz_t normal_0;
	iphyee_glslc_vec3_xyz_t normal_1;
	iphyee_glslc_vec3_xyz_t normal_2;
};

struct iphyee_glslc_array_pixel_t {
	uint32_t pixel[0];
};

struct iphyee_glslc_array_depth_t {
	float depth[0];
};

struct iphyee_glslc_array_vertex_t {
	iphyee_glslc_vec3_xyz_t vertex[0];
};

struct iphyee_glslc_array_texture_t {
	iphyee_glslc_vec2_uv_t texture[0];
};

struct iphyee_glslc_array_normal_t {
	iphyee_glslc_vec3_xyz_t normal[0];
};

struct iphyee_glslc_array_weight_t {
	iphyee_glslc_joint_weight_t weight[0];
};

struct iphyee_glslc_array_effect_t {
	iphyee_glslc_joint_effect_t effect[0];
};

struct iphyee_glslc_array_fusion_t {
	iphyee_glslc_index_fusion_t fusion[0];
};

struct iphyee_glslc_array_tri3_fusion_t {
	iphyee_glslc_tri3_fusion_t tri3[0];
};

struct iphyee_glslc_array_index_t {
	uint32_t index[0];
};

struct iphyee_glslc_image_t {
	iphyee_glslc_pointer_array_pixel_t pixel;
	uint32_t width;
	uint32_t height;
};

struct iphyee_glslc_texture_pool_t {
	iphyee_glslc_image_t texture[0];
};

struct iphyee_glslc_model_data_t {
	iphyee_glslc_pointer_array_vertex_t data_vertex;
	iphyee_glslc_pointer_array_texture_t data_texture;
	iphyee_glslc_pointer_array_normal_t data_normal;
	iphyee_glslc_pointer_array_weight_t data_weight;
	iphyee_glslc_pointer_array_effect_t data_effect;
	iphyee_glslc_pointer_array_fusion_t data_fusion;
};

struct iphyee_glslc_model_count_t {
	uint32_t count_vertex;
	uint32_t count_texture;
	uint32_t count_normal;
	uint32_t count_weight;
	uint32_t count_effect;
	uint32_t count_fusion;
};

struct iphyee_glslc_model_inst_t {
	iphyee_glslc_model_data_t data;
	iphyee_glslc_model_count_t count;
};

struct iphyee_glslc_model_t {
	iphyee_glslc_model_inst_t inst;
	iphyee_glslc_pointer_array_tri3_fusion_t transform_fusion;
	uint32_t fusion_offset;
	uint32_t fusion_number;
	uint32_t texture_index;
	uint32_t texture_alpha;
};

struct iphyee_glslc_tri3_box_t {
	uint32_t xmin;
	uint32_t xmax;
	uint32_t ymin;
	uint32_t ymax;
};

struct iphyee_glslc_tri3_inside_t {
	float inverse_r0c0;
	float inverse_r0c1;
	float inverse_r1c0;
	float inverse_r1c1;
	float base_at_x;
	float base_at_y;
};

struct iphyee_glslc_tri3_depth_t {
	float depth_0;
	float depth_1;
	float depth_2;
};

struct iphyee_glslc_tri3_index_t {
	uint32_t index_0;
	uint32_t index_1;
	uint32_t index_2;
};

struct iphyee_glslc_tri3_render_t {
	iphyee_glslc_pointer_model_t model;
	iphyee_glslc_tri3_inside_t inside;
	iphyee_glslc_tri3_depth_t depth;
	uint32_t fusion_index;
};

struct iphyee_glslc_array_tri3_render_t {
	iphyee_glslc_tri3_render_t tri3[0];
};

struct iphyee_glslc_index_list_t {
	uint32_t next;
	uint32_t index;
};

struct iphyee_glslc_array_index_list_t {
	iphyee_glslc_index_list_t list[0];
};

struct iphyee_glslc_rasterize_block_t {
	uint32_t list_next[0];
};

struct iphyee_glslc_rasterize_t {
	iphyee_glslc_image_t image;
	iphyee_glslc_pointer_array_depth_t depth;
	iphyee_glslc_pointer_texture_pool_t tpool;
	iphyee_glslc_pointer_array_tri3_render_t tri3;
	iphyee_glslc_pointer_array_index_list_t list;
	iphyee_glslc_pointer_rasterize_block_t block_normal;
	iphyee_glslc_pointer_rasterize_block_t block_alpha;
	uint32_t tri3_max_count;
	uint32_t tri3_cur_count;
	uint32_t list_max_count;
	uint32_t list_cur_count;
	uint32_t block_width;
	uint32_t block_height;
	uint32_t block_h_count;
	uint32_t block_v_count;
};

#endif
