#include "box.include.h"
#include "inner.fullbox.h"

typedef struct sample_size_t {
	uint32_t sample_size;
	uint32_t sample_count;
} __attribute__ ((packed)) sample_size_t;

mpeg4$define$dump(box, stsz)
{
	inner_fullbox_t fullbox;
	const uint32_t *entry_size;
	uint64_t all_sample_size;
	sample_size_t sample;
	uint32_t sample_size, i;
	if (!mpeg4$define(inner, fullbox, get)(&fullbox, &data, &size))
		goto label_fail;
	mlog_level_dump("version = %u, flags = %06x\n", fullbox.version, fullbox.flags);
	if (fullbox.version)
		goto label_fail;
	if (size < sizeof(sample))
		goto label_fail;
	sample.sample_size = mpeg4_n32(*(const uint32_t *) data);
	data += sizeof(sample.sample_size);
	sample.sample_count = mpeg4_n32(*(const uint32_t *) data);
	data += sizeof(sample.sample_count);
	size -= sizeof(sample);
	mlog_level_dump("sample_size:  %u\n", sample.sample_size);
	mlog_level_dump("sample_count: %u\n", sample.sample_count);
	entry_size = (const uint32_t *) data;
	level += 1;
	all_sample_size = 0;
	if (!sample.sample_size)
	{
		for (i = 0; i < sample.sample_count && size >= sizeof(uint32_t); ++i)
		{
			all_sample_size += (sample_size = mpeg4_n32(*entry_size));
			if (unidata->dump_samples)
				mlog_level_dump("[%6u] = %u\n", i, sample_size);
			++entry_size;
			size -= sizeof(uint32_t);
		}
		if (i < sample.sample_count)
			goto label_fail;
	}
	else all_sample_size = sample.sample_size * sample.sample_count;
	if (size) goto label_fail;
	mlog_level_dump("all sample_size: %lu\n", all_sample_size);
	return inst;
	label_fail:
	return NULL;
}
