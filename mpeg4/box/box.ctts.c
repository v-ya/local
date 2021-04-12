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
	uint32_t entry_count;
	if (!mpeg4$define(inner, fullbox, get)(&fullbox, &data, &size))
		goto label_fail;
	mlog_level_dump("version = %u, flags = %06x\n", fullbox.version, fullbox.flags);
	if (size < sizeof(entry_count))
		goto label_fail;
	entry_count = mpeg4_n32(*(const uint32_t *) data);
	data += sizeof(entry_count);
	size -= sizeof(entry_count);
	mlog_level_dump("entry count = %u\n", entry_count);
	offset = (const composition_offset_t *) data;
	level += 1;
	if (fullbox.version == 0)
	{
		while (entry_count && size >= sizeof(composition_offset_t))
		{
			mlog_level_dump("sample_count = %u, sample_offset = %u\n",
				mpeg4_n32(offset->sample_count),
				mpeg4_n32(offset->sample_offset.v0));
			++offset;
			size -= sizeof(composition_offset_t);
			--entry_count;
		}
	}
	else if (fullbox.version == 1)
	{
		while (entry_count && size >= sizeof(composition_offset_t))
		{
			mlog_level_dump("sample_count = %u, sample_offset = %d\n",
				mpeg4_n32(offset->sample_count),
				mpeg4_n32(offset->sample_offset.v1));
			++offset;
			size -= sizeof(composition_offset_t);
			--entry_count;
		}
	}
	else goto label_fail;
	if (!size) return inst;
	label_fail:
	return NULL;
}
