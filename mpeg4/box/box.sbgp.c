#include "box.include.h"
#include "inner.fullbox.h"
#include "inner.type.h"
#include "inner.data.h"

typedef struct sample2group_item_t {
	uint32_t sample_count;
	uint32_t group_description_index;
} __attribute__ ((packed)) sample2group_item_t;

typedef struct mpeg4_stuff__sample2group_t {
	inner_fullbox_t fullbox;
	inner_array_t sample2group;
	mpeg4_box_type_t grouping_type;
	mpeg4_box_type_t grouping_type_parameter;
} mpeg4_stuff__sample2group_t;

typedef struct mpeg4_stuff__sample2group_s {
	mpeg4_stuff_t stuff;
	mpeg4_stuff__sample2group_t pri;
} mpeg4_stuff__sample2group_s;

static mpeg4$define$dump(sbgp)
{
	inner_fullbox_t fullbox;
	const sample2group_item_t *item;
	char type_string[16];
	mpeg4_box_type_t type;
	uint64_t all_sample_count;
	uint32_t entry_count, sample_count;
	uint32_t level = unidata->dump_level;
	if (!mpeg4$define(inner, fullbox, get)(&fullbox, &data, &size))
		goto label_fail;
	mpeg4$define(inner, fullbox, dump)(mlog, &fullbox, NULL, level);
	if (fullbox.version > 1)
		goto label_fail;
	if (!mpeg4$define(inner, data, get)(&type, sizeof(type), &data, &size))
		goto label_fail;
	mlog_level_dump("grouping type:           %s\n", mpeg4$define(inner, type, string)(type_string, type));
	if (fullbox.version == 1)
	{
		if (!mpeg4$define(inner, data, get)(&type, sizeof(type), &data, &size))
			goto label_fail;
		mlog_level_dump("grouping type parameter: %s\n", mpeg4$define(inner, type, string)(type_string, type));
	}
	if (!mpeg4$define(inner, uint32_t, get)(&entry_count, &data, &size))
		goto label_fail;
	mlog_level_dump("entry count:             %u\n", entry_count);
	item = (const sample2group_item_t *) data;
	level += mlog_level_width;
	all_sample_count = 0;
	while (entry_count && size >= sizeof(sample2group_item_t))
	{
		all_sample_count += (sample_count = mpeg4_n32(item->sample_count));
		mlog_level_dump("sample_count = %u, group_description_index = %u\n",
			sample_count,
			mpeg4_n32(item->group_description_index));
		++item;
		size -= sizeof(sample2group_item_t);
		--entry_count;
	}
	if (size || entry_count)
		goto label_fail;
	mlog_level_dump("all sample_count = %lu\n", all_sample_count);
	return atom;
	label_fail:
	return NULL;
}

static mpeg4$define$stuff$init(sbgp, mpeg4_stuff__sample2group_s)
{
	mpeg4$define(inner, array, init)(&r->pri.sample2group, sizeof(sample2group_item_t));
	return &r->stuff;
}

static mpeg4$define$stuff$free(sbgp, mpeg4_stuff__sample2group_s)
{
	mpeg4$define(inner, array, clear)(&r->pri.sample2group);
	mpeg4_stuff_free_default_func(&r->stuff);
}

static mpeg4$define$create(sbgp)
{
	return mpeg4_stuff_alloc(atom, inst, type,
		sizeof(mpeg4_stuff__sample2group_s),
		(mpeg4_stuff_init_f) mpeg4$define(stuff, sbgp, init),
		(refer_free_f) mpeg4$define(stuff, sbgp, free));
}

static mpeg4$define$parse(sbgp)
{
	inner_fullbox_t fullbox;
	const sample2group_item_t *item;
	mpeg4_box_type_t type;
	uint32_t entry_count;
	if (!mpeg4$define(inner, fullbox, get)(&fullbox, &data, &size))
		goto label_fail;
	if (!mpeg4$stuff$method$call(stuff, set$version_and_flags, fullbox.version, fullbox.flags))
		goto label_fail;
	if (!mpeg4$define(inner, data, get)(&type, sizeof(type), &data, &size))
		goto label_fail;
	if (!mpeg4$stuff$method$call(stuff, set$major_brand, type))
		goto label_fail;
	if (fullbox.version == 1)
	{
		if (!mpeg4$define(inner, data, get)(&type, sizeof(type), &data, &size))
			goto label_fail;
		if (!mpeg4$stuff$method$call(stuff, set$minor_brand, type))
			goto label_fail;
	}
	if (!mpeg4$define(inner, uint32_t, get)(&entry_count, &data, &size))
		goto label_fail;
	if ((uint64_t) entry_count * sizeof(sample2group_item_t) != size)
		goto label_fail;
	item = (const sample2group_item_t *) data;
	while (entry_count)
	{
		if (!mpeg4$stuff$method$call(stuff, add$sample_to_group,
			mpeg4_n32(item->sample_count),
			mpeg4_n32(item->group_description_index)))
			goto label_fail;
		++item;
		--entry_count;
	}
	return stuff;
	label_fail:
	return NULL;
}

static mpeg4$define$calc(sbgp)
{
	mpeg4_stuff__sample2group_t *restrict r = &((mpeg4_stuff__sample2group_s *) stuff)->pri;
	uint64_t size = sizeof(mpeg4_full_box_suffix_t) + sizeof(mpeg4_box_type_t) + sizeof(uint32_t);
	if (r->fullbox.version == 1)
		size += sizeof(mpeg4_box_type_t);
	size += sizeof(sample2group_item_t) * r->sample2group.number;
	return mpeg4_stuff_calc_okay(stuff, size);
}

static mpeg4$define$build(sbgp)
{
	mpeg4_stuff__sample2group_t *restrict r = &((mpeg4_stuff__sample2group_s *) stuff)->pri;
	const sample2group_item_t *restrict src;
	sample2group_item_t *restrict dst;
	uintptr_t i;
	data = mpeg4$define(inner, fullbox, set)(data, &r->fullbox);
	data = mpeg4$define(inner, data, set)(data, &r->grouping_type, sizeof(r->grouping_type));
	if (r->fullbox.version == 1)
		data = mpeg4$define(inner, data, set)(data, &r->grouping_type_parameter, sizeof(r->grouping_type_parameter));
	dst = (sample2group_item_t *) mpeg4$define(inner, uint32_t, set)(data, (uint32_t) (i = r->sample2group.number));
	src = (const sample2group_item_t *) r->sample2group.array;
	while (i)
	{
		dst->sample_count = mpeg4_n32(src->sample_count);
		dst->group_description_index = mpeg4_n32(src->group_description_index);
		++dst;
		++src;
		--i;
	}
	return stuff;
}

static inner_method_set_fullbox(sbgp, mpeg4_stuff__sample2group_s, pri.fullbox, 1);
static inner_method_get_fullbox(sbgp, mpeg4_stuff__sample2group_s, pri.fullbox);

static const mpeg4_stuff_t* mpeg4$define(stuff, sbgp, set$major_brand)(mpeg4_stuff__sample2group_s *restrict r, mpeg4_box_type_t major_brand)
{
	r->pri.grouping_type = major_brand;
	return &r->stuff;
}

static const mpeg4_stuff_t* mpeg4$define(stuff, sbgp, set$minor_brand)(mpeg4_stuff__sample2group_s *restrict r, mpeg4_box_type_t minor_brand)
{
	r->pri.grouping_type_parameter = minor_brand;
	return &r->stuff;
}

static const mpeg4_stuff_t* mpeg4$define(stuff, sbgp, add$sample_to_group)(mpeg4_stuff__sample2group_s *restrict r, uint32_t sample_count, uint32_t group_description_index)
{
	sample2group_item_t *restrict p;
	uintptr_t n;
	if ((n = r->pri.sample2group.number) &&
		(p = (sample2group_item_t *) r->pri.sample2group.array + (n - 1))->group_description_index == group_description_index)
		goto label_append;
	if ((p = (sample2group_item_t *) mpeg4$define(inner, array, append_point)(&r->pri.sample2group, 1)))
	{
		p->sample_count = sample_count;
		p->group_description_index = group_description_index;
		return &r->stuff;
		label_append:
		p->sample_count += sample_count;
		return &r->stuff;
	}
	return NULL;
}

static const mpeg4$define$alloc(sbgp)
{
	mpeg4_atom_s *restrict r;
	r = mpeg4_atom_alloc_empty();
	if (r)
	{
		r->interface.dump = mpeg4$define(atom, sbgp, dump);
		r->interface.create = mpeg4$define(atom, sbgp, create);
		r->interface.parse = mpeg4$define(atom, sbgp, parse);
		r->interface.calc = mpeg4$define(atom, sbgp, calc);
		r->interface.build = mpeg4$define(atom, sbgp, build);
		if (
			mpeg4$stuff$method$set(r, sbgp, set$version_and_flags) &&
			mpeg4$stuff$method$set(r, sbgp, get$version_and_flags) &&
			mpeg4$stuff$method$set(r, sbgp, set$major_brand) &&
			mpeg4$stuff$method$set(r, sbgp, set$minor_brand) &&
			mpeg4$stuff$method$set(r, sbgp, add$sample_to_group)
		) return r;
		refer_free(r);
	}
	return NULL;
}

mpeg4$define$find(sbgp)
{
	static const mpeg4_box_type_t type = { .c = "sbgp" };
	return mpeg4_find_atom(inst, mpeg4$define(atom, sbgp, alloc), type.v, 0);
}
