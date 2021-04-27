#include "box.include.h"
#include "inner.fullbox.h"
#include "inner.data.h"

typedef struct handler_uni_t {
	uint32_t pre_defined;
	mpeg4_box_type_t handler_type;
	uint32_t reserved[3];
	char name[];
} __attribute__ ((packed)) handler_uni_t;

static mpeg4$define$dump(hdlr)
{
	inner_fullbox_t fullbox;
	handler_uni_t header;
	uint32_t level = unidata->dump_level;
	if (!mpeg4$define(inner, fullbox, get)(&fullbox, &data, &size))
		goto label_fail;
	mpeg4$define(inner, fullbox, dump)(mlog, &fullbox, NULL, level);
	if (fullbox.version)
		goto label_fail;
	if (!mpeg4$define(inner, data, get)(&header, sizeof(header), &data, &size))
		goto label_fail;
	mlog_level_dump("handler type: %c%c%c%c\n",
		header.handler_type.c[0],
		header.handler_type.c[1],
		header.handler_type.c[2],
		header.handler_type.c[3]);
	if (size && !data[size - 1])
	{
		mlog_level_dump("name:         %s\n", (const char *) data);
		return atom;
	}
	label_fail:
	return NULL;
}

static const mpeg4$define$alloc(hdlr)
{
	mpeg4_atom_s *restrict r;
	r = mpeg4_atom_alloc_empty();
	if (r)
	{
		r->interface.dump = mpeg4$define(atom, hdlr, dump);
	}
	return r;
}

mpeg4$define$find(hdlr)
{
	static const mpeg4_box_type_t type = { .c = "hdlr" };
	return mpeg4_find_atom(inst, mpeg4$define(atom, hdlr, alloc), type.v, 0);
}
