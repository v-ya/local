#include "box.include.h"
#include "inner.fullbox.h"

mpeg4$define$dump(box, elng)
{
	inner_fullbox_t fullbox;
	if (!mpeg4$define(inner, fullbox, get)(&fullbox, &data, &size))
		goto label_fail;
	mlog_level_dump("version = %u, flags = %06x\n", fullbox.version, fullbox.flags);
	if (fullbox.version != 0)
		goto label_fail;
	if (size && !data[size - 1])
	{
		mlog_level_dump("extended language: %s\n", (const char *) data);
		return inst;
	}
	label_fail:
	return NULL;
}
