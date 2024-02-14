#ifndef _iphyee_glsl_h_
#define _iphyee_glsl_h_

#include <stdint.h>
#include "iphyee.mat4x4.h"

typedef uint64_t iphyee_glslc_pointer_t;
typedef struct iphyee_glslc_vec3_xyz_t iphyee_glslc_vec3_xyz_t;
typedef struct iphyee_glslc_vec2_uv_t iphyee_glslc_vec2_uv_t;
typedef struct iphyee_glslc_index_fusion_t iphyee_glslc_index_fusion_t;
typedef struct iphyee_glslc_array_pixel_t iphyee_glslc_array_pixel_t;
typedef struct iphyee_glslc_array_vertex_t iphyee_glslc_array_vertex_t;
typedef struct iphyee_glslc_array_texture_t iphyee_glslc_array_texture_t;
typedef struct iphyee_glslc_array_normal_t iphyee_glslc_array_normal_t;
typedef struct iphyee_glslc_array_fusion_t iphyee_glslc_array_fusion_t;
typedef struct iphyee_glslc_image_t iphyee_glslc_image_t;
typedef struct iphyee_glslc_model_data_t iphyee_glslc_model_data_t;
typedef struct iphyee_glslc_model_count_t iphyee_glslc_model_count_t;
typedef struct iphyee_glslc_model_t iphyee_glslc_model_t;

typedef iphyee_glslc_pointer_t iphyee_glslc_pointer_uint_t;
typedef iphyee_glslc_pointer_t iphyee_glslc_pointer_vec3_xyz_t;
typedef iphyee_glslc_pointer_t iphyee_glslc_pointer_vec2_uv_t;
typedef iphyee_glslc_pointer_t iphyee_glslc_pointer_index_fusion_t;
typedef iphyee_glslc_pointer_t iphyee_glslc_pointer_image_t;
typedef iphyee_glslc_pointer_t iphyee_glslc_pointer_model_t;

struct iphyee_glslc_vec3_xyz_t {
	float x;
	float y;
	float z;
};

struct iphyee_glslc_vec2_uv_t {
	float u;
	float v;
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

#endif
