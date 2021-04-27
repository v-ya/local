#include "box.include.h"
#include "inner.type.h"

static const mpeg4$define$alloc(dinf)
{
	mpeg4_atom_s *restrict r;
	r = mpeg4$define(atom, container, alloc)(inst);
	if (r)
	{
		if (
			mpeg4$inner$type$alloc$layer(r, inst, dref)
		) return r;
		refer_free(r);
	}
	return NULL;
}

mpeg4$define$find(dinf)
{
	static const mpeg4_box_type_t type = { .c = "dinf" };
	return mpeg4_find_atom(inst, mpeg4$define(atom, dinf, alloc), type.v, 0);
}
