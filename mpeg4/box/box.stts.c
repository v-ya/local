#include "box.include.h"
#include "inner.fullbox.h"

typedef struct time2sample_delta_t {
	uint32_t sample_count;
	uint32_t sample_delta;
} __attribute__ ((packed)) time2sample_delta_t;

mpeg4$define$dump(box, stts)
{
	inner_fullbox_t fullbox;
	const time2sample_delta_t *delta;
	uint64_t all_sample_count;
	uint32_t entry_count, sample_count;
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
	delta = (const time2sample_delta_t *) data;
	level += 1;
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
	return inst;
	label_fail:
	return NULL;
}
