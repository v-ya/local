#include "loader.model.view.h"

static iphyee_loader_model_view_t* iphyee_loader_model_view_initial(iphyee_loader_model_view_t *restrict v, const layer_model_item_s *restrict d)
{
	layer_file_s *block;
	uint64_t offset, size;
	if (d && layer_model_get_d(d, &block, &offset, &size) && block &&
		(v->m = layer_file_mmap(block, offset, size, 0, layer_file_io_read)))
	{
		v->block = (layer_file_s *) refer_save(block);
		v->data = layer_file_mdata(v->block, v->m, &v->size);
		return v;
	}
	return NULL;
}
static void iphyee_loader_model_view_finally(iphyee_loader_model_view_t *restrict v)
{
	v->size = 0;
	v->data = NULL;
	if (v->block)
	{
		if (v->m)
		{
			layer_file_munmap(v->block, v->m);
			v->m = NULL;
		}
		refer_free(v->block);
		v->block = NULL;
	}
}

static void iphyee_loader_model_view_surface_free_func(iphyee_loader_model_view_surface_s *restrict r)
{
	iphyee_loader_model_view_finally(&r->view);
	if (r->texture_name) refer_free(r->texture_name);
}
static iphyee_loader_model_view_surface_s* iphyee_loader_model_view_surface_alloc(const layer_model_item_s *restrict surface)
{
	iphyee_loader_model_view_surface_s *restrict r;
	const layer_model_item_s *restrict v;
	if (surface && (r = (iphyee_loader_model_view_surface_s *) refer_alloz(sizeof(iphyee_loader_model_view_surface_s))))
	{
		refer_set_free(r, (refer_free_f) iphyee_loader_model_view_surface_free_func);
		if ((v = layer_model_get_o_child(surface, iphyee_loader_sname__d_surface)) &&
			iphyee_loader_model_view_initial(&r->view, v))
		{
			if ((v = layer_model_get_o_child(surface, iphyee_loader_sname__s_texture_name)))
				r->texture_name = refer_save(layer_model_get_s(v));
			return r;
		}
		refer_free(r);
	}
	return NULL;
}

static void iphyee_loader_model_view_free_func(iphyee_loader_model_view_s *restrict r)
{
	if (r->surface) refer_free(r->surface);
	if (r->weight_joint) refer_free(r->weight_joint);
	iphyee_loader_model_view_finally(&r->fragment);
	iphyee_loader_model_view_finally(&r->effect);
	iphyee_loader_model_view_finally(&r->weight);
	iphyee_loader_model_view_finally(&r->normal);
	iphyee_loader_model_view_finally(&r->texture);
	iphyee_loader_model_view_finally(&r->vertex);
}

iphyee_loader_model_view_s* iphyee_loader_model_view_alloc(const layer_model_item_s *restrict object)
{
	iphyee_loader_model_view_surface_s *restrict surface;
	iphyee_loader_model_view_s *restrict r;
	const layer_model_item_s *restrict v;
	const vattr_vlist_t *restrict vl;
	const vattr_s *restrict meshes;
	if (object && (r = (iphyee_loader_model_view_s *) refer_alloz(sizeof(iphyee_loader_model_view_s))))
	{
		refer_set_free(r, (refer_free_f) iphyee_loader_model_view_free_func);
		if (!(v = layer_model_get_o_child(object, iphyee_loader_sname__e_bits_value)))
			goto label_fail;
		r->bits_value = (uint32_t) layer_model_get_e_value(v);
		if (!(v = layer_model_get_o_child(object, iphyee_loader_sname__e_bits_index)))
			goto label_fail;
		r->bits_index = (uint32_t) layer_model_get_e_value(v);
		if ((v = layer_model_get_o_child(object, iphyee_loader_sname__o_vertex)) &&
			!iphyee_loader_model_view_initial(&r->vertex, layer_model_get_o_child(
				v, iphyee_loader_sname__d_vertex)))
			goto label_fail;
		if ((v = layer_model_get_o_child(object, iphyee_loader_sname__o_texture)) &&
			!iphyee_loader_model_view_initial(&r->texture, layer_model_get_o_child(
				v, iphyee_loader_sname__d_texture)))
			goto label_fail;
		if ((v = layer_model_get_o_child(object, iphyee_loader_sname__o_normal)) &&
			!iphyee_loader_model_view_initial(&r->normal, layer_model_get_o_child(
				v, iphyee_loader_sname__d_normal)))
			goto label_fail;
		if ((v = layer_model_get_o_child(object, iphyee_loader_sname__o_effect)))
		{
			r->weight_joint = (const layer_model_item_s *) refer_save(
				layer_model_get_o_child(v, iphyee_loader_sname__a_weight_joint));
			if (!iphyee_loader_model_view_initial(&r->weight, layer_model_get_o_child(
				v, iphyee_loader_sname__d_weight)))
				goto label_fail;
			if (!iphyee_loader_model_view_initial(&r->effect, layer_model_get_o_child(
				v, iphyee_loader_sname__d_effect)))
				goto label_fail;
		}
		if ((v = layer_model_get_o_child(object, iphyee_loader_sname__o_fragment)))
		{
			if (!iphyee_loader_model_view_initial(&r->fragment, layer_model_get_o_child(
				v, iphyee_loader_sname__d_fragment)))
				goto label_fail;
			if ((v = layer_model_get_o_child(v, iphyee_loader_sname__o_meshes)) &&
				(meshes = layer_model_get_o_vattr(v)))
			{
				if (!(r->surface = vattr_alloc()))
					goto label_fail;
				for (vl = meshes->vattr; vl; vl = vl->vattr_next)
				{
					if ((surface = iphyee_loader_model_view_surface_alloc(
						layer_model_get_o_child(v, vl->vslot->key))))
					{
						vattr_insert_tail(r->surface, vl->vslot->key, surface);
						refer_free(surface);
					}
				}
			}
		}
		r->vertex_at_fragment = r->vertex.block?(r->length_of_fragment++):~(uintptr_t) 0;
		r->texture_at_fragment = r->texture.block?(r->length_of_fragment++):~(uintptr_t) 0;
		r->normal_at_fragment = r->normal.block?(r->length_of_fragment++):~(uintptr_t) 0;
		r->effect_at_fragment = r->effect.block?(r->length_of_fragment++):~(uintptr_t) 0;
		return r;
		label_fail:
		refer_free(r);
	}
	return NULL;
}
