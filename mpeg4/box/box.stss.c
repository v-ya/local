#include "box.include.h"
#include "inner.fullbox.h"

mpeg4$define$dump(box, stss)
{
	inner_fullbox_t fullbox;
	const uint32_t *number;
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
	number = (const uint32_t *) data;
	level += 1;
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
	return inst;
	label_fail:
	return NULL;
}
