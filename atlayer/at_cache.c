#include "at_cache.h"
#include <stdlib.h>

void at_cache_init(register at_cache_t *restrict atc, uintptr_t size, uintptr_t number)
{
	if (!size)
		size = sizeof(void *);
	if (number < 16)
		number = 16;
	atc->list = NULL;
	atc->ptr = NULL;
	atc->size = (size + (sizeof(void *) - 1)) & ~(sizeof(void *) - 1);
	atc->number = number;
}

void at_cache_uini(register at_cache_t *restrict atc)
{
	register void *next, *p;
	next = atc->ptr;
	atc->list = NULL;
	atc->ptr = NULL;
	while ((p = next))
	{
		next = *(void **) p;
		free(p);
	}
}

static void* at_cache_pull_alloc(register at_cache_t *restrict atc)
{
	register void *p, *q;
	register uintptr_t n, size;
	p = malloc((n = atc->number) * (size = atc->size));
	if (p)
	{
		*(void **) p = atc->ptr;
		atc->ptr = p;
		p = (void *) ((uintptr_t) p + size);
		--n;
		q = atc->list;
		while (--n)
		{
			*(void **) p = q;
			q = p;
			p = (void *) ((uintptr_t) p + size);
		}
		atc->list = q;
	}
	return p;
}

void* at_cache_pull(register at_cache_t *restrict atc)
{
	register void *p;
	if ((p = atc->list))
	{
		atc->list = *(void **) p;
		return p;
	}
	return at_cache_pull_alloc(atc);
}

void at_cache_push(register at_cache_t *restrict atc, register void *v)
{
	*(void **) v = atc->list;
	atc->list = v;
}

uintptr_t at_cache_b_leak(register const at_cache_t *restrict atc)
{
	register void *v;
	register uintptr_t a, b;
	a = b = 0;
	v = atc->list;
	while (v)
	{
		++a;
		v = *(void **) v;
	}
	v = atc->ptr;
	while (v)
	{
		++b;
		v = *(void **) v;
	}
	return b * atc->number - b - a;
}

struct at_cache_array_t {
	uintptr_t offset;
	uintptr_t number;
	uintptr_t tail;
	uintptr_t size;
	at_cache_t atc_array[];
};

at_cache_array_t* at_cache_array_alloc(uintptr_t size, uintptr_t number, uintptr_t offset, uintptr_t nc)
{
	register at_cache_array_t *restrict r;
	register uintptr_t i;
	if (size && number)
	{
		r = (at_cache_array_t *) malloc(sizeof(at_cache_array_t) + sizeof(at_cache_t) * number);
		if (r)
		{
			r->offset = offset;
			r->number = number;
			r->tail = offset + number;
			r->size = size;
			for (i = 0; i < number; ++i)
				at_cache_init(r->atc_array + i, size * (offset + i), nc);
			return r;
		}
	}
	return NULL;
}

void at_cache_array_free(register at_cache_array_t *restrict atca)
{
	register uintptr_t i;
	i = atca->number;
	do {
		at_cache_uini(atca->atc_array + --i);
	} while (i);
	free(atca);
}

void* at_cache_array_pull(register at_cache_array_t *restrict atca, uintptr_t n)
{
	if (n < atca->tail)
		return at_cache_pull(atca->atc_array + ((n > atca->offset)?(n - atca->offset):0));
	return malloc(atca->size * n);
}

void at_cache_array_push(register at_cache_array_t *restrict atca, void *v, uintptr_t n)
{
	if (n < atca->tail)
		at_cache_push(atca->atc_array + ((n > atca->offset)?(n - atca->offset):0), v);
	else free(v);
}

uintptr_t at_cache_array_b_leak(register const at_cache_array_t *restrict atca)
{
	register uintptr_t r, i, n;
	r = 0;
	n = atca->number;
	for (i = 0; i < n; ++i)
		r += at_cache_b_leak(atca->atc_array + i);
	return r;
}
