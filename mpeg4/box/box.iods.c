#include "box.include.h"
#include "inner.fullbox.h"
#include "inner.data.h"
#include "inner.bindata.h"

static mpeg4$define$dump(iods)
{
	inner_fullbox_t fullbox;
	uint32_t level = unidata->dump_level;
	if (!mpeg4$define(inner, fullbox, get)(&fullbox, &data, &size))
		goto label_fail;
	mpeg4$define(inner, fullbox, dump)(mlog, &fullbox, NULL, level);
	if (fullbox.version)
		goto label_fail;
	mlog_level_dump("object descriptor:\n");
	mpeg4$define(inner, bindata, dump)(mlog, data, size, 0, level + mlog_level_width);
	return atom;
	label_fail:
	return NULL;
}

static const mpeg4$define$alloc(iods)
{
	mpeg4_atom_s *restrict r;
	r = mpeg4_atom_alloc_empty();
	if (r)
	{
		r->interface.dump = mpeg4$define(atom, iods, dump);
	}
	return r;
}

mpeg4$define$find(iods)
{
	static const mpeg4_box_type_t type = { .c = "iods" };
	return mpeg4_find_atom(inst, mpeg4$define(atom, iods, alloc), type.v, 0);
}
