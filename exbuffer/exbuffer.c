#include "exbuffer.h"
#include <malloc.h>
#include <memory.h>

#define exbuffer_default_size   4096
#define exbuffer_min_size       64
#define exbuffer_max_log2align  20

#define exbuffer_align2mask(_l2a)  (((uintptr_t) 1 << _l2a) - 1)

static inline uintptr_t exbuffer_size_fix(register uintptr_t size)
{
	if (size && size <= 0x80000000)
	{
		size -= 1;
		size |= size >> 16;
		size |= size >> 8;
		size |= size >> 4;
		size |= size >> 2;
		size |= size >> 1;
		size += 1;
		return size;
	}
	return 0;
}

static inline uintptr_t exbuffer_default_align(register uintptr_t cpos)
{
	return (cpos + 15) & ~(uintptr_t) 15;
}

static inline void* exbuffer_align(void *ptr, register uint32_t log2align)
{
	register uintptr_t m = exbuffer_align2mask(log2align);
	return (void *) (((uintptr_t) ptr + m) & ~m);
}

static exbuffer_t* exbuffer_exsize(exbuffer_t *restrict r, uintptr_t size)
{
	uint8_t *restrict data;
	if ((size = exbuffer_size_fix(size)) &&
		(data = realloc(r->data, size)))
	{
		r->data = data;
		r->size = size;
		return r;
	}
	return NULL;
}

exbuffer_t* exbuffer_init(exbuffer_t *restrict eb, register uintptr_t dsize)
{
	if (!dsize)
		dsize = exbuffer_default_size;
	if (dsize < exbuffer_min_size)
		dsize = exbuffer_min_size;
	if ((dsize = exbuffer_size_fix(dsize)))
	{
		eb->data = (uint8_t *) malloc(dsize);
		if (eb->data)
		{
			eb->size = dsize;
			eb->used = 0;
			eb->cpos = 0;
			return eb;
		}
	}
	return NULL;
}

void exbuffer_uini(exbuffer_t *restrict eb)
{
	if (eb->data)
		free(eb->data);
	eb->data = NULL;
	eb->size = 0;
	eb->used = 0;
	eb->cpos = 0;
}

exbuffer_t* exbuffer_alloc(uintptr_t dsize)
{
	exbuffer_t *restrict r;
	r = (exbuffer_t *) malloc(sizeof(exbuffer_t));
	if (r)
	{
		if (exbuffer_init(r, dsize))
			return r;
		free(r);
	}
	return NULL;
}

void exbuffer_free(exbuffer_t *restrict eb)
{
	exbuffer_uini(eb);
	free(eb);
}

uintptr_t exbuffer_save(const exbuffer_t *restrict eb)
{
	return eb->cpos;
}

void exbuffer_load(exbuffer_t *restrict eb, uintptr_t cpos)
{
	eb->cpos = cpos;
	eb->used = 0;
}

void exbuffer_clear(exbuffer_t *restrict eb)
{
	eb->cpos = 0;
	eb->used = 0;
}

void* exbuffer_need(exbuffer_t *restrict eb, uintptr_t size)
{
	uintptr_t want_size;
	if (eb->size - eb->cpos >= size)
	{
		label_okay:
		return eb->data + eb->cpos;
	}
	if ((want_size = size + eb->cpos) > eb->size)
	{
		if (exbuffer_exsize(eb, want_size))
			goto label_okay;
	}
	return NULL;
}

void* exbuffer_mono(exbuffer_t *restrict eb, uintptr_t size, uint32_t log2align, uintptr_t *restrict rpos)
{
	void *restrict r;
	if (log2align <= exbuffer_max_log2align)
	{
		size += exbuffer_align2mask(log2align);
		if ((r = exbuffer_need(eb, size)))
		{
			eb->cpos = exbuffer_default_align(eb->cpos + size);
			r = exbuffer_align(r, log2align);
			if (rpos) *rpos = (uintptr_t) r - (uintptr_t) eb->data;
			return r;
		}
	}
	return NULL;
}

void* exbuffer_monz(exbuffer_t *restrict eb, uintptr_t size, uint32_t log2align, uintptr_t *restrict rpos)
{
	void *restrict r;
	if (log2align <= exbuffer_max_log2align)
	{
		uintptr_t osize;
		osize = size;
		size += exbuffer_align2mask(log2align);
		if ((r = exbuffer_need(eb, size)))
		{
			eb->cpos = exbuffer_default_align(eb->cpos + size);
			r = exbuffer_align(r, log2align);
			if (rpos) *rpos = (uintptr_t) r - (uintptr_t) eb->data;
			return memset(r, 0, osize);
		}
	}
	return NULL;
}

void* exbuffer_append(exbuffer_t *restrict eb, const void *data, uintptr_t size)
{
	void *restrict r;
	if ((r = exbuffer_need(eb, eb->used + size)))
	{
		memcpy(r + eb->used, data, size);
		eb->used += size;
	}
	return r;
}

void* exbuffer_get_ptr(const exbuffer_t *restrict eb, uintptr_t rpos, uint32_t log2align)
{
	return exbuffer_align(eb->data + rpos, log2align);
}

void* exbuffer_curr_ptr(const exbuffer_t *restrict eb, uintptr_t *restrict used)
{
	if (used) *used = eb->used;
	return eb->data + eb->cpos;
}
