#include "box.include.h"
#include "inner.type.h"

static const mpeg4$define$alloc(moov)
{
	mpeg4_atom_s *restrict r;
	r = mpeg4$define(atom, container, alloc)(inst);
	if (r)
	{
		if (
			mpeg4$inner$type$alloc$layer(r, inst, mvhd) &&
			mpeg4$inner$type$alloc$layer(r, inst, iods) &&
			mpeg4$inner$type$alloc$layer(r, inst, trak) &&
			mpeg4$inner$type$alloc$layer(r, inst, udta)
		) return r;
		refer_free(r);
	}
	return NULL;
}

mpeg4$define$find(moov)
{
	static const mpeg4_box_type_t type = { .c = "moov" };
	return mpeg4_find_atom(inst, mpeg4$define(atom, moov, alloc), type.v, 0);
}
