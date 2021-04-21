#include "box.include.h"
#include "inner.fullbox.h"

mpeg4$define$dump(box, container_count)
{
	inner_fullbox_t fullbox;
	uint32_t entry_count;
	if (!mpeg4$define(inner, fullbox, get)(&fullbox, &data, &size))
		goto label_fail;
	mlog_level_dump("version = %u, flags = %06x\n", fullbox.version, fullbox.flags);
	if (size < sizeof(entry_count))
		goto label_fail;
	entry_count = mpeg4_n32(*(const uint32_t *) data);
	data += sizeof(entry_count);
	size -= sizeof(entry_count);
	mlog_level_dump("entry count: %u\n", entry_count);
	return mpeg4$define(box, container, dump)(inst, mlog, data, size, unidata, level);
	label_fail:
	return NULL;
}
