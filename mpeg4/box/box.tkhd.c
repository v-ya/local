#include "box.include.h"
#include "inner.fullbox.h"
#include "inner.timespec.h"
#include "inner.trackid.h"
#include "inner.matrix.h"
#include "inner.data.h"
#include <memory.h>

typedef struct track_header_uni_t {
	uint32_t reserved2[2];
	int16_t layer;
	int16_t alternate_group;
	int16_t volume;        // 8.8
	uint16_t reserved1;
	inner_matrix_t matrix;
	uint32_t width;        // fix16.16
	uint32_t height;       // fix16.16
} __attribute__ ((packed)) track_header_uni_t;

typedef struct mpeg4_stuff__track_header_t {
	inner_fullbox_t fullbox;
	inner_trackid_t trackid;
	int16_t layer;
	int16_t alternate_group;
	double volume;
	double matrix[9];
	double width;
	double height;
} mpeg4_stuff__track_header_t;

typedef struct mpeg4_stuff__track_header_s {
	mpeg4_stuff_t stuff;
	mpeg4_stuff__track_header_t pri;
} mpeg4_stuff__track_header_s;

static mpeg4$define$dump(tkhd)
{
	static const char *flag_name[4] = {
		[0] = "track_enabled",
		[1] = "track_in_movie",
		[2] = "track_in_preview",
		[3] = "track_size_is_aspect_ratio"
	};
	inner_fullbox_t fullbox;
	inner_trackid_t trackid;
	track_header_uni_t header;
	char buffer[128];
	uint32_t level = unidata->dump_level;
	if (!mpeg4$define(inner, fullbox, get)(&fullbox, &data, &size))
		goto label_fail;
	mpeg4$define(inner, fullbox, dump)(
		mlog,
		&fullbox,
		mpeg4$define(inner, flags, string)(buffer, fullbox.flags, flag_name, 4),
		level);
	if (fullbox.version == 0)
	{
		if (!mpeg4$define(inner, trackid32, get)(&trackid, &data, &size))
			goto label_fail;
	}
	else if (fullbox.version == 1)
	{
		if (!mpeg4$define(inner, trackid64, get)(&trackid, &data, &size))
			goto label_fail;
	}
	else goto label_fail;
	mlog_level_dump("creation time:     %s (%lu)\n", mpeg4$define(inner, time1904, string)(buffer, trackid.creation_time), trackid.creation_time);
	mlog_level_dump("modification time: %s (%lu)\n", mpeg4$define(inner, time1904, string)(buffer, trackid.modification_time), trackid.modification_time);
	mlog_level_dump("track ID:          %u\n", trackid.track_id);
	mlog_level_dump("duration:          %s (%lu)\n", mpeg4$define(inner, duration, string)(buffer, (double) trackid.duration / unidata->timescale), trackid.duration);
	if (!mpeg4$define(inner, data, get)(&header, sizeof(header), &data, &size))
		goto label_fail;
	mlog_level_dump("layer:             %d\n", mpeg4_n16(header.layer));
	mlog_level_dump("alternate group:   %d\n", mpeg4_n16(header.alternate_group));
	mlog_level_dump("volume:            %g\n", mpeg4_fix_point(mpeg4_n16(header.volume), 8, 8));
	mlog_level_dump("matrix:\n");
	mpeg4$define(inner, matrix, dump)(mlog, &header.matrix, level + mlog_level_width);
	mlog_level_dump("width:             %g\n", mpeg4_fix_point(mpeg4_n32(header.width), 16, 16));
	mlog_level_dump("height:            %g\n", mpeg4_fix_point(mpeg4_n32(header.height), 16, 16));
	if (!size) return atom;
	label_fail:
	return NULL;
}

static mpeg4$define$stuff$init(tkhd, mpeg4_stuff__track_header_s)
{
	r->pri.volume = 0;
	mpeg4$define(inner, matrix, init)(r->pri.matrix);
	r->pri.width = 0;
	r->pri.height = 0;
	return &r->stuff;
}

static mpeg4$define$create(tkhd)
{
	return mpeg4_stuff_alloc(atom, inst, type,
		sizeof(mpeg4_stuff__track_header_s),
		(mpeg4_stuff_init_f) mpeg4$define(stuff, tkhd, init),
		NULL);
}

static mpeg4$define$parse(tkhd)
{
	inner_fullbox_t fullbox;
	inner_trackid_t trackid;
	track_header_uni_t header;
	double matrix[9];
	if (!mpeg4$define(inner, fullbox, get)(&fullbox, &data, &size))
		goto label_fail;
	if (!mpeg4$stuff$method$call(stuff, set$version_and_flags, fullbox.version, fullbox.flags))
		goto label_fail;
	if (fullbox.version == 0)
	{
		if (!mpeg4$define(inner, trackid32, get)(&trackid, &data, &size))
			goto label_fail;
	}
	else if (fullbox.version == 1)
	{
		if (!mpeg4$define(inner, trackid64, get)(&trackid, &data, &size))
			goto label_fail;
	}
	else goto label_fail;
	if (!mpeg4$stuff$method$call(stuff, set$creation_time, trackid.creation_time))
		goto label_fail;
	if (!mpeg4$stuff$method$call(stuff, set$modification_time, trackid.modification_time))
		goto label_fail;
	if (!mpeg4$stuff$method$call(stuff, set$track_id, trackid.track_id))
		goto label_fail;
	if (!mpeg4$stuff$method$call(stuff, set$duration, trackid.duration))
		goto label_fail;
	if (!mpeg4$define(inner, data, get)(&header, sizeof(header), &data, &size))
		goto label_fail;
	mpeg4$define(inner, matrix, get)(matrix, &header.matrix);
	if (!mpeg4$stuff$method$call(stuff, set$track_layer, mpeg4_n16(header.layer)))
		goto label_fail;
	if (!mpeg4$stuff$method$call(stuff, set$alternate_group, mpeg4_n16(header.alternate_group)))
		goto label_fail;
	if (!mpeg4$stuff$method$call(stuff, set$volume, mpeg4_fix_point(mpeg4_n16(header.volume), 8, 8)))
		goto label_fail;
	if (!mpeg4$stuff$method$call(stuff, set$matrix, matrix))
		goto label_fail;
	if (!mpeg4$stuff$method$call(stuff, set$resolution,
		mpeg4_fix_point(mpeg4_n32(header.width), 16, 16),
		mpeg4_fix_point(mpeg4_n32(header.height), 16, 16)))
		goto label_fail;
	if (!size) return stuff;
	label_fail:
	return NULL;
}

static mpeg4$define$calc(tkhd)
{
	switch (((mpeg4_stuff__track_header_s *) stuff)->pri.fullbox.version)
	{
		case 0: return mpeg4_stuff_calc_okay(stuff,
				sizeof(mpeg4_full_box_suffix_t) +
				sizeof(inner_trackid_32_t) +
				sizeof(track_header_uni_t));
		case 1: return mpeg4_stuff_calc_okay(stuff,
				sizeof(mpeg4_full_box_suffix_t) +
				sizeof(inner_trackid_64_t) +
				sizeof(track_header_uni_t));
		default: return NULL;
	}
}

static mpeg4$define$build(tkhd)
{
	mpeg4_stuff__track_header_t *restrict r = &((mpeg4_stuff__track_header_s *) stuff)->pri;
	track_header_uni_t header = {
		.reserved2 = {0, 0},
		.reserved1 = 0
	};
	data = mpeg4$define(inner, fullbox, set)(data, &r->fullbox);
	if (r->fullbox.version == 0)
		data = mpeg4$define(inner, trackid32, set)(data, &r->trackid);
	else if (r->fullbox.version == 1)
		data = mpeg4$define(inner, trackid64, set)(data, &r->trackid);
	else goto label_fail;
	header.layer = r->layer;
	header.alternate_group = r->alternate_group;
	header.volume = mpeg4_n16(mpeg4_fix_point_gen(r->volume, int16_t, 8, 8));
	mpeg4$define(inner, matrix, set)(&header.matrix, r->matrix);
	header.width = mpeg4_n32(mpeg4_fix_point_gen(r->width, uint32_t, 16, 16));
	header.height = mpeg4_n32(mpeg4_fix_point_gen(r->height, uint32_t, 16, 16));
	mpeg4$define(inner, data, set)(data, &header, sizeof(header));
	return stuff;
	label_fail:
	return NULL;
}

static const mpeg4_stuff_t* mpeg4$define(stuff, tkhd, set$version_and_flags)(mpeg4_stuff__track_header_s *restrict r, uint32_t version, uint32_t flags)
{
	if (version > 1) goto label_fail;
	r->pri.fullbox.version = version;
	r->pri.fullbox.flags = flags;
	return &r->stuff;
	label_fail:
	return NULL;
}

static const mpeg4_stuff_t* mpeg4$define(stuff, tkhd, set$creation_time)(mpeg4_stuff__track_header_s *restrict r, uint64_t creation_time)
{
	r->pri.trackid.creation_time = creation_time;
	return &r->stuff;
}

static const mpeg4_stuff_t* mpeg4$define(stuff, tkhd, set$modification_time)(mpeg4_stuff__track_header_s *restrict r, uint64_t modification_time)
{
	r->pri.trackid.modification_time = modification_time;
	return &r->stuff;
}

static const mpeg4_stuff_t* mpeg4$define(stuff, tkhd, set$track_id)(mpeg4_stuff__track_header_s *restrict r, uint32_t track_id)
{
	r->pri.trackid.track_id = track_id;
	return &r->stuff;
}

static const mpeg4_stuff_t* mpeg4$define(stuff, tkhd, set$duration)(mpeg4_stuff__track_header_s *restrict r, uint64_t duration)
{
	r->pri.trackid.duration = duration;
	return &r->stuff;
}

static const mpeg4_stuff_t* mpeg4$define(stuff, tkhd, set$track_layer)(mpeg4_stuff__track_header_s *restrict r, int16_t track_layer)
{
	r->pri.layer = track_layer;
	return &r->stuff;
}

static const mpeg4_stuff_t* mpeg4$define(stuff, tkhd, set$alternate_group)(mpeg4_stuff__track_header_s *restrict r, int16_t alternate_group)
{
	r->pri.alternate_group = alternate_group;
	return &r->stuff;
}

static const mpeg4_stuff_t* mpeg4$define(stuff, tkhd, set$volume)(mpeg4_stuff__track_header_s *restrict r, double volume)
{
	r->pri.volume = volume;
	return &r->stuff;
}

static const mpeg4_stuff_t* mpeg4$define(stuff, tkhd, set$matrix)(mpeg4_stuff__track_header_s *restrict r, double matrix[9])
{
	memcpy(r->pri.matrix, matrix, sizeof(r->pri.matrix));
	return &r->stuff;
}

static const mpeg4_stuff_t* mpeg4$define(stuff, tkhd, set$resolution)(mpeg4_stuff__track_header_s *restrict r, double width, double height)
{
	if (width < 0) width = 0;
	if (height < 0) height = 0;
	r->pri.width = width;
	r->pri.height = height;
	return &r->stuff;
}

static const mpeg4$define$alloc(tkhd)
{
	mpeg4_atom_s *restrict r;
	r = mpeg4_atom_alloc_empty();
	if (r)
	{
		r->interface.dump = mpeg4$define(atom, tkhd, dump);
		r->interface.create = mpeg4$define(atom, tkhd, create);
		r->interface.parse = mpeg4$define(atom, tkhd, parse);
		r->interface.calc = mpeg4$define(atom, tkhd, calc);
		r->interface.build = mpeg4$define(atom, tkhd, build);
		if (
			mpeg4$stuff$method$set(r, tkhd, set$version_and_flags) &&
			mpeg4$stuff$method$set(r, tkhd, set$creation_time) &&
			mpeg4$stuff$method$set(r, tkhd, set$modification_time) &&
			mpeg4$stuff$method$set(r, tkhd, set$track_id) &&
			mpeg4$stuff$method$set(r, tkhd, set$duration) &&
			mpeg4$stuff$method$set(r, tkhd, set$track_layer) &&
			mpeg4$stuff$method$set(r, tkhd, set$alternate_group) &&
			mpeg4$stuff$method$set(r, tkhd, set$volume) &&
			mpeg4$stuff$method$set(r, tkhd, set$matrix) &&
			mpeg4$stuff$method$set(r, tkhd, set$resolution)
		) return r;
	}
	return r;
}

mpeg4$define$find(tkhd)
{
	static const mpeg4_box_type_t type = { .c = "tkhd" };
	return mpeg4_find_atom(inst, mpeg4$define(atom, tkhd, alloc), type.v, 0);
}
