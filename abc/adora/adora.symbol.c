#include "adora.h"
#include <exbuffer.h>
#include <hashmap.h>
#include <vattr.h>

typedef struct abc_adora_symbol_segment_s abc_adora_symbol_segment_s;

struct abc_adora_symbol_segment_s {
	refer_nstring_t segment;
	uint32_t segment_index;
	uint32_t symbol_count;
};

struct abc_adora_symbol_s {
	exbuffer_t symbol_buffer;
	hashmap_t symbol_pool;
	vattr_s *segment_pool;
	const abc_adora_symbol_t *symbol_array;
	uintptr_t symbol_count;
};

// segment

static void abc_adora_symbol_segment_free_func(abc_adora_symbol_segment_s *restrict r)
{
	if (r->segment) refer_free(r->segment);
}

static abc_adora_symbol_segment_s* abc_adora_symbol_segment_fetch(vattr_s *restrict segment_pool, const char *restrict segment)
{
	abc_adora_symbol_segment_s *restrict r;
	if ((r = (abc_adora_symbol_segment_s *) vattr_get_first(segment_pool, segment)))
		goto label_okay;
	else if ((r = (abc_adora_symbol_segment_s *) refer_alloz(sizeof(abc_adora_symbol_segment_s))))
	{
		refer_set_free(r, (refer_free_f) abc_adora_symbol_segment_free_func);
		if ((r->segment = refer_dump_nstring(segment)))
		{
			r->segment_index = segment_pool->vslot.number;
			r->symbol_count = 0;
			if (vattr_set(segment_pool, segment, r))
			{
				refer_free(r);
				label_okay:
				return r;
			}
		}
		refer_free(r);
	}
	return NULL;
}

// symbol

static const abc_adora_symbol_t* abc_adora_symbol_new_symbol_by_nstring(abc_adora_symbol_s *restrict s, const char *restrict segment, refer_nstring_t symbol, uint32_t symbol_type)
{
	abc_adora_symbol_segment_s *restrict seg;
	abc_adora_symbol_t *restrict p;
	if ((seg = abc_adora_symbol_segment_fetch(s->segment_pool, segment)) &&
		(p = (abc_adora_symbol_t *) exbuffer_need(&s->symbol_buffer, (s->symbol_count + 1) * sizeof(*p))))
	{
		s->symbol_array = p;
		p += s->symbol_count;
		p->segment = (refer_nstring_t) refer_save(seg->segment);
		p->symbol = (refer_nstring_t) refer_save(symbol);
		p->segment_index = seg->segment_index;
		p->symbol_index = s->symbol_count;
		p->symbol_per_segment = seg->symbol_count;
		p->symbol_type = symbol_type;
		if (hashmap_set_name(&s->symbol_pool, symbol->string, (void *) s->symbol_count, NULL))
		{
			seg->symbol_count += 1;
			s->symbol_count += 1;
			return p;
		}
	}
	return NULL;
}

static const abc_adora_symbol_t* abc_adora_symbol_new_symbol_by_cstring(abc_adora_symbol_s *restrict s, const char *restrict segment, const char *restrict symbol, uint32_t symbol_type)
{
	refer_nstring_t sym;
	const abc_adora_symbol_t *restrict result;
	result = NULL;
	if ((sym = refer_dump_nstring(symbol)))
	{
		result = abc_adora_symbol_new_symbol_by_nstring(s, segment, sym, symbol_type);
		refer_free(sym);
	}
	return result;
}

// api

static void abc_adora_symbol_free_func(abc_adora_symbol_s *restrict r)
{
	const abc_adora_symbol_t *restrict p;
	uintptr_t i, n;
	p = r->symbol_array;
	n = r->symbol_count;
	for (i = 0; i < n; ++i)
	{
		if (p[i].segment) refer_free(p[i].segment);
		if (p[i].symbol) refer_free(p[i].symbol);
	}
	if (r->segment_pool) refer_free(r->segment_pool);
	hashmap_uini(&r->symbol_pool);
	exbuffer_uini(&r->symbol_buffer);
}

abc_adora_symbol_s* abc_adora_symbol_alloc(void)
{
	abc_adora_symbol_s *restrict r;
	if ((r = (abc_adora_symbol_s *) refer_alloz(sizeof(abc_adora_symbol_s))))
	{
		refer_set_free(r, (refer_free_f) abc_adora_symbol_free_func);
		if (exbuffer_init(&r->symbol_buffer, 0) &&
			hashmap_init(&r->symbol_pool) &&
			(r->segment_pool = vattr_alloc()))
			return r;
		refer_free(r);
	}
	return NULL;
}

void abc_adora_symbol_clear(abc_adora_symbol_s *restrict s)
{
	const abc_adora_symbol_t *restrict p;
	uintptr_t i, n;
	p = s->symbol_array;
	n = s->symbol_count;
	for (i = 0; i < n; ++i)
	{
		if (p[i].segment) refer_free(p[i].segment);
		if (p[i].symbol) refer_free(p[i].symbol);
	}
	s->symbol_count = 0;
	vattr_clear(s->segment_pool);
	hashmap_clear(&s->symbol_pool);
	exbuffer_clear(&s->symbol_buffer);
}

abc_adora_symbol_s* abc_adora_symbol_fetch_segment(abc_adora_symbol_s *restrict s, const char *restrict segment)
{
	if (segment && abc_adora_symbol_segment_fetch(s->segment_pool, segment))
		return s;
	return NULL;
}

const abc_adora_symbol_t* abc_adora_symbol_create_nstring(abc_adora_symbol_s *restrict s, const char *restrict segment, refer_nstring_t symbol, uint32_t symbol_type)
{
	if (segment && symbol && !hashmap_find_name(&s->symbol_pool, symbol->string))
		return abc_adora_symbol_new_symbol_by_nstring(s, segment, symbol, symbol_type);
	return NULL;
}

const abc_adora_symbol_t* abc_adora_symbol_create_cstring(abc_adora_symbol_s *restrict s, const char *restrict segment, const char *restrict symbol, uint32_t symbol_type)
{
	if (segment && symbol && !hashmap_find_name(&s->symbol_pool, symbol))
		return abc_adora_symbol_new_symbol_by_cstring(s, segment, symbol, symbol_type);
	return NULL;
}

const abc_adora_symbol_s* abc_adora_symbol_find_segment(const abc_adora_symbol_s *restrict s, const char *restrict segment, uint32_t *restrict segment_index, uint32_t *restrict symbol_count)
{
	const abc_adora_symbol_segment_s *restrict seg;
	if (segment && (seg = (abc_adora_symbol_segment_s *) vattr_get_first(s->segment_pool, segment)))
	{
		if (segment_index) *segment_index = seg->segment_index;
		if (symbol_count) *symbol_count = seg->symbol_count;
		return s;
	}
	return NULL;
}

const abc_adora_symbol_t* abc_adora_symbol_find_symbol(abc_adora_symbol_s *restrict s, const char *restrict segment, const char *restrict symbol)
{
	const hashmap_vlist_t *restrict vl;
	uintptr_t pos;
	if (segment && symbol && (vl = hashmap_find_name(&s->symbol_pool, symbol)) &&
		(pos = (uintptr_t) vl->value) < s->symbol_count)
		return s->symbol_array + pos;
	return NULL;
}

uintptr_t abc_adora_symbol_count_segment(abc_adora_symbol_s *restrict s)
{
	return (uintptr_t) s->segment_pool->vslot.number;
}

uintptr_t abc_adora_symbol_count_symbol(abc_adora_symbol_s *restrict s)
{
	return s->symbol_count;
}
