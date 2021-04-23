#include "box.include.h"
#include "inner.type.h"

static const mpeg4$define$alloc(stsd)
{
	mpeg4_atom_t *restrict r;
	r = mpeg4$define(atom, container_count, alloc)(inst);
	if (r)
	{
		if (
			1
		) return r;
		refer_free(r);
	}
	return NULL;
}

mpeg4$define$find(stsd)
{
	static const mpeg4_box_type_t type = { .c = "stsd" };
	return mpeg4_find_atom(inst, mpeg4$define(atom, stsd, alloc), type.v, 0);
}
