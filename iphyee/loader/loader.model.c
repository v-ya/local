#include "loader.model.h"

#define d_oc_mm(_name)         iphyee_loader_sname__##_name, iphyee_loader_stype__##_name
#define d_oc_ml(_name, _type)  iphyee_loader_sname__##_name, layer_model_stype__##_type

// block

static void iphyee_loader_model_block_free_func(iphyee_loader_model_block_s *restrict r)
{
	if (r->item_block) refer_free(r->item_block);
	if (r->item_count) refer_free(r->item_count);
	if (r->block) refer_free(r->block);
}
static iphyee_loader_model_block_s* iphyee_loader_model_block_alloc(layer_model_item_s *restrict object, const char *restrict block_name, const char *restrict count_name)
{
	iphyee_loader_model_block_s *restrict r;
	if (object && (r = (iphyee_loader_model_block_s *) refer_alloz(sizeof(iphyee_loader_model_block_s))))
	{
		refer_set_free(r, (refer_free_f) iphyee_loader_model_block_free_func);
		if (refer_save(r->item_block = layer_model_set_o_create(object, block_name, layer_model_stype__data)) &&
			refer_save(r->item_count = layer_model_set_o_create(object, count_name, layer_model_stype__uint)) &&
			(r->block = layer_file_alloc_mbuf(NULL, 0)) &&
			layer_model_set_d(r->item_block, r->block, 0, 0))
			return r;
		refer_free(r);
	}
	return NULL;
}
static iphyee_loader_model_block_s* iphyee_loader_model_block_update(iphyee_loader_model_block_s *restrict r)
{
	if (layer_model_set_d(r->item_block, r->block, 0, layer_file_size(r->block)) &&
		layer_model_set_u(r->item_count, (uint64_t) r->count))
		return r;
	return NULL;
}

// effect

static void iphyee_loader_model_effect_free_func(iphyee_loader_model_effect_s *restrict r)
{
	if (r->item_joint) refer_free(r->item_joint);
	if (r->weight) refer_free(r->weight);
	if (r->effect) refer_free(r->effect);
}
static iphyee_loader_model_effect_s* iphyee_loader_model_effect_alloc(layer_model_item_s *restrict object)
{
	iphyee_loader_model_effect_s *restrict r;
	if (object && (r = (iphyee_loader_model_effect_s *) refer_alloz(sizeof(iphyee_loader_model_effect_s))))
	{
		refer_set_free(r, (refer_free_f) iphyee_loader_model_effect_free_func);
		if (refer_save(r->item_joint = layer_model_set_o_create(object, d_oc_ml(a_weight_joint, string))) &&
			(r->weight = iphyee_loader_model_block_alloc(object,
				iphyee_loader_sname__d_weight, iphyee_loader_sname__u_weight_count)) &&
			(r->effect = iphyee_loader_model_block_alloc(object,
				iphyee_loader_sname__d_effect, iphyee_loader_sname__u_effect_count)))
			return r;
		refer_free(r);
	}
	return NULL;
}

// fragment

static void iphyee_loader_model_fragment_free_func(iphyee_loader_model_fragment_s *restrict r)
{
	uintptr_t i, n;
	for (i = 0, n = r->length_of_fragment; i < n; ++i)
	{
		if (r->block_check[i])
			refer_free(r->block_check[i]);
	}
	if (r->surface) refer_free(r->surface);
	if (r->item_meshes) refer_free(r->item_meshes);
	if (r->fragment) refer_free(r->fragment);
}
static iphyee_loader_model_fragment_s* iphyee_loader_model_fragment_alloc(const iphyee_loader_model_s *restrict model, layer_model_item_s *restrict object)
{
	iphyee_loader_model_fragment_s *restrict r;
	iphyee_loader_model_block_s *block_check[8];
	uintptr_t length_of_fragment, i;
	length_of_fragment = 0;
	if (model->vertex) block_check[length_of_fragment++] = model->vertex;
	if (model->texture) block_check[length_of_fragment++] = model->texture;
	if (model->normal) block_check[length_of_fragment++] = model->normal;
	if (model->effect) block_check[length_of_fragment++] = model->effect->effect;
	if (object && length_of_fragment && (r = (iphyee_loader_model_fragment_s *) refer_alloz(
		sizeof(iphyee_loader_model_fragment_s) + sizeof(iphyee_loader_model_block_s *) * length_of_fragment)))
	{
		refer_set_free(r, (refer_free_f) iphyee_loader_model_fragment_free_func);
		if ((r->fragment = iphyee_loader_model_block_alloc(object,
				iphyee_loader_sname__d_fragment, iphyee_loader_sname__u_fragment_count)) &&
			refer_save(r->item_meshes = layer_model_set_o_create(object, d_oc_mm(o_meshes))) &&
			(r->surface = vattr_alloc()))
		{
			r->length_of_fragment = length_of_fragment;
			r->block_check_count = length_of_fragment;
			for (i = 0; i < length_of_fragment; ++i)
				r->block_check[i] = (iphyee_loader_model_block_s *) refer_save(block_check[i]);
			return r;
		}
		refer_free(r);
	}
	return NULL;
}

// surface

static void iphyee_loader_model_surface_free_func(iphyee_loader_model_surface_s *restrict r)
{
	if (r->name) refer_free(r->name);
	if (r->item_surface) refer_free(r->item_surface);
	if (r->fragment) refer_free(r->fragment);
	if (r->surface) refer_free(r->surface);
}
static iphyee_loader_model_surface_s* iphyee_loader_model_surface_alloc(iphyee_loader_model_fragment_s *restrict fragment, const char *restrict name, uintptr_t length)
{
	iphyee_loader_model_surface_s *restrict r;
	if (fragment && (r = (iphyee_loader_model_surface_s *) refer_alloz(sizeof(iphyee_loader_model_surface_s))))
	{
		refer_set_free(r, (refer_free_f) iphyee_loader_model_surface_free_func);
		if ((r->name = refer_dump_string_with_length(name, length)) &&
			refer_save(r->item_surface = layer_model_set_o_create(
				fragment->item_meshes, r->name, iphyee_loader_stype__o_surface)) &&
			(r->fragment = (iphyee_loader_model_block_s *) refer_save(fragment->fragment)) &&
			(r->surface = iphyee_loader_model_block_alloc(r->item_surface,
				iphyee_loader_sname__d_surface, iphyee_loader_sname__u_surface_count)))
		{
			if (vattr_insert_tail(fragment->surface, r->name, r))
			{
				refer_free(r);
				return r;
			}
		}
		refer_free(r);
	}
	return NULL;
}

// func

static layer_file_s* iphyee_loader_model_write_vec3_value32(layer_file_s *restrict block, const double vec3[3])
{
	float data[3];
	data[0] = (float) vec3[0];
	data[1] = (float) vec3[1];
	data[2] = (float) vec3[2];
	if (layer_file_write(block, data, 1, sizeof(float) * 3))
		return block;
	return NULL;
}
static layer_file_s* iphyee_loader_model_write_vec3_value64(layer_file_s *restrict block, const double vec3[3])
{
	if (layer_file_write(block, vec3, 1, sizeof(double) * 3))
		return block;
	return NULL;
}
static layer_file_s* iphyee_loader_model_write_vec2_value32(layer_file_s *restrict block, const double vec2[2])
{
	float data[2];
	data[0] = (float) vec2[0];
	data[1] = (float) vec2[1];
	if (layer_file_write(block, data, 1, sizeof(float) * 2))
		return block;
	return NULL;
}
static layer_file_s* iphyee_loader_model_write_vec2_value64(layer_file_s *restrict block, const double vec2[2])
{
	if (layer_file_write(block, vec2, 1, sizeof(double) * 2))
		return block;
	return NULL;
}
static layer_file_s* iphyee_loader_model_write_weight_value32(layer_file_s *restrict block, uintptr_t joint_index, double weight)
{
	struct { uint32_t ji; float w; } data;
	data.ji = (uint32_t) joint_index;
	data.w = (float) weight;
	if (layer_file_write(block, &data, 1, sizeof(data)))
		return block;
	return NULL;
}
static layer_file_s* iphyee_loader_model_write_weight_value64(layer_file_s *restrict block, uintptr_t joint_index, double weight)
{
	struct { uint64_t ji; double w; } data;
	data.ji = (uint64_t) joint_index;
	data.w = (double) weight;
	if (layer_file_write(block, &data, 1, sizeof(data)))
		return block;
	return NULL;
}
static layer_file_s* iphyee_loader_model_write_range_index16(layer_file_s *restrict block, uintptr_t offset, uintptr_t number)
{
	uint16_t data[2];
	data[0] = (uint16_t) offset;
	data[1] = (uint16_t) number;
	if (layer_file_write(block, &data, 1, sizeof(data)))
		return block;
	return NULL;
}
static layer_file_s* iphyee_loader_model_write_range_index32(layer_file_s *restrict block, uintptr_t offset, uintptr_t number)
{
	uint32_t data[2];
	data[0] = (uint32_t) offset;
	data[1] = (uint32_t) number;
	if (layer_file_write(block, &data, 1, sizeof(data)))
		return block;
	return NULL;
}
static layer_file_s* iphyee_loader_model_write_range_index64(layer_file_s *restrict block, uintptr_t offset, uintptr_t number)
{
	uint64_t data[2];
	data[0] = (uint64_t) offset;
	data[1] = (uint64_t) number;
	if (layer_file_write(block, &data, 1, sizeof(data)))
		return block;
	return NULL;
}
static layer_file_s* iphyee_loader_model_write_fragment_index16(layer_file_s *restrict block, const uintptr_t index_array[], uintptr_t index_count)
{
	uint16_t data[8];
	switch (index_count)
	{
		case 8: data[7] = (uint16_t) index_array[7]; // fall through
		case 7: data[6] = (uint16_t) index_array[6]; // fall through
		case 6: data[5] = (uint16_t) index_array[5]; // fall through
		case 5: data[4] = (uint16_t) index_array[4]; // fall through
		case 4: data[3] = (uint16_t) index_array[3]; // fall through
		case 3: data[2] = (uint16_t) index_array[2]; // fall through
		case 2: data[1] = (uint16_t) index_array[1]; // fall through
		case 1: data[0] = (uint16_t) index_array[0]; // fall through
			if (layer_file_write(block, &data, 1, sizeof(uint16_t) * index_count))
				return block;
		default: break;
	}
	return NULL;
}
static layer_file_s* iphyee_loader_model_write_fragment_index32(layer_file_s *restrict block, const uintptr_t index_array[], uintptr_t index_count)
{
	uint32_t data[8];
	switch (index_count)
	{
		case 8: data[7] = (uint32_t) index_array[7]; // fall through
		case 7: data[6] = (uint32_t) index_array[6]; // fall through
		case 6: data[5] = (uint32_t) index_array[5]; // fall through
		case 5: data[4] = (uint32_t) index_array[4]; // fall through
		case 4: data[3] = (uint32_t) index_array[3]; // fall through
		case 3: data[2] = (uint32_t) index_array[2]; // fall through
		case 2: data[1] = (uint32_t) index_array[1]; // fall through
		case 1: data[0] = (uint32_t) index_array[0]; // fall through
			if (layer_file_write(block, &data, 1, sizeof(uint32_t) * index_count))
				return block;
		default: break;
	}
	return NULL;
}
static layer_file_s* iphyee_loader_model_write_fragment_index64(layer_file_s *restrict block, const uintptr_t index_array[], uintptr_t index_count)
{
	uint64_t data[8];
	switch (index_count)
	{
		case 8: data[7] = (uint64_t) index_array[7]; // fall through
		case 7: data[6] = (uint64_t) index_array[6]; // fall through
		case 6: data[5] = (uint64_t) index_array[5]; // fall through
		case 5: data[4] = (uint64_t) index_array[4]; // fall through
		case 4: data[3] = (uint64_t) index_array[3]; // fall through
		case 3: data[2] = (uint64_t) index_array[2]; // fall through
		case 2: data[1] = (uint64_t) index_array[1]; // fall through
		case 1: data[0] = (uint64_t) index_array[0]; // fall through
			if (layer_file_write(block, &data, 1, sizeof(uint64_t) * index_count))
				return block;
		default: break;
	}
	return NULL;
}

static iphyee_loader_model_func_t* iphyee_loader_model_func_initial(iphyee_loader_model_func_t *restrict func, uintptr_t *restrict index_max, uint32_t bits_value, uint32_t bits_index)
{
	if (bits_value == 32)
	{
		func->write_vertex = iphyee_loader_model_write_vec3_value32;
		func->write_texture = iphyee_loader_model_write_vec2_value32;
		func->write_normal = iphyee_loader_model_write_vec3_value32;
		func->write_weight = iphyee_loader_model_write_weight_value32;
	}
	else if (bits_value == 64)
	{
		func->write_vertex = iphyee_loader_model_write_vec3_value64;
		func->write_texture = iphyee_loader_model_write_vec2_value64;
		func->write_normal = iphyee_loader_model_write_vec3_value64;
		func->write_weight = iphyee_loader_model_write_weight_value64;
	}
	else goto label_fail;
	if (bits_index == 16)
	{
		func->write_effect = iphyee_loader_model_write_range_index16;
		func->write_fragment = iphyee_loader_model_write_fragment_index16;
		func->write_surface = iphyee_loader_model_write_range_index16;
		*index_max = (uintptr_t) (((uint64_t) 1 << 16) - 1);
	}
	else if (bits_index == 32)
	{
		func->write_effect = iphyee_loader_model_write_range_index32;
		func->write_fragment = iphyee_loader_model_write_fragment_index32;
		func->write_surface = iphyee_loader_model_write_range_index32;
		*index_max = (uintptr_t) (((uint64_t) 1 << 32) - 1);
	}
	else if (bits_index == 64)
	{
		func->write_effect = iphyee_loader_model_write_range_index64;
		func->write_fragment = iphyee_loader_model_write_fragment_index64;
		func->write_surface = iphyee_loader_model_write_range_index64;
		*index_max = (uintptr_t) ~(uint64_t) 0;
	}
	else goto label_fail;
	return func;
	label_fail:
	return NULL;
}

// model

static void iphyee_loader_model_free_func(iphyee_loader_model_s *restrict r)
{
	if (r->bonex) refer_free(r->bonex);
	if (r->fragment) refer_free(r->fragment);
	if (r->effect) refer_free(r->effect);
	if (r->normal) refer_free(r->normal);
	if (r->texture) refer_free(r->texture);
	if (r->vertex) refer_free(r->vertex);
	if (r->model) refer_free(r->model);
	if (r->m) refer_free(r->m);
}

iphyee_loader_model_s* iphyee_loader_model_alloc(const iphyee_loader_s *restrict loader, iphyee_loader_model_eblock_t eblock, uint32_t bits_value, uint32_t bits_index)
{
	iphyee_loader_model_s *restrict r;
	layer_model_item_s *restrict bits;
	if (loader && (r = (iphyee_loader_model_s *) refer_alloz(sizeof(iphyee_loader_model_s))))
	{
		refer_set_free(r, (refer_free_f) iphyee_loader_model_free_func);
		r->m = (const layer_model_s *) refer_save(loader->m);
		if (iphyee_loader_model_func_initial(&r->func, &r->index_max, bits_value, bits_index) &&
			(r->model = layer_model_create_item(r->m, iphyee_loader_stype__o_model)) &&
			(bits = layer_model_set_o_create(r->model, d_oc_mm(e_bits_value))) &&
			layer_model_set_e_value(bits, (uint64_t) bits_value) &&
			(bits = layer_model_set_o_create(r->model, d_oc_mm(e_bits_index))) &&
			layer_model_set_e_value(bits, (uint64_t) bits_index))
		{
			if (eblock & iphyee_loader_model_eblock__vertex)
			{
				if (!(r->vertex = iphyee_loader_model_block_alloc(
					layer_model_set_o_create(r->model, d_oc_mm(o_vertex)),
					iphyee_loader_sname__d_vertex,
					iphyee_loader_sname__u_vertex_count
				))) goto label_fail;
			}
			if (eblock & iphyee_loader_model_eblock__texture)
			{
				if (!(r->texture = iphyee_loader_model_block_alloc(
					layer_model_set_o_create(r->model, d_oc_mm(o_texture)),
					iphyee_loader_sname__d_texture,
					iphyee_loader_sname__u_texture_count
				))) goto label_fail;
			}
			if (eblock & iphyee_loader_model_eblock__normal)
			{
				if (!(r->normal = iphyee_loader_model_block_alloc(
					layer_model_set_o_create(r->model, d_oc_mm(o_normal)),
					iphyee_loader_sname__d_normal,
					iphyee_loader_sname__u_normal_count
				))) goto label_fail;
			}
			if (eblock & iphyee_loader_model_eblock__effect)
			{
				if (!(r->effect = iphyee_loader_model_effect_alloc(
					layer_model_set_o_create(r->model, d_oc_mm(o_effect)))))
					goto label_fail;
			}
			if (!(r->fragment = iphyee_loader_model_fragment_alloc(r,
				layer_model_set_o_create(r->model, d_oc_mm(o_fragment)))))
				goto label_fail;
			if (eblock & iphyee_loader_model_eblock__nocheck)
				r->fragment->block_check_count = 0;
			r->bits_value = bits_value;
			r->bits_index = bits_index;
			return r;
		}
		label_fail:
		refer_free(r);
	}
	return NULL;
}

// api

iphyee_loader_model_s* iphyee_loader_model_append_vertex(iphyee_loader_model_s *restrict r, const double vertex[3])
{
	iphyee_loader_model_block_s *restrict block;
	if ((block = r->vertex) && r->func.write_vertex(block->block, vertex) && (block->count += 1) < r->index_max)
		return r;
	return NULL;
}
iphyee_loader_model_s* iphyee_loader_model_append_texture(iphyee_loader_model_s *restrict r, const double texture[2])
{
	iphyee_loader_model_block_s *restrict block;
	if ((block = r->texture) && r->func.write_texture(block->block, texture) && (block->count += 1) < r->index_max)
		return r;
	return NULL;
}
iphyee_loader_model_s* iphyee_loader_model_append_normal(iphyee_loader_model_s *restrict r, const double normal[3])
{
	iphyee_loader_model_block_s *restrict block;
	if ((block = r->normal) && r->func.write_normal(block->block, normal) && (block->count += 1) < r->index_max)
		return r;
	return NULL;
}
iphyee_loader_model_s* iphyee_loader_model_append_weight_joint(iphyee_loader_model_s *restrict r, const char *restrict joint_name)
{
	iphyee_loader_model_effect_s *restrict effect;
	layer_model_item_s *restrict str;
	if ((effect = r->effect) && joint_name && (str = layer_model_set_a_create(
		effect->item_joint, layer_model_stype__string)))
	{
		effect->joint_count += 1;
		if (layer_model_set_s_str(str, joint_name))
			return r;
	}
	return NULL;
}
iphyee_loader_model_s* iphyee_loader_model_append_weight(iphyee_loader_model_s *restrict r, uintptr_t joint_index, double weight)
{
	iphyee_loader_model_effect_s *restrict effect;
	iphyee_loader_model_block_s *restrict block;
	if ((effect = r->effect) && (block = effect->weight) && joint_index < effect->joint_count &&
		r->func.write_weight(block->block, joint_index, weight) && (block->count += 1) < r->index_max)
		return r;
	return NULL;
}
iphyee_loader_model_s* iphyee_loader_model_append_effect(iphyee_loader_model_s *restrict r, uintptr_t weight_offset, uintptr_t weight_number)
{
	iphyee_loader_model_effect_s *restrict effect;
	iphyee_loader_model_block_s *restrict block;
	if ((effect = r->effect) && (block = effect->weight) &&
		weight_offset <= block->count &&
		weight_number <= block->count - weight_offset &&
		(block = effect->effect) &&
		r->func.write_effect(block->block, weight_offset, weight_number) &&
		(block->count += 1) < r->index_max)
		return r;
	return NULL;
}
iphyee_loader_model_s* iphyee_loader_model_append_fragment(iphyee_loader_model_s *restrict r, const uintptr_t index_array[], uintptr_t index_count)
{
	iphyee_loader_model_fragment_s *restrict fragment;
	iphyee_loader_model_block_s *restrict block;
	uintptr_t i, n;
	if ((fragment = r->fragment) && index_count == fragment->length_of_fragment)
	{
		for (i = 0, n = fragment->block_check_count; i < n; ++i)
		{
			if (index_array[i] >= fragment->block_check[i]->count)
				goto label_fail;
		}
		if ((block = fragment->fragment) &&
			r->func.write_fragment(block->block, index_array, index_count) &&
			(block->count += 1) < r->index_max)
			return r;
	}
	label_fail:
	return NULL;
}
iphyee_loader_model_surface_s* iphyee_loader_model_append_fragment_surface(iphyee_loader_model_s *restrict r, const char *restrict surface_name, uintptr_t name_length)
{
	iphyee_loader_model_fragment_s *restrict fragment;
	if ((fragment = r->fragment))
		return iphyee_loader_model_surface_alloc(fragment, surface_name, name_length);
	return NULL;
}
iphyee_loader_model_s* iphyee_loader_model_append_surface(iphyee_loader_model_s *restrict r, iphyee_loader_model_surface_s *restrict surface, uintptr_t fragment_offset, uintptr_t fragment_number)
{
	iphyee_loader_model_block_s *restrict block;
	if (surface && (block = surface->fragment) && fragment_number >= 3 &&
		fragment_offset <= block->count &&
		fragment_number <= block->count - fragment_offset &&
		(block = surface->surface) &&
		r->func.write_surface(block->block, fragment_offset, fragment_number) &&
		(block->count += 1) < r->index_max)
		return r;
	return NULL;
}

iphyee_loader_model_s* iphyee_loader_model_update(iphyee_loader_model_s *restrict r)
{
	iphyee_loader_model_effect_s *effect;
	iphyee_loader_model_fragment_s *fragment;
	iphyee_loader_model_surface_s *surface;
	iphyee_loader_model_s *result;
	vattr_vlist_t *restrict vl;
	result = r;
	if (r->vertex && !iphyee_loader_model_block_update(r->vertex))
		result = NULL;
	if (r->texture && !iphyee_loader_model_block_update(r->texture))
		result = NULL;
	if (r->normal && !iphyee_loader_model_block_update(r->normal))
		result = NULL;
	if ((effect = r->effect))
	{
		if (!iphyee_loader_model_block_update(effect->weight))
			result = NULL;
		if (!iphyee_loader_model_block_update(effect->effect))
			result = NULL;
	}
	if ((fragment = r->fragment))
	{
		if (!iphyee_loader_model_block_update(fragment->fragment))
			result = NULL;
		for (vl = fragment->surface->vattr; vl; vl = vl->vattr_next)
		{
			surface = (iphyee_loader_model_surface_s *) vl->value;
			if (!iphyee_loader_model_block_update(surface->surface))
				result = NULL;
		}
	}
	return result;
}
