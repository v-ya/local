#ifndef _iphyee_loader_model_view_h_
#define _iphyee_loader_model_view_h_

#include "loader.h"
#include <vattr.h>

typedef struct iphyee_loader_model_view_t iphyee_loader_model_view_t;
typedef struct iphyee_loader_model_view_surface_s iphyee_loader_model_view_surface_s;
typedef struct iphyee_loader_model_view_s iphyee_loader_model_view_s;

struct iphyee_loader_model_view_t {
	layer_file_s *block;
	layer_file_memory_t m;
	const void *data;
	uintptr_t size;
};

struct iphyee_loader_model_view_surface_s {
	iphyee_loader_model_view_t view;
	refer_nstring_t texture_name;
};

struct iphyee_loader_model_view_s {
	iphyee_loader_model_view_t vertex;
	iphyee_loader_model_view_t texture;
	iphyee_loader_model_view_t normal;
	iphyee_loader_model_view_t weight;
	iphyee_loader_model_view_t effect;
	iphyee_loader_model_view_t fragment;
	const layer_model_item_s *weight_joint;
	vattr_s *surface;
	uintptr_t vertex_at_fragment;
	uintptr_t texture_at_fragment;
	uintptr_t normal_at_fragment;
	uintptr_t effect_at_fragment;
	uintptr_t length_of_fragment;
	uint32_t bits_value;
	uint32_t bits_index;
};

iphyee_loader_model_view_s* iphyee_loader_model_view_alloc(const layer_model_item_s *restrict object);

#endif
