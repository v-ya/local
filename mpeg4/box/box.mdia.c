#include "box.include.h"
#include "inner.type.h"

static const mpeg4$define$alloc(mdia)
{
	mpeg4_atom_t *restrict r;
	r = mpeg4$define(atom, container, alloc)(inst);
	if (r)
	{
		if (
			mpeg4$inner$type$alloc$layer(r, inst, mdhd) &&
			mpeg4$inner$type$alloc$layer(r, inst, hdlr) &&
			mpeg4$inner$type$alloc$layer(r, inst, elng) &&
			mpeg4$inner$type$alloc$layer(r, inst, minf)
		) return r;
		refer_free(r);
	}
	return NULL;
}

mpeg4$define$find(mdia)
{
	static const mpeg4_box_type_t type = { .c = "mdia" };
	return mpeg4_find_atom(inst, mpeg4$define(atom, mdia, alloc), type.v, 0);
}
