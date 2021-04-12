#include "box.include.h"
#include "inner.fullbox.h"

mpeg4$define$dump(box, nmhd)
{
	inner_fullbox_t fullbox;
	if (!mpeg4$define(inner, fullbox, get)(&fullbox, &data, &size))
		goto label_fail;
	mlog_level_dump("version = %u, flags = %06x\n", fullbox.version, fullbox.flags);
	if (fullbox.version != 0)
		goto label_fail;
	if (!size)
		return inst;
	label_fail:
	return NULL;
}
