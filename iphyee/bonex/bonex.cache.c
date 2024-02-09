#include "bonex.cache.h"
#include <memory.h>

// pn context

typedef struct iphyee_bonex_cache_pn_context_t iphyee_bonex_cache_pn_context_t;

struct iphyee_bonex_cache_pn_context_t {
	iphyee_mat4x4_t c[4];
	iphyee_mat4x4_t *dst_p_mat4x4;
	iphyee_mat4x4_t *dst_n_mat4x4;
	iphyee_mat4x4_t *src_p_mat4x4;
	iphyee_mat4x4_t *src_n_mat4x4;
	uintptr_t count;
	uintptr_t index;
};

static void iphyee_bonex_cache_pn_initial(iphyee_bonex_cache_pn_context_t *restrict c, iphyee_mat4x4_t *dst_p_mat4x4, iphyee_mat4x4_t *dst_n_mat4x4, uintptr_t count)
{
	c->dst_p_mat4x4 = dst_p_mat4x4;
	c->dst_n_mat4x4 = dst_n_mat4x4;
	c->src_p_mat4x4 = NULL;
	c->src_n_mat4x4 = NULL;
	c->count = count;
	c->index = 0;
}
static void iphyee_bonex_cache_pn_append(iphyee_bonex_cache_pn_context_t *restrict c, iphyee_mat4x4_t *src_p_mat4x4, iphyee_mat4x4_t *src_n_mat4x4)
{
	iphyee_mat4x4_t *restrict dst_p_mat4x4;
	iphyee_mat4x4_t *restrict dst_n_mat4x4;
	uintptr_t ci;
	if (c->index++)
	{
		if (c->index < c->count)
		{
			ci = c->index & 1;
			dst_p_mat4x4 = &c->c[ci];
			dst_n_mat4x4 = &c->c[ci + 2];
		}
		else
		{
			dst_p_mat4x4 = c->dst_p_mat4x4;
			dst_n_mat4x4 = c->dst_n_mat4x4;
		}
		iphyee_mat4x4_mul(dst_p_mat4x4, c->src_p_mat4x4, src_p_mat4x4);
		iphyee_mat4x4_mul(dst_n_mat4x4, src_n_mat4x4, c->src_n_mat4x4);
		c->src_p_mat4x4 = dst_p_mat4x4;
		c->src_n_mat4x4 = dst_n_mat4x4;
	}
	else if (c->index < c->count)
	{
		c->src_p_mat4x4 = src_p_mat4x4;
		c->src_n_mat4x4 = src_n_mat4x4;
	}
	else
	{
		iphyee_mat4x4_cpy(c->dst_p_mat4x4, src_p_mat4x4);
		iphyee_mat4x4_cpy(c->dst_n_mat4x4, src_n_mat4x4);
	}
}
static void iphyee_bonex_cache_pn_vector(iphyee_bonex_cache_pn_context_t *restrict c, iphyee_mat4x4_t *src_p_array, iphyee_mat4x4_t *src_n_array, uintptr_t count)
{
	uintptr_t i;
	for (i = 0; i < count; ++i)
		iphyee_bonex_cache_pn_append(c, src_p_array + i, src_n_array + i);
}

// cache

static void iphyee_bonex_cache_free_func(iphyee_bonex_cache_s *restrict r)
{
	iphyee_bonex_cache_va_t *restrict p;
	uint32_t i, n;
	p = r->va_array;
	n = r->value_count;
	for (i = 0; i < n; ++i)
	{
		if (p[i].value_complex)
			refer_free(p[i].value_complex);
	}
}

static iphyee_bonex_cache_s* iphyee_bonex_cache_alloc_empty(uintptr_t joint_count, uintptr_t fixed_count, uintptr_t inode_count, uintptr_t value_count)
{
	iphyee_bonex_cache_s *restrict r;
	uintptr_t align_block, size;
	size = align_block = ((sizeof(iphyee_bonex_cache_s) + 15) / 16 * 16);
	size += sizeof(iphyee_mat4x4_t) * 2 * (joint_count + fixed_count + value_count);
	size += sizeof(iphyee_bonex_cache_va_t) * value_count;
	size += sizeof(iphyee_bonex_cache_ia_t) * inode_count;
	size += sizeof(iphyee_bonex_cache_fa_t) * fixed_count;
	size += sizeof(iphyee_bonex_cache_ja_t) * joint_count;
	size += sizeof(float) * value_count;
	if ((r = (iphyee_bonex_cache_s *) refer_alloz(size)))
	{
		refer_set_free(r, (refer_free_f) iphyee_bonex_cache_free_func);
		r->joint_p_array = (iphyee_mat4x4_t *) ((uint8_t *) r + align_block);
		r->joint_n_array = r->joint_p_array + joint_count;
		r->fixed_p_array = r->joint_n_array + joint_count;
		r->fixed_n_array = r->fixed_p_array + fixed_count;
		r->value_p_array = r->fixed_n_array + fixed_count;
		r->value_n_array = r->value_p_array + value_count;
		r->va_array = (iphyee_bonex_cache_va_t *) (r->value_n_array + value_count);
		r->ia_array = (iphyee_bonex_cache_ia_t *) (r->va_array + value_count);
		r->fa_array = (iphyee_bonex_cache_fa_t *) (r->ia_array + inode_count);
		r->ja_array = (iphyee_bonex_cache_ja_t *) (r->fa_array + fixed_count);
		r->value_array = (float *) (r->ja_array + joint_count);
		r->joint_count = (uint32_t) joint_count;
		r->fixed_count = (uint32_t) fixed_count;
		r->inode_count = (uint32_t) inode_count;
		r->value_count = (uint32_t) value_count;
		return r;
	}
	return NULL;
}

static uint32_t* iphyee_bonex_cache_initial_link_value(uint32_t *restrict link_value_start, uint32_t *restrict link_value_count, uint32_t value_index)
{
	if (*link_value_start + *link_value_count == value_index)
	{
		*link_value_count += 1;
		return link_value_count;
	}
	else if (!~*link_value_start && !*link_value_count)
	{
		*link_value_start = value_index;
		*link_value_count = 1;
		return link_value_start;
	}
	return NULL;
}
static void iphyee_bonex_cache_initial_loop_joint(iphyee_bonex_cache_s *restrict r, const iphyee_bonex_s *restrict bonex)
{
	iphyee_mat4x4_t *restrict joint_p_array;
	iphyee_mat4x4_t *restrict joint_n_array;
	const iphyee_bonex_joint_s *restrict p;
	iphyee_bonex_cache_ja_t *restrict ja;
	uintptr_t i, n;
	joint_p_array = r->joint_p_array;
	joint_n_array = r->joint_n_array;
	for (i = 0, n = bonex->joint_count; i < n; ++i)
	{
		iphyee_mat4x4_set_e(joint_p_array + i);
		iphyee_mat4x4_set_e(joint_n_array + i);
		p = bonex->joint_array[i];
		ja = &r->ja_array[i];
		ja->need_update = 1;
		ja->base_joint_index = (uint32_t) p->index.base_joint_index;
		ja->link_joint_start = (uint32_t) p->index.link_joint_start;
		ja->link_joint_count = (uint32_t) p->index.link_joint_count;
		ja->link_fixed_start = (uint32_t) p->index.fix_mat4x4_start;
		ja->link_fixed_count = (uint32_t) p->index.fix_mat4x4_count;
	}
}
static void iphyee_bonex_cache_initial_loop_fixed(iphyee_bonex_cache_s *restrict r, const iphyee_bonex_s *restrict bonex)
{
	iphyee_mat4x4_t *restrict fixed_p_array;
	iphyee_mat4x4_t *restrict fixed_n_array;
	iphyee_bonex_cache_fa_t *restrict fa;
	uintptr_t i, n;
	fixed_p_array = r->fixed_p_array;
	fixed_n_array = r->fixed_n_array;
	for (i = 0, n = bonex->fix_mat4x4_count; i < n; ++i)
	{
		iphyee_mat4x4_set_e(fixed_p_array + i);
		iphyee_mat4x4_set_e(fixed_n_array + i);
		fa = &r->fa_array[i];
		fa->need_update = 1;
		fa->base_joint_index = ~(uint32_t) 0;
		fa->link_value_start = ~(uint32_t) 0;
		fa->link_value_count = 0;
	}
}
static void iphyee_bonex_cache_initial_loop_value(iphyee_bonex_cache_s *restrict r, const iphyee_bonex_s *restrict bonex)
{
	iphyee_mat4x4_t *restrict value_p_array;
	iphyee_mat4x4_t *restrict value_n_array;
	iphyee_bonex_cache_va_t *restrict va;
	uintptr_t i, n;
	value_p_array = r->value_p_array;
	value_n_array = r->value_n_array;
	for (i = 0, n = bonex->value_count; i < n; ++i)
	{
		iphyee_mat4x4_set_e(value_p_array + i);
		iphyee_mat4x4_set_e(value_n_array + i);
		va = &r->va_array[i];
		va->need_update = 1;
		va->base_joint_index = ~(uint32_t) 0;
		va->base_fixed_index = ~(uint32_t) 0;
		va->base_inode_index = ~(uint32_t) 0;
		va->value_simple = NULL;
		va->value_complex = NULL;
	}
}
static iphyee_bonex_cache_s* iphyee_bonex_cache_initial_loop_coord(iphyee_bonex_cache_s *restrict r, const iphyee_bonex_s *restrict bonex)
{
	const iphyee_bonex_coord_s *restrict p;
	iphyee_bonex_cache_fa_t *restrict fa;
	iphyee_bonex_cache_va_t *restrict va;
	uintptr_t i, n;
	uint32_t vi, fi, ji;
	for (i = 0, n = bonex->coord_count; i < n; ++i)
	{
		p = bonex->coord_array[i];
		vi = (uint32_t) p->index.this_value_index;
		fi = (uint32_t) p->index.fix_mat4x4_index;
		ji = (uint32_t) p->index.base_joint_index;
		if (vi >= r->value_count) goto label_fail;
		va = &r->va_array[vi];
		va->base_joint_index = ji;
		va->base_fixed_index = fi;
		va->value_simple = p->bonex_simple_fix;
		if (fi >= r->fixed_count) goto label_fail;
		fa = &r->fa_array[fi];
		fa->base_joint_index = ji;
		if (!iphyee_bonex_cache_initial_link_value(&fa->link_value_start, &fa->link_value_count, vi))
			goto label_fail;
	}
	return r;
	label_fail:
	return NULL;
}
static iphyee_bonex_cache_s* iphyee_bonex_cache_initial_loop_inode(iphyee_bonex_cache_s *restrict r, const iphyee_bonex_s *restrict bonex)
{
	const iphyee_bonex_inode_s *restrict p;
	iphyee_bonex_cache_ia_t *restrict ia;
	iphyee_bonex_cache_fa_t *restrict fa;
	iphyee_bonex_cache_va_t *restrict va;
	uintptr_t i, n;
	uint32_t vi, fi, ji;
	for (i = 0, n = bonex->inode_count; i < n; ++i)
	{
		p = bonex->inode_array[i];
		ia = &r->ia_array[i];
		ia->value_minimum = p->default_value_minimum;
		ia->value_maximum = p->default_value_maximum;
		ia->value_multiplier = p->default_value_multiplier;
		ia->value_addend = p->default_value_addend;
		vi = (uint32_t) p->index.this_value_index;
		fi = (uint32_t) p->index.fix_mat4x4_index;
		ji = (uint32_t) p->index.base_joint_index;
		if (vi >= r->value_count) goto label_fail;
		va = &r->va_array[vi];
		va->base_joint_index = ji;
		va->base_fixed_index = fi;
		va->base_inode_index = (uint32_t) i;
		va->value_simple = p->bonex_simple_fix;
		va->value_complex = (iphyee_bonex_complex_s *) refer_save(p->bonex_complex_fix);
		if (fi >= r->fixed_count) goto label_fail;
		fa = &r->fa_array[fi];
		fa->base_joint_index = ji;
		if (!iphyee_bonex_cache_initial_link_value(&fa->link_value_start, &fa->link_value_count, vi))
			goto label_fail;
	}
	return r;
	label_fail:
	return NULL;
}

iphyee_bonex_cache_s* iphyee_bonex_cache_alloc(const iphyee_bonex_s *restrict bonex)
{
	iphyee_bonex_cache_s *restrict r;
	uint32_t i, n;
	if (bonex->bonex_index_okay && (r = iphyee_bonex_cache_alloc_empty(
		bonex->joint_count, bonex->fix_mat4x4_count, bonex->inode_count, bonex->value_count)))
	{
		iphyee_bonex_cache_initial_loop_joint(r, bonex);
		iphyee_bonex_cache_initial_loop_fixed(r, bonex);
		iphyee_bonex_cache_initial_loop_value(r, bonex);
		if (iphyee_bonex_cache_initial_loop_coord(r, bonex) &&
			iphyee_bonex_cache_initial_loop_inode(r, bonex))
		{
			for (i = 0, n = r->value_count; i < n; ++i)
				iphyee_bonex_cache_set_value(r, i, 0);
			return r;
		}
		refer_free(r);
	}
	return NULL;
}

iphyee_bonex_cache_s* iphyee_bonex_cache_dump(const iphyee_bonex_cache_s *restrict cache)
{
	iphyee_bonex_cache_s *restrict r;
	iphyee_bonex_cache_va_t *restrict p;
	uint32_t i, n;
	if ((r = iphyee_bonex_cache_alloc_empty(cache->joint_count, cache->fixed_count, cache->inode_count, cache->value_count)))
	{
		memcpy(r->joint_p_array, cache->joint_p_array, sizeof(iphyee_mat4x4_t) * r->joint_count);
		memcpy(r->joint_n_array, cache->joint_n_array, sizeof(iphyee_mat4x4_t) * r->joint_count);
		memcpy(r->fixed_p_array, cache->fixed_p_array, sizeof(iphyee_mat4x4_t) * r->fixed_count);
		memcpy(r->fixed_n_array, cache->fixed_n_array, sizeof(iphyee_mat4x4_t) * r->fixed_count);
		memcpy(r->value_p_array, cache->value_p_array, sizeof(iphyee_mat4x4_t) * r->value_count);
		memcpy(r->value_n_array, cache->value_n_array, sizeof(iphyee_mat4x4_t) * r->value_count);
		memcpy(r->va_array, cache->va_array, sizeof(iphyee_bonex_cache_va_t) * r->value_count);
		memcpy(r->ia_array, cache->ia_array, sizeof(iphyee_bonex_cache_ia_t) * r->inode_count);
		memcpy(r->fa_array, cache->fa_array, sizeof(iphyee_bonex_cache_fa_t) * r->fixed_count);
		memcpy(r->ja_array, cache->ja_array, sizeof(iphyee_bonex_cache_ja_t) * r->joint_count);
		memcpy(r->value_array, cache->value_array, sizeof(float) * r->value_count);
		p = r->va_array;
		n = r->value_count;
		for (i = 0; i < n; ++i)
		{
			if (p[i].value_complex)
				refer_save(p[i].value_complex);
		}
		return r;
	}
	return NULL;
}

static void iphyee_bonex_cache_update_joint(iphyee_bonex_cache_s *restrict r, uint32_t joint_start, uint32_t joint_count)
{
	iphyee_bonex_cache_ja_t *restrict pj;
	pj = r->ja_array;
	for (joint_count += joint_start; joint_start < joint_count; ++joint_start)
	{
		if (!pj[joint_start].need_update)
		{
			pj[joint_start].need_update = 1;
			iphyee_bonex_cache_update_joint(r, pj[joint_start].link_joint_start, pj[joint_start].link_joint_count);
		}
	}
}

iphyee_bonex_cache_s* iphyee_bonex_cache_set_value(iphyee_bonex_cache_s *restrict r, uint32_t value_index, float value)
{
	iphyee_bonex_cache_va_t *restrict pv;
	iphyee_bonex_cache_ia_t *restrict pi;
	if (value_index < r->value_count)
	{
		pv = &r->va_array[value_index];
		if (pv->base_inode_index < r->inode_count)
		{
			pi = &r->ia_array[pv->base_inode_index];
			if (value < pi->value_minimum) value = pi->value_minimum;
			if (value > pi->value_maximum) value = pi->value_maximum;
			value = value * pi->value_multiplier + pi->value_addend;
		}
		r->value_array[value_index] = value;
		if (!pv->need_update)
		{
			pv->need_update = 1;
			iphyee_bonex_cache_update_joint(r, pv->base_inode_index, 1);
		}
		return r;
	}
	return NULL;
}

static void iphyee_bonex_cache_fetch_value(iphyee_bonex_cache_s *restrict r, uint32_t value_start, uint32_t value_count)
{
	iphyee_bonex_cache_va_t *restrict va;
	iphyee_mat4x4_t *restrict value_p_array;
	iphyee_mat4x4_t *restrict value_n_array;
	float *restrict value_array;
	iphyee_bonex_simple_f value_simple;
	iphyee_bonex_complex_s *restrict value_complex;
	float value;
	va = r->va_array;
	value_p_array = r->value_p_array;
	value_n_array = r->value_n_array;
	value_array = r->value_array;
	for (value_count += value_start; value_start < value_count; ++value_start)
	{
		if (va[value_start].need_update)
		{
			va[value_start].need_update = 0;
			value = value_array[value_start];
			if ((value_simple = va[value_start].value_simple))
			{
				value_simple(value_p_array + value_start, value);
				value_simple(value_n_array + value_start, -value);
			}
			else if ((value_complex = va[value_start].value_complex))
			{
				value_complex->mat4x4gen(value_complex, value_p_array + value_start, value);
				value_complex->mat4x4gen(value_complex, value_n_array + value_start, -value);
			}
		}
	}
}
static void iphyee_bonex_cache_fetch_fixed(iphyee_bonex_cache_s *restrict r, uint32_t fixed_start, uint32_t fixed_count)
{
	iphyee_bonex_cache_pn_context_t context;
	iphyee_mat4x4_t *restrict fixed_p_array;
	iphyee_mat4x4_t *restrict fixed_n_array;
	iphyee_mat4x4_t *restrict value_p_array;
	iphyee_mat4x4_t *restrict value_n_array;
	iphyee_bonex_cache_fa_t *restrict fa;
	uintptr_t link_value_start;
	uintptr_t link_value_count;
	fixed_p_array = r->fixed_p_array;
	fixed_n_array = r->fixed_n_array;
	value_p_array = r->value_p_array;
	value_n_array = r->value_n_array;
	fa = r->fa_array;
	for (fixed_count += fixed_start; fixed_start < fixed_count; ++fixed_start)
	{
		if (fa[fixed_start].need_update)
		{
			fa[fixed_start].need_update = 0;
			link_value_start = fa[fixed_start].link_value_start;
			link_value_count = fa[fixed_start].link_value_count;
			iphyee_bonex_cache_fetch_value(r, link_value_start, link_value_count);
			iphyee_bonex_cache_pn_initial(&context, fixed_p_array + fixed_start, fixed_n_array + fixed_start, link_value_count);
			iphyee_bonex_cache_pn_vector(&context, value_p_array + link_value_start, value_n_array + link_value_start, link_value_count);
		}
	}
}
static void iphyee_bonex_cache_touch_joint(iphyee_bonex_cache_s *restrict r, uint32_t joint_index)
{
	iphyee_bonex_cache_pn_context_t context;
	iphyee_bonex_cache_ja_t *restrict ja;
	uintptr_t base_joint_index;
	uintptr_t base_joint_exist;
	uintptr_t link_fixed_start;
	uintptr_t link_fixed_count;
	ja = r->ja_array + joint_index;
	base_joint_exist = ((base_joint_index = ja->base_joint_index) < r->joint_count)?1:0;
	link_fixed_start = ja->link_fixed_start;
	link_fixed_count = ja->link_fixed_count;
	iphyee_bonex_cache_pn_initial(&context, r->joint_p_array + joint_index, r->joint_n_array + joint_index, link_fixed_count + base_joint_exist);
	if (base_joint_exist) iphyee_bonex_cache_pn_append(&context, r->joint_p_array + base_joint_index, r->joint_n_array + base_joint_index);
	iphyee_bonex_cache_pn_vector(&context, r->fixed_p_array + link_fixed_start, r->fixed_n_array + link_fixed_start, link_fixed_count);
}
static iphyee_bonex_cache_s* iphyee_bonex_cache_fetch_joint(iphyee_bonex_cache_s *restrict r, uint32_t joint_index)
{
	iphyee_bonex_cache_ja_t *restrict ja;
	if (joint_index < r->joint_count)
	{
		if ((ja = r->ja_array)[joint_index].need_update)
		{
			ja[joint_index].need_update = 0;
			iphyee_bonex_cache_fetch_joint(r, ja[joint_index].base_joint_index);
			iphyee_bonex_cache_fetch_fixed(r, ja[joint_index].link_fixed_start, ja[joint_index].link_fixed_count);
			iphyee_bonex_cache_touch_joint(r, joint_index);
		}
		return r;
	}
	return NULL;
}
const iphyee_mat4x4_t* iphyee_bonex_cache_joint_p_mat4x4(iphyee_bonex_cache_s *restrict r, uint32_t joint_index)
{
	if (iphyee_bonex_cache_fetch_joint(r, joint_index))
		return &r->joint_p_array[joint_index];
	return NULL;
}
const iphyee_mat4x4_t* iphyee_bonex_cache_joint_n_mat4x4(iphyee_bonex_cache_s *restrict r, uint32_t joint_index)
{
	if (iphyee_bonex_cache_fetch_joint(r, joint_index))
		return &r->joint_n_array[joint_index];
	return NULL;
}
