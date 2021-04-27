#include "box.include.h"
#include "inner.fullbox.h"

static mpeg4$define$dump(nmhd)
{
	inner_fullbox_t fullbox;
	uint32_t level = unidata->dump_level;
	if (!mpeg4$define(inner, fullbox, get)(&fullbox, &data, &size))
		goto label_fail;
	mpeg4$define(inner, fullbox, dump)(mlog, &fullbox, NULL, level);
	if (fullbox.version != 0)
		goto label_fail;
	if (!size)
		return atom;
	label_fail:
	return NULL;
}

static const mpeg4$define$alloc(nmhd)
{
	mpeg4_atom_s *restrict r;
	r = mpeg4_atom_alloc_empty();
	if (r)
	{
		r->interface.dump = mpeg4$define(atom, nmhd, dump);
	}
	return r;
}

mpeg4$define$find(nmhd)
{
	static const mpeg4_box_type_t type = { .c = "nmhd" };
	return mpeg4_find_atom(inst, mpeg4$define(atom, nmhd, alloc), type.v, 0);
}
