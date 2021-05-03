#include "box.include.h"
#include "inner.fullbox.h"
#include "inner.data.h"

typedef struct time2sample_t {
	uint32_t sample_count;
	uint32_t sample_delta;
} __attribute__ ((packed)) time2sample_t;

typedef struct mpeg4_stuff__time2sample_t {
	inner_fullbox_t fullbox;
	inner_array_t time2sample;
} mpeg4_stuff__time2sample_t;

typedef struct mpeg4_stuff__time2sample_s {
	mpeg4_stuff_t stuff;
	mpeg4_stuff__time2sample_t pri;
} mpeg4_stuff__time2sample_s;

static mpeg4$define$dump(stts)
{
	inner_fullbox_t fullbox;
	const time2sample_t *item;
	uint64_t all_sample_count;
	uint32_t entry_count, sample_count;
	uint32_t level = unidata->dump_level;
	if (!mpeg4$define(inner, fullbox, get)(&fullbox, &data, &size))
		goto label_fail;
	mpeg4$define(inner, fullbox, dump)(mlog, &fullbox, NULL, level);
	if (fullbox.version)
		goto label_fail;
	if (!mpeg4$define(inner, uint32_t, get)(&entry_count, &data, &size))
		goto label_fail;
	mlog_level_dump("entry count: %u\n", entry_count);
	item = (const time2sample_t *) data;
	level += mlog_level_width;
	all_sample_count = 0;
	while (entry_count && size >= sizeof(time2sample_t))
	{
		all_sample_count += (sample_count = mpeg4_n32(item->sample_count));
		if (unidata->dump_samples)
			mlog_level_dump("sample_count = %u, sample_delta = %u\n",
				sample_count,
				mpeg4_n32(item->sample_delta));
		++item;
		size -= sizeof(time2sample_t);
		--entry_count;
	}
	if (size || entry_count)
		goto label_fail;
	mlog_level_dump("all sample_count = %lu\n", all_sample_count);
	return atom;
	label_fail:
	return NULL;
}

static mpeg4$define$stuff$init(stts, mpeg4_stuff__time2sample_s)
{
	mpeg4$define(inner, array, init)(&r->pri.time2sample, sizeof(time2sample_t));
	return &r->stuff;
}

static mpeg4$define$stuff$free(stts, mpeg4_stuff__time2sample_s)
{
	mpeg4$define(inner, array, clear)(&r->pri.time2sample);
	mpeg4_stuff_free_default_func(&r->stuff);
}

static mpeg4$define$create(stts)
{
	return mpeg4_stuff_alloc(atom, inst, type,
		sizeof(mpeg4_stuff__time2sample_s),
		(mpeg4_stuff_init_f) mpeg4$define(stuff, stts, init),
		(refer_free_f) mpeg4$define(stuff, stts, free));
}

static mpeg4$define$parse(stts)
{
	inner_fullbox_t fullbox;
	const time2sample_t *item;
	uint32_t entry_count;
	if (!mpeg4$define(inner, fullbox, get)(&fullbox, &data, &size))
		goto label_fail;
	if (!mpeg4$stuff$method$call(stuff, set$version_and_flags, fullbox.version, fullbox.flags))
		goto label_fail;
	if (!mpeg4$define(inner, uint32_t, get)(&entry_count, &data, &size))
		goto label_fail;
	if ((uint64_t) entry_count * sizeof(time2sample_t) != size)
		goto label_fail;
	item = (const time2sample_t *) data;
	while (entry_count)
	{
		if (!mpeg4$stuff$method$call(stuff, add$time_to_sample,
			mpeg4_n32(item->sample_count),
			mpeg4_n32(item->sample_delta)))
			goto label_fail;
		++item;
		--entry_count;
	}
	return stuff;
	label_fail:
	return NULL;
}

static mpeg4$define$calc(stts)
{
	mpeg4_stuff__time2sample_t *restrict r = &((mpeg4_stuff__time2sample_s *) stuff)->pri;
	return mpeg4_stuff_calc_okay(stuff, sizeof(mpeg4_full_box_suffix_t) + sizeof(uint32_t) + sizeof(time2sample_t) * r->time2sample.number);
}

static mpeg4$define$build(stts)
{
	mpeg4_stuff__time2sample_t *restrict r = &((mpeg4_stuff__time2sample_s *) stuff)->pri;
	const time2sample_t *restrict src;
	time2sample_t *restrict dst;
	uintptr_t i;
	data = mpeg4$define(inner, fullbox, set)(data, &r->fullbox);
	dst = (time2sample_t *) mpeg4$define(inner, uint32_t, set)(data, (uint32_t) (i = r->time2sample.number));
	src = (const time2sample_t *) r->time2sample.array;
	while (i)
	{
		dst->sample_count = mpeg4_n32(src->sample_count);
		dst->sample_delta = mpeg4_n32(src->sample_delta);
		++dst;
		++src;
		--i;
	}
	return stuff;
}

static inner_method_set_fullbox(stts, mpeg4_stuff__time2sample_s, pri.fullbox, 0);
static inner_method_get_fullbox(stts, mpeg4_stuff__time2sample_s, pri.fullbox);

static const mpeg4_stuff_t* mpeg4$define(stuff, stts, add$time_to_sample)(mpeg4_stuff__time2sample_s *restrict r, uint32_t sample_count, uint32_t sample_delta)
{
	time2sample_t *restrict p;
	uintptr_t n;
	if ((n = r->pri.time2sample.number) &&
		(p = (time2sample_t *) r->pri.time2sample.array + (n - 1))->sample_delta == sample_delta)
		goto label_append;
	if ((p = (time2sample_t *) mpeg4$define(inner, array, append_point)(&r->pri.time2sample, 1)))
	{
		p->sample_count = sample_count;
		p->sample_delta = sample_delta;
		return &r->stuff;
		label_append:
		p->sample_count += sample_count;
		return &r->stuff;
	}
	return NULL;
}

static const mpeg4$define$alloc(stts)
{
	mpeg4_atom_s *restrict r;
	r = mpeg4_atom_alloc_empty();
	if (r)
	{
		r->interface.dump = mpeg4$define(atom, stts, dump);
		r->interface.create = mpeg4$define(atom, stts, create);
		r->interface.parse = mpeg4$define(atom, stts, parse);
		r->interface.calc = mpeg4$define(atom, stts, calc);
		r->interface.build = mpeg4$define(atom, stts, build);
		if (
			mpeg4$stuff$method$set(r, stts, set$version_and_flags) &&
			mpeg4$stuff$method$set(r, stts, get$version_and_flags) &&
			mpeg4$stuff$method$set(r, stts, add$time_to_sample)
		) return r;
		refer_free(r);
	}
	return NULL;
}

mpeg4$define$find(stts)
{
	static const mpeg4_box_type_t type = { .c = "stts" };
	return mpeg4_find_atom(inst, mpeg4$define(atom, stts, alloc), type.v, 0);
}
