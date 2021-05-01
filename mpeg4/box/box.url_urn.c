#include "box.include.h"
#include "inner.fullbox.h"
#include "inner.data.h"
#include <string.h>

typedef struct mpeg4_stuff__data_entry_url_urn_t {
	inner_fullbox_t fullbox;
	char *name;
	char *location;
	uintptr_t n_name;
	uintptr_t n_location;
} mpeg4_stuff__data_entry_url_urn_t;

typedef struct mpeg4_stuff__data_entry_url_urn_s {
	mpeg4_stuff_t stuff;
	mpeg4_stuff__data_entry_url_urn_t pri;
} mpeg4_stuff__data_entry_url_urn_s;

static const char *urln_flag_name[1] = {
	[0] = "media_data_in_same_file"
};

static mpeg4$define$dump(url_)
{
	inner_fullbox_t fullbox;
	char buffer[32];
	uint32_t level = unidata->dump_level;
	if (!mpeg4$define(inner, fullbox, get)(&fullbox, &data, &size))
		goto label_fail;
	mpeg4$define(inner, fullbox, dump)(
		mlog,
		&fullbox,
		mpeg4$define(inner, flags, string)(buffer, fullbox.flags, urln_flag_name, 1),
		level);
	if (fullbox.version)
		goto label_fail;
	if (!(fullbox.flags & 1))
	{
		if (!size || data[size - 1])
			goto label_fail;
		mlog_level_dump("name: %s\n", (const char *) data);
		size = 0;
	}
	if (!size) return atom;
	label_fail:
	return NULL;
}

static mpeg4$define$dump(urn_)
{
	inner_fullbox_t fullbox;
	size_t n;
	char buffer[32];
	uint32_t level = unidata->dump_level;
	if (!mpeg4$define(inner, fullbox, get)(&fullbox, &data, &size))
		goto label_fail;
	mpeg4$define(inner, fullbox, dump)(
		mlog,
		&fullbox,
		mpeg4$define(inner, flags, string)(buffer, fullbox.flags, urln_flag_name, 1),
		level);
	if (fullbox.version)
		goto label_fail;
	if (!(fullbox.flags & 1))
	{
		if (!size || data[size - 1])
			goto label_fail;
		mlog_level_dump("name:     %s\n", (const char *) data);
		n = strlen((const char *) data) + 1;
		if (n >= size) goto label_fail;
		mlog_level_dump("location: %s\n", (const char *) data + n);
		size = 0;
	}
	if (!size) return atom;
	label_fail:
	return NULL;
}

static mpeg4$define$stuff$free(url_urn, mpeg4_stuff__data_entry_url_urn_s)
{
	mpeg4$define(inner, string, set)(&r->pri.name, &r->pri.n_name, NULL);
	mpeg4$define(inner, string, set)(&r->pri.location, &r->pri.n_location, NULL);
	mpeg4_stuff_free_default_func(&r->stuff);
}

static mpeg4$define$create(url_urn)
{
	return mpeg4_stuff_alloc(atom, inst, type,
		sizeof(mpeg4_stuff__data_entry_url_urn_s),
		NULL,
		(refer_free_f) mpeg4$define(stuff, url_urn, free));
}

static mpeg4$define$parse(url_)
{
	inner_fullbox_t fullbox;
	if (!mpeg4$define(inner, fullbox, get)(&fullbox, &data, &size))
		goto label_fail;
	if (!mpeg4$stuff$method$call(stuff, set$version_and_flags, fullbox.version, fullbox.flags))
		goto label_fail;
	if (!(fullbox.flags & 1))
	{
		if (!size || data[size - 1])
			goto label_fail;
		if (!mpeg4$stuff$method$call(stuff, set$name, (const char *) data))
			goto label_fail;
		size = 0;
	}
	if (!size) return stuff;
	label_fail:
	return NULL;
}

static mpeg4$define$parse(urn_)
{
	inner_fullbox_t fullbox;
	size_t n;
	if (!mpeg4$define(inner, fullbox, get)(&fullbox, &data, &size))
		goto label_fail;
	if (!mpeg4$stuff$method$call(stuff, set$version_and_flags, fullbox.version, fullbox.flags))
		goto label_fail;
	if (!(fullbox.flags & 1))
	{
		if (!size || data[size - 1])
			goto label_fail;
		if (!mpeg4$stuff$method$call(stuff, set$name, (const char *) data))
			goto label_fail;
		n = strlen((const char *) data) + 1;
		if (n >= size)
			goto label_fail;
		if (!mpeg4$stuff$method$call(stuff, set$location, (const char *) data + n))
			goto label_fail;
		size = 0;
	}
	if (!size) return stuff;
	label_fail:
	return NULL;
}

static mpeg4$define$calc(url_)
{
	mpeg4_stuff__data_entry_url_urn_t *restrict r = &((mpeg4_stuff__data_entry_url_urn_s *) stuff)->pri;
	return mpeg4_stuff_calc_okay(stuff, sizeof(mpeg4_full_box_suffix_t) + (!(r->fullbox.flags & 1)?(r->n_name + 1):0));
}

static mpeg4$define$calc(urn_)
{
	mpeg4_stuff__data_entry_url_urn_t *restrict r = &((mpeg4_stuff__data_entry_url_urn_s *) stuff)->pri;
	return mpeg4_stuff_calc_okay(stuff, sizeof(mpeg4_full_box_suffix_t) + (!(r->fullbox.flags & 1)?(r->n_name + r->n_location + 2):0));
}

static mpeg4$define$build(url_)
{
	mpeg4_stuff__data_entry_url_urn_t *restrict r = &((mpeg4_stuff__data_entry_url_urn_s *) stuff)->pri;
	data = mpeg4$define(inner, fullbox, set)(data, &r->fullbox);
	if (r->n_name)
		data = mpeg4$define(inner, data, set)(data, r->name, r->n_name);
	*data = 0;
	return stuff;
}

static mpeg4$define$build(urn_)
{
	mpeg4_stuff__data_entry_url_urn_t *restrict r = &((mpeg4_stuff__data_entry_url_urn_s *) stuff)->pri;
	data = mpeg4$define(inner, fullbox, set)(data, &r->fullbox);
	if (r->n_name)
		data = mpeg4$define(inner, data, set)(data, r->name, r->n_name);
	*data++ = 0;
	if (r->n_location)
		data = mpeg4$define(inner, data, set)(data, r->location, r->n_location);
	*data = 0;
	return stuff;
}

static const mpeg4_stuff_t* mpeg4$define(stuff, url_urn, set$version_and_flags)(mpeg4_stuff__data_entry_url_urn_s *restrict r, uint32_t version, uint32_t flags)
{
	if (version) goto label_fail;
	r->pri.fullbox.version = version;
	r->pri.fullbox.flags = flags;
	return &r->stuff;
	label_fail:
	return NULL;
}

static const mpeg4_stuff_t* mpeg4$define(stuff, url_urn, set$name)(mpeg4_stuff__data_entry_url_urn_s *restrict r, const char *restrict name)
{
	if (mpeg4$define(inner, string, set)(&r->pri.name, &r->pri.n_name, name))
		return &r->stuff;
	return NULL;
}

static const mpeg4_stuff_t* mpeg4$define(stuff, urn_, set$location)(mpeg4_stuff__data_entry_url_urn_s *restrict r, const char *restrict location)
{
	if (mpeg4$define(inner, string, set)(&r->pri.location, &r->pri.n_location, location))
		return &r->stuff;
	return NULL;
}

static const mpeg4$define$alloc(url_)
{
	mpeg4_atom_s *restrict r;
	r = mpeg4_atom_alloc_empty();
	if (r)
	{
		r->interface.dump = mpeg4$define(atom, url_, dump);
		r->interface.create = mpeg4$define(atom, url_urn, create);
		r->interface.parse = mpeg4$define(atom, url_, parse);
		r->interface.calc = mpeg4$define(atom, url_, calc);
		r->interface.build = mpeg4$define(atom, url_, build);
		if (
			mpeg4$stuff$method$set(r, url_urn, set$version_and_flags) &&
			mpeg4$stuff$method$set(r, url_urn, set$name)
		) return r;
		refer_free(r);
	}
	return NULL;
}

static const mpeg4$define$alloc(urn_)
{
	mpeg4_atom_s *restrict r;
	r = mpeg4_atom_alloc_empty();
	if (r)
	{
		r->interface.dump = mpeg4$define(atom, urn_, dump);
		r->interface.create = mpeg4$define(atom, url_urn, create);
		r->interface.parse = mpeg4$define(atom, urn_, parse);
		r->interface.calc = mpeg4$define(atom, urn_, calc);
		r->interface.build = mpeg4$define(atom, urn_, build);
		if (
			mpeg4$stuff$method$set(r, url_urn, set$version_and_flags) &&
			mpeg4$stuff$method$set(r, url_urn, set$name) &&
			mpeg4$stuff$method$set(r, urn_, set$location)
		) return r;
		refer_free(r);
	}
	return NULL;
}

mpeg4$define$find(url_)
{
	static const mpeg4_box_type_t type = { .c = "url " };
	return mpeg4_find_atom(inst, mpeg4$define(atom, url_, alloc), type.v, 0);
}

mpeg4$define$find(urn_)
{
	static const mpeg4_box_type_t type = { .c = "urn " };
	return mpeg4_find_atom(inst, mpeg4$define(atom, urn_, alloc), type.v, 0);
}
