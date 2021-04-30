#include "box.include.h"
#include "inner.fullbox.h"
#include "inner.data.h"

typedef struct mpeg4_stuff__extended_language_t {
	inner_fullbox_t fullbox;
	char *name;
	size_t length;
} mpeg4_stuff__extended_language_t;

typedef struct mpeg4_stuff__extended_language_s {
	mpeg4_stuff_t stuff;
	mpeg4_stuff__extended_language_t pri;
} mpeg4_stuff__extended_language_s;

static mpeg4$define$dump(elng)
{
	inner_fullbox_t fullbox;
	uint32_t level = unidata->dump_level;
	if (!mpeg4$define(inner, fullbox, get)(&fullbox, &data, &size))
		goto label_fail;
	mpeg4$define(inner, fullbox, dump)(mlog, &fullbox, NULL, level);
	if (fullbox.version)
		goto label_fail;
	if (size && !data[size - 1])
	{
		mlog_level_dump("extended language: %s\n", (const char *) data);
		return atom;
	}
	label_fail:
	return NULL;
}

static mpeg4$define$stuff$free(elng, mpeg4_stuff__extended_language_s)
{
	mpeg4$define(inner, string, set)(&r->pri.name, &r->pri.length, NULL);
	mpeg4_stuff_free_default_func(&r->stuff);
}

static mpeg4$define$create(elng)
{
	return mpeg4_stuff_alloc(atom, inst, type,
		sizeof(mpeg4_stuff__extended_language_s),
		NULL,
		(refer_free_f) mpeg4$define(stuff, elng, free));
}

static mpeg4$define$parse(elng)
{
	inner_fullbox_t fullbox;
	if (!mpeg4$define(inner, fullbox, get)(&fullbox, &data, &size))
		goto label_fail;
	if (!mpeg4$stuff$method$call(stuff, set$version_and_flags, fullbox.version, fullbox.flags))
		goto label_fail;
	if (!size || data[size - 1])
		goto label_fail;
	if (!mpeg4$stuff$method$call(stuff, set$name, (const char *) data))
		goto label_fail;
	return stuff;
	label_fail:
	return NULL;
}

static mpeg4$define$calc(elng)
{
	mpeg4_stuff__extended_language_t *restrict r = &((mpeg4_stuff__extended_language_s *) stuff)->pri;
	mpeg4_stuff_calc_okay(stuff, sizeof(mpeg4_full_box_suffix_t) + r->length + 1);
	return stuff;
}

static mpeg4$define$build(elng)
{
	mpeg4_stuff__extended_language_t *restrict r = &((mpeg4_stuff__extended_language_s *) stuff)->pri;
	data = mpeg4$define(inner, fullbox, set)(data, &r->fullbox);
	if (r->length)
		data = mpeg4$define(inner, data, set)(data, r->name, r->length);
	*data = 0;
	return stuff;
}

static const mpeg4_stuff_t* mpeg4$define(stuff, elng, set$version_and_flags)(mpeg4_stuff__extended_language_s *restrict r, uint32_t version, uint32_t flags)
{
	if (version) goto label_fail;
	r->pri.fullbox.version = version;
	r->pri.fullbox.flags = flags;
	return &r->stuff;
	label_fail:
	return NULL;
}

static const mpeg4_stuff_t* mpeg4$define(stuff, elng, set$name)(mpeg4_stuff__extended_language_s *restrict r, const char *restrict name)
{
	if (mpeg4$define(inner, string, set)(&r->pri.name, &r->pri.length, name))
		return &r->stuff;
	return NULL;
}

static const mpeg4$define$alloc(elng)
{
	mpeg4_atom_s *restrict r;
	r = mpeg4_atom_alloc_empty();
	if (r)
	{
		r->interface.dump = mpeg4$define(atom, elng, dump);
		r->interface.create = mpeg4$define(atom, elng, create);
		r->interface.parse = mpeg4$define(atom, elng, parse);
		r->interface.calc = mpeg4$define(atom, elng, calc);
		r->interface.build = mpeg4$define(atom, elng, build);
		if (
			mpeg4$stuff$method$set(r, elng, set$version_and_flags) &&
			mpeg4$stuff$method$set(r, elng, set$name)
		) return r;
		refer_free(r);
	}
	return NULL;
}

mpeg4$define$find(elng)
{
	static const mpeg4_box_type_t type = { .c = "elng" };
	return mpeg4_find_atom(inst, mpeg4$define(atom, elng, alloc), type.v, 0);
}
