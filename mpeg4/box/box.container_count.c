#include "box.include.h"
#include "inner.fullbox.h"
#include "inner.data.h"

typedef struct mpeg4_stuff__container_count_s {
	mpeg4_stuff_t stuff;
	inner_fullbox_t fullbox;
	uint32_t entry_count;
} mpeg4_stuff__container_count_s;

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

static mpeg4$define$create(container_count)
{
	return mpeg4_stuff_alloc(atom, inst, type, sizeof(mpeg4_stuff__container_count_s), NULL, NULL);
}

mpeg4$define$alloc(container_count)
{
	mpeg4_atom_s *restrict r;
	r = mpeg4$define(atom, container, alloc)(inst);
	if (r)
	{
		r->interface.dump = mpeg4$define(atom, container_count, dump);
		r->interface.create = mpeg4$define(atom, container_count, create);
		r->interface.parse = NULL;
		r->interface.calc = NULL;
		r->interface.build = NULL;
	}
	return r;
}
