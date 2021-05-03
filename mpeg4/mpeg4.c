#include "mpeg4.h"
#include "mpeg4.inst.h"
#include "mpeg4.box.h"
#include "box/box.include.h"
#include "box/inner.type.h"
#include "box/inner.timespec.h"
#include <string.h>

static void mpeg4_free_func(mpeg4_s *restrict r)
{
	if (r->pool)
		rbtree_clear(&r->pool);
}

const mpeg4_s* mpeg4_alloc(void)
{
	mpeg4_s *restrict r;
	if ((r = (mpeg4_s *) refer_alloz(sizeof(mpeg4_s))))
	{
		refer_set_free(r, (refer_free_f) mpeg4_free_func);
		if ((r->unknow = mpeg4$define(atom, unknow, find)(r)) &&
			(r->root = mpeg4$define(atom, file, find)(r)))
			return r;
		refer_free(r);
	}
	return NULL;
}

void mpeg4_verbose_dump(const mpeg4_s *restrict inst, mlog_s *restrict mlog)
{
	mlog_printf(mlog, "d: dump, a: create, c: calc, b: build, p: parse\n");
	mlog_printf(mlog, "{root}\n");
	mpeg4_dump_atom_verbose_tree(inst->root, mlog, mlog_level_inital + mlog_level_width);
	mlog_printf(mlog, "{unknow}\n");
	mpeg4_dump_atom_verbose_tree(inst->unknow, mlog, mlog_level_inital + mlog_level_width);
}

const mpeg4_s* mpeg4_dump(const mpeg4_s *restrict inst, mlog_s *restrict mlog, const uint8_t *restrict data, uint64_t size, uint32_t dump_samples)
{
	register const mpeg4_atom_s *restrict atom;
	register mpeg4_dump_f dump;
	if ((dump = (atom = inst->root)->interface.dump))
	{
		mpeg4_atom_dump_t unidata = {
			.dump_level = mlog_level_inital,
			.dump_samples = !!dump_samples,
			.timescale = 1
		};
		if (dump(inst->root, mlog, data, size, &unidata))
			return inst;
	}
	return NULL;
}

mpeg4_stuff_s* mpeg4_create_root_stuff(const mpeg4_s *restrict inst)
{
	register const mpeg4_atom_s *restrict atom;
	register mpeg4_create_f create;
	if ((create = (atom = inst->root)->interface.create))
		return create(atom, inst, (mpeg4_box_type_t) { .v = 0 });
	return NULL;
}

mpeg4_stuff_s* mpeg4_create_stuff(const mpeg4_stuff_s *restrict container, const char *restrict type)
{
	register const mpeg4_atom_s *restrict atom;
	register mpeg4_create_f create;
	mpeg4_box_type_t t;
	if (container && type && (atom = container->atom) &&
		(t = mpeg4$define(inner, type, check)(type)).v &&
		(atom = mpeg4_atom_layer_find(atom, t.v)) &&
		(create = atom->interface.create))
		return create(atom, container->inst, t);
	return NULL;
}

mpeg4_stuff_s* mpeg4_link_stuff(mpeg4_stuff_s *restrict container, mpeg4_stuff_s *restrict stuff)
{
	if (mpeg4_atom_layer_find(container->atom, stuff->info.type.v))
		return mpeg4_stuff_container_link(container, stuff, NULL);
	return NULL;
}

mpeg4_stuff_s* mpeg4_link_before_stuff(mpeg4_stuff_s *restrict after, mpeg4_stuff_s *restrict stuff)
{
	mpeg4_stuff_s *restrict container;
	if ((container = after->link.container) && mpeg4_atom_layer_find(container->atom, stuff->info.type.v))
		return mpeg4_stuff_container_link(container, stuff, after);
	return NULL;
}

mpeg4_stuff_s* mpeg4_append_stuff(mpeg4_stuff_s *restrict container, const char *restrict type)
{
	mpeg4_box_type_t t;
	if (container && type && (t = mpeg4$define(inner, type, check)(type)).v)
		return mpeg4_stuff_container_append(container, t);
	return NULL;
}

mpeg4_stuff_s* mpeg4_stuff_container(mpeg4_stuff_s *restrict stuff)
{
	return stuff->link.container;
}

mpeg4_stuff_s* mpeg4_stuff_next(mpeg4_stuff_s *restrict stuff)
{
	return stuff->link.next;
}

mpeg4_stuff_s* mpeg4_stuff_next_same(mpeg4_stuff_s *restrict stuff)
{
	return stuff->link.same_next;
}

mpeg4_stuff_s* mpeg4_find_stuff(mpeg4_stuff_s *restrict container, const char *restrict type)
{
	mpeg4_box_type_t t;
	if (container && (!type || (t = mpeg4$define(inner, type, check)(type)).v))
		return mpeg4_stuff_container_find(container, type?&t:NULL);
	return NULL;
}

mpeg4_stuff_s* mpeg4_find_path_stuff(mpeg4_stuff_s *restrict container, const char *const restrict *restrict path)
{
	mpeg4_box_type_t t;
	while (container && *path)
	{
		if (!(t = mpeg4$define(inner, type, check)(*path)).v)
			goto label_fail;
		container = mpeg4_stuff_container_find(container, &t);
		++path;
	}
	return container;
	label_fail:
	return NULL;
}

mpeg4_stuff_s* mpeg4_parse(const mpeg4_s *restrict inst, const uint8_t *restrict data, uint64_t size)
{
	register mpeg4_stuff_s *restrict r;
	register mpeg4_parse_f parse;
	r = mpeg4_create_root_stuff(inst);
	if (r)
	{
		if ((parse = r->atom->interface.parse) && parse(r, data, size) && mpeg4_parse_stco_and_mdat(r, data))
			return r;
		refer_free(r);
	}
	return NULL;
}

mpeg4_stuff_s* mpeg4_calc(mpeg4_stuff_s *restrict stuff)
{
	register mpeg4_calc_f calc;
	if (stuff->info.calc_okay)
		goto label_okay;
	if ((calc = stuff->atom->interface.calc) && calc(stuff))
	{
		label_okay:
		return stuff;
	}
	return NULL;
}

uint64_t mpeg4_size(const mpeg4_stuff_s *restrict stuff)
{
	if (stuff->info.calc_okay)
		return stuff->info.all_size;
	return 0;
}

const mpeg4_stuff_s* mpeg4_build(const mpeg4_stuff_s *restrict stuff, uint8_t *restrict data)
{
	register mpeg4_build_f build;
	if (stuff->info.calc_okay && (build = stuff->atom->interface.build) && build(stuff, data))
		return stuff;
	return NULL;
}

#define stuff_method_call(_stuff, _mid, ...)  (mpeg4_stuff_calc_invalid(_stuff), mpeg4$stuff$method$call(_stuff, _mid, ##__VA_ARGS__))

const mpeg4_stuff_s* mpeg4_stuff__set_major_brand(mpeg4_stuff_s *restrict r, const char *restrict major_brand)
{
	mpeg4_box_type_t t = {.v = 0};
	if (major_brand)
		t = mpeg4$define(inner, type, check)(major_brand);
	if (t.v) return stuff_method_call(r, set$major_brand, t);
	return NULL;
}

const mpeg4_stuff_s* mpeg4_stuff__set_minor_brand(mpeg4_stuff_s *restrict r, const char *restrict minor_brand)
{
	mpeg4_box_type_t t = {.v = 0};
	if (minor_brand)
		t = mpeg4$define(inner, type, check)(minor_brand);
	if (t.v) return stuff_method_call(r, set$minor_brand, t);
	return NULL;
}

const mpeg4_stuff_s* mpeg4_stuff__set_minor_version(mpeg4_stuff_s *restrict r, uint32_t minor_version)
{
	return stuff_method_call(r, set$minor_version, minor_version);
}

const mpeg4_stuff_s* mpeg4_stuff__add_compatible_brand(mpeg4_stuff_s *restrict r, const char *restrict compatible_brand)
{
	mpeg4_box_type_t t = {.v = 0};
	if (compatible_brand)
		t = mpeg4$define(inner, type, check)(compatible_brand);
	if (t.v) return stuff_method_call(r, add$compatible_brands, &t, 1);
	return NULL;
}

const mpeg4_stuff_s* mpeg4_stuff__set_version_and_flags(mpeg4_stuff_s *restrict r, uint32_t version, uint32_t flags)
{
	return stuff_method_call(r, set$version_and_flags, version, flags);
}

const mpeg4_stuff_s* mpeg4_stuff__get_version_and_flags(mpeg4_stuff_s *restrict r, uint32_t *restrict version, uint32_t *restrict flags)
{
	return stuff_method_call(r, get$version_and_flags, version, flags);
}

const mpeg4_stuff_s* mpeg4_stuff__set_data(mpeg4_stuff_s *restrict r, const void *data, uint64_t size)
{
	return stuff_method_call(r, set$data, data, size);
}

const mpeg4_stuff_s* mpeg4_stuff__add_data(mpeg4_stuff_s *restrict r, const void *data, uint64_t size, uint64_t *restrict offset)
{
	return stuff_method_call(r, add$data, data, size, offset);
}

const mpeg4_stuff_s* mpeg4_stuff__calc_offset(mpeg4_stuff_s *restrict r, uint64_t *restrict offset)
{
	return stuff_method_call(r, calc$offset, offset);
}

const mpeg4_stuff_s* mpeg4_stuff__set_creation_time(mpeg4_stuff_s *restrict r, uint64_t creation_time)
{
	return stuff_method_call(r, set$creation_time, mpeg4$define(inner, utc, time1904)(creation_time));
}

const mpeg4_stuff_s* mpeg4_stuff__set_modification_time(mpeg4_stuff_s *restrict r, uint64_t modification_time)
{
	return stuff_method_call(r, set$modification_time, mpeg4$define(inner, utc, time1904)(modification_time));
}

const mpeg4_stuff_s* mpeg4_stuff__set_timescale(mpeg4_stuff_s *restrict r, uint32_t timescale)
{
	return stuff_method_call(r, set$timescale, timescale);
}

const mpeg4_stuff_s* mpeg4_stuff__set_duration(mpeg4_stuff_s *restrict r, uint64_t duration)
{
	return stuff_method_call(r, set$duration, duration);
}

const mpeg4_stuff_s* mpeg4_stuff__set_rate(mpeg4_stuff_s *restrict r, double rate)
{
	return stuff_method_call(r, set$rate, rate);
}

const mpeg4_stuff_s* mpeg4_stuff__set_volume(mpeg4_stuff_s *restrict r, double volume)
{
	return stuff_method_call(r, set$volume, volume);
}

const mpeg4_stuff_s* mpeg4_stuff__set_matrix(mpeg4_stuff_s *restrict r, double matrix[9])
{
	return stuff_method_call(r, set$matrix, matrix);
}

const mpeg4_stuff_s* mpeg4_stuff__set_next_track_id(mpeg4_stuff_s *restrict r, uint32_t next_track_id)
{
	return stuff_method_call(r, set$next_track_id, next_track_id);
}

const mpeg4_stuff_s* mpeg4_stuff__set_track_id(mpeg4_stuff_s *restrict r, uint32_t track_id)
{
	return stuff_method_call(r, set$track_id, track_id);
}

const mpeg4_stuff_s* mpeg4_stuff__set_track_layer(mpeg4_stuff_s *restrict r, int16_t track_layer)
{
	return stuff_method_call(r, set$track_layer, track_layer);
}

const mpeg4_stuff_s* mpeg4_stuff__set_alternate_group(mpeg4_stuff_s *restrict r, int16_t alternate_group)
{
	return stuff_method_call(r, set$alternate_group, alternate_group);
}

const mpeg4_stuff_s* mpeg4_stuff__set_resolution(mpeg4_stuff_s *restrict r, double width, double height)
{
	return stuff_method_call(r, set$resolution, width, height);
}

const mpeg4_stuff_s* mpeg4_stuff__set_language(mpeg4_stuff_s *restrict r, const char *restrict language)
{
	char lc3[4];
	lc3[3] = 0;
	if (language && (lc3[0] = language[0]) >= '`' && lc3[0] <= '~' &&
		(lc3[1] = language[1]) >= '`' && lc3[1] <= '~' &&
		(lc3[2] = language[2]) >= '`' && lc3[2] <= '~')
		return stuff_method_call(r, set$language, lc3);
	return NULL;
}

const mpeg4_stuff_s* mpeg4_stuff__set_name(mpeg4_stuff_s *restrict r, const char *restrict name)
{
	return stuff_method_call(r, set$name, name);
}

const mpeg4_stuff_s* mpeg4_stuff__set_location(mpeg4_stuff_s *restrict r, const char *restrict location)
{
	return stuff_method_call(r, set$location, location);
}

const mpeg4_stuff_s* mpeg4_stuff__set_graphicsmode(mpeg4_stuff_s *restrict r, uint16_t graphicsmode)
{
	return stuff_method_call(r, set$graphicsmode, graphicsmode);
}

const mpeg4_stuff_s* mpeg4_stuff__set_opcolor(mpeg4_stuff_s *restrict r, uint16_t red, uint16_t green, uint16_t blue)
{
	return stuff_method_call(r, set$opcolor, red, green, blue);
}

const mpeg4_stuff_s* mpeg4_stuff__set_balance(mpeg4_stuff_s *restrict r, double balance)
{
	return stuff_method_call(r, set$balance, balance);
}

const mpeg4_stuff_s* mpeg4_stuff__add_edit_list_item(mpeg4_stuff_s *restrict r, uint64_t segment_duration, int64_t media_time, uint16_t media_rate_integer, uint16_t media_rate_fraction)
{
	return stuff_method_call(r, add$edit_list_item, segment_duration, media_time, media_rate_integer, media_rate_fraction);
}

const mpeg4_stuff_s* mpeg4_stuff__add_chunk_offset(mpeg4_stuff_s *restrict r, mpeg4_stuff_s *restrict mdat, uint64_t offset, uint32_t *restrict chunk_id)
{
	return stuff_method_call(r, add$chunk_offset, mdat, offset, chunk_id);
}

const mpeg4_stuff_s* mpeg4_stuff__set_sample_count(mpeg4_stuff_s *restrict r, uint32_t sample_size, uint32_t sample_count)
{
	return stuff_method_call(r, set$sample_count, sample_size, sample_count);
}

const mpeg4_stuff_s* mpeg4_stuff__add_sample_size(mpeg4_stuff_s *restrict r, uint32_t sample_size, uint32_t *restrict sample_id)
{
	return stuff_method_call(r, add$sample_size, sample_size, sample_id);
}

const mpeg4_stuff_s* mpeg4_stuff__add_sample_to_chunk(mpeg4_stuff_s *restrict r, uint32_t first_chunk, uint32_t samples_per_chunk, uint32_t sample_description_index)
{
	return stuff_method_call(r, add$sample_to_chunk, first_chunk, samples_per_chunk, sample_description_index);
}

const mpeg4_stuff_s* mpeg4_stuff__add_time_to_sample(mpeg4_stuff_s *restrict r, uint32_t sample_count, uint32_t sample_delta)
{
	return stuff_method_call(r, add$time_to_sample, sample_count, sample_delta);
}

const mpeg4_stuff_s* mpeg4_stuff__add_composition_offset(mpeg4_stuff_s *restrict r, uint32_t sample_count, int32_t sample_offset)
{
	return stuff_method_call(r, add$composition_offset, sample_count, sample_offset);
}

const mpeg4_stuff_s* mpeg4_stuff__add_sync_sample(mpeg4_stuff_s *restrict r, uint32_t sample_id)
{
	return stuff_method_call(r, add$sync_sample, sample_id);
}

const mpeg4_stuff_s* mpeg4_stuff__add_sample_to_group(mpeg4_stuff_s *restrict r, uint32_t sample_count, uint32_t group_description_index)
{
	return stuff_method_call(r, add$sample_to_group, sample_count, group_description_index);
}

const mpeg4_stuff_s* mpeg4_stuff__set_ilst_data_text(mpeg4_stuff_s *restrict r, const char *restrict text)
{
	return stuff_method_call(r, set$ilst_data_text, text, text?strlen(text):0);
}
