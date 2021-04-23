#include "box.include.h"
#include "inner.fullbox.h"

static mpeg4$define$dump(elng)
{
	inner_fullbox_t fullbox;
	uint32_t level = unidata->dump_level;
	if (!mpeg4$define(inner, fullbox, get)(&fullbox, &data, &size))
		goto label_fail;
	mpeg4$define(inner, fullbox, dump)(mlog, &fullbox, NULL, level);
	if (fullbox.version != 0)
		goto label_fail;
	if (size && !data[size - 1])
	{
		mlog_level_dump("extended language: %s\n", (const char *) data);
		return atom;
	}
	label_fail:
	return NULL;
}

static const mpeg4$define$alloc(elng)
{
	mpeg4_atom_t *restrict r;
	r = mpeg4_atom_alloc_empty();
	if (r)
	{
		r->interface.dump = mpeg4$define(atom, elng, dump);
	}
	return r;
}

mpeg4$define$find(elng)
{
	static const mpeg4_box_type_t type = { .c = "elng" };
	return mpeg4_find_atom(inst, mpeg4$define(atom, elng, alloc), type.v, 0);
}
