#include "box.include.h"
#include "inner.fullbox.h"
#include "inner.type.h"
#include "inner.data.h"

typedef struct handler_uni_t {
	uint32_t pre_defined;
	mpeg4_box_type_t handler_type;
	uint32_t reserved[3];
	char name[];
} __attribute__ ((packed)) handler_uni_t;

typedef struct mpeg4_stuff__handler_t {
	inner_fullbox_t fullbox;
	mpeg4_box_type_t handler_type;
	char *name;
	uintptr_t length;
} mpeg4_stuff__handler_t;

typedef struct mpeg4_stuff__handler_s {
	mpeg4_stuff_t stuff;
	mpeg4_stuff__handler_t pri;
} mpeg4_stuff__handler_s;

static mpeg4$define$dump(hdlr)
{
	inner_fullbox_t fullbox;
	handler_uni_t header;
	char type_string[16];
	uint32_t level = unidata->dump_level;
	if (!mpeg4$define(inner, fullbox, get)(&fullbox, &data, &size))
		goto label_fail;
	mpeg4$define(inner, fullbox, dump)(mlog, &fullbox, NULL, level);
	if (fullbox.version)
		goto label_fail;
	if (!mpeg4$define(inner, data, get)(&header, sizeof(header), &data, &size))
		goto label_fail;
	mlog_level_dump("handler type: %s\n", mpeg4$define(inner, type, string)(type_string, header.handler_type));
	if (size && !data[size - 1])
	{
		mlog_level_dump("name:         %s\n", (const char *) data);
		return atom;
	}
	label_fail:
	return NULL;
}

static mpeg4$define$stuff$free(hdlr, mpeg4_stuff__handler_s)
{
	mpeg4$define(inner, string, set)(&r->pri.name, &r->pri.length, NULL);
	mpeg4_stuff_free_default_func(&r->stuff);
}

static mpeg4$define$create(hdlr)
{
	return mpeg4_stuff_alloc(atom, inst, type,
		sizeof(mpeg4_stuff__handler_s),
		NULL,
		(refer_free_f) mpeg4$define(stuff, hdlr, free));
}

static mpeg4$define$parse(hdlr)
{
	inner_fullbox_t fullbox;
	handler_uni_t header;
	if (!mpeg4$define(inner, fullbox, get)(&fullbox, &data, &size))
		goto label_fail;
	if (!mpeg4$stuff$method$call(stuff, set$version_and_flags, fullbox.version, fullbox.flags))
		goto label_fail;
	if (!mpeg4$define(inner, data, get)(&header, sizeof(header), &data, &size))
		goto label_fail;
	if (!mpeg4$stuff$method$call(stuff, set$major_brand, header.handler_type))
		goto label_fail;
	if (!size || data[size - 1])
		goto label_fail;
	if (!mpeg4$stuff$method$call(stuff, set$name, (const char *) data))
		goto label_fail;
	return stuff;
	label_fail:
	return NULL;
}

static mpeg4$define$calc(hdlr)
{
	mpeg4_stuff__handler_t *restrict r = &((mpeg4_stuff__handler_s *) stuff)->pri;
	return mpeg4_stuff_calc_okay(stuff, sizeof(mpeg4_full_box_suffix_t) + sizeof(handler_uni_t) + r->length + 1);
}

static mpeg4$define$build(hdlr)
{
	mpeg4_stuff__handler_t *restrict r = &((mpeg4_stuff__handler_s *) stuff)->pri;
	handler_uni_t header = {
		.pre_defined = 0,
		.reserved = {0, 0, 0}
	};
	header.handler_type.v = r->handler_type.v;
	data = mpeg4$define(inner, fullbox, set)(data, &r->fullbox);
	data = mpeg4$define(inner, data, set)(data, &header, sizeof(header));
	if (r->length)
		data = mpeg4$define(inner, data, set)(data, r->name, r->length);
	*data = 0;
	return stuff;
}

static inner_method_set_fullbox(hdlr, mpeg4_stuff__handler_s, pri.fullbox, 0);
static inner_method_get_fullbox(hdlr, mpeg4_stuff__handler_s, pri.fullbox);

static inner_method_set_simple_param(hdlr, major_brand, mpeg4_stuff__handler_s, mpeg4_box_type_t, pri.handler_type);

static const mpeg4_stuff_t* mpeg4$define(stuff, hdlr, set$name)(mpeg4_stuff__handler_s *restrict r, const char *restrict name)
{
	if (mpeg4$define(inner, string, set)(&r->pri.name, &r->pri.length, name))
		return &r->stuff;
	return NULL;
}

static const mpeg4$define$alloc(hdlr)
{
	mpeg4_atom_s *restrict r;
	r = mpeg4_atom_alloc_empty();
	if (r)
	{
		r->interface.dump = mpeg4$define(atom, hdlr, dump);
		r->interface.create = mpeg4$define(atom, hdlr, create);
		r->interface.parse = mpeg4$define(atom, hdlr, parse);
		r->interface.calc = mpeg4$define(atom, hdlr, calc);
		r->interface.build = mpeg4$define(atom, hdlr, build);
		if (
			mpeg4$stuff$method$set(r, hdlr, set$version_and_flags) &&
			mpeg4$stuff$method$set(r, hdlr, get$version_and_flags) &&
			mpeg4$stuff$method$set(r, hdlr, set$major_brand) &&
			mpeg4$stuff$method$set(r, hdlr, set$name)
		) return r;
		refer_free(r);
	}
	return NULL;
}

mpeg4$define$find(hdlr)
{
	static const mpeg4_box_type_t type = { .c = "hdlr" };
	return mpeg4_find_atom(inst, mpeg4$define(atom, hdlr, alloc), type.v, 0);
}
