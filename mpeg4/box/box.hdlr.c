#include "box.include.h"
#include "inner.fullbox.h"

typedef struct handler_uni_t {
	uint32_t pre_defined;
	uint32_t handler_type;
	uint32_t reserved[3];
	char name[];
} __attribute__ ((packed)) handler_uni_t;

mpeg4$define$dump(box, hdlr)
{
	inner_fullbox_t fullbox;
	mpeg4_box_type_t type;
	if (!mpeg4$define(inner, fullbox, get)(&fullbox, &data, &size))
		goto label_fail;
	mlog_level_dump("version = %u, flags = %06x\n", fullbox.version, fullbox.flags);
	if (fullbox.version != 0)
		goto label_fail;
	if (size < sizeof(handler_uni_t))
		goto label_fail;
	type.v = ((const handler_uni_t *) data)->handler_type;
	mlog_level_dump("handler type: %c%c%c%c\n", type.c[0], type.c[1], type.c[2], type.c[3]);
	data += sizeof(handler_uni_t);
	size -= sizeof(handler_uni_t);
	if (size && !data[size - 1])
	{
		mlog_level_dump("name:         %s\n", (const char *) data);
		return inst;
	}
	label_fail:
	return NULL;
}
