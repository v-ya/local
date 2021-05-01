#include "box.include.h"
#include "inner.fullbox.h"
#include "inner.data.h"
#include "inner.bindata.h"
#include <memory.h>
#include <alloca.h>

typedef enum ilst_copyright_data_flag_t {
	ilst_copyright_data_flag$text = 0x01,
} ilst_copyright_data_flag_t;

typedef struct mpeg4_stuff__ilst_copyright_data_t {
	inner_fullbox_t fullbox;
	char *text;
	uintptr_t text_length;
} mpeg4_stuff__ilst_copyright_data_t;

typedef struct mpeg4_stuff__ilst_copyright_data_s {
	mpeg4_stuff_t stuff;
	mpeg4_stuff__ilst_copyright_data_t pri;
} mpeg4_stuff__ilst_copyright_data_s;

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

static mpeg4$define$stuff$init(ilst_copyright_data, mpeg4_stuff__ilst_copyright_data_s)
{
	r->pri.fullbox.flags = ilst_copyright_data_flag$text;
	return &r->stuff;
}

static mpeg4$define$stuff$free(ilst_copyright_data, mpeg4_stuff__ilst_copyright_data_s)
{
	mpeg4$define(inner, string, set)(&r->pri.text, &r->pri.text_length, NULL);
	mpeg4_stuff_free_default_func(&r->stuff);
}

static mpeg4$define$create(ilst_copyright_data)
{
	return mpeg4_stuff_alloc(atom, inst, type,
		sizeof(mpeg4_stuff__ilst_copyright_data_s),
		(mpeg4_stuff_init_f) mpeg4$define(stuff, ilst_copyright_data, init),
		(refer_free_f) mpeg4$define(stuff, ilst_copyright_data, free));
}

static mpeg4$define$parse(ilst_copyright_data)
{
	inner_fullbox_t fullbox;
	uint32_t reserve;
	if (!mpeg4$define(inner, fullbox, get)(&fullbox, &data, &size))
		goto label_fail;
	if (!mpeg4$stuff$method$call(stuff, set$version_and_flags, fullbox.version, fullbox.flags))
		goto label_fail;
	if (!mpeg4$define(inner, uint32_t, get)(&reserve, &data, &size))
		goto label_fail;
	switch (fullbox.flags)
	{
		case ilst_copyright_data_flag$text:
			if (!mpeg4$stuff$method$call(stuff, set$ilst_data_text, (const char *) data, (uintptr_t) size))
				goto label_fail;
			break;
		default:
			break;
	}
	return stuff;
	label_fail:
	return NULL;
}

static mpeg4$define$calc(ilst_copyright_data)
{
	mpeg4_stuff__ilst_copyright_data_t *restrict r = &((mpeg4_stuff__ilst_copyright_data_s *) stuff)->pri;
	uint64_t inner_size = sizeof(mpeg4_full_box_suffix_t) + sizeof(uint32_t);
	switch (r->fullbox.flags)
	{
		case ilst_copyright_data_flag$text:
			inner_size += r->text_length;
			break;
		default:
			break;
	}
	return mpeg4_stuff_calc_okay(stuff, inner_size);
}

static mpeg4$define$build(ilst_copyright_data)
{
	mpeg4_stuff__ilst_copyright_data_t *restrict r = &((mpeg4_stuff__ilst_copyright_data_s *) stuff)->pri;
	data = mpeg4$define(inner, fullbox, set)(data, &r->fullbox);
	data = mpeg4$define(inner, uint32_t, set)(data, 0);
	switch (r->fullbox.flags)
	{
		case ilst_copyright_data_flag$text:
			if (r->text_length)
				memcpy(data, r->text, r->text_length);
			break;
		default:
			break;
	}
	return stuff;
}

static const mpeg4_stuff_t* mpeg4$define(stuff, ilst_copyright_data, set$version_and_flags)(mpeg4_stuff__ilst_copyright_data_s *restrict r, uint32_t version, uint32_t flags)
{
	if (version) goto label_fail;
	r->pri.fullbox.version = version;
	r->pri.fullbox.flags = flags;
	return &r->stuff;
	label_fail:
	return NULL;
}

static const mpeg4_stuff_t* mpeg4$define(stuff, ilst_copyright_data, set$ilst_data_text)(mpeg4_stuff__ilst_copyright_data_s *restrict r, const char *restrict text, uintptr_t length)
{
	r->pri.fullbox.flags = ilst_copyright_data_flag$text;
	if (mpeg4$define(inner, string, set_with_length)(&r->pri.text, &r->pri.text_length, text, length))
		return &r->stuff;
	return NULL;
}

static const mpeg4$define$alloc(ilst_copyright_data)
{
	mpeg4_atom_s *restrict r;
	r = mpeg4_atom_alloc_empty();
	if (r)
	{
		r->interface.dump = mpeg4$define(atom, ilst_copyright_data, dump);
		r->interface.create = mpeg4$define(atom, ilst_copyright_data, create);
		r->interface.parse = mpeg4$define(atom, ilst_copyright_data, parse);
		r->interface.calc = mpeg4$define(atom, ilst_copyright_data, calc);
		r->interface.build = mpeg4$define(atom, ilst_copyright_data, build);
		if (
			mpeg4$stuff$method$set(r, ilst_copyright_data, set$version_and_flags) &&
			mpeg4$stuff$method$set(r, ilst_copyright_data, set$ilst_data_text)
		) return r;
		refer_free(r);
	}
	return NULL;
}

mpeg4$define$find(ilst_copyright_data)
{
	static const mpeg4_box_type_t type = { .c = "data" };
	static const mpeg4_box_type_t extra = { .c = "ilst" };
	return mpeg4_find_atom(inst, mpeg4$define(atom, ilst_copyright_data, alloc), type.v, extra.v);
}
