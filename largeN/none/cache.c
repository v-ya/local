#include "../largeN.h"
#include <stdlib.h>
#include <memory.h>

largeN_cache_t* largeN_cache_alloc(register uint64_t size)
{
	register largeN_cache_t *restrict r;
	r = (largeN_cache_t *) malloc(sizeof(largeN_cache_t));
	if (r)
	{
		if (!size) size = 1024;
		if ((r->cache = (uint64_t *) malloc(sizeof(uint64_t) * size)))
		{
			r->size = size;
			r->number = 0;
			return r;
		}
		free(r);
	}
	return NULL;
}

void largeN_cache_free(register largeN_cache_t *restrict cache)
{
	free(cache->cache);
	free(cache);
}

uint64_t largeN_cache_get(register largeN_cache_t *restrict cache)
{
	return cache->number;
}

void largeN_cache_set(register largeN_cache_t *restrict cache, register uint64_t stack)
{
	if (stack > cache->size)
		stack = cache->size;
	cache->number = stack;
}

static inline largeN_cache_t* largeN_cache_resize(register largeN_cache_t *restrict cache, register uint64_t n)
{
	register uint64_t size;
	register uint64_t *restrict chip;
	size = cache->size;
	do {
		if (size & ((uint64_t) 1 << 63))
			goto label_fail;
		size <<= 1;
	} while (size < n);
	chip = (uint64_t *) realloc(cache->cache, size);
	if (chip)
	{
		cache->cache = chip;
		cache->size = size;
		return cache;
	}
	label_fail:
	return NULL;
}

uint64_t* largeN_cache_pull(register largeN_cache_t *restrict cache, register uint64_t n)
{
	if ((n += cache->number) <= cache->size)
	{
		label_pull:
		cache->number = n;
		return cache->cache + n;
	}
	else if (largeN_cache_resize(cache, n))
		goto label_pull;
	return NULL;
}

uint64_t* largeN_cache_zpull(register largeN_cache_t *restrict cache, register uint64_t n)
{
	register uint64_t z;
	z = sizeof(uint64_t) * n;
	if ((n += cache->number) <= cache->size)
	{
		label_pull:
		cache->number = n;
		return (uint64_t *) memset(cache->cache + n, 0, z);
	}
	else if (largeN_cache_resize(cache, n))
		goto label_pull;
	return NULL;
}
