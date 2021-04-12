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
	delta = (const time2sample_delta_t *) data;
	level += 1;
	while (entry_count && size >= sizeof(time2sample_delta_t))
	{
		mlog_level_dump("sample_count = %u, sample_delta = %u\n",
			mpeg4_n32(delta->sample_count),
			mpeg4_n32(delta->sample_delta));
		++delta;
		size -= sizeof(time2sample_delta_t);
		--entry_count;
	}
	if (!size) return inst;
	label_fail:
	return NULL;
}
