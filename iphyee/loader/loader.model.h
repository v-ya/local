#ifndef _iphyee_loader_model_h_
#define _iphyee_loader_model_h_

#include "loader.h"
#include <vattr.h>

typedef enum iphyee_loader_model_eblock_t iphyee_loader_model_eblock_t;
typedef struct iphyee_loader_model_func_t iphyee_loader_model_func_t;
typedef struct iphyee_loader_model_block_s iphyee_loader_model_block_s;
typedef struct iphyee_loader_model_effect_s iphyee_loader_model_effect_s;
typedef struct iphyee_loader_model_fragment_s iphyee_loader_model_fragment_s;
typedef struct iphyee_loader_model_surface_s iphyee_loader_model_surface_s;
typedef struct iphyee_loader_model_bonex_s iphyee_loader_model_bonex_s;
typedef struct iphyee_loader_model_s iphyee_loader_model_s;

typedef layer_file_s* (*iphyee_loader_model_write_vec3_f)(layer_file_s *restrict block, const double vec3[3]);
typedef layer_file_s* (*iphyee_loader_model_write_vec2_f)(layer_file_s *restrict block, const double vec2[3]);
typedef layer_file_s* (*iphyee_loader_model_write_weight_f)(layer_file_s *restrict block, uintptr_t joint_index, double weight);
typedef layer_file_s* (*iphyee_loader_model_write_range_f)(layer_file_s *restrict block, uintptr_t offset, uintptr_t number);
typedef layer_file_s* (*iphyee_loader_model_write_fragment_f)(layer_file_s *restrict block, const uintptr_t index_array[], uintptr_t index_count);

enum iphyee_loader_model_eblock_t {
	iphyee_loader_model_eblock__vertex  = 0x0001,
	iphyee_loader_model_eblock__normal  = 0x0002,
	iphyee_loader_model_eblock__texture = 0x0004,
	iphyee_loader_model_eblock__effect  = 0x0008,
	iphyee_loader_model_eblock__bonex   = 0x0010,
	iphyee_loader_model_eblock__nocheck = 0x8000,
};

struct iphyee_loader_model_func_t {
	iphyee_loader_model_write_vec3_f write_vertex;
	iphyee_loader_model_write_vec3_f write_normal;
	iphyee_loader_model_write_vec2_f write_texture;
	iphyee_loader_model_write_weight_f write_weight;
	iphyee_loader_model_write_range_f write_effect;
	iphyee_loader_model_write_fragment_f write_fragment;
	iphyee_loader_model_write_range_f write_surface;
};

struct iphyee_loader_model_block_s {
	layer_model_item_s *item_block;
	layer_model_item_s *item_count;
	layer_file_s *block;
	uintptr_t count;
};

struct iphyee_loader_model_effect_s {
	layer_model_item_s *item_joint;
	iphyee_loader_model_block_s *weight;
	iphyee_loader_model_block_s *effect;
	uintptr_t joint_count;
};

struct iphyee_loader_model_fragment_s {
	iphyee_loader_model_block_s *fragment;
	layer_model_item_s *item_meshes;
	vattr_s *surface;
	uintptr_t length_of_fragment;
	uintptr_t block_check_count;
	iphyee_loader_model_block_s *block_check[];
};

struct iphyee_loader_model_surface_s {
	refer_string_t name;
	layer_model_item_s *item_surface;
	iphyee_loader_model_block_s *fragment;
	iphyee_loader_model_block_s *surface;
};

struct iphyee_loader_model_bonex_s {
	layer_model_item_s *item_bonex_bind;
	layer_model_item_s *item_bonex_fixed;
};

struct iphyee_loader_model_s {
	const layer_model_s *m;
	layer_model_item_s *model;
	iphyee_loader_model_block_s *vertex;
	iphyee_loader_model_block_s *normal;
	iphyee_loader_model_block_s *texture;
	iphyee_loader_model_effect_s *effect;
	iphyee_loader_model_fragment_s *fragment;
	iphyee_loader_model_bonex_s *bonex;
	iphyee_loader_model_func_t func;
	uintptr_t index_max;
	uint32_t bits_value;
	uint32_t bits_index;
};

iphyee_loader_model_s* iphyee_loader_model_alloc(const iphyee_loader_s *restrict loader, iphyee_loader_model_eblock_t eblock, uint32_t bits_value, uint32_t bits_index);
iphyee_loader_model_s* iphyee_loader_model_append_vertex(iphyee_loader_model_s *restrict r, const double vertex[3]);
iphyee_loader_model_s* iphyee_loader_model_append_normal(iphyee_loader_model_s *restrict r, const double normal[3]);
iphyee_loader_model_s* iphyee_loader_model_append_texture(iphyee_loader_model_s *restrict r, const double texture[2]);
iphyee_loader_model_s* iphyee_loader_model_append_weight_joint(iphyee_loader_model_s *restrict r, const char *restrict joint_name);
iphyee_loader_model_s* iphyee_loader_model_append_weight(iphyee_loader_model_s *restrict r, uintptr_t joint_index, double weight);
iphyee_loader_model_s* iphyee_loader_model_append_effect(iphyee_loader_model_s *restrict r, uintptr_t weight_offset, uintptr_t weight_number);
iphyee_loader_model_s* iphyee_loader_model_append_fragment(iphyee_loader_model_s *restrict r, const uintptr_t index_array[], uintptr_t index_count);
iphyee_loader_model_surface_s* iphyee_loader_model_append_fragment_surface(iphyee_loader_model_s *restrict r, const char *restrict surface_name, uintptr_t name_length);
iphyee_loader_model_s* iphyee_loader_model_append_surface(iphyee_loader_model_s *restrict r, iphyee_loader_model_surface_s *restrict surface, uintptr_t fragment_offset, uintptr_t fragment_number);
iphyee_loader_model_s* iphyee_loader_model_update(iphyee_loader_model_s *restrict r);

#endif
