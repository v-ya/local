#include "box.include.h"
#include "inner.fullbox.h"
#include "inner.data.h"

typedef struct sample_size_t {
	uint32_t sample_size;
	uint32_t sample_count;
} __attribute__ ((packed)) sample_size_t;

typedef struct mpeg4_stuff__sample_size_t {
	inner_fullbox_t fullbox;
	inner_array_t samples;
	uint32_t sample_size;
	uint32_t sample_count;
} mpeg4_stuff__sample_size_t;

typedef struct mpeg4_stuff__sample_size_s {
	mpeg4_stuff_t stuff;
	mpeg4_stuff__sample_size_t pri;
} mpeg4_stuff__sample_size_s;

static mpeg4$define$dump(stsz)
{
	inner_fullbox_t fullbox;
	const uint32_t *entry_size;
	uint64_t all_sample_size;
	sample_size_t sample;
	uint32_t sample_size, i;
	uint32_t level = unidata->dump_level;
	if (!mpeg4$define(inner, fullbox, get)(&fullbox, &data, &size))
		goto label_fail;
	mpeg4$define(inner, fullbox, dump)(mlog, &fullbox, NULL, level);
	if (fullbox.version)
		goto label_fail;
	if (!mpeg4$define(inner, data, get)(&sample, sizeof(sample), &data, &size))
		goto label_fail;
	sample.sample_size = mpeg4_n32(sample.sample_size);
	sample.sample_count = mpeg4_n32(sample.sample_count);
	mlog_level_dump("sample_size:  %u\n", sample.sample_size);
	mlog_level_dump("sample_count: %u\n", sample.sample_count);
	entry_size = (const uint32_t *) data;
	level += mlog_level_width;
	all_sample_size = 0;
	if (!sample.sample_size)
	{
		for (i = 0; i < sample.sample_count && size >= sizeof(uint32_t); ++i)
		{
			all_sample_size += (sample_size = mpeg4_n32(*entry_size));
			if (unidata->dump_samples)
				mlog_level_dump("(%6u) = %u\n", i + 1, sample_size);
			++entry_size;
			size -= sizeof(uint32_t);
		}
		if (i < sample.sample_count)
			goto label_fail;
	}
	else all_sample_size = sample.sample_size * sample.sample_count;
	if (size) goto label_fail;
	mlog_level_dump("all sample_size: %lu\n", all_sample_size);
	return atom;
	label_fail:
	return NULL;
}

static mpeg4$define$stuff$init(stsz, mpeg4_stuff__sample_size_s)
{
	mpeg4$define(inner, array, init)(&r->pri.samples, sizeof(uint32_t));
	return &r->stuff;
}

static mpeg4$define$stuff$free(stsz, mpeg4_stuff__sample_size_s)
{
	mpeg4$define(inner, array, clear)(&r->pri.samples);
	mpeg4_stuff_free_default_func(&r->stuff);
}

static mpeg4$define$create(stsz)
{
	return mpeg4_stuff_alloc(atom, inst, type,
		sizeof(mpeg4_stuff__sample_size_s),
		(mpeg4_stuff_init_f) mpeg4$define(stuff, stsz, init),
		(refer_free_f) mpeg4$define(stuff, stsz, free));
}

static mpeg4$define$parse(stsz)
{

	inner_fullbox_t fullbox;
	const uint32_t *entry_size;
	sample_size_t sample;
	uint32_t i;
	if (!mpeg4$define(inner, fullbox, get)(&fullbox, &data, &size))
		goto label_fail;
	if (!mpeg4$stuff$method$call(stuff, set$version_and_flags, fullbox.version, fullbox.flags))
		goto label_fail;
	if (!mpeg4$define(inner, data, get)(&sample, sizeof(sample), &data, &size))
		goto label_fail;
	sample.sample_size = mpeg4_n32(sample.sample_size);
	sample.sample_count = mpeg4_n32(sample.sample_count);
	entry_size = (const uint32_t *) data;
	if (!sample.sample_size)
	{
		if ((uint64_t) sample.sample_count * sizeof(uint32_t) != size)
			goto label_fail;
		i = sample.sample_count;
		while (i)
		{
			if (!mpeg4$stuff$method$call(stuff, add$sample_size, mpeg4_n32(*entry_size++), NULL))
				goto label_fail;
			--i;
		}
	}
	else
	{
		if (size) goto label_fail;
		if (!mpeg4$stuff$method$call(stuff, set$sample_count, sample.sample_size, sample.sample_count))
			goto label_fail;
	}
	return stuff;
	label_fail:
	return NULL;
}

static mpeg4$define$calc(stsz)
{
	mpeg4_stuff__sample_size_t *restrict r = &((mpeg4_stuff__sample_size_s *) stuff)->pri;
	return mpeg4_stuff_calc_okay(stuff, r->sample_size?
		(sizeof(mpeg4_full_box_suffix_t) + sizeof(sample_size_t)):
		(sizeof(mpeg4_full_box_suffix_t) + sizeof(sample_size_t) + sizeof(uint32_t) * r->samples.number));
}

static mpeg4$define$build(stsz)
{
	mpeg4_stuff__sample_size_t *restrict r = &((mpeg4_stuff__sample_size_s *) stuff)->pri;
	const uint32_t *restrict src;
	uint32_t *restrict dst;
	uintptr_t i;
	data = mpeg4$define(inner, fullbox, set)(data, &r->fullbox);
	if (!r->sample_size)
	{
		data = mpeg4$define(inner, uint32_t, set)(data, 0);
		dst = (uint32_t *) mpeg4$define(inner, uint32_t, set)(data, (uint32_t) (i = r->samples.number));
		src = (const uint32_t *) r->samples.array;
		while (i)
		{
			*dst++ = mpeg4_n32(*src++);
			--i;
		}
	}
	else
	{
		data = mpeg4$define(inner, uint32_t, set)(data, r->sample_size);
		mpeg4$define(inner, uint32_t, set)(data, r->sample_count);
	}
	return stuff;
}

static inner_method_set_fullbox(stsz, mpeg4_stuff__sample_size_s, pri.fullbox, 0);
static inner_method_get_fullbox(stsz, mpeg4_stuff__sample_size_s, pri.fullbox);

static const mpeg4_stuff_t* mpeg4$define(stuff, stsz, set$sample_count)(mpeg4_stuff__sample_size_s *restrict r, uint32_t sample_size, uint32_t sample_count)
{
	r->pri.sample_size = sample_size;
	r->pri.sample_count = sample_count;
	return &r->stuff;
}

static const mpeg4_stuff_t* mpeg4$define(stuff, stsz, add$sample_size)(mpeg4_stuff__sample_size_s *restrict r, uint32_t sample_size, uint32_t *restrict sample_id)
{
	uint32_t *restrict p;
	if (sample_id) *sample_id = (uint32_t) (r->pri.samples.number + 1);
	if ((p = (uint32_t *) mpeg4$define(inner, array, append_point)(&r->pri.samples, 1)))
	{
		*p = sample_size;
		return &r->stuff;
	}
	return NULL;
}

static const mpeg4$define$alloc(stsz)
{
	mpeg4_atom_s *restrict r;
	r = mpeg4_atom_alloc_empty();
	if (r)
	{
		r->interface.dump = mpeg4$define(atom, stsz, dump);
		r->interface.create = mpeg4$define(atom, stsz, create);
		r->interface.parse = mpeg4$define(atom, stsz, parse);
		r->interface.calc = mpeg4$define(atom, stsz, calc);
		r->interface.build = mpeg4$define(atom, stsz, build);
		if (
			mpeg4$stuff$method$set(r, stsz, set$version_and_flags) &&
			mpeg4$stuff$method$set(r, stsz, get$version_and_flags) &&
			mpeg4$stuff$method$set(r, stsz, set$sample_count) &&
			mpeg4$stuff$method$set(r, stsz, add$sample_size)
		) return r;
		refer_free(r);
	}
	return NULL;
}

mpeg4$define$find(stsz)
{
	static const mpeg4_box_type_t type = { .c = "stsz" };
	return mpeg4_find_atom(inst, mpeg4$define(atom, stsz, alloc), type.v, 0);
}
