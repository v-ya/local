#include "box.include.h"
#include "inner.type.h"

static const mpeg4$define$alloc(edts)
{
	mpeg4_atom_s *restrict r;
	r = mpeg4$define(atom, container, alloc)(inst);
	if (r)
	{
		if (
			mpeg4$inner$type$alloc$layer(r, inst, elst)
		) return r;
		refer_free(r);
	}
	return NULL;
}

mpeg4$define$find(edts)
{
	static const mpeg4_box_type_t type = { .c = "edts" };
	return mpeg4_find_atom(inst, mpeg4$define(atom, edts, alloc), type.v, 0);
}
