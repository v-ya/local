#ifndef _iphyee_glsl_h_
#define _iphyee_glsl_h_

#include <stdint.h>
#include "iphyee.mat4x4.h"

typedef uint64_t iphyee_glslc_pointer_t;
typedef struct iphyee_glslc_vec3_xyz_t iphyee_glslc_vec3_xyz_t;
typedef struct iphyee_glslc_vec2_uv_t iphyee_glslc_vec2_uv_t;
typedef struct iphyee_glslc_vec3_bgr_t iphyee_glslc_vec3_bgr_t;
typedef struct iphyee_glslc_index_fusion_t iphyee_glslc_index_fusion_t;
typedef struct iphyee_glslc_array_pixel_t iphyee_glslc_array_pixel_t;
typedef struct iphyee_glslc_array_vertex_t iphyee_glslc_array_vertex_t;
typedef struct iphyee_glslc_array_texture_t iphyee_glslc_array_texture_t;
typedef struct iphyee_glslc_array_normal_t iphyee_glslc_array_normal_t;
typedef struct iphyee_glslc_array_fusion_t iphyee_glslc_array_fusion_t;
typedef struct iphyee_glslc_image_t iphyee_glslc_image_t;
typedef struct iphyee_glslc_texture_pool_t iphyee_glslc_texture_pool_t;
typedef struct iphyee_glslc_model_data_t iphyee_glslc_model_data_t;
typedef struct iphyee_glslc_model_count_t iphyee_glslc_model_count_t;
typedef struct iphyee_glslc_model_t iphyee_glslc_model_t;
typedef struct iphyee_glslc_tri3_box_t iphyee_glslc_tri3_box_t;
typedef struct iphyee_glslc_tri3_inside_t iphyee_glslc_tri3_inside_t;
typedef struct iphyee_glslc_tri3_depth_t iphyee_glslc_tri3_depth_t;
typedef struct iphyee_glslc_tri3_render_t iphyee_glslc_tri3_render_t;

typedef iphyee_glslc_pointer_t iphyee_glslc_pointer_uint_t;
typedef iphyee_glslc_pointer_t iphyee_glslc_pointer_vec3_xyz_t;
typedef iphyee_glslc_pointer_t iphyee_glslc_pointer_vec2_uv_t;
typedef iphyee_glslc_pointer_t iphyee_glslc_pointer_index_fusion_t;
typedef iphyee_glslc_pointer_t iphyee_glslc_pointer_image_t;
typedef iphyee_glslc_pointer_t iphyee_glslc_pointer_texture_pool_t;
typedef iphyee_glslc_pointer_t iphyee_glslc_pointer_model_t;
typedef iphyee_glslc_pointer_t iphyee_glslc_pointer_tri3_render_t;

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

struct iphyee_glslc_index_fusion_t {
	uint32_t vertex;
	uint32_t texture;
	uint32_t normal;
};

struct iphyee_glslc_array_pixel_t {
	iphyee_glslc_pointer_uint_t pixel;
};

struct iphyee_glslc_array_vertex_t {
	iphyee_glslc_pointer_vec3_xyz_t vertex;
};

struct iphyee_glslc_array_texture_t {
	iphyee_glslc_pointer_vec2_uv_t texture;
};

struct iphyee_glslc_array_normal_t {
	iphyee_glslc_pointer_vec3_xyz_t normal;
};

struct iphyee_glslc_array_fusion_t {
	iphyee_glslc_pointer_index_fusion_t fusion;
};

struct iphyee_glslc_image_t {
	iphyee_glslc_array_pixel_t pixels;
	uint32_t width;
	uint32_t height;
};

struct iphyee_glslc_texture_pool_t {
	iphyee_glslc_pointer_image_t texture;
};

struct iphyee_glslc_model_data_t {
	iphyee_glslc_array_vertex_t data_vertex;
	iphyee_glslc_array_texture_t data_texture;
	iphyee_glslc_array_normal_t data_normal;
	iphyee_glslc_array_fusion_t data_fusion;
};

struct iphyee_glslc_model_count_t {
	uint32_t count_vertex;
	uint32_t count_texture;
	uint32_t count_normal;
	uint32_t count_fusion;
};

struct iphyee_glslc_model_t {
	iphyee_mat4x4_t transform;
	iphyee_glslc_model_data_t data;
	iphyee_glslc_model_count_t count;
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
	float scale_k1;
	float scale_k2;
};

struct iphyee_glslc_tri3_depth_t {
	float depth_0;
	float depth_1;
	float depth_2;
};

struct iphyee_glslc_tri3_render_t {
	iphyee_glslc_pointer_model_t model;
	iphyee_glslc_tri3_box_t box;
	iphyee_glslc_tri3_inside_t inside;
	iphyee_glslc_tri3_depth_t depth;
	uint32_t fusion_index;
};

#endif
