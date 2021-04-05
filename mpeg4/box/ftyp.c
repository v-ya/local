#include "box.func.h"

typedef struct file_type_t {
	mpeg4_box_type_t major_brand;
	uint32_t minor_version;
	mpeg4_box_type_t compatible_brands[];
} __attribute__ ((packed)) file_type_t;

mpeg4$define$dump(box, ftyp)
{
	register mpeg4_box_type_t type;
	if (size < sizeof(file_type_t))
		goto label_fail;
	type = ((const file_type_t *) data)->major_brand;
	mlog_level_dump("major-brand:   %c%c%c%c\n", type.c[0], type.c[1], type.c[2], type.c[3]);
	mlog_level_dump("minor-version: %08x\n", mpeg4_n32(((const file_type_t *) data)->minor_version));
	mlog_level_dump("compatible-brand:\n");
	data = (const uint8_t *) ((const file_type_t *) data)->compatible_brands;
	size -= sizeof(file_type_t);
	++level;
	while (size >= sizeof(mpeg4_box_type_t))
	{
		type = *(const mpeg4_box_type_t *) data;
		mlog_level_dump("%c%c%c%c\n", type.c[0], type.c[1], type.c[2], type.c[3]);
		data += sizeof(mpeg4_box_type_t);
		size -= sizeof(mpeg4_box_type_t);
	}
	if (!size) return inst;
	label_fail:
	return NULL;
}
