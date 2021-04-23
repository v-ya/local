#include "box.include.h"
#include "inner.type.h"

static const mpeg4$define$alloc(udta)
{
	mpeg4_atom_t *restrict r;
	r = mpeg4$define(atom, container, alloc)(inst);
	if (r)
	{
		if (
			mpeg4$inner$type$alloc$layer(r, inst, meta)
		) return r;
		refer_free(r);
	}
	return NULL;
}

mpeg4$define$find(udta)
{
	static const mpeg4_box_type_t type = { .c = "udta" };
	return mpeg4_find_atom(inst, mpeg4$define(atom, udta, alloc), type.v, 0);
}
