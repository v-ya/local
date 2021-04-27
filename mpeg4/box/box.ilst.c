#include "box.include.h"
#include "inner.type.h"

static const mpeg4$define$alloc(ilst)
{
	mpeg4_atom_s *restrict r;
	r = mpeg4$define(atom, container, alloc)(inst);
	if (r)
	{
		if (
			mpeg4$inner$type$alloc$layer2(r, inst, ilst_copyright, "©ART") &&
			mpeg4$inner$type$alloc$layer2(r, inst, ilst_copyright, "©nam") &&
			mpeg4$inner$type$alloc$layer2(r, inst, ilst_copyright, "©too")
		) return r;
		refer_free(r);
	}
	return NULL;
}

mpeg4$define$find(ilst)
{
	static const mpeg4_box_type_t type = { .c = "ilst" };
	static const mpeg4_box_type_t extra = { .c = "meta" };
	return mpeg4_find_atom(inst, mpeg4$define(atom, ilst, alloc), type.v, extra.v);
}
