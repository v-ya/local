#include "adora.h"
#include <exbuffer.h>
#include <rbtree.h>

typedef struct abc_adora_import_t abc_adora_import_t;

struct abc_adora_import_t {
	uint64_t vaild_mask;
	abc_adora_import_offset_t import[];
};

struct abc_adora_import_s {
	exbuffer_t segment_buffer;
	exbuffer_t import_buffer;
	rbtree_t *segment2index;
	rbtree_t *import2index;
	uintptr_t segment_count;
	uintptr_t import_count;
	uint32_t import_tnum;
	uint32_t import_size;
};

static void abc_adora_import_free_func(abc_adora_import_s *restrict r)
{
	rbtree_clear(&r->import2index);
	rbtree_clear(&r->segment2index);
	exbuffer_uini(&r->import_buffer);
	exbuffer_uini(&r->segment_buffer);
}

abc_adora_import_s* abc_adora_import_alloc(uint32_t import_type_number)
{
	abc_adora_import_s *restrict r;
	if (import_type_number && import_type_number < 64 &&
		(r = (abc_adora_import_s *) refer_alloz(sizeof(abc_adora_import_s))))
	{
		refer_set_free(r, (refer_free_f) abc_adora_import_free_func);
		if (exbuffer_init(&r->segment_buffer, 0) && exbuffer_init(&r->import_buffer, 0))
		{
			r->import_tnum = import_type_number;
			r->import_size = sizeof(abc_adora_import_t) + sizeof(abc_adora_import_offset_t) * import_type_number;
			return r;
		}
		refer_free(r);
	}
	return NULL;
}

void abc_adora_import_clear(abc_adora_import_s *restrict i)
{
	i->segment_count = 0;
	i->import_count = 0;
	rbtree_clear(&i->import2index);
	rbtree_clear(&i->segment2index);
	exbuffer_clear(&i->import_buffer);
	exbuffer_clear(&i->segment_buffer);
}

abc_adora_import_s* abc_adora_import_create_segment(abc_adora_import_s *restrict i, uint32_t segment_index, uint64_t segment_offset)
{
	uint64_t *restrict p;
	uintptr_t n;
	if (!rbtree_find(&i->segment2index, NULL, (uint64_t) segment_index) &&
		(p = (uint64_t *) exbuffer_need(&i->segment_buffer, ((n = i->segment_count) + 1) * sizeof(*p))))
	{
		p += n;
		*p = segment_offset;
		if (rbtree_insert(&i->segment2index, NULL, (uint64_t) segment_index, (void *) n, NULL))
		{
			i->segment_count = n + 1;
			return i;
		}
	}
	return NULL;
}

abc_adora_import_s* abc_adora_import_create_import(abc_adora_import_s *restrict i, uint32_t import_index, uintptr_t n, const abc_adora_import_setting_t setting[])
{
	abc_adora_import_t *restrict p;
	const uint64_t *restrict so;
	const rbtree_t *restrict vl;
	uintptr_t c, index;
	uint32_t t;
	if (!rbtree_find(&i->import2index, NULL, (uint64_t) import_index) &&
		exbuffer_need(&i->import_buffer, ((c = i->import_count) + 1) * i->import_size))
	{
		p = (abc_adora_import_t *) (i->import_buffer.data + i->import_size * c);
		so = (const uint64_t *) i->segment_buffer.data;
		p->vaild_mask = 0;
		for (index = 0; index < n; ++index)
		{
			if ((t = setting[index].import_type) >= i->import_tnum)
				goto label_fail;
			if (!(vl = rbtree_find(&i->segment2index, NULL, (uint64_t) setting[index].segment_index)))
				goto label_fail;
			p->import[t].segment_offset = so[(uintptr_t) vl->value];
			p->import[t].symbol_offset = setting[index].symbol_offset;
			p->vaild_mask |= (uint64_t) 1 << t;
		}
		if (rbtree_insert(&i->import2index, NULL, (uint64_t) import_index, (void *) c, NULL))
		{
			i->import_count = c + 1;
			return i;
		}
	}
	label_fail:
	return NULL;
}

const abc_adora_import_s* abc_adora_import_get_offset(const abc_adora_import_s *restrict i, uint32_t import_index, uintptr_t n, const uint32_t import_type[], abc_adora_import_offset_t offset[])
{
	abc_adora_import_t *restrict p;
	const rbtree_t *restrict vl;
	uintptr_t index;
	uint64_t vaild_mask;
	uint32_t t;
	if ((vl = rbtree_find(&i->import2index, NULL, (uint64_t) import_index)))
	{
		p = (abc_adora_import_t *) (i->import_buffer.data + i->import_size * (uintptr_t) vl->value);
		vaild_mask = p->vaild_mask;
		for (index = 0; index < n; ++index)
		{
			if ((t = import_type[index]) >= i->import_tnum)
				goto label_fail;
			if (!(vaild_mask & ((uint64_t) 1 << t)))
				goto label_fail;
			offset[index].segment_offset = p->import[t].segment_offset;
			offset[index].symbol_offset = p->import[t].symbol_offset;
		}
		return i;
	}
	label_fail:
	return NULL;
}
