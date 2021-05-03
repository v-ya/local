#include "box.include.h"
#include "inner.fullbox.h"

typedef struct mpeg4_stuff__null_media_header_s {
	mpeg4_stuff_t stuff;
	inner_fullbox_t fullbox;
} mpeg4_stuff__null_media_header_s;

static mpeg4$define$dump(nmhd)
{
	inner_fullbox_t fullbox;
	uint32_t level = unidata->dump_level;
	if (!mpeg4$define(inner, fullbox, get)(&fullbox, &data, &size))
		goto label_fail;
	mpeg4$define(inner, fullbox, dump)(mlog, &fullbox, NULL, level);
	if (fullbox.version)
		goto label_fail;
	if (!size) return atom;
	label_fail:
	return NULL;
}

static mpeg4$define$create(nmhd)
{
	return mpeg4_stuff_alloc(atom, inst, type,
		sizeof(mpeg4_stuff__null_media_header_s),
		NULL,
		NULL);
}

static mpeg4$define$parse(nmhd)
{
	inner_fullbox_t fullbox;
	if (!mpeg4$define(inner, fullbox, get)(&fullbox, &data, &size))
		goto label_fail;
	if (!mpeg4$stuff$method$call(stuff, set$version_and_flags, fullbox.version, fullbox.flags))
		goto label_fail;
	if (!size) return stuff;
	label_fail:
	return NULL;
}

static mpeg4$define$calc(nmhd)
{
	return mpeg4_stuff_calc_okay(stuff, sizeof(mpeg4_full_box_suffix_t));
}

static mpeg4$define$build(nmhd)
{
	mpeg4$define(inner, fullbox, set)(data, &((mpeg4_stuff__null_media_header_s *) stuff)->fullbox);
	return stuff;
}

static inner_method_set_fullbox(nmhd, mpeg4_stuff__null_media_header_s, fullbox, 0);
static inner_method_get_fullbox(nmhd, mpeg4_stuff__null_media_header_s, fullbox);

static const mpeg4$define$alloc(nmhd)
{
	mpeg4_atom_s *restrict r;
	r = mpeg4_atom_alloc_empty();
	if (r)
	{
		r->interface.dump = mpeg4$define(atom, nmhd, dump);
		r->interface.create = mpeg4$define(atom, nmhd, create);
		r->interface.parse = mpeg4$define(atom, nmhd, parse);
		r->interface.calc = mpeg4$define(atom, nmhd, calc);
		r->interface.build = mpeg4$define(atom, nmhd, build);
		if (
			mpeg4$stuff$method$set(r, nmhd, set$version_and_flags) &&
			mpeg4$stuff$method$set(r, nmhd, get$version_and_flags)
		) return r;
		refer_free(r);
	}
	return NULL;
}

mpeg4$define$find(nmhd)
{
	static const mpeg4_box_type_t type = { .c = "nmhd" };
	return mpeg4_find_atom(inst, mpeg4$define(atom, nmhd, alloc), type.v, 0);
}
