#include "box.include.h"
#include "inner.fullbox.h"
#include "inner.data.h"

mpeg4$define$dump(container);
static mpeg4$define$dump(container_count)
{
	inner_fullbox_t fullbox;
	uint32_t entry_count;
	uint32_t level = unidata->dump_level;
	if (!mpeg4$define(inner, fullbox, get)(&fullbox, &data, &size))
		goto label_fail;
	mpeg4$define(inner, fullbox, dump)(mlog, &fullbox, NULL, level);
	if (!mpeg4$define(inner, uint32_t, get)(&entry_count, &data, &size))
		goto label_fail;
	mlog_level_dump("entry count: %u\n", entry_count);
	return mpeg4$define(atom, container, dump)(atom, mlog, data, size, unidata);
	label_fail:
	return NULL;
}

mpeg4$define$alloc(container_count)
{
	mpeg4_atom_t *restrict r;
	r = mpeg4$define(atom, container, alloc)(inst);
	if (r)
	{
		r->interface.dump = mpeg4$define(atom, container_count, dump);
	}
	return r;
}
