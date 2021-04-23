#include "box.include.h"
#include "inner.bindata.h"

#define free_max_dump_data_length  128

static mpeg4$define$dump(free_skip)
{
	uint32_t level = unidata->dump_level;
	mlog_level_dump("free space: %lu bytes\n", size);
	if (size > free_max_dump_data_length)
		size = free_max_dump_data_length;
	mpeg4$define(inner, bindata, dump)(mlog, data, size, 0, level + mlog_level_width);
	return atom;
}

static const mpeg4$define$alloc(free_skip)
{
	mpeg4_atom_t *restrict r;
	r = mpeg4_atom_alloc_empty();
	if (r)
	{
		r->interface.dump = mpeg4$define(atom, free_skip, dump);
	}
	return r;
}

mpeg4$define$find(free_skip)
{
	static const mpeg4_box_type_t type = { .c = "free" };
	static const mpeg4_box_type_t extra = { .c = "skip" };
	return mpeg4_find_atom(inst, mpeg4$define(atom, free_skip, alloc), type.v, extra.v);
}
