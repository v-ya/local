#include "box.include.h"
#include "inner.type.h"

static const mpeg4$define$alloc(meta)
{
	mpeg4_atom_s *restrict r;
	r = mpeg4$define(atom, container_full, alloc)(inst);
	if (r)
	{
		if (
			mpeg4$inner$type$alloc$layer(r, inst, hdlr) &&
			mpeg4$inner$type$alloc$layer(r, inst, ilst)
		) return r;
		refer_free(r);
	}
	return NULL;
}

mpeg4$define$find(meta)
{
	static const mpeg4_box_type_t type = { .c = "meta" };
	return mpeg4_find_atom(inst, mpeg4$define(atom, meta, alloc), type.v, 0);
}
