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
			.timescale = 1,
			.temp_size = 0
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

mpeg4_stuff_s* mpeg4_stuff_first(mpeg4_stuff_s *restrict stuff)
{
	mpeg4_stuff_s *restrict container;
	if ((container = stuff->link.container))
		return container->container.list;
	return NULL;
}

mpeg4_stuff_s* mpeg4_stuff_first_same(mpeg4_stuff_s *restrict stuff)
{
	mpeg4_stuff_container_type_finder_t *restrict finder;
	if ((finder = stuff->link.finder))
		return finder->same_list;
	return NULL;
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
#define stuff_method_call_d0(_f, _id)\
	const mpeg4_stuff_s* mpeg4_stuff__##_f##_##_id(mpeg4_stuff_s *restrict r)\
	{return stuff_method_call(r, _f##$##_id);}
#define stuff_method_call_d1(_f, _id, _t1, _v1)\
	const mpeg4_stuff_s* mpeg4_stuff__##_f##_##_id(mpeg4_stuff_s *restrict r, _t1 _v1)\
	{return stuff_method_call(r, _f##$##_id, _v1);}
#define stuff_method_call_d2(_f, _id, _t1, _v1, _t2, _v2)\
	const mpeg4_stuff_s* mpeg4_stuff__##_f##_##_id(mpeg4_stuff_s *restrict r, _t1 _v1, _t2 _v2)\
	{return stuff_method_call(r, _f##$##_id, _v1, _v2);}
#define stuff_method_call_d3(_f, _id, _t1, _v1, _t2, _v2, _t3, _v3)\
	const mpeg4_stuff_s* mpeg4_stuff__##_f##_##_id(mpeg4_stuff_s *restrict r, _t1 _v1, _t2 _v2, _t3 _v3)\
	{return stuff_method_call(r, _f##$##_id, _v1, _v2, _v3);}
#define stuff_method_call_d4(_f, _id, _t1, _v1, _t2, _v2, _t3, _v3, _t4, _v4)\
	const mpeg4_stuff_s* mpeg4_stuff__##_f##_##_id(mpeg4_stuff_s *restrict r, _t1 _v1, _t2 _v2, _t3 _v3, _t4 _v4)\
	{return stuff_method_call(r, _f##$##_id, _v1, _v2, _v3, _v4);}

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

stuff_method_call_d1(set, minor_version, uint32_t, minor_version)

const mpeg4_stuff_s* mpeg4_stuff__add_compatible_brand(mpeg4_stuff_s *restrict r, const char *restrict compatible_brand)
{
	mpeg4_box_type_t t = {.v = 0};
	if (compatible_brand)
		t = mpeg4$define(inner, type, check)(compatible_brand);
	if (t.v) return stuff_method_call(r, add$compatible_brands, &t, 1);
	return NULL;
}

stuff_method_call_d2(set, version_and_flags, uint32_t, version, uint32_t, flags)
stuff_method_call_d2(get, version_and_flags, uint32_t *restrict, version, uint32_t *restrict, flags)
stuff_method_call_d2(set, data, const void *, data, uint64_t, size)
stuff_method_call_d3(add, data, const void *, data, uint64_t, size, uint64_t *restrict, offset)
stuff_method_call_d1(calc, offset, uint64_t *restrict, offset)

const mpeg4_stuff_s* mpeg4_stuff__set_creation_time(mpeg4_stuff_s *restrict r, uint64_t creation_time)
{
	return stuff_method_call(r, set$creation_time, mpeg4$define(inner, utc, time1904)(creation_time));
}

const mpeg4_stuff_s* mpeg4_stuff__set_modification_time(mpeg4_stuff_s *restrict r, uint64_t modification_time)
{
	return stuff_method_call(r, set$modification_time, mpeg4$define(inner, utc, time1904)(modification_time));
}

stuff_method_call_d1(set, timescale, uint32_t, timescale)
stuff_method_call_d1(set, duration, uint64_t, duration)
stuff_method_call_d1(set, rate, double, rate)
stuff_method_call_d1(set, volume, double, volume)

const mpeg4_stuff_s* mpeg4_stuff__set_matrix(mpeg4_stuff_s *restrict r, double matrix[9])
{
	return stuff_method_call(r, set$matrix, matrix);
}

stuff_method_call_d1(set, next_track_id, uint32_t, next_track_id)
stuff_method_call_d1(set, track_id, uint32_t, track_id)
stuff_method_call_d1(set, track_layer, int16_t, track_layer)
stuff_method_call_d1(set, alternate_group, int16_t, alternate_group)
stuff_method_call_d2(set, track_resolution, double, width, double, height)

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

stuff_method_call_d1(set, name, const char *restrict, name)
stuff_method_call_d1(set, location, const char *restrict, location)
stuff_method_call_d1(set, graphicsmode, uint16_t, graphicsmode)
stuff_method_call_d3(set, opcolor, uint16_t, red, uint16_t, green, uint16_t, blue)
stuff_method_call_d1(set, balance, double, balance)
stuff_method_call_d1(set, data_reference_index, uint16_t, data_reference_index)
stuff_method_call_d2(set, pixel_resolution, uint16_t, width, uint16_t, height)
stuff_method_call_d2(set, ppi_resolution, double, horizre, double, vertre)
stuff_method_call_d1(set, frame_per_sample, uint16_t, frame_per_sample)
stuff_method_call_d1(set, audio_channel_count, uint16_t, audio_channel_count)
stuff_method_call_d1(set, audio_sample_size, uint16_t, audio_sample_size)
stuff_method_call_d3(set, bitrate, uint32_t, buffer_size_db, uint32_t, max_bitrate, uint32_t, avg_bitrate)
stuff_method_call_d2(set, pixel_aspect_ratio, uint32_t, h_spacing, uint32_t, v_spacing)
stuff_method_call_d1(set, es_id, uint16_t, es_id)
stuff_method_call_d2(set, decoder_config_profile, uint8_t, object_profile_indication, uint8_t, stream_type)
stuff_method_call_d4(set, avcC_indication, uint8_t, avc_profile_indication, uint8_t, profile_compatibility, uint8_t, avc_level_indication, uint8_t, length_size)
stuff_method_call_d3(set, avcC_extra, uint8_t, chroma_format, uint8_t, bit_depth_luma, uint8_t, bit_depth_chroma)
stuff_method_call_d2(add, avcC_sps, const void *, data, uint64_t, size)
stuff_method_call_d2(add, avcC_pps, const void *, data, uint64_t, size)
stuff_method_call_d2(add, avcC_extra_spse, const void *, data, uint64_t, size)
stuff_method_call_d4(add, edit_list_item, uint64_t, segment_duration, int64_t, media_time, uint16_t, media_rate_integer, uint16_t, media_rate_fraction)
stuff_method_call_d3(add, chunk_offset, mpeg4_stuff_s *restrict, mdat, uint64_t, offset, uint32_t *restrict, chunk_id)
stuff_method_call_d2(set, sample_count, uint32_t, sample_size, uint32_t, sample_count)
stuff_method_call_d2(add, sample_size, uint32_t, sample_size, uint32_t *restrict, sample_id)
stuff_method_call_d3(add, sample_to_chunk, uint32_t, first_chunk, uint32_t, samples_per_chunk, uint32_t, sample_description_index)
stuff_method_call_d2(add, time_to_sample, uint32_t, sample_count, uint32_t, sample_delta)
stuff_method_call_d2(add, composition_offset, uint32_t, sample_count, int32_t, sample_offset)
stuff_method_call_d1(add, sync_sample, uint32_t, sample_id)
stuff_method_call_d2(add, sample_to_group, uint32_t, sample_count, uint32_t, group_description_index)
stuff_method_call_d1(set, default_sample_description_index, uint32_t, default_sample_description_index)
stuff_method_call_d1(set, roll_distance, int16_t, roll_distance)

const mpeg4_stuff_s* mpeg4_stuff__set_ilst_data_text(mpeg4_stuff_s *restrict r, const char *restrict text)
{
	return stuff_method_call(r, set$ilst_data_text, text, text?strlen(text):0);
}
