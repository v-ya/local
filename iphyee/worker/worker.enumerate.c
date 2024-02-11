#include "worker.inner.h"

static void iphyee_worker_enumerate_free_func(iphyee_worker_enumerate_s *restrict r)
{
	refer_t *restrict p;
	uintptr_t i, n;
	p = r->array;
	n = r->count;
	for (i = 0; i < n; ++i)
	{
		if (p[i]) refer_free(p[i]);
	}
}

iphyee_worker_enumerate_s* iphyee_worker_enumerate_alloc(uintptr_t count, uintptr_t bsize)
{
	iphyee_worker_enumerate_s *restrict r;
	uintptr_t size;
	size = (sizeof(iphyee_worker_enumerate_s) + sizeof(refer_t) * 2) + count * (bsize + sizeof(refer_t));
	if ((r = (iphyee_worker_enumerate_s *) refer_alloz(size)))
	{
		refer_set_free(r, (refer_free_f) iphyee_worker_enumerate_free_func);
		r->count = count;
		r->bsize = bsize;
		r->array = (refer_t *) (r + 1);
		r->edata = (void *) (r->array + ((count + 1) & ~(uintptr_t) 1));
		return r;
	}
	return NULL;
}
