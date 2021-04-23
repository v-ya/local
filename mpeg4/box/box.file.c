#include "box.include.h"
#include "inner.type.h"

static const mpeg4$define$alloc(file)
{
	mpeg4_atom_t *restrict r;
	r = mpeg4$define(atom, container, alloc)(inst);
	if (r)
	{
		if (
			mpeg4$inner$type$alloc$layer(r, inst, ftyp) &&
			mpeg4$inner$type$alloc$layer(r, inst, mdat) &&
			mpeg4$inner$type$alloc$layer(r, inst, moov)
		) return r;
		refer_free(r);
	}
	return NULL;
}

mpeg4$define$find(file)
{
	static const mpeg4_box_type_t type = { .c = "file" };
	static const mpeg4_box_type_t extra = { .c = "root" };
	return mpeg4_find_atom(inst, mpeg4$define(atom, file, alloc), type.v, extra.v);
}
