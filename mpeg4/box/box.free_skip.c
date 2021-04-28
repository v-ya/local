#include "box.include.h"
#include "inner.bindata.h"
#include "inner.data.h"
#include <stdlib.h>
#include <memory.h>

#define free_max_dump_data_length  128

typedef struct mpeg4_stuff__free_skip_t {
	void *data;
	uintptr_t size;
} mpeg4_stuff__free_skip_t;

typedef struct mpeg4_stuff__free_skip_s {
	mpeg4_stuff_t stuff;
	mpeg4_stuff__free_skip_t pri;
} mpeg4_stuff__free_skip_s;

static mpeg4$define$dump(free_skip)
{
	uint32_t level = unidata->dump_level;
	mlog_level_dump("free space: %lu bytes\n", size);
	if (size > free_max_dump_data_length)
		size = free_max_dump_data_length;
	mpeg4$define(inner, bindata, dump)(mlog, data, size, 0, level + mlog_level_width);
	return atom;
}

static mpeg4$define$stuff$free(free_skip, mpeg4_stuff__free_skip_s)
{
	if (r->pri.data)
		free(r->pri.data);
	mpeg4_stuff_free_default_func(&r->stuff);
}

static mpeg4$define$create(free_skip)
{
	return mpeg4_stuff_alloc(atom, inst, type,
		sizeof(mpeg4_stuff__free_skip_s),
		NULL,
		(refer_free_f) mpeg4$define(stuff, free_skip, free));
}

static mpeg4$define$parse(free_skip)
{
	return (mpeg4_stuff_t *) mpeg4$stuff$method$call(stuff, set$data, data, (uintptr_t) size);
}

static mpeg4$define$calc(free_skip)
{
	mpeg4_stuff__free_skip_t *restrict r = &((mpeg4_stuff__free_skip_s *) stuff)->pri;
	mpeg4_stuff_calc_okay(stuff, (uint64_t) r->size);
	return stuff;
}

static mpeg4$define$build(free_skip)
{
	mpeg4_stuff__free_skip_t *restrict r = &((mpeg4_stuff__free_skip_s *) stuff)->pri;
	if (r->size)
	{
		if (!r->data) goto label_fail;
		mpeg4$define(inner, data, set)(data, r->data, r->size);
	}
	return stuff;
	label_fail:
	return NULL;
}

static const mpeg4_stuff_t* mpeg4$define(stuff, free_skip, set$data)(mpeg4_stuff__free_skip_s *restrict r, const void *data, uintptr_t size)
{
	void *ndata = NULL;
	if (size)
	{
		ndata = malloc(size);
		if (!ndata)
			goto label_fail;
		memcpy(ndata, data, size);
	}
	if (r->pri.data)
		free(r->pri.data);
	r->pri.data = ndata;
	r->pri.size = size;
	return &r->stuff;
	label_fail:
	return NULL;
}

static const mpeg4$define$alloc(free_skip)
{
	mpeg4_atom_s *restrict r;
	r = mpeg4_atom_alloc_empty();
	if (r)
	{
		r->interface.dump = mpeg4$define(atom, free_skip, dump);
		r->interface.create = mpeg4$define(atom, free_skip, create);
		r->interface.parse = mpeg4$define(atom, free_skip, parse);
		r->interface.calc = mpeg4$define(atom, free_skip, calc);
		r->interface.build = mpeg4$define(atom, free_skip, build);
		if (mpeg4$stuff$method$set(r, free_skip, set$data))
			return r;
		refer_free(r);
	}
	return NULL;
}

mpeg4$define$find(free_skip)
{
	static const mpeg4_box_type_t type = { .c = "free" };
	static const mpeg4_box_type_t extra = { .c = "skip" };
	return mpeg4_find_atom(inst, mpeg4$define(atom, free_skip, alloc), type.v, extra.v);
}
