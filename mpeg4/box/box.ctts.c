#include "box.include.h"
#include "inner.fullbox.h"
#include "inner.data.h"

typedef struct composition_offset_t {
	uint32_t sample_count;
	union {
		uint32_t v0;
		int32_t v1;
	} sample_offset;
} __attribute__ ((packed)) composition_offset_t;

typedef struct mpeg4_stuff__composition_offset_t {
	inner_fullbox_t fullbox;
	inner_array_t composition_offset;
} mpeg4_stuff__composition_offset_t;

typedef struct mpeg4_stuff__composition_offset_s {
	mpeg4_stuff_t stuff;
	mpeg4_stuff__composition_offset_t pri;
} mpeg4_stuff__composition_offset_s;

static mpeg4$define$dump(ctts)
{
	inner_fullbox_t fullbox;
	const composition_offset_t *offset;
	uint64_t all_sample_count;
	uint32_t entry_count, sample_count;
	uint32_t level = unidata->dump_level;
	if (!mpeg4$define(inner, fullbox, get)(&fullbox, &data, &size))
		goto label_fail;
	mpeg4$define(inner, fullbox, dump)(mlog, &fullbox, NULL, level);
	if (fullbox.version > 1)
		goto label_fail;
	if (!mpeg4$define(inner, uint32_t, get)(&entry_count, &data, &size))
		goto label_fail;
	mlog_level_dump("entry count: %u\n", entry_count);
	offset = (const composition_offset_t *) data;
	level += mlog_level_width;
	all_sample_count = 0;
	while (entry_count && size >= sizeof(composition_offset_t))
	{
		all_sample_count += (sample_count = mpeg4_n32(offset->sample_count));
		if (unidata->dump_samples)
		{
			if (!fullbox.version) mlog_level_dump("sample_count = %u, sample_offset = %u\n",
				sample_count,
				mpeg4_n32(offset->sample_offset.v0));
			else mlog_level_dump("sample_count = %u, sample_offset = %d\n",
				sample_count,
				mpeg4_n32(offset->sample_offset.v1));
		}
		++offset;
		size -= sizeof(composition_offset_t);
		--entry_count;
	}
	if (size || entry_count)
		goto label_fail;
	mlog_level_dump("all sample_count = %lu\n", all_sample_count);
	return atom;
	label_fail:
	return NULL;
}

static mpeg4$define$stuff$init(ctts, mpeg4_stuff__composition_offset_s)
{
	mpeg4$define(inner, array, init)(&r->pri.composition_offset, sizeof(composition_offset_t));
	return &r->stuff;
}

static mpeg4$define$stuff$free(ctts, mpeg4_stuff__composition_offset_s)
{
	mpeg4$define(inner, array, clear)(&r->pri.composition_offset);
	mpeg4_stuff_free_default_func(&r->stuff);
}

static mpeg4$define$create(ctts)
{
	return mpeg4_stuff_alloc(atom, inst, type,
		sizeof(mpeg4_stuff__composition_offset_s),
		(mpeg4_stuff_init_f) mpeg4$define(stuff, ctts, init),
		(refer_free_f) mpeg4$define(stuff, ctts, free));
}

static mpeg4$define$parse(ctts)
{
	inner_fullbox_t fullbox;
	const composition_offset_t *item;
	uint32_t entry_count;
	if (!mpeg4$define(inner, fullbox, get)(&fullbox, &data, &size))
		goto label_fail;
	if (!mpeg4$stuff$method$call(stuff, set$version_and_flags, fullbox.version, fullbox.flags))
		goto label_fail;
	if (!mpeg4$define(inner, uint32_t, get)(&entry_count, &data, &size))
		goto label_fail;
	if ((uint64_t) entry_count * sizeof(composition_offset_t) != size)
		goto label_fail;
	item = (const composition_offset_t *) data;
	while (entry_count)
	{
		if (!mpeg4$stuff$method$call(stuff, add$composition_offset,
			mpeg4_n32(item->sample_count),
			mpeg4_n32(item->sample_offset.v1)))
			goto label_fail;
		++item;
		--entry_count;
	}
	return stuff;
	label_fail:
	return NULL;
}

static mpeg4$define$calc(ctts)
{
	mpeg4_stuff__composition_offset_t *restrict r = &((mpeg4_stuff__composition_offset_s *) stuff)->pri;
	return mpeg4_stuff_calc_okay(stuff, sizeof(mpeg4_full_box_suffix_t) + sizeof(uint32_t) + sizeof(composition_offset_t) * r->composition_offset.number);
}

static mpeg4$define$build(ctts)
{
	mpeg4_stuff__composition_offset_t *restrict r = &((mpeg4_stuff__composition_offset_s *) stuff)->pri;
	const composition_offset_t *restrict src;
	composition_offset_t *restrict dst;
	uintptr_t i;
	data = mpeg4$define(inner, fullbox, set)(data, &r->fullbox);
	dst = (composition_offset_t *) mpeg4$define(inner, uint32_t, set)(data, (uint32_t) (i = r->composition_offset.number));
	src = (const composition_offset_t *) r->composition_offset.array;
	while (i)
	{
		dst->sample_count = mpeg4_n32(src->sample_count);
		dst->sample_offset.v0 = mpeg4_n32(src->sample_offset.v0);
		++dst;
		++src;
		--i;
	}
	return stuff;
}

static const mpeg4_stuff_t* mpeg4$define(stuff, ctts, set$version_and_flags)(mpeg4_stuff__composition_offset_s *restrict r, uint32_t version, uint32_t flags)
{
	if (version > 1) goto label_fail;
	r->pri.fullbox.version = version;
	r->pri.fullbox.flags = flags;
	return &r->stuff;
	label_fail:
	return NULL;
}

static const mpeg4_stuff_t* mpeg4$define(stuff, ctts, add$composition_offset)(mpeg4_stuff__composition_offset_s *restrict r, uint32_t sample_count, int32_t sample_offset)
{
	composition_offset_t *restrict p;
	uintptr_t n;
	if ((n = r->pri.composition_offset.number) &&
		(p = (composition_offset_t *) r->pri.composition_offset.array + (n - 1))->sample_offset.v1 == sample_offset)
		goto label_append;
	if ((p = (composition_offset_t *) mpeg4$define(inner, array, append_point)(&r->pri.composition_offset, 1)))
	{
		p->sample_count = sample_count;
		p->sample_offset.v1 = sample_offset;
		return &r->stuff;
		label_append:
		p->sample_count += sample_count;
		return &r->stuff;
	}
	return NULL;
}

static const mpeg4$define$alloc(ctts)
{
	mpeg4_atom_s *restrict r;
	r = mpeg4_atom_alloc_empty();
	if (r)
	{
		r->interface.dump = mpeg4$define(atom, ctts, dump);
		r->interface.create = mpeg4$define(atom, ctts, create);
		r->interface.parse = mpeg4$define(atom, ctts, parse);
		r->interface.calc = mpeg4$define(atom, ctts, calc);
		r->interface.build = mpeg4$define(atom, ctts, build);
		if (
			mpeg4$stuff$method$set(r, ctts, set$version_and_flags) &&
			mpeg4$stuff$method$set(r, ctts, add$composition_offset)
		) return r;
		refer_free(r);
	}
	return NULL;
}

mpeg4$define$find(ctts)
{
	static const mpeg4_box_type_t type = { .c = "ctts" };
	return mpeg4_find_atom(inst, mpeg4$define(atom, ctts, alloc), type.v, 0);
}
