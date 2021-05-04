#include "box.include.h"
#include "inner.bindata.h"
#include "inner.data.h"
#include <stdlib.h>
#include <memory.h>

#define mpeg4_max_dump_data_length  256

typedef struct mpeg4_stuff__data_t {
	void *data;
	uint64_t size;
} mpeg4_stuff__data_t;

typedef struct mpeg4_stuff__data_s {
	mpeg4_stuff_t stuff;
	mpeg4_stuff__data_t pri;
} mpeg4_stuff__data_s;

static mpeg4$define$dump(data)
{
	uint32_t level = unidata->dump_level;
	mlog_level_dump("data space: %lu bytes\n", size);
	if (size > mpeg4_max_dump_data_length)
		size = mpeg4_max_dump_data_length;
	mpeg4$define(inner, bindata, dump)(mlog, data, size, 0, level + mlog_level_width);
	return atom;
}

static mpeg4$define$stuff$free(data, mpeg4_stuff__data_s)
{
	if (r->pri.data)
		free(r->pri.data);
	mpeg4_stuff_free_default_func(&r->stuff);
}

static mpeg4$define$create(data)
{
	return mpeg4_stuff_alloc(atom, inst, type,
		sizeof(mpeg4_stuff__data_s),
		NULL,
		(refer_free_f) mpeg4$define(stuff, data, free));
}

static mpeg4$define$parse(data)
{
	return (mpeg4_stuff_t *) mpeg4$stuff$method$call(stuff, set$data, data, size);
}

static mpeg4$define$calc(data)
{
	return mpeg4_stuff_calc_okay(stuff, ((mpeg4_stuff__data_s *) stuff)->pri.size);
}

static mpeg4$define$build(data)
{
	mpeg4_stuff__data_t *restrict r = &((mpeg4_stuff__data_s *) stuff)->pri;
	if (r->size)
	{
		if (!r->data) goto label_fail;
		mpeg4$define(inner, data, set)(data, r->data, r->size);
	}
	return stuff;
	label_fail:
	return NULL;
}

static const mpeg4_stuff_t* mpeg4$define(stuff, data, set$data)(mpeg4_stuff__data_s *restrict r, const void *data, uint64_t size)
{
	void *ndata = NULL;
	if (size)
	{
		ndata = malloc((size_t) size);
		if (!ndata)
			goto label_fail;
		memcpy(ndata, data, (size_t) size);
	}
	if (r->pri.data)
		free(r->pri.data);
	r->pri.data = ndata;
	r->pri.size = size;
	return &r->stuff;
	label_fail:
	return NULL;
}

static const mpeg4$define$alloc(data)
{
	mpeg4_atom_s *restrict r;
	r = mpeg4_atom_alloc_empty();
	if (r)
	{
		r->interface.dump = mpeg4$define(atom, data, dump);
		r->interface.create = mpeg4$define(atom, data, create);
		r->interface.parse = mpeg4$define(atom, data, parse);
		r->interface.calc = mpeg4$define(atom, data, calc);
		r->interface.build = mpeg4$define(atom, data, build);
		if (mpeg4$stuff$method$set(r, data, set$data))
			return r;
		refer_free(r);
	}
	return NULL;
}

mpeg4$define$find(data)
{
	static const mpeg4_box_type_t type = { .c = "data" };
	static const mpeg4_box_type_t extra = { .c = "data" };
	return mpeg4_find_atom(inst, mpeg4$define(atom, data, alloc), type.v, extra.v);
}
