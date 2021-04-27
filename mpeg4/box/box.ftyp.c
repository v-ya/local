#include "box.include.h"

typedef struct file_type_t {
	mpeg4_box_type_t major_brand;
	uint32_t minor_version;
	mpeg4_box_type_t compatible_brands[];
} __attribute__ ((packed)) file_type_t;

static mpeg4$define$dump(ftyp)
{
	mpeg4_box_type_t type;
	uint32_t level = unidata->dump_level;
	if (size < sizeof(file_type_t))
		goto label_fail;
	type = ((const file_type_t *) data)->major_brand;
	mlog_level_dump("major-brand:   %c%c%c%c\n", type.c[0], type.c[1], type.c[2], type.c[3]);
	mlog_level_dump("minor-version: %08x\n", mpeg4_n32(((const file_type_t *) data)->minor_version));
	mlog_level_dump("compatible-brand:\n");
	data = (const uint8_t *) ((const file_type_t *) data)->compatible_brands;
	size -= sizeof(file_type_t);
	level += mlog_level_width;
	while (size >= sizeof(mpeg4_box_type_t))
	{
		type = *(const mpeg4_box_type_t *) data;
		mlog_level_dump("%c%c%c%c\n", type.c[0], type.c[1], type.c[2], type.c[3]);
		data += sizeof(mpeg4_box_type_t);
		size -= sizeof(mpeg4_box_type_t);
	}
	if (!size) return atom;
	label_fail:
	return NULL;
}

static const mpeg4$define$alloc(ftyp)
{
	mpeg4_atom_s *restrict r;
	r = mpeg4_atom_alloc_empty();
	if (r)
	{
		r->interface.dump = mpeg4$define(atom, ftyp, dump);
	}
	return r;
}

mpeg4$define$find(ftyp)
{
	static const mpeg4_box_type_t type = { .c = "ftyp" };
	return mpeg4_find_atom(inst, mpeg4$define(atom, ftyp, alloc), type.v, 0);
}
