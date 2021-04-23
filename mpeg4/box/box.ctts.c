#include "box.include.h"
#include "inner.fullbox.h"
#include "inner.data.h"

typedef struct composition_offset_t {
	uint32_t sample_count;
	union {
		uint32_t v0;
		int32_t v1;
	} sample_offset;
} __attribute__ ((packed)) composition_offset_t;

static mpeg4$define$dump(ctts)
{
	inner_fullbox_t fullbox;
	const composition_offset_t *offset;
	uint64_t all_sample_count;
	uint32_t entry_count, sample_count;
	uint32_t level = unidata->dump_level;
	if (!mpeg4$define(inner, fullbox, get)(&fullbox, &data, &size))
		goto label_fail;
	mpeg4$define(inner, fullbox, dump)(mlog, &fullbox, NULL, level);
	if (fullbox.version > 1)
		goto label_fail;
	if (!mpeg4$define(inner, uint32_t, get)(&entry_count, &data, &size))
		goto label_fail;
	mlog_level_dump("entry count: %u\n", entry_count);
	offset = (const composition_offset_t *) data;
	level += mlog_level_width;
	all_sample_count = 0;
	while (entry_count && size >= sizeof(composition_offset_t))
	{
		all_sample_count += (sample_count = mpeg4_n32(offset->sample_count));
		if (unidata->dump_samples)
		{
			if (!fullbox.version) mlog_level_dump("sample_count = %u, sample_offset = %u\n",
				sample_count,
				mpeg4_n32(offset->sample_offset.v0));
			else mlog_level_dump("sample_count = %u, sample_offset = %d\n",
				sample_count,
				mpeg4_n32(offset->sample_offset.v1));
		}
		++offset;
		size -= sizeof(composition_offset_t);
		--entry_count;
	}
	if (size || entry_count)
		goto label_fail;
	mlog_level_dump("all sample_count = %lu\n", all_sample_count);
	return atom;
	label_fail:
	return NULL;
}

static const mpeg4$define$alloc(ctts)
{
	mpeg4_atom_t *restrict r;
	r = mpeg4_atom_alloc_empty();
	if (r)
	{
		r->interface.dump = mpeg4$define(atom, ctts, dump);
	}
	return r;
}

mpeg4$define$find(ctts)
{
	static const mpeg4_box_type_t type = { .c = "ctts" };
	return mpeg4_find_atom(inst, mpeg4$define(atom, ctts, alloc), type.v, 0);
}
