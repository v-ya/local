#include "box.include.h"
#include "inner.fullbox.h"
#include "inner.data.h"

typedef struct sample2chunk_item_t {
	uint32_t first_chunk;
	uint32_t samples_per_chunk;
	uint32_t sample_description_index;
} __attribute__ ((packed)) sample2chunk_item_t;

typedef struct mpeg4_stuff__sample2chunk_t {
	inner_fullbox_t fullbox;
	inner_array_t sample2chunk;
} mpeg4_stuff__sample2chunk_t;

typedef struct mpeg4_stuff__sample2chunk_s {
	mpeg4_stuff_t stuff;
	mpeg4_stuff__sample2chunk_t pri;
} mpeg4_stuff__sample2chunk_s;

static mpeg4$define$dump(stsc)
{
	inner_fullbox_t fullbox;
	const sample2chunk_item_t *item;
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
	item = (const sample2chunk_item_t *) data;
	level += mlog_level_width;
	while (entry_count && size >= sizeof(sample2chunk_item_t))
	{
		if (unidata->dump_samples)
			mlog_level_dump("first_chunk = %u, samples_per_chunk = %u, sample_description_index = %u\n",
				mpeg4_n32(item->first_chunk),
				mpeg4_n32(item->samples_per_chunk),
				mpeg4_n32(item->sample_description_index));
		++item;
		size -= sizeof(sample2chunk_item_t);
		--entry_count;
	}
	if (size || entry_count)
		goto label_fail;
	return atom;
	label_fail:
	return NULL;
}

static mpeg4$define$stuff$init(stsc, mpeg4_stuff__sample2chunk_s)
{
	mpeg4$define(inner, array, init)(&r->pri.sample2chunk, sizeof(sample2chunk_item_t));
	return &r->stuff;
}

static mpeg4$define$stuff$free(stsc, mpeg4_stuff__sample2chunk_s)
{
	mpeg4$define(inner, array, clear)(&r->pri.sample2chunk);
	mpeg4_stuff_free_default_func(&r->stuff);
}

static mpeg4$define$create(stsc)
{
	return mpeg4_stuff_alloc(atom, inst, type,
		sizeof(mpeg4_stuff__sample2chunk_s),
		(mpeg4_stuff_init_f) mpeg4$define(stuff, stsc, init),
		(refer_free_f) mpeg4$define(stuff, stsc, free));
}

static mpeg4$define$parse(stsc)
{

	inner_fullbox_t fullbox;
	const sample2chunk_item_t *restrict item;
	uint32_t entry_count;
	if (!mpeg4$define(inner, fullbox, get)(&fullbox, &data, &size))
		goto label_fail;
	if (!mpeg4$stuff$method$call(stuff, set$version_and_flags, fullbox.version, fullbox.flags))
		goto label_fail;
	if (!mpeg4$define(inner, uint32_t, get)(&entry_count, &data, &size))
		goto label_fail;
	if ((uint64_t) entry_count * sizeof(sample2chunk_item_t) != size)
		goto label_fail;
	item = (const sample2chunk_item_t *) data;
	while (entry_count)
	{
		if (!mpeg4$stuff$method$call(stuff, add$sample_to_chunk,
			mpeg4_n32(item->first_chunk),
			mpeg4_n32(item->samples_per_chunk),
			mpeg4_n32(item->sample_description_index)))
			goto label_fail;
		++item;
		--entry_count;
	}
	return stuff;
	label_fail:
	return NULL;
}

static mpeg4$define$calc(stsc)
{
	mpeg4_stuff__sample2chunk_t *restrict r = &((mpeg4_stuff__sample2chunk_s *) stuff)->pri;
	return mpeg4_stuff_calc_okay(stuff, sizeof(mpeg4_full_box_suffix_t) + sizeof(uint32_t) + sizeof(sample2chunk_item_t) * r->sample2chunk.number);
}

static mpeg4$define$build(stsc)
{
	mpeg4_stuff__sample2chunk_t *restrict r = &((mpeg4_stuff__sample2chunk_s *) stuff)->pri;
	const sample2chunk_item_t *restrict src;
	sample2chunk_item_t *restrict dst;
	uintptr_t i;
	data = mpeg4$define(inner, fullbox, set)(data, &r->fullbox);
	dst = (sample2chunk_item_t *) mpeg4$define(inner, uint32_t, set)(data, (uint32_t) (i = r->sample2chunk.number));
	src = (const sample2chunk_item_t *) r->sample2chunk.array;
	while (i)
	{
		dst->first_chunk = mpeg4_n32(src->first_chunk);
		dst->samples_per_chunk = mpeg4_n32(src->samples_per_chunk);
		dst->sample_description_index = mpeg4_n32(src->sample_description_index);
		++dst;
		++src;
		--i;
	}
	return stuff;
}

static inner_method_set_fullbox(stsc, mpeg4_stuff__sample2chunk_s, pri.fullbox, 0);
static inner_method_get_fullbox(stsc, mpeg4_stuff__sample2chunk_s, pri.fullbox);

static const mpeg4_stuff_t* mpeg4$define(stuff, stsc, add$sample_to_chunk)(mpeg4_stuff__sample2chunk_s *restrict r, uint32_t first_chunk, uint32_t samples_per_chunk, uint32_t sample_description_index)
{
	sample2chunk_item_t *restrict p;
	if ((p = (sample2chunk_item_t *) mpeg4$define(inner, array, append_point)(&r->pri.sample2chunk, 1)))
	{
		p->first_chunk = first_chunk;
		p->samples_per_chunk = samples_per_chunk;
		p->sample_description_index = sample_description_index;
		return &r->stuff;
	}
	return NULL;
}

static const mpeg4$define$alloc(stsc)
{
	mpeg4_atom_s *restrict r;
	r = mpeg4_atom_alloc_empty();
	if (r)
	{
		r->interface.dump = mpeg4$define(atom, stsc, dump);
		r->interface.create = mpeg4$define(atom, stsc, create);
		r->interface.parse = mpeg4$define(atom, stsc, parse);
		r->interface.calc = mpeg4$define(atom, stsc, calc);
		r->interface.build = mpeg4$define(atom, stsc, build);
		if (
			mpeg4$stuff$method$set(r, stsc, set$version_and_flags) &&
			mpeg4$stuff$method$set(r, stsc, get$version_and_flags) &&
			mpeg4$stuff$method$set(r, stsc, add$sample_to_chunk)
		) return r;
		refer_free(r);
	}
	return NULL;
}

mpeg4$define$find(stsc)
{
	static const mpeg4_box_type_t type = { .c = "stsc" };
	return mpeg4_find_atom(inst, mpeg4$define(atom, stsc, alloc), type.v, 0);
}
