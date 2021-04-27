#include "box.include.h"

static mpeg4$define$dump(mdat)
{
	#define mlevel(_n)  (1.0 / (1u << _n))
	uint32_t level = unidata->dump_level;
	mlog_level_dump("media data size: %lu B = %.1f KiB = %.2f MiB = %.2f GiB\n", size, size * mlevel(10), size * mlevel(20), size * mlevel(30));
	return atom;
}

static const mpeg4$define$alloc(mdat)
{
	mpeg4_atom_s *restrict r;
	r = mpeg4_atom_alloc_empty();
	if (r)
	{
		r->interface.dump = mpeg4$define(atom, mdat, dump);
	}
	return r;
}

mpeg4$define$find(mdat)
{
	static const mpeg4_box_type_t type = { .c = "mdat" };
	return mpeg4_find_atom(inst, mpeg4$define(atom, mdat, alloc), type.v, 0);
}
