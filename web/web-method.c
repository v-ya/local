#include "web-method.h"
#include <stdlib.h>
#include <memory.h>

struct web_method_s {
	refer_nstring_t *m;
	uintptr_t size;
	uintptr_t used;
	uintptr_t max_size;
};

static void web_method_free_func(web_method_s *restrict r)
{
	refer_nstring_t *restrict p;
	if ((p = r->m))
	{
		for (uintptr_t i = 0; i < r->used; ++i)
			if (p[i]) refer_free(p[i]);
		free(p);
	}
}

static web_method_s* web_method_alloc_empty(uintptr_t default_size, uintptr_t max_size)
{
	web_method_s *restrict r;
	if (default_size && (r = (web_method_s *) refer_alloz(sizeof(web_method_s))))
	{
		refer_set_free(r, (refer_free_f) web_method_free_func);
		r->max_size = max_size;
		if (!(r->size = default_size) ||
			(r->m = (refer_nstring_t *) calloc(default_size, sizeof(refer_nstring_t))))
			return r;
		refer_free(r);
	}
	return NULL;
}

static uintptr_t web_method_alloc_exsize(web_method_s *restrict r)
{
	refer_nstring_t *m;
	uintptr_t size;
	if (r->size < r->max_size)
	{
		size = r->size << 1;
		if (size > r->max_size)
			size = r->max_size;
		m = (refer_nstring_t *) realloc(r->m, sizeof(refer_nstring_t) * size);
		if (m)
		{
			memset(m + r->size, 0, sizeof(refer_nstring_t) * (size - r->size));
			r->m = m;
			r->size = size;
		}
	}
	return r->size;
}

web_method_s* web_method_alloc(void)
{
	web_method_s *restrict r;
	if ((r = web_method_alloc_empty(web_method_id_custom * 2, 1024)))
	{
		#define d_method(_id, _name)  if (!(r->m[web_method_id_##_id] = refer_dump_nstring(#_name))) goto label_fail
		d_method(get, GET);
		d_method(head, HEAD);
		d_method(post, POST);
		d_method(put, PUT);
		d_method(delete, DELETE);
		d_method(trace, TRACE);
		d_method(options, OPTIONS);
		d_method(connect, CONNECT);
		r->used = web_method_id_custom + 1;
		#undef d_method
		return r;
		label_fail:
		refer_free(r);
	}
	return NULL;
}

refer_nstring_t web_method_get(web_method_s *restrict r, web_method_id_t mid)
{
	if ((uintptr_t) mid < r->used)
		return r->m[(uintptr_t) mid];
	return NULL;
}

refer_nstring_t web_method_set(web_method_s *restrict r, const char *restrict method, web_method_id_t *restrict mid)
{
	uintptr_t p;
	if (method && ((p = r->used) < r->size || p < web_method_alloc_exsize(r)))
	{
		if ((r->m[p] = refer_dump_nstring(method)))
		{
			r->used = p + 1;
			if (mid) *mid = (web_method_id_t) p;
			return r->m[p];
		}
	}
	return NULL;
}
