#include "box.include.h"
#include "inner.fullbox.h"

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

mpeg4$define$alloc(container_full)
{
	mpeg4_atom_t *restrict r;
	r = mpeg4$define(atom, container, alloc)(inst);
	if (r)
	{
		r->interface.dump = mpeg4$define(atom, container_full, dump);
	}
	return r;
}
