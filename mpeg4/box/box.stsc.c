#include "box.include.h"
#include "inner.fullbox.h"
#include "inner.data.h"

typedef struct sample2chunk_item_t {
	uint32_t first_chunk;
	uint32_t samples_per_chunk;
	uint32_t sample_description_index;
} __attribute__ ((packed)) sample2chunk_item_t;

static mpeg4$define$dump(stsc)
{
	inner_fullbox_t fullbox;
	const sample2chunk_item_t *item;
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
	item = (const sample2chunk_item_t *) data;
	level += mlog_level_width;
	while (entry_count && size >= sizeof(sample2chunk_item_t))
	{
		if (unidata->dump_samples)
			mlog_level_dump("first_chunk = %u, samples_per_chunk = %u, sample_description_index = %u\n",
				mpeg4_n32(item->first_chunk),
				mpeg4_n32(item->samples_per_chunk),
				mpeg4_n32(item->sample_description_index));
		++item;
		size -= sizeof(sample2chunk_item_t);
		--entry_count;
	}
	if (size || entry_count)
		goto label_fail;
	return atom;
	label_fail:
	return NULL;
}

static const mpeg4$define$alloc(stsc)
{
	mpeg4_atom_t *restrict r;
	r = mpeg4_atom_alloc_empty();
	if (r)
	{
		r->interface.dump = mpeg4$define(atom, stsc, dump);
	}
	return r;
}

mpeg4$define$find(stsc)
{
	static const mpeg4_box_type_t type = { .c = "stsc" };
	return mpeg4_find_atom(inst, mpeg4$define(atom, stsc, alloc), type.v, 0);
}
