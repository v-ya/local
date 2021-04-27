#include "box.include.h"
#include "inner.type.h"

static const mpeg4$define$alloc(trak)
{
	mpeg4_atom_s *restrict r;
	r = mpeg4$define(atom, container, alloc)(inst);
	if (r)
	{
		if (
			mpeg4$inner$type$alloc$layer(r, inst, tkhd) &&
			mpeg4$inner$type$alloc$layer(r, inst, edts) &&
			mpeg4$inner$type$alloc$layer(r, inst, mdia)
		) return r;
		refer_free(r);
	}
	return NULL;
}

mpeg4$define$find(trak)
{
	static const mpeg4_box_type_t type = { .c = "trak" };
	return mpeg4_find_atom(inst, mpeg4$define(atom, trak, alloc), type.v, 0);
}
