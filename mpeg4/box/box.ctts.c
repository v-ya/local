#include "box.include.h"
#include "inner.fullbox.h"

typedef struct composition_offset_t {
	uint32_t sample_count;
	union {
		uint32_t v0;
		int32_t v1;
	} sample_offset;
} __attribute__ ((packed)) composition_offset_t;

mpeg4$define$dump(box, ctts)
{
	inner_fullbox_t fullbox;
	const composition_offset_t *offset;
	uint64_t all_sample_count;
	uint32_t entry_count, sample_count;
	if (!mpeg4$define(inner, fullbox, get)(&fullbox, &data, &size))
		goto label_fail;
	if (fullbox.version > 1)
		goto label_fail;
	mlog_level_dump("version = %u, flags = %06x\n", fullbox.version, fullbox.flags);
	if (size < sizeof(entry_count))
		goto label_fail;
	entry_count = mpeg4_n32(*(const uint32_t *) data);
	data += sizeof(entry_count);
	size -= sizeof(entry_count);
	mlog_level_dump("entry count: %u\n", entry_count);
	offset = (const composition_offset_t *) data;
	level += 1;
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
	return inst;
	label_fail:
	return NULL;
}
