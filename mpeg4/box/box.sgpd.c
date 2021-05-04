#include "box.include.h"
#include "inner.fullbox.h"
#include "inner.bindata.h"
#include "inner.type.h"
#include "inner.data.h"

typedef struct mpeg4_stuff__sample_group_description_t {
	inner_fullbox_t fullbox;
	mpeg4_box_type_t grouping_type;
	uint32_t default_length;
	uint32_t default_sample_description_index;
} mpeg4_stuff__sample_group_description_t;

typedef struct mpeg4_stuff__sample_group_description_s {
	mpeg4_stuff_t stuff;
	mpeg4_stuff__sample_group_description_t pri;
} mpeg4_stuff__sample_group_description_s;

static mpeg4$define$dump(sgpd)
{
	mpeg4_atom_dump_t ud;
	const mpeg4_atom_s *restrict layer;
	inner_fullbox_t fullbox;
	char type_string[16];
	mpeg4_box_type_t type;
	uint32_t n, default_length, length;
	uint32_t level = unidata->dump_level;
	if (!mpeg4$define(inner, fullbox, get)(&fullbox, &data, &size))
		goto label_fail;
	mpeg4$define(inner, fullbox, dump)(mlog, &fullbox, NULL, level);
	if (fullbox.version > 2)
		goto label_fail;
	if (!mpeg4$define(inner, data, get)(&type, sizeof(type), &data, &size))
		goto label_fail;
	mlog_level_dump("grouping type:                    %s\n", mpeg4$define(inner, type, string)(type_string, type));
	layer = mpeg4_atom_layer_find(atom, type.v);
	default_length = 0;
	if (fullbox.version == 1)
	{
		if (!mpeg4$define(inner, uint32_t, get)(&default_length, &data, &size))
			goto label_fail;
		mlog_level_dump("default length:                   %u\n", default_length);
	}
	if (fullbox.version == 2)
	{
		if (!mpeg4$define(inner, uint32_t, get)(&n, &data, &size))
			goto label_fail;
		mlog_level_dump("default sample description index: %u\n", n);
	}
	if (!mpeg4$define(inner, uint32_t, get)(&n, &data, &size))
		goto label_fail;
	mlog_level_dump("entry count:                      %u\n", n);
	if (layer && layer->interface.dump)
	{
		ud = *unidata;
		while (size && n)
		{
			if (fullbox.version == 1 && !default_length)
			{
				if (!mpeg4$define(inner, uint32_t, get)(&length, &data, &size) || !length)
					goto label_fail;
			}
			else length = default_length;
			mlog_level_dump("[%s] %u\n", type_string, length);
			ud.dump_level = level + mlog_level_width;
			ud.temp_size = 0;
			if (!layer->interface.dump(layer, mlog, data, length?length:size, &ud))
				goto label_fail;
			if (!ud.temp_size)
				goto label_fail;
			if (length && ud.temp_size != length)
				goto label_fail;
			data += ud.temp_size;
			size -= ud.temp_size;
			--n;
		}
		if (size || n)
			goto label_fail;
	}
	else
	{
		mlog_level_dump("unknow(%s[%u]): %u bytes\n", type_string, n, size);
		mpeg4$define(inner, bindata, dump)(mlog, data, size, 0, level + mlog_level_width);
	}
	return atom;
	label_fail:
	return NULL;
}

static mpeg4$define$create(sgpd)
{
	return mpeg4_stuff_alloc(atom, inst, type, sizeof(mpeg4_stuff__sample_group_description_s), NULL, NULL);
}

static mpeg4$define$parse(sgpd)
{
	const mpeg4_atom_s *restrict layer;
	mpeg4_stuff_t *restrict value;
	inner_fullbox_t fullbox;
	mpeg4_box_type_t type;
	uint32_t n, default_length, length;
	value = NULL;
	if (!mpeg4$define(inner, fullbox, get)(&fullbox, &data, &size))
		goto label_fail;
	if (!mpeg4$stuff$method$call(stuff, set$version_and_flags, fullbox.version, fullbox.flags))
		goto label_fail;
	if (!mpeg4$define(inner, data, get)(&type, sizeof(type), &data, &size))
		goto label_fail;
	layer = mpeg4_atom_layer_find(stuff->atom, type.v);
	if (layer && layer->interface.parse && layer->interface.calc)
	{
		default_length = 0;
		if (fullbox.version == 1)
		{
			if (!mpeg4$define(inner, uint32_t, get)(&default_length, &data, &size))
				goto label_fail;
		}
		if (fullbox.version == 2)
		{
			if (!mpeg4$define(inner, uint32_t, get)(&n, &data, &size))
				goto label_fail;
			if (!mpeg4$stuff$method$call(stuff, set$default_sample_description_index, n))
				goto label_fail;
		}
		if (!mpeg4$define(inner, uint32_t, get)(&n, &data, &size))
			goto label_fail;
		if (!n && !size)
			goto mpeg4_unlink;
		while (size && n)
		{
			if (fullbox.version == 1 && !default_length)
			{
				if (!mpeg4$define(inner, uint32_t, get)(&length, &data, &size) || !length)
					goto label_fail;
			}
			else length = default_length;
			if (!(value = mpeg4_stuff_container_append(stuff, type)))
				goto label_fail;
			if (!layer->interface.parse(value, data, length?length:size))
				goto label_fail;
			if (!layer->interface.calc(value))
				goto label_fail;
			if (!value->info.inner_size)
				goto label_fail;
			if (length && value->info.inner_size != (uint64_t) length)
				goto label_fail;
			data += value->info.inner_size;
			size -= value->info.inner_size;
			value = NULL;
			--n;
		}
		if (size || n)
			goto label_fail;
	}
	else
	{
		mpeg4_unlink:
		mpeg4_stuff_unlink(stuff);
	}
	return stuff;
	label_fail:
	if (value) mpeg4_stuff_unlink(value);
	return NULL;
}

static mpeg4$define$calc(sgpd)
{
	mpeg4_stuff_t *restrict value;
	mpeg4_calc_f calc;
	uint64_t size, n;
	mpeg4_stuff__sample_group_description_t *restrict r = &((mpeg4_stuff__sample_group_description_s *) stuff)->pri;
	size = n = 0;
	r->default_length = 0;
	for (value = stuff->container.list; value; value = value->link.next)
	{
		if (value->info.calc_okay)
		{
			label_calc_okay:
			if (!value->info.inner_size)
				goto label_fail;
			size += value->info.inner_size;
			if (!n) r->default_length = (uint32_t) value->info.inner_size;
			else if ((uint64_t) r->default_length != value->info.inner_size)
				r->default_length = 0;
			++n;
			continue;
		}
		if ((calc = value->atom->interface.calc) && calc(value) && value->info.calc_okay)
			goto label_calc_okay;
		label_fail:
		return NULL;
	}
	stuff->info.link_number = n;
	size += sizeof(mpeg4_full_box_suffix_t) + sizeof(mpeg4_box_type_t) + sizeof(uint32_t);
	if (r->fullbox.version == 1)
	{
		size += sizeof(uint32_t);
		if (!r->default_length)
			size += sizeof(uint32_t) * n;
	}
	if (r->fullbox.version == 2)
		size += sizeof(uint32_t);
	return mpeg4_stuff_calc_okay(stuff, size);
}

static mpeg4$define$build(sgpd)
{
	mpeg4_stuff__sample_group_description_t *restrict r = &((mpeg4_stuff__sample_group_description_s *) stuff)->pri;
	mpeg4_stuff_t *restrict value;
	mpeg4_build_f build;
	if (!stuff->info.calc_okay)
		goto label_fail;
	data = mpeg4$define(inner, fullbox, set)(data, &r->fullbox);
	data = mpeg4$define(inner, data, set)(data, &r->grouping_type, sizeof(r->grouping_type));
	if (r->fullbox.version == 1)
		data = mpeg4$define(inner, uint32_t, set)(data, r->default_length);
	if (r->fullbox.version == 2)
		data = mpeg4$define(inner, uint32_t, set)(data, r->default_sample_description_index);
	data = mpeg4$define(inner, uint32_t, set)(data, (uint32_t) stuff->info.link_number);
	for (value = stuff->container.list; value; value = value->link.next)
	{
		if (!value->info.calc_okay)
			goto label_fail;
		if (r->fullbox.version == 1 && !r->default_length)
			data = mpeg4$define(inner, uint32_t, set)(data, (uint32_t) value->info.inner_size);
		if (!(build = value->atom->interface.build))
			goto label_fail;
		if (!build(value, data))
			goto label_fail;
		data += value->info.inner_size;
	}
	return stuff;
	label_fail:
	return NULL;
}

static mpeg4$define$container_testing(sgpd)
{
	mpeg4_stuff__sample_group_description_t *restrict r = &((mpeg4_stuff__sample_group_description_s *) stuff)->pri;
	if (!r->grouping_type.v || r->grouping_type.v == type.v)
		return stuff;
	return NULL;
}

static mpeg4$define$container_update(sgpd)
{
	mpeg4_stuff__sample_group_description_t *restrict r = &((mpeg4_stuff__sample_group_description_s *) stuff)->pri;
	mpeg4_stuff_t *restrict first = stuff->container.list;
	if (first) r->grouping_type.v = first->info.type.v;
	else r->grouping_type.v = 0;
}

static inner_method_set_fullbox(sgpd, mpeg4_stuff__sample_group_description_s, pri.fullbox, 2);
static inner_method_get_fullbox(sgpd, mpeg4_stuff__sample_group_description_s, pri.fullbox);

static const mpeg4_stuff_t* mpeg4$define(stuff, sgpd, set$default_sample_description_index)(mpeg4_stuff__sample_group_description_s *restrict r, uint32_t default_sample_description_index)
{
	r->pri.default_sample_description_index = default_sample_description_index;
	return &r->stuff;
}

static const mpeg4$define$alloc(sgpd)
{
	mpeg4_atom_s *restrict r;
	r = mpeg4_atom_alloc_empty();
	if (r)
	{
		r->interface.dump = mpeg4$define(atom, sgpd, dump);
		r->interface.create = mpeg4$define(atom, sgpd, create);
		r->interface.parse = mpeg4$define(atom, sgpd, parse);
		r->interface.calc = mpeg4$define(atom, sgpd, calc);
		r->interface.build = mpeg4$define(atom, sgpd, build);
		r->interface.container_testing = mpeg4$define(atom, sgpd, container_testing);
		r->interface.container_update = mpeg4$define(atom, sgpd, container_update);
		if (
			mpeg4$inner$type$alloc$layer2(r, inst, sgpd_roll, "roll") &&
			mpeg4$stuff$method$set(r, sgpd, set$version_and_flags) &&
			mpeg4$stuff$method$set(r, sgpd, get$version_and_flags) &&
			mpeg4$stuff$method$set(r, sgpd, set$default_sample_description_index)
		) return r;
		refer_free(r);
	}
	return NULL;
}

mpeg4$define$find(sgpd)
{
	static const mpeg4_box_type_t type = { .c = "sgpd" };
	return mpeg4_find_atom(inst, mpeg4$define(atom, sgpd, alloc), type.v, 0);
}
