#include "box.include.h"
#include "inner.fullbox.h"
#include <string.h>

static const char *urln_flag_name[1] = {
	[0] = "media_data_in_same_file"
};

static mpeg4$define$dump(url_)
{
	inner_fullbox_t fullbox;
	char buffer[32];
	uint32_t level = unidata->dump_level;
	if (!mpeg4$define(inner, fullbox, get)(&fullbox, &data, &size))
		goto label_fail;
	mpeg4$define(inner, fullbox, dump)(
		mlog,
		&fullbox,
		mpeg4$define(inner, flags, string)(buffer, fullbox.flags, urln_flag_name, 1),
		level);
	if (fullbox.version)
		goto label_fail;
	if (!(fullbox.flags & 1))
	{
		if (!size || data[size - 1])
			goto label_fail;
		mlog_level_dump("name: %s\n", (const char *) data);
		size = 0;
	}
	if (!size)
		return atom;
	label_fail:
	return NULL;
}

static mpeg4$define$dump(urn_)
{
	inner_fullbox_t fullbox;
	size_t n;
	char buffer[32];
	uint32_t level = unidata->dump_level;
	if (!mpeg4$define(inner, fullbox, get)(&fullbox, &data, &size))
		goto label_fail;
	mpeg4$define(inner, fullbox, dump)(
		mlog,
		&fullbox,
		mpeg4$define(inner, flags, string)(buffer, fullbox.flags, urln_flag_name, 1),
		level);
	if (fullbox.version)
		goto label_fail;
	if (!(fullbox.flags & 1))
	{
		if (!size || data[size - 1])
			goto label_fail;
		mlog_level_dump("name:     %s\n", (const char *) data);
		n = strlen((const char *) data) + 1;
		if (n < size)
			mlog_level_dump("location: %s\n", (const char *) data + n);
		size = 0;
	}
	if (!size)
		return atom;
	label_fail:
	return NULL;
}

static const mpeg4$define$alloc(url_)
{
	mpeg4_atom_t *restrict r;
	r = mpeg4_atom_alloc_empty();
	if (r)
	{
		r->interface.dump = mpeg4$define(atom, url_, dump);
	}
	return r;
}

static const mpeg4$define$alloc(urn_)
{
	mpeg4_atom_t *restrict r;
	r = mpeg4_atom_alloc_empty();
	if (r)
	{
		r->interface.dump = mpeg4$define(atom, urn_, dump);
	}
	return r;
}

mpeg4$define$find(url_)
{
	static const mpeg4_box_type_t type = { .c = "url " };
	return mpeg4_find_atom(inst, mpeg4$define(atom, url_, alloc), type.v, 0);
}

mpeg4$define$find(urn_)
{
	static const mpeg4_box_type_t type = { .c = "urn " };
	return mpeg4_find_atom(inst, mpeg4$define(atom, urn_, alloc), type.v, 0);
}
