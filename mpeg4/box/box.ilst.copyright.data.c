#include "box.include.h"
#include "inner.fullbox.h"
#include "inner.data.h"
#include "inner.bindata.h"
#include <memory.h>
#include <alloca.h>

typedef enum ilst_copyright_data_flag_t {
	ilst_copyright_data_flag$text = 0x01,
} ilst_copyright_data_flag_t;

static mpeg4$define$dump(ilst_copyright_data)
{
	inner_fullbox_t fullbox;
	char *restrict text;
	uint32_t reserve;
	uint32_t level = unidata->dump_level;
	if (!mpeg4$define(inner, fullbox, get)(&fullbox, &data, &size))
		goto label_fail;
	mpeg4$define(inner, fullbox, dump)(mlog, &fullbox, NULL, level);
	if (!mpeg4$define(inner, uint32_t, get)(&reserve, &data, &size))
		goto label_fail;
	mlog_level_dump("reserve: %08x (%u)\n", reserve, reserve);
	switch (fullbox.flags)
	{
		case ilst_copyright_data_flag$text:
			mlog_level_dump("type:    text\n");
			if (size > 0xffff) size = 0xffff;
			text = alloca(size + 1);
			if (text)
			{
				memcpy(text, data, size);
				text[size] = 0;
				mlog_level_dump("value:   %s\n", text);
			}
			break;
		default:
			mlog_level_dump("type:    unknow\n");
			mlog_level_dump("value:\n");
			mpeg4$define(inner, bindata, dump)(mlog, data, size, 0, level + mlog_level_width);
	}
	return atom;
	label_fail:
	return NULL;
}

static const mpeg4$define$alloc(ilst_copyright_data)
{
	mpeg4_atom_t *restrict r;
	r = mpeg4_atom_alloc_empty();
	if (r)
	{
		r->interface.dump = mpeg4$define(atom, ilst_copyright_data, dump);
	}
	return r;
}

mpeg4$define$find(ilst_copyright_data)
{
	static const mpeg4_box_type_t type = { .c = "data" };
	static const mpeg4_box_type_t extra = { .c = "ilst" };
	return mpeg4_find_atom(inst, mpeg4$define(atom, ilst_copyright_data, alloc), type.v, extra.v);
}
