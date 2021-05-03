#include "box.include.h"
#include "inner.fullbox.h"
#include "inner.timespec.h"
#include "inner.data.h"

typedef struct edit_list_time32_t {
	uint32_t segment_duration;
	int32_t media_time;
} __attribute__ ((packed)) edit_list_time32_t;

typedef struct edit_list_time64_t {
	uint64_t segment_duration;
	int64_t media_time;
} __attribute__ ((packed)) edit_list_time64_t;

typedef struct edit_list_rate_t {
	uint16_t media_rate_integer;
	uint16_t media_rate_fraction;
} __attribute__ ((packed)) edit_list_rate_t;

typedef struct edit_list_t {
	uint64_t segment_duration;
	int64_t media_time;
	uint16_t media_rate_integer;
	uint16_t media_rate_fraction;
} edit_list_t;

typedef struct mpeg4_stuff__edit_list_t {
	inner_fullbox_t fullbox;
	inner_array_t edit_list;
} mpeg4_stuff__edit_list_t;

typedef struct mpeg4_stuff__edit_list_s {
	mpeg4_stuff_t stuff;
	mpeg4_stuff__edit_list_t pri;
} mpeg4_stuff__edit_list_s;

static inline edit_list_t* mpeg4$define(inner, edit_list_rate, get)(edit_list_t *restrict r, const uint8_t *restrict *restrict data, uint64_t *restrict size)
{
	edit_list_rate_t rate;
	if (mpeg4$define(inner, data, get)(&rate, sizeof(rate), data, size))
	{
		r->media_rate_integer = mpeg4_n16(rate.media_rate_integer);
		r->media_rate_fraction = mpeg4_n16(rate.media_rate_fraction);
		return r;
	}
	return NULL;
}

static inline uint8_t* mpeg4$define(inner, edit_list_rate, set)(uint8_t *restrict data, edit_list_t *restrict r)
{
	edit_list_rate_t rate;
	rate.media_rate_integer = mpeg4_n16((uint16_t) r->media_rate_integer);
	rate.media_rate_fraction = mpeg4_n16((uint16_t) r->media_rate_fraction);
	return mpeg4$define(inner, data, set)(data, &rate, sizeof(rate));
}

static inline edit_list_t* mpeg4$define(inner, edit_list_32, get)(edit_list_t *restrict r, const uint8_t *restrict *restrict data, uint64_t *restrict size)
{
	edit_list_time32_t t;
	if (mpeg4$define(inner, data, get)(&t, sizeof(t), data, size))
	{
		r->segment_duration = mpeg4_n32(t.segment_duration);
		r->media_time = mpeg4_n32(t.media_time);
		return mpeg4$define(inner, edit_list_rate, get)(r, data, size);
	}
	return NULL;
}

static inline uint8_t* mpeg4$define(inner, edit_list_32, set)(uint8_t *restrict data, edit_list_t *restrict r)
{
	edit_list_time32_t t;
	t.segment_duration = mpeg4_n32((uint32_t) r->segment_duration);
	t.media_time = mpeg4_n32((int32_t) r->media_time);
	return mpeg4$define(inner, edit_list_rate, set)(mpeg4$define(inner, data, set)(data, &t, sizeof(t)), r);
}

static inline edit_list_t* mpeg4$define(inner, edit_list_64, get)(edit_list_t *restrict r, const uint8_t *restrict *restrict data, uint64_t *restrict size)
{
	edit_list_time64_t t;
	if (mpeg4$define(inner, data, get)(&t, sizeof(t), data, size))
	{
		r->segment_duration = mpeg4_n64(t.segment_duration);
		r->media_time = mpeg4_n64(t.media_time);
		return mpeg4$define(inner, edit_list_rate, get)(r, data, size);
	}
	return NULL;
}

static inline uint8_t* mpeg4$define(inner, edit_list_64, set)(uint8_t *restrict data, edit_list_t *restrict r)
{
	edit_list_time64_t t;
	t.segment_duration = mpeg4_n64(r->segment_duration);
	t.media_time = mpeg4_n64(r->media_time);
	return mpeg4$define(inner, edit_list_rate, set)(mpeg4$define(inner, data, set)(data, &t, sizeof(t)), r);
}

static void mpeg4$define(inner, edit_list_t, dump)(mlog_s *restrict mlog, uint32_t level, edit_list_t *restrict r, mpeg4_atom_dump_t *restrict unidata)
{
	char buffer[64];
	mlog_level_dump("segment duration:    %s (%lu)\n", mpeg4$define(inner, duration, string)(buffer, (double) r->segment_duration / unidata->timescale), r->segment_duration);
	mlog_level_dump("media time:          %s (%ld)\n", (r->media_time >= 0)?mpeg4$define(inner, duration, string)(buffer, (double) r->media_time / unidata->timescale):"-", r->media_time);
	mlog_level_dump("media rate integer:  %u\n", r->media_rate_integer);
	mlog_level_dump("media rate fraction: %u\n", r->media_rate_fraction);
}

static mpeg4$define$dump(elst)
{
	edit_list_t edit;
	inner_fullbox_t fullbox;
	uint32_t entry_count;
	uint32_t level = unidata->dump_level;
	if (!mpeg4$define(inner, fullbox, get)(&fullbox, &data, &size))
		goto label_fail;
	mpeg4$define(inner, fullbox, dump)(mlog, &fullbox, NULL, level);
	if (fullbox.version > 1)
		goto label_fail;
	if (!mpeg4$define(inner, uint32_t, get)(&entry_count, &data, &size))
		goto label_fail;
	mlog_level_dump("entry count: %u\n", entry_count);
	level += mlog_level_width;
	if (fullbox.version == 0)
	{
		while (entry_count && size)
		{
			if (!mpeg4$define(inner, edit_list_32, get)(&edit, &data, &size))
				goto label_fail;
			mpeg4$define(inner, edit_list_t, dump)(mlog, level, &edit, unidata);
			--entry_count;
		}
	}
	else if (fullbox.version == 1)
	{
		while (entry_count && size)
		{
			if (!mpeg4$define(inner, edit_list_64, get)(&edit, &data, &size))
				goto label_fail;
			mpeg4$define(inner, edit_list_t, dump)(mlog, level, &edit, unidata);
			--entry_count;
		}
	}
	else goto label_fail;
	if (size || entry_count)
		goto label_fail;
	return atom;
	label_fail:
	return NULL;
}

static mpeg4$define$stuff$init(elst, mpeg4_stuff__edit_list_s)
{
	mpeg4$define(inner, array, init)(&r->pri.edit_list, sizeof(edit_list_t));
	return &r->stuff;
}

static mpeg4$define$stuff$free(elst, mpeg4_stuff__edit_list_s)
{
	mpeg4$define(inner, array, clear)(&r->pri.edit_list);
	mpeg4_stuff_free_default_func(&r->stuff);
}

static mpeg4$define$create(elst)
{
	return mpeg4_stuff_alloc(atom, inst, type,
		sizeof(mpeg4_stuff__edit_list_s),
		(mpeg4_stuff_init_f) mpeg4$define(stuff, elst, init),
		(refer_free_f) mpeg4$define(stuff, elst, free));
}

static mpeg4$define$parse(elst)
{
	inner_fullbox_t fullbox;
	edit_list_t edit;
	uint32_t entry_count;
	if (!mpeg4$define(inner, fullbox, get)(&fullbox, &data, &size))
		goto label_fail;
	if (!mpeg4$stuff$method$call(stuff, set$version_and_flags, fullbox.version, fullbox.flags))
		goto label_fail;
	if (!mpeg4$define(inner, uint32_t, get)(&entry_count, &data, &size))
		goto label_fail;
	if (fullbox.version == 0)
	{
		while (entry_count && size)
		{
			if (!mpeg4$define(inner, edit_list_32, get)(&edit, &data, &size))
				goto label_fail;
			if (!mpeg4$stuff$method$call(stuff,
				add$edit_list_item,
				edit.segment_duration,
				edit.media_time,
				edit.media_rate_integer,
				edit.media_rate_fraction))
				goto label_fail;
			--entry_count;
		}
	}
	else if (fullbox.version == 1)
	{
		while (entry_count && size)
		{
			if (!mpeg4$define(inner, edit_list_64, get)(&edit, &data, &size))
				goto label_fail;
			if (!mpeg4$stuff$method$call(stuff,
				add$edit_list_item,
				edit.segment_duration,
				edit.media_time,
				edit.media_rate_integer,
				edit.media_rate_fraction))
				goto label_fail;
			--entry_count;
		}
	}
	else goto label_fail;
	if (size || entry_count)
		goto label_fail;
	return stuff;
	label_fail:
	return NULL;
}

static mpeg4$define$calc(elst)
{
	mpeg4_stuff__edit_list_t *restrict r = &((mpeg4_stuff__edit_list_s *) stuff)->pri;
	switch (r->fullbox.version)
	{
		case 0: return mpeg4_stuff_calc_okay(stuff, sizeof(mpeg4_full_box_suffix_t) + sizeof(uint32_t) +
				(sizeof(edit_list_time32_t) + sizeof(edit_list_rate_t)) * r->edit_list.number);
		case 1: return mpeg4_stuff_calc_okay(stuff, sizeof(mpeg4_full_box_suffix_t) + sizeof(uint32_t) +
				(sizeof(edit_list_time64_t) + sizeof(edit_list_rate_t)) * r->edit_list.number);
		default: return NULL;
	}
}

static mpeg4$define$build(elst)
{
	mpeg4_stuff__edit_list_t *restrict r = &((mpeg4_stuff__edit_list_s *) stuff)->pri;
	edit_list_t *restrict array;
	uint32_t i, n;
	data = mpeg4$define(inner, fullbox, set)(data, &r->fullbox);
	data = mpeg4$define(inner, uint32_t, set)(data, n = (uint32_t) r->edit_list.number);
	if (n)
	{
		if (!(array = (edit_list_t *) r->edit_list.array))
			goto label_fail;
		if (r->fullbox.version == 0)
		{
			for (i = 0; i < n; ++i)
				data = mpeg4$define(inner, edit_list_32, set)(data, array + i);
		}
		else if (r->fullbox.version == 1)
		{
			for (i = 0; i < n; ++i)
				data = mpeg4$define(inner, edit_list_64, set)(data, array + i);
		}
		else goto label_fail;
	}
	return stuff;
	label_fail:
	return NULL;
}

static inner_method_set_fullbox(elst, mpeg4_stuff__edit_list_s, pri.fullbox, 1);
static inner_method_get_fullbox(elst, mpeg4_stuff__edit_list_s, pri.fullbox);

static const mpeg4_stuff_t* mpeg4$define(stuff, elst, add$edit_list_item)(mpeg4_stuff__edit_list_s *restrict r, uint64_t segment_duration, int64_t media_time, uint16_t media_rate_integer, uint16_t media_rate_fraction)
{
	edit_list_t *restrict p;
	if ((p = mpeg4$define(inner, array, append_point)(&r->pri.edit_list, 1)))
	{
		p->segment_duration = segment_duration;
		p->media_time = media_time;
		p->media_rate_integer = media_rate_integer;
		p->media_rate_fraction = media_rate_fraction;
		return &r->stuff;
	}
	return NULL;
}

static const mpeg4$define$alloc(elst)
{
	mpeg4_atom_s *restrict r;
	r = mpeg4_atom_alloc_empty();
	if (r)
	{
		r->interface.dump = mpeg4$define(atom, elst, dump);
		r->interface.create = mpeg4$define(atom, elst, create);
		r->interface.parse = mpeg4$define(atom, elst, parse);
		r->interface.calc = mpeg4$define(atom, elst, calc);
		r->interface.build = mpeg4$define(atom, elst, build);
		if (
			mpeg4$stuff$method$set(r, elst, set$version_and_flags) &&
			mpeg4$stuff$method$set(r, elst, get$version_and_flags) &&
			mpeg4$stuff$method$set(r, elst, add$edit_list_item)
		) return r;
		refer_free(r);
	}
	return NULL;
}

mpeg4$define$find(elst)
{
	static const mpeg4_box_type_t type = { .c = "elst" };
	return mpeg4_find_atom(inst, mpeg4$define(atom, elst, alloc), type.v, 0);
}
