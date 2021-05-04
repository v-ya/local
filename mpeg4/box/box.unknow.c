#include "box.include.h"
#include "inner.bindata.h"

#define unknow_max_dump_data_length  32

static mpeg4$define$dump(unknow)
{
	uint32_t level = unidata->dump_level;
	mlog_level_dump("unknow: %u bytes\n", size);
	if (size > unknow_max_dump_data_length)
		size = unknow_max_dump_data_length;
	mpeg4$define(inner, bindata, dump)(mlog, data, size, 0, level + mlog_level_width);
	return atom;
}

static const mpeg4$define$alloc(unknow)
{
	mpeg4_atom_s *restrict r;
	r = mpeg4_atom_alloc_empty();
	if (r) r->interface.dump = mpeg4$define(atom, unknow, dump);
	return r;
}

mpeg4$define$find(unknow)
{
	static const mpeg4_box_type_t type = { .c = "know" };
	static const mpeg4_box_type_t extra = { .c = "-un-" };
	return mpeg4_find_atom(inst, mpeg4$define(atom, unknow, alloc), type.v, extra.v);
}
