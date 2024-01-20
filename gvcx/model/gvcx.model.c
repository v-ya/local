#include "gvcx.model.h"

gvcx_model_item_s* gvcx_model_inner_create_item(const gvcx_model_s *restrict m, gvcx_model_flag_t flag, const char *restrict tname)
{
	const gvcx_model_type_s *restrict t;
	gvcx_model_type_t type;
	type = gvcx_model_type_enum(tname);
	t = m->type[type];
	if (t->create) return t->create(t, m, type, flag, tname);
	return NULL;
}

static void gvcx_model_free_func(refer_t *restrict r)
{
	uintptr_t i, n;
	for (i = 0, n = sizeof(gvcx_model_s) / sizeof(refer_t); i < n; ++i)
	{
		if (r[i]) refer_free(r[i]);
	}
}

const gvcx_model_s* gvcx_model_alloc(void)
{
	gvcx_model_s *restrict r;
	if ((r = (gvcx_model_s *) refer_alloz(sizeof(gvcx_model_s))))
	{
		refer_set_free(r, (refer_free_f) gvcx_model_free_func);
		if (!(r->pool_enum = vattr_alloc())) goto label_fail;
		if (!(r->pool_object = vattr_alloc())) goto label_fail;
		if (!(r->pool_preset = vattr_alloc())) goto label_fail;
		#define d_type(_t)  if (!(r->type[gvcx_model_type__##_t] = gvcx_model_type_create__##_t())) goto label_fail
		d_type(any);
		d_type(u);
		d_type(i);
		d_type(f);
		d_type(b);
		d_type(s);
		d_type(d);
		d_type(e);
		d_type(a);
		d_type(o);
		d_type(preset);
		#undef d_type
		return r;
		label_fail:
		refer_free(r);
	}
	return NULL;
}

gvcx_model_item_s* gvcx_model_create_item(const gvcx_model_s *restrict m, const char *restrict type)
{
	return gvcx_model_inner_create_item(m, gvcx_model_flag__write, type);
}

gvcx_model_item_s* gvcx_model_copyto_item(const gvcx_model_s *restrict m, gvcx_model_item_s *restrict dst, const gvcx_model_item_s *restrict src)
{
	const gvcx_model_type_s *restrict t;
	if ((uintptr_t) dst != (uintptr_t) src && dst->type == src->type &&
		(dst->flag & gvcx_model_flag__write) && dst->tname == src->tname)
	{
		t = m->type[dst->type];
		if (t->copyto) return t->copyto(t, dst, src);
	}
	return NULL;
}
