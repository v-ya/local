#include "box.include.h"
#include "inner.fullbox.h"
#include "inner.data.h"

typedef struct time2sample_delta_t {
	uint32_t sample_count;
	uint32_t sample_delta;
} __attribute__ ((packed)) time2sample_delta_t;

static mpeg4$define$dump(stts)
{
	inner_fullbox_t fullbox;
	const time2sample_delta_t *delta;
	uint64_t all_sample_count;
	uint32_t entry_count, sample_count;
	uint32_t level = unidata->dump_level;
	if (!mpeg4$define(inner, fullbox, get)(&fullbox, &data, &size))
		goto label_fail;
	mpeg4$define(inner, fullbox, dump)(mlog, &fullbox, NULL, level);
	if (fullbox.version)
		goto label_fail;
	if (!mpeg4$define(inner, uint32_t, get)(&entry_count, &data, &size))
		goto label_fail;
	mlog_level_dump("entry count: %u\n", entry_count);
	delta = (const time2sample_delta_t *) data;
	level += mlog_level_width;
	all_sample_count = 0;
	while (entry_count && size >= sizeof(time2sample_delta_t))
	{
		all_sample_count += (sample_count = mpeg4_n32(delta->sample_count));
		if (unidata->dump_samples)
			mlog_level_dump("sample_count = %u, sample_delta = %u\n",
				sample_count,
				mpeg4_n32(delta->sample_delta));
		++delta;
		size -= sizeof(time2sample_delta_t);
		--entry_count;
	}
	if (size || entry_count)
		goto label_fail;
	mlog_level_dump("all sample_count = %lu\n", all_sample_count);
	return atom;
	label_fail:
	return NULL;
}

static const mpeg4$define$alloc(stts)
{
	mpeg4_atom_t *restrict r;
	r = mpeg4_atom_alloc_empty();
	if (r)
	{
		r->interface.dump = mpeg4$define(atom, stts, dump);
	}
	return r;
}

mpeg4$define$find(stts)
{
	static const mpeg4_box_type_t type = { .c = "stts" };
	return mpeg4_find_atom(inst, mpeg4$define(atom, stts, alloc), type.v, 0);
}
