#include "kiya_args.h"
#include <stdlib.h>
#include <string.h>

#define kiya_args_size       8

struct kiya_args_pool_t {
	uintptr_t pos;
	uintptr_t size;
	char data[];
};

static kiya_args_t* kiya_args_ex(kiya_args_t *restrict args)
{
	void *r;
	r = realloc(args->argv, args->size * sizeof(const char *) * 2);
	if (r)
	{
		args->argv = (const char **) r;
		args->size <<= 1;
		return args;
	}
	return NULL;
}

static inline kiya_args_pool_t* kiya_args_pool_check(kiya_args_pool_t *restrict pool, register uintptr_t n)
{
	if (n + pool->pos <= pool->size)
		return pool;
	return NULL;
}

kiya_args_pool_t* kiya_args_pool_alloc(uintptr_t size)
{
	kiya_args_pool_t *restrict r;
	r = (kiya_args_pool_t *) malloc(sizeof(kiya_args_pool_t) + size);
	if (r)
	{
		r->pos = 0;
		r->size = size;
	}
	return r;
}

kiya_args_t* kiya_args_alloc(void)
{
	kiya_args_t *restrict r;
	r = (kiya_args_t *) malloc(sizeof(kiya_args_t));
	if (r)
	{
		r->n = 0;
		if ((r->argv = (const char **) malloc(sizeof(const char *) * (r->size = kiya_args_size))))
			return r;
		free(r);
	}
	return NULL;
}

void kiya_args_pool_free(kiya_args_pool_t *restrict r)
{
	free(r);
}

void kiya_args_free(kiya_args_t *restrict r)
{
	free(r->argv);
	free(r);
}

void kiya_args_idol(kiya_args_t *restrict args, const char *restrict idol)
{
	if (!args->n && idol)
		args->argv[args->n++] = idol;
}

kiya_args_pool_t* kiya_args_set(kiya_args_pool_t *restrict pool, kiya_args_t *restrict args, const char *restrict value)
{
	uintptr_t n;
	n = strlen(value) + 1;
	if (kiya_args_pool_check(pool, n))
	{
		memcpy(pool->data + pool->pos, value, n);
		if (args->n < args->size || kiya_args_ex(args))
		{
			args->argv[args->n++] = pool->data + pool->pos;
			pool->pos += n;
			return pool;
		}
	}
	return NULL;
}
