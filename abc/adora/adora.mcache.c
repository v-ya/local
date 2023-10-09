#include "adora.h"
#include <exbuffer.h>
#include <memory.h>

struct abc_adora_mcache_s {
	exbuffer_t mcache;
};

static void adora_mcache_free_func(abc_adora_mcache_s *restrict r)
{
	exbuffer_uini(&r->mcache);
}

abc_adora_mcache_s* abc_adora_mcache_alloc(void)
{
	abc_adora_mcache_s *restrict r;
	if ((r = (abc_adora_mcache_s *) refer_alloz(sizeof(abc_adora_mcache_s))))
	{
		refer_set_free(r, (refer_free_f) adora_mcache_free_func);
		if (exbuffer_init(&r->mcache, 0))
			return r;
		refer_free(r);
	}
	return NULL;
}

abc_adora_mcache_s* abc_adora_mcache_append(abc_adora_mcache_s *restrict m, const void *restrict data, uintptr_t size)
{
	if (exbuffer_append(&m->mcache, data, size))
		return m;
	return NULL;
}

abc_adora_mcache_s* abc_adora_mcache_fillch(abc_adora_mcache_s *restrict m, int32_t ch, uintptr_t count)
{
	uint8_t *restrict dst;
	uintptr_t n;
	n = m->mcache.used;
	if ((dst = (uint8_t *) exbuffer_need(&m->mcache, n + count)))
	{
		memset(dst + n, ch, count);
		m->mcache.used = n + count;
		return m;
	}
	return NULL;
}

uintptr_t abc_adora_mcache_offset(const abc_adora_mcache_s *restrict m)
{
	return m->mcache.used;
}

abc_adora_mcache_s* abc_adora_mcache_restore(abc_adora_mcache_s *restrict m, uintptr_t offset)
{
	if (offset <= m->mcache.used)
	{
		m->mcache.used = offset;
		return m;
	}
	return NULL;
}

uint8_t* abc_adora_mcache_mapping(abc_adora_mcache_s *restrict m, uintptr_t *restrict size)
{
	if (size) *size = m->mcache.used;
	return m->mcache.data;
}

void abc_adora_mcache_clear(abc_adora_mcache_s *restrict m)
{
	m->mcache.used = 0;
}
