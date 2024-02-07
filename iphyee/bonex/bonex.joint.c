#include "bonex.h"

static void iphyee_bonex_joint_free_func(iphyee_bonex_joint_s *restrict r)
{
	if (r->name) refer_free(r->name);
	if (r->base) refer_free(r->base);
}

static iphyee_bonex_joint_s* iphyee_bonex_joint_alloc_empty(void)
{
	iphyee_bonex_joint_s *restrict r;
	uintptr_t default_index;
	if ((r = (iphyee_bonex_joint_s *) refer_alloc(sizeof(iphyee_bonex_joint_s))))
	{
		r->name = r->base = NULL;
		refer_set_free(r, (refer_free_f) iphyee_bonex_joint_free_func);
		default_index = ~(uintptr_t) 0;
		r->index.this_joint_index = default_index;
		r->index.base_joint_index = default_index;
		r->index.link_joint_start = default_index;
		r->index.link_joint_count = 0;
		r->index.link_coord_start = default_index;
		r->index.link_coord_count = 0;
		r->index.link_inode_start = default_index;
		r->index.link_inode_count = 0;
		r->index.link_value_start = default_index;
		r->index.link_value_count = 0;
		r->index.fix_mat4x4_start = default_index;
		r->index.fix_mat4x4_count = 0;
		return r;
	}
	return NULL;
}

iphyee_bonex_joint_s* iphyee_bonex_joint_alloc(const char *restrict name, const iphyee_bonex_joint_s *restrict base)
{
	iphyee_bonex_joint_s *restrict r;
	if (name && (r = iphyee_bonex_joint_alloc_empty()))
	{
		if ((r->name = refer_dump_string(name)))
		{
			r->base = (refer_string_t) (base?refer_save(base->name):NULL);
			return r;
		}
		refer_free(r);
	}
	return NULL;
}

iphyee_bonex_joint_s* iphyee_bonex_joint_dump(const iphyee_bonex_joint_s *restrict src)
{
	iphyee_bonex_joint_s *restrict r;
	if (src && (r = iphyee_bonex_joint_alloc_empty()))
	{
		r->name = (refer_string_t) refer_save(src->name);
		r->base = (refer_string_t) refer_save(src->base);
		return r;
	}
	return NULL;
}
