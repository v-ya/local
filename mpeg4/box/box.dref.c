#include "box.include.h"
#include "inner.type.h"

static const mpeg4$define$alloc(dref)
{
	mpeg4_atom_s *restrict r;
	r = mpeg4$define(atom, container_count, alloc)(inst);
	if (r)
	{
		if (
			mpeg4$inner$type$alloc$layer2(r, inst, url_, "url ") &&
			mpeg4$inner$type$alloc$layer2(r, inst, urn_, "urn ")
		) return r;
		refer_free(r);
	}
	return NULL;
}

mpeg4$define$find(dref)
{
	static const mpeg4_box_type_t type = { .c = "dref" };
	return mpeg4_find_atom(inst, mpeg4$define(atom, dref, alloc), type.v, 0);
}
