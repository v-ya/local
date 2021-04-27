#include "box.include.h"
#include "inner.type.h"

static const mpeg4$define$alloc(stbl)
{
	mpeg4_atom_s *restrict r;
	r = mpeg4$define(atom, container, alloc)(inst);
	if (r)
	{
		if (
			mpeg4$inner$type$alloc$layer(r, inst, stsd) &&
			mpeg4$inner$type$alloc$layer(r, inst, stts) &&
			mpeg4$inner$type$alloc$layer(r, inst, ctts) &&
			mpeg4$inner$type$alloc$layer(r, inst, stss) &&
			mpeg4$inner$type$alloc$layer(r, inst, stsz) &&
			mpeg4$inner$type$alloc$layer(r, inst, stsc) &&
			mpeg4$inner$type$alloc$layer(r, inst, stco) &&
			mpeg4$inner$type$alloc$layer(r, inst, co64)
		) return r;
		refer_free(r);
	}
	return NULL;
}

mpeg4$define$find(stbl)
{
	static const mpeg4_box_type_t type = { .c = "stbl" };
	return mpeg4_find_atom(inst, mpeg4$define(atom, stbl, alloc), type.v, 0);
}
