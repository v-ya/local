#include "box.include.h"
#include "inner.fullbox.h"
#include "inner.data.h"

typedef struct mpeg4_stuff__sync_sample_t {
	inner_fullbox_t fullbox;
	inner_array_t sync_sample;
} mpeg4_stuff__sync_sample_t;

typedef struct mpeg4_stuff__sync_sample_s {
	mpeg4_stuff_t stuff;
	mpeg4_stuff__sync_sample_t pri;
} mpeg4_stuff__sync_sample_s;

static mpeg4$define$dump(stss)
{
	inner_fullbox_t fullbox;
	const uint32_t *number;
	uint32_t entry_count;
	uint32_t level = unidata->dump_level;
	if (!mpeg4$define(inner, fullbox, get)(&fullbox, &data, &size))
		goto label_fail;
	mpeg4$define(inner, fullbox, dump)(mlog, &fullbox, NULL, level);
	if (fullbox.version)
		goto label_fail;
	if (!mpeg4$define(inner, uint32_t, get)(&entry_count, &data, &size))
		goto label_fail;
	mlog_level_dump("entry count: %u\n", entry_count);
	number = (const uint32_t *) data;
	level += mlog_level_width;
	while (entry_count && size >= sizeof(uint32_t))
	{
		if (unidata->dump_samples)
			mlog_level_dump("sample_number = %u\n",
				mpeg4_n32(*number));
		++number;
		size -= sizeof(uint32_t);
		--entry_count;
	}
	if (size || entry_count)
		goto label_fail;
	return atom;
	label_fail:
	return NULL;
}

static mpeg4$define$stuff$init(stss, mpeg4_stuff__sync_sample_s)
{
	mpeg4$define(inner, array, init)(&r->pri.sync_sample, sizeof(uint32_t));
	return &r->stuff;
}

static mpeg4$define$stuff$free(stss, mpeg4_stuff__sync_sample_s)
{
	mpeg4$define(inner, array, clear)(&r->pri.sync_sample);
	mpeg4_stuff_free_default_func(&r->stuff);
}

static mpeg4$define$create(stss)
{
	return mpeg4_stuff_alloc(atom, inst, type,
		sizeof(mpeg4_stuff__sync_sample_s),
		(mpeg4_stuff_init_f) mpeg4$define(stuff, stss, init),
		(refer_free_f) mpeg4$define(stuff, stss, free));
}

static mpeg4$define$parse(stss)
{
	inner_fullbox_t fullbox;
	const uint32_t *sync_sample;
	uint32_t entry_count;
	if (!mpeg4$define(inner, fullbox, get)(&fullbox, &data, &size))
		goto label_fail;
	if (!mpeg4$stuff$method$call(stuff, set$version_and_flags, fullbox.version, fullbox.flags))
		goto label_fail;
	if (!mpeg4$define(inner, uint32_t, get)(&entry_count, &data, &size))
		goto label_fail;
	if ((uint64_t) entry_count * sizeof(uint32_t) != size)
		goto label_fail;
	sync_sample = (const uint32_t *) data;
	while (entry_count)
	{
		if (!mpeg4$stuff$method$call(stuff, add$sync_sample,
			mpeg4_n32(*sync_sample++)))
			goto label_fail;
		--entry_count;
	}
	return stuff;
	label_fail:
	return NULL;
}

static mpeg4$define$calc(stss)
{
	mpeg4_stuff__sync_sample_t *restrict r = &((mpeg4_stuff__sync_sample_s *) stuff)->pri;
	return mpeg4_stuff_calc_okay(stuff, sizeof(mpeg4_full_box_suffix_t) + sizeof(uint32_t) + sizeof(uint32_t) * r->sync_sample.number);
}

static mpeg4$define$build(stss)
{
	mpeg4_stuff__sync_sample_t *restrict r = &((mpeg4_stuff__sync_sample_s *) stuff)->pri;
	const uint32_t *restrict src;
	uint32_t *restrict dst;
	uintptr_t i;
	data = mpeg4$define(inner, fullbox, set)(data, &r->fullbox);
	dst = (uint32_t *) mpeg4$define(inner, uint32_t, set)(data, (uint32_t) (i = r->sync_sample.number));
	src = (const uint32_t *) r->sync_sample.array;
	while (i)
	{
		*dst++ = mpeg4_n32(*src++);
		--i;
	}
	return stuff;
}

static const mpeg4_stuff_t* mpeg4$define(stuff, stss, set$version_and_flags)(mpeg4_stuff__sync_sample_s *restrict r, uint32_t version, uint32_t flags)
{
	if (version) goto label_fail;
	r->pri.fullbox.version = version;
	r->pri.fullbox.flags = flags;
	return &r->stuff;
	label_fail:
	return NULL;
}

static const mpeg4_stuff_t* mpeg4$define(stuff, stss, add$sync_sample)(mpeg4_stuff__sync_sample_s *restrict r, uint32_t sample_id)
{
	uint32_t *restrict p;
	if ((p = (uint32_t *) mpeg4$define(inner, array, append_point)(&r->pri.sync_sample, 1)))
	{
		*p = sample_id;
		return &r->stuff;
	}
	return NULL;
}

static const mpeg4$define$alloc(stss)
{
	mpeg4_atom_s *restrict r;
	r = mpeg4_atom_alloc_empty();
	if (r)
	{
		r->interface.dump = mpeg4$define(atom, stss, dump);
		r->interface.create = mpeg4$define(atom, stss, create);
		r->interface.parse = mpeg4$define(atom, stss, parse);
		r->interface.calc = mpeg4$define(atom, stss, calc);
		r->interface.build = mpeg4$define(atom, stss, build);
		if (
			mpeg4$stuff$method$set(r, stss, set$version_and_flags) &&
			mpeg4$stuff$method$set(r, stss, add$sync_sample)
		) return r;
		refer_free(r);
	}
	return NULL;
}

mpeg4$define$find(stss)
{
	static const mpeg4_box_type_t type = { .c = "stss" };
	return mpeg4_find_atom(inst, mpeg4$define(atom, stss, alloc), type.v, 0);
}
