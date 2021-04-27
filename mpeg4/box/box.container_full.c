#include "box.include.h"
#include "inner.fullbox.h"

typedef struct mpeg4_stuff__container_full_s {
	mpeg4_stuff_t stuff;
	inner_fullbox_t fullbox;
} mpeg4_stuff__container_full_s;

mpeg4$define$dump(container);
static mpeg4$define$dump(container_full)
{
	inner_fullbox_t fullbox;
	uint32_t level = unidata->dump_level;
	if (!mpeg4$define(inner, fullbox, get)(&fullbox, &data, &size))
		goto label_fail;
	mpeg4$define(inner, fullbox, dump)(mlog, &fullbox, NULL, level);
	return mpeg4$define(atom, container, dump)(atom, mlog, data, size, unidata);
	label_fail:
	return NULL;
}

static mpeg4$define$create(container_full)
{
	return mpeg4_stuff_alloc(atom, inst, type, sizeof(mpeg4_stuff__container_full_s), NULL, NULL);
}

mpeg4$define$alloc(container_full)
{
	mpeg4_atom_s *restrict r;
	r = mpeg4$define(atom, container, alloc)(inst);
	if (r)
	{
		r->interface.dump = mpeg4$define(atom, container_full, dump);
		r->interface.create = mpeg4$define(atom, container_full, create);
		r->interface.parse = NULL;
		r->interface.calc = NULL;
		r->interface.build = NULL;
	}
	return r;
}
