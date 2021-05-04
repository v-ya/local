#include "box.include.h"
#include "inner.fullbox.h"
#include "inner.timespec.h"
#include "inner.matrix.h"
#include "inner.data.h"
#include <memory.h>

typedef struct movie_header_uni_t {
	int32_t rate;          // 16.16
	int16_t volume;        // 8.8
	uint16_t reserved1;
	uint32_t reserved2[2];
	inner_matrix_t matrix;
	uint32_t pre_defined[6];
	uint32_t next_track_id;
} __attribute__ ((packed)) movie_header_uni_t;

typedef struct mpeg4_stuff__movie_header_t {
	inner_fullbox_t fullbox;
	inner_timespec_t timespec;
	double rate;
	double volume;
	double matrix[9];
	uint32_t next_track_id;
} mpeg4_stuff__movie_header_t;

typedef struct mpeg4_stuff__movie_header_s {
	mpeg4_stuff_t stuff;
	mpeg4_stuff__movie_header_t pri;
} mpeg4_stuff__movie_header_s;

static mpeg4$define$dump(mvhd)
{
	inner_fullbox_t fullbox;
	inner_timespec_t timespec;
	movie_header_uni_t header;
	char buffer[64];
	uint32_t level = unidata->dump_level;
	if (!mpeg4$define(inner, fullbox, get)(&fullbox, &data, &size))
		goto label_fail;
	mpeg4$define(inner, fullbox, dump)(mlog, &fullbox, NULL, level);
	if (fullbox.version == 0)
	{
		if (!mpeg4$define(inner, timespec32, get)(&timespec, &data, &size))
			goto label_fail;
	}
	else if (fullbox.version == 1)
	{
		if (!mpeg4$define(inner, timespec64, get)(&timespec, &data, &size))
			goto label_fail;
	}
	else goto label_fail;
	unidata->timescale = timespec.timescale;
	mlog_level_dump("creation time:     %s (%lu)\n", mpeg4$define(inner, time1904, string)(buffer, timespec.creation_time), timespec.creation_time);
	mlog_level_dump("modification time: %s (%lu)\n", mpeg4$define(inner, time1904, string)(buffer, timespec.modification_time), timespec.modification_time);
	mlog_level_dump("timescale:         %u\n", timespec.timescale);
	mlog_level_dump("duration:          %s (%lu)\n", mpeg4$define(inner, duration, string)(buffer, (double) timespec.duration / timespec.timescale), timespec.duration);
	if (!mpeg4$define(inner, data, get)(&header, sizeof(header), &data, &size))
		goto label_fail;
	header.next_track_id = mpeg4_n32(header.next_track_id);
	mlog_level_dump("rate:              %g\n", mpeg4_fix_point(mpeg4_n32(header.rate), 16, 16));
	mlog_level_dump("volume:            %g\n", mpeg4_fix_point(mpeg4_n16(header.volume), 8, 8));
	mlog_level_dump("matrix:\n");
	mpeg4$define(inner, matrix, dump)(mlog, &header.matrix, level + mlog_level_width);
	mlog_level_dump("next_track_id:     %u (%08x)\n", header.next_track_id, header.next_track_id);
	if (!size) return atom;
	label_fail:
	return NULL;
}

static mpeg4$define$stuff$init(mvhd, mpeg4_stuff__movie_header_s)
{
	r->pri.timespec.timescale = 1000;
	r->pri.rate = 1;
	r->pri.volume = 1;
	mpeg4$define(inner, matrix, init)(r->pri.matrix);
	return &r->stuff;
}

static mpeg4$define$create(mvhd)
{
	return mpeg4_stuff_alloc(atom, inst, type,
		sizeof(mpeg4_stuff__movie_header_s),
		(mpeg4_stuff_init_f) mpeg4$define(stuff, mvhd, init),
		NULL);
}

static mpeg4$define$parse(mvhd)
{
	inner_fullbox_t fullbox;
	inner_timespec_t timespec;
	movie_header_uni_t header;
	double matrix[9];
	if (!mpeg4$define(inner, fullbox, get)(&fullbox, &data, &size))
		goto label_fail;
	if (!mpeg4$stuff$method$call(stuff, set$version_and_flags, fullbox.version, fullbox.flags))
		goto label_fail;
	if (fullbox.version == 0)
	{
		if (!mpeg4$define(inner, timespec32, get)(&timespec, &data, &size))
			goto label_fail;
	}
	else if (fullbox.version == 1)
	{
		if (!mpeg4$define(inner, timespec64, get)(&timespec, &data, &size))
			goto label_fail;
	}
	else goto label_fail;
	if (!mpeg4$stuff$method$call(stuff, set$creation_time, timespec.creation_time))
		goto label_fail;
	if (!mpeg4$stuff$method$call(stuff, set$modification_time, timespec.modification_time))
		goto label_fail;
	if (!mpeg4$stuff$method$call(stuff, set$timescale, timespec.timescale))
		goto label_fail;
	if (!mpeg4$stuff$method$call(stuff, set$duration, timespec.duration))
		goto label_fail;
	if (!mpeg4$define(inner, data, get)(&header, sizeof(header), &data, &size))
		goto label_fail;
	mpeg4$define(inner, matrix, get)(matrix, &header.matrix);
	if (!mpeg4$stuff$method$call(stuff, set$rate, mpeg4_fix_point(mpeg4_n32(header.rate), 16, 16)))
		goto label_fail;
	if (!mpeg4$stuff$method$call(stuff, set$volume, mpeg4_fix_point(mpeg4_n16(header.volume), 8, 8)))
		goto label_fail;
	if (!mpeg4$stuff$method$call(stuff, set$matrix, matrix))
		goto label_fail;
	if (!mpeg4$stuff$method$call(stuff, set$next_track_id, mpeg4_n32(header.next_track_id)))
		goto label_fail;
	if (!size) return stuff;
	label_fail:
	return NULL;
}

static mpeg4$define$calc(mvhd)
{
	switch (((mpeg4_stuff__movie_header_s *) stuff)->pri.fullbox.version)
	{
		case 0: return mpeg4_stuff_calc_okay(stuff,
				sizeof(mpeg4_full_box_suffix_t) +
				sizeof(inner_timespec_32_t) +
				sizeof(movie_header_uni_t));
		case 1: return mpeg4_stuff_calc_okay(stuff,
				sizeof(mpeg4_full_box_suffix_t) +
				sizeof(inner_timespec_64_t) +
				sizeof(movie_header_uni_t));
		default: return NULL;
	}
}

static mpeg4$define$build(mvhd)
{
	mpeg4_stuff__movie_header_t *restrict r = &((mpeg4_stuff__movie_header_s *) stuff)->pri;
	movie_header_uni_t header = {
		.reserved1 = 0,
		.reserved2 = {0, 0},
		.pre_defined = {0, 0, 0, 0, 0, 0}
	};
	data = mpeg4$define(inner, fullbox, set)(data, &r->fullbox);
	if (r->fullbox.version == 0)
		data = mpeg4$define(inner, timespec32, set)(data, &r->timespec);
	else if (r->fullbox.version == 1)
		data = mpeg4$define(inner, timespec64, set)(data, &r->timespec);
	else goto label_fail;
	header.rate = mpeg4_n32(mpeg4_fix_point_gen(r->rate, int32_t, 16, 16));
	header.volume = mpeg4_n16(mpeg4_fix_point_gen(r->volume, int16_t, 8, 8));
	mpeg4$define(inner, matrix, set)(&header.matrix, r->matrix);
	header.next_track_id = mpeg4_n32(r->next_track_id);
	mpeg4$define(inner, data, set)(data, &header, sizeof(header));
	return stuff;
	label_fail:
	return NULL;
}

static inner_method_set_fullbox(mvhd, mpeg4_stuff__movie_header_s, pri.fullbox, 1);
static inner_method_get_fullbox(mvhd, mpeg4_stuff__movie_header_s, pri.fullbox);
static inner_method_set_simple_param(mvhd, creation_time, mpeg4_stuff__movie_header_s, uint64_t, pri.timespec.creation_time);
static inner_method_set_simple_param(mvhd, modification_time, mpeg4_stuff__movie_header_s, uint64_t, pri.timespec.modification_time);

static const mpeg4_stuff_t* mpeg4$define(stuff, mvhd, set$timescale)(mpeg4_stuff__movie_header_s *restrict r, uint32_t timescale)
{
	if (timescale)
	{
		r->pri.timespec.timescale = timescale;
		return &r->stuff;
	}
	return NULL;
}

static inner_method_set_simple_param(mvhd, duration, mpeg4_stuff__movie_header_s, uint64_t, pri.timespec.duration);
static inner_method_set_simple_param(mvhd, rate, mpeg4_stuff__movie_header_s, double, pri.rate);
static inner_method_set_simple_param(mvhd, volume, mpeg4_stuff__movie_header_s, double, pri.volume);

static const mpeg4_stuff_t* mpeg4$define(stuff, mvhd, set$matrix)(mpeg4_stuff__movie_header_s *restrict r, double matrix[9])
{
	memcpy(r->pri.matrix, matrix, sizeof(r->pri.matrix));
	return &r->stuff;
}

static inner_method_set_simple_param(mvhd, next_track_id, mpeg4_stuff__movie_header_s, uint32_t, pri.next_track_id);

static const mpeg4$define$alloc(mvhd)
{
	mpeg4_atom_s *restrict r;
	r = mpeg4_atom_alloc_empty();
	if (r)
	{
		r->interface.dump = mpeg4$define(atom, mvhd, dump);
		r->interface.create = mpeg4$define(atom, mvhd, create);
		r->interface.parse = mpeg4$define(atom, mvhd, parse);
		r->interface.calc = mpeg4$define(atom, mvhd, calc);
		r->interface.build = mpeg4$define(atom, mvhd, build);
		if (
			mpeg4$stuff$method$set(r, mvhd, set$version_and_flags) &&
			mpeg4$stuff$method$set(r, mvhd, get$version_and_flags) &&
			mpeg4$stuff$method$set(r, mvhd, set$creation_time) &&
			mpeg4$stuff$method$set(r, mvhd, set$modification_time) &&
			mpeg4$stuff$method$set(r, mvhd, set$timescale) &&
			mpeg4$stuff$method$set(r, mvhd, set$duration) &&
			mpeg4$stuff$method$set(r, mvhd, set$rate) &&
			mpeg4$stuff$method$set(r, mvhd, set$volume) &&
			mpeg4$stuff$method$set(r, mvhd, set$matrix) &&
			mpeg4$stuff$method$set(r, mvhd, set$next_track_id)
		) return r;
	}
	return r;
}

mpeg4$define$find(mvhd)
{
	static const mpeg4_box_type_t type = { .c = "mvhd" };
	return mpeg4_find_atom(inst, mpeg4$define(atom, mvhd, alloc), type.v, 0);
}
