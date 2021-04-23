#include "box.include.h"
#include "inner.type.h"

static const mpeg4$define$alloc(minf)
{
	mpeg4_atom_t *restrict r;
	r = mpeg4$define(atom, container, alloc)(inst);
	if (r)
	{
		if (
			mpeg4$inner$type$alloc$layer(r, inst, nmhd) &&
			mpeg4$inner$type$alloc$layer(r, inst, dinf) &&
			mpeg4$inner$type$alloc$layer(r, inst, stbl)
		) return r;
		refer_free(r);
	}
	return NULL;
}

mpeg4$define$find(minf)
{
	static const mpeg4_box_type_t type = { .c = "minf" };
	return mpeg4_find_atom(inst, mpeg4$define(atom, minf, alloc), type.v, 0);
}
