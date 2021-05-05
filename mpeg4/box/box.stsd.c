#include "box.include.h"
#include "inner.type.h"

static const mpeg4$define$alloc(stsd)
{
	mpeg4_atom_s *restrict r;
	r = mpeg4$define(atom, container_count, alloc)(inst);
	if (r)
	{
		if (
			mpeg4$inner$type$alloc$layer2(r, inst, stsd_visual, "avc1") &&
			mpeg4$inner$type$alloc$layer2(r, inst, stsd_visual, "av01") &&
			mpeg4$inner$type$alloc$layer2(r, inst, stsd_audio, "mp4a") &&
			mpeg4$inner$type$alloc$layer2(r, inst, stsd_audio, "fLaC")
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
