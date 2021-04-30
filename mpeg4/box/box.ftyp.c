#include "box.include.h"
#include "inner.type.h"
#include "inner.data.h"

typedef struct file_type_t {
	mpeg4_box_type_t major_brand;
	uint32_t minor_version;
	mpeg4_box_type_t compatible_brands[];
} __attribute__ ((packed)) file_type_t;

typedef struct mpeg4_stuff__file_type_t {
	mpeg4_box_type_t major_brand;
	uint32_t minor_version;
	inner_array_t compatible_brands;
} mpeg4_stuff__file_type_t;

typedef struct mpeg4_stuff__file_type_s {
	mpeg4_stuff_t stuff;
	mpeg4_stuff__file_type_t pri;
} mpeg4_stuff__file_type_s;

static mpeg4$define$dump(ftyp)
{
	char type_string[16];
	mpeg4_box_type_t type;
	uint32_t level = unidata->dump_level;
	if (size < sizeof(file_type_t))
		goto label_fail;
	type = ((const file_type_t *) data)->major_brand;
	mlog_level_dump("major-brand:   %s\n", mpeg4$define(inner, type, string)(type_string, type));
	mlog_level_dump("minor-version: %08x\n", mpeg4_n32(((const file_type_t *) data)->minor_version));
	mlog_level_dump("compatible-brand:\n");
	data = (const uint8_t *) ((const file_type_t *) data)->compatible_brands;
	size -= sizeof(file_type_t);
	level += mlog_level_width;
	while (size >= sizeof(mpeg4_box_type_t))
	{
		type = *(const mpeg4_box_type_t *) data;
		mlog_level_dump("%s\n", mpeg4$define(inner, type, string)(type_string, type));
		data += sizeof(mpeg4_box_type_t);
		size -= sizeof(mpeg4_box_type_t);
	}
	if (!size) return atom;
	label_fail:
	return NULL;
}

static mpeg4$define$stuff$init(ftyp, mpeg4_stuff__file_type_s)
{
	r->pri.major_brand = (mpeg4_box_type_t) {.c = "isom"};
	r->pri.minor_version = 1;
	mpeg4$define(inner, array, init)(&r->pri.compatible_brands, sizeof(mpeg4_box_type_t));
	return &r->stuff;
}

static mpeg4$define$stuff$free(ftyp, mpeg4_stuff__file_type_s)
{
	mpeg4$define(inner, array, clear)(&r->pri.compatible_brands);
	mpeg4_stuff_free_default_func(&r->stuff);
}

static mpeg4$define$create(ftyp)
{
	return mpeg4_stuff_alloc(atom, inst, type,
		sizeof(mpeg4_stuff__file_type_s),
		(mpeg4_stuff_init_f) mpeg4$define(stuff, ftyp, init),
		(refer_free_f) mpeg4$define(stuff, ftyp, free));
}

static mpeg4$define$parse(ftyp)
{
	if (size < sizeof(file_type_t))
		goto label_fail;
	if (!mpeg4$stuff$method$call(stuff, set$major_brand, ((const file_type_t *) data)->major_brand))
		goto label_fail;
	if (!mpeg4$stuff$method$call(stuff, set$minor_version, mpeg4_n32(((const file_type_t *) data)->minor_version)))
		goto label_fail;
	data += sizeof(file_type_t);
	size -= sizeof(file_type_t);
	if (!(size % sizeof(mpeg4_box_type_t)))
	{
		if (size)
		{
			if (!mpeg4$stuff$method$call(stuff, add$compatible_brands, (const mpeg4_box_type_t *) data, size / sizeof(mpeg4_box_type_t)))
				goto label_fail;
		}
		return stuff;
	}
	label_fail:
	return NULL;
}

static mpeg4$define$calc(ftyp)
{
	mpeg4_stuff__file_type_t *restrict r = &((mpeg4_stuff__file_type_s *) stuff)->pri;
	mpeg4_stuff_calc_okay(stuff, sizeof(file_type_t) + sizeof(mpeg4_box_type_t) * r->compatible_brands.number);
	return stuff;
}

static mpeg4$define$build(ftyp)
{
	mpeg4_stuff__file_type_t *restrict r = &((mpeg4_stuff__file_type_s *) stuff)->pri;
	data[0] = r->major_brand.c[0];
	data[1] = r->major_brand.c[1];
	data[2] = r->major_brand.c[2];
	data[3] = r->major_brand.c[3];
	data = mpeg4$define(inner, uint32_t, set)(data + 4, r->minor_version);
	if (r->compatible_brands.number)
		mpeg4$define(inner, data, set)(data, r->compatible_brands.array, r->compatible_brands.number * sizeof(mpeg4_box_type_t));
	return stuff;
}

static const mpeg4_stuff_t* mpeg4$define(stuff, ftyp, set$major_brand)(mpeg4_stuff__file_type_s *restrict r, mpeg4_box_type_t major_brand)
{
	r->pri.major_brand = major_brand;
	return &r->stuff;
}

static const mpeg4_stuff_t* mpeg4$define(stuff, ftyp, set$minor_version)(mpeg4_stuff__file_type_s *restrict r, uint32_t minor_version)
{
	r->pri.minor_version = minor_version;
	return &r->stuff;
}

static const mpeg4_stuff_t* mpeg4$define(stuff, ftyp, add$compatible_brands)(mpeg4_stuff__file_type_s *restrict r, const mpeg4_box_type_t *restrict compatible_brands, uint32_t n)
{
	if (mpeg4$define(inner, array, append_data)(&r->pri.compatible_brands, (uintptr_t) n, compatible_brands))
		return &r->stuff;
	return NULL;
}

static const mpeg4$define$alloc(ftyp)
{
	mpeg4_atom_s *restrict r;
	r = mpeg4_atom_alloc_empty();
	if (r)
	{
		r->interface.dump = mpeg4$define(atom, ftyp, dump);
		r->interface.create = mpeg4$define(atom, ftyp, create);
		r->interface.parse = mpeg4$define(atom, ftyp, parse);
		r->interface.calc = mpeg4$define(atom, ftyp, calc);
		r->interface.build = mpeg4$define(atom, ftyp, build);
		if (
			mpeg4$stuff$method$set(r, ftyp, set$major_brand) &&
			mpeg4$stuff$method$set(r, ftyp, set$minor_version) &&
			mpeg4$stuff$method$set(r, ftyp, add$compatible_brands)
		) return r;
		refer_free(r);
	}
	return NULL;
}

mpeg4$define$find(ftyp)
{
	static const mpeg4_box_type_t type = { .c = "ftyp" };
	return mpeg4_find_atom(inst, mpeg4$define(atom, ftyp, alloc), type.v, 0);
}
