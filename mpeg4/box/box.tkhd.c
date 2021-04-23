#include "box.include.h"
#include "inner.fullbox.h"
#include "inner.timespec.h"
#include "inner.trackid.h"
#include "inner.matrix.h"
#include "inner.data.h"

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

static const mpeg4$define$alloc(tkhd)
{
	mpeg4_atom_t *restrict r;
	r = mpeg4_atom_alloc_empty();
	if (r)
	{
		r->interface.dump = mpeg4$define(atom, tkhd, dump);
	}
	return r;
}

mpeg4$define$find(tkhd)
{
	static const mpeg4_box_type_t type = { .c = "tkhd" };
	return mpeg4_find_atom(inst, mpeg4$define(atom, tkhd, alloc), type.v, 0);
}
