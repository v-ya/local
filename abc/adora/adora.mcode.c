#include "adora.h"
#include <exbuffer.h>
#include <memory.h>

typedef struct adora_mcode_s adora_mcode_s;

struct adora_mcode_s {
	abc_adora_mcode_s am;
	exbuffer_t mcode;
};

// am func

static abc_adora_mcode_s* adora_mcode_append(adora_mcode_s *restrict m, const void *restrict data, uintptr_t size)
{
	if (exbuffer_append(&m->mcode, data, size))
		return &m->am;
	return NULL;
}

static abc_adora_mcode_s* adora_mcode_fillch(adora_mcode_s *restrict m, int32_t ch, uintptr_t count)
{
	uint8_t *restrict dst;
	uintptr_t n;
	n = m->mcode.used;
	if ((dst = (uint8_t *) exbuffer_need(&m->mcode, n + count)))
	{
		memset(dst + n, ch, count);
		m->mcode.used = n + count;
		return &m->am;
	}
	return NULL;
}

static uintptr_t adora_mcode_offset(const adora_mcode_s *restrict m)
{
	return m->mcode.used;
}

static abc_adora_mcode_s* adora_mcode_restore(adora_mcode_s *restrict m, uintptr_t offset)
{
	if (offset <= m->mcode.used)
	{
		m->mcode.used = offset;
		return &m->am;
	}
	return NULL;
}

static uint8_t* adora_mcode_mapping(adora_mcode_s *restrict m, uintptr_t *restrict size)
{
	if (size) *size = m->mcode.used;
	return m->mcode.data;
}

static void adora_mcode_clear(adora_mcode_s *restrict m)
{
	m->mcode.used = 0;
}

// inner api

static void adora_mcode_free_func(adora_mcode_s *restrict r)
{
	exbuffer_uini(&r->mcode);
}

abc_adora_mcode_s* abc_adora_mcode_alloc(void)
{
	adora_mcode_s *restrict r;
	if ((r = (adora_mcode_s *) refer_alloz(sizeof(adora_mcode_s))))
	{
		refer_set_free(r, (refer_free_f) adora_mcode_free_func);
		if (exbuffer_init(&r->mcode, 0))
		{
			#define d_fset(_n)  r->am._n = (abc_adora_mcode_##_n##_f) adora_mcode_##_n
			d_fset(append);
			d_fset(fillch);
			d_fset(offset);
			d_fset(restore);
			d_fset(mapping);
			d_fset(clear);
			#undef d_fset
			return &r->am;
		}
		refer_free(r);
	}
	return NULL;
}
