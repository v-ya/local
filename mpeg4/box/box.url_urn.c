#include "box.include.h"
#include "inner.fullbox.h"
#include <string.h>

static const char *urln_flag_name[1] = {
	[0] = "media_data_in_same_file"
};

mpeg4$define$dump(box, url_)
{
	inner_fullbox_t fullbox;
	char buffer[32];
	if (!mpeg4$define(inner, fullbox, get)(&fullbox, &data, &size))
		goto label_fail;
	mlog_level_dump("version = %u, flags = %06x (%s)\n",
		fullbox.version,
		fullbox.flags,
		mpeg4$define(inner, flags, string)(buffer, fullbox.flags, urln_flag_name, 1));
	if (fullbox.version)
		goto label_fail;
	if (size)
	{
		if (data[size - 1])
			goto label_fail;
		mlog_level_dump("location: %s\n", (const char *) data);
	}
	return inst;
	label_fail:
	return NULL;
}

mpeg4$define$dump(box, urn_)
{
	inner_fullbox_t fullbox;
	size_t n;
	char buffer[32];
	if (!mpeg4$define(inner, fullbox, get)(&fullbox, &data, &size))
		goto label_fail;
	mlog_level_dump("version = %u, flags = %06x (%s)\n",
		fullbox.version,
		fullbox.flags,
		mpeg4$define(inner, flags, string)(buffer, fullbox.flags, urln_flag_name, 1));
	if (fullbox.version)
		goto label_fail;
	if (size)
	{
		if (data[size - 1])
			goto label_fail;
		mlog_level_dump("name:     %s\n", (const char *) data);
		n = strlen((const char *) data) + 1;
		if (n < size)
			mlog_level_dump("location: %s\n", (const char *) data + n);
	}
	return inst;
	label_fail:
	return NULL;
}
