#include "box.include.h"
#include "inner.fullbox.h"
#include "inner.data.h"

static mpeg4$define$dump(stss)
{
	inner_fullbox_t fullbox;
	const uint32_t *number;
	uint32_t entry_count;
	uint32_t level = unidata->dump_level;
	if (!mpeg4$define(inner, fullbox, get)(&fullbox, &data, &size))
		goto label_fail;
	mpeg4$define(inner, fullbox, dump)(mlog, &fullbox, NULL, level);
	if (fullbox.version)
		goto label_fail;
	if (!mpeg4$define(inner, uint32_t, get)(&entry_count, &data, &size))
		goto label_fail;
	mlog_level_dump("entry count: %u\n", entry_count);
	number = (const uint32_t *) data;
	level += mlog_level_width;
	while (entry_count && size >= sizeof(uint32_t))
	{
		if (unidata->dump_samples)
			mlog_level_dump("sample_number = %u\n",
				mpeg4_n32(*number));
		++number;
		size -= sizeof(uint32_t);
		--entry_count;
	}
	if (size || entry_count)
		goto label_fail;
	return atom;
	label_fail:
	return NULL;
}

static const mpeg4$define$alloc(stss)
{
	mpeg4_atom_s *restrict r;
	r = mpeg4_atom_alloc_empty();
	if (r)
	{
		r->interface.dump = mpeg4$define(atom, stss, dump);
	}
	return r;
}

mpeg4$define$find(stss)
{
	static const mpeg4_box_type_t type = { .c = "stss" };
	return mpeg4_find_atom(inst, mpeg4$define(atom, stss, alloc), type.v, 0);
}
