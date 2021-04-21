#include "box.include.h"
#include "inner.fullbox.h"

typedef struct sample2chunk_item_t {
	uint32_t first_chunk;
	uint32_t samples_per_chunk;
	uint32_t sample_description_index;
} __attribute__ ((packed)) sample2chunk_item_t;

mpeg4$define$dump(box, stsc)
{
	inner_fullbox_t fullbox;
	const sample2chunk_item_t *item;
	uint32_t entry_count;
	if (!mpeg4$define(inner, fullbox, get)(&fullbox, &data, &size))
		goto label_fail;
	mlog_level_dump("version = %u, flags = %06x\n", fullbox.version, fullbox.flags);
	if (fullbox.version)
		goto label_fail;
	if (size < sizeof(entry_count))
		goto label_fail;
	entry_count = mpeg4_n32(*(const uint32_t *) data);
	data += sizeof(entry_count);
	size -= sizeof(entry_count);
	mlog_level_dump("entry count: %u\n", entry_count);
	item = (const sample2chunk_item_t *) data;
	level += 1;
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
	return inst;
	label_fail:
	return NULL;
}
