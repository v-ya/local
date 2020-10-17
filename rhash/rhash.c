#include "rhash.h"
#include <memory.h>

static const char *rhash_default_method = "*>^~";
static const uint32_t rhash_default_level = 4;

static void rhash_free_func(register rhash_s *restrict r)
{
	if (r->random)
		refer_free(r->random);
}

rhash64_s* rhash64_alloc_by_r64(random64_s *restrict r64, size_t n, uint64_t key)
{
	register rhash64_s *restrict r;
	if (r64 && n)
	{
		r = (rhash64_s *) refer_alloz(sizeof(random64_s) + sizeof(*r->rdata) * n);
		if (r)
		{
			refer_set_free(r, (refer_free_f) rhash_free_func);
			r->header.random = (random_s *) refer_save(r64);
			r->header.size = n;
			r64->init((random_s *) r64, key);
			return r;
		}
	}
	return NULL;
}

rhash64_s* rhash64_alloc(const char *restrict method, size_t n, uint64_t key)
{
	register rhash64_s *restrict r;
	register random_s *restrict r64;
	r = NULL;
	if (!method) method = rhash_default_method;
	r64 = random64_alloc_sequence(rhash_default_level, method);
	if (r64)
	{
		r = rhash64_alloc_by_r64(&r64->r64, n, key);
		refer_free(r64);
	}
	return r;
}

void rhash64_reinit(rhash64_s *restrict r, uint64_t key)
{
	memset(r->rdata, 0, sizeof(*r->rdata) * r->header.size);
	r->header.random->r64.init(r->header.random, key);
	r->header.index = 0;
}

void rhash64_send(rhash64_s *restrict r, register const uint64_t *restrict data, size_t n)
{
	register random_s *restrict rd;
	register random64_set_f func;
	register uint64_t *restrict rdata;
	register size_t i;
	uint64_t *rdata_save;
	size_t nn;
	rd = r->header.random;
	func = rd->r64.set;
	i = r->header.index;
	nn = r->header.size;
	rdata_save = r->rdata;
	if (i + n >= nn)
	{
		rdata = rdata_save + i;
		i = nn - i;
		n -= i;
		do {
			*rdata++ ^= func(rd ,*data++);
		} while (--i);
		while (n >= nn)
		{
			n -= nn;
			i = nn;
			rdata = rdata_save;
			do {
				*rdata++ ^= func(rd ,*data++);
			} while (--i);
		}
		if (!n) goto label_end;
	}
	rdata = rdata_save + i;
	n += i;
	i = n - i;
	while (i--)
		*rdata++ ^= func(rd ,*data++);
	label_end:
	r->header.index = n;
	return ;
}

uint64_t* rhash64_final(rhash64_s *restrict r)
{
	register random_s *restrict random;
	register uint64_t *restrict rdata;
	register size_t n;
	register uint64_t x;
	random = r->header.random;
	rdata = r->rdata;
	n = r->header.size;
	x = 0;
	do {
		x ^= rdata[--n];
	} while (n);
	n = r->header.size;
	do {
		x ^= (*rdata++ ^= random64_set(random, x));
	} while (--n);
	return r->rdata;
}
