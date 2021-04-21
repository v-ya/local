#include "box.include.h"
#include "inner.fullbox.h"

mpeg4$define$dump(box, meta)
{
	inner_fullbox_t fullbox;
	if (!mpeg4$define(inner, fullbox, get)(&fullbox, &data, &size))
		goto label_fail;
	mlog_level_dump("version = %u, flags = %06x\n", fullbox.version, fullbox.flags);
	return mpeg4$define(box, container, dump)(inst, mlog, data, size, unidata, level);
	label_fail:
	return NULL;
}
