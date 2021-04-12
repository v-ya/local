#include "box.include.h"
#include "inner.fullbox.h"
#include "inner.timespec.h"
#include "inner.matrix.h"
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

mpeg4$define$dump(box, mvhd)
{
	inner_fullbox_t fullbox;
	inner_timespec_t timespec;
	movie_header_uni_t header;
	char buffer[64];
	if (!mpeg4$define(inner, fullbox, get)(&fullbox, &data, &size))
		goto label_fail;
	mlog_level_dump("version = %u, flags = %06x\n", fullbox.version, fullbox.flags);
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
	if (size < sizeof(header))
		goto label_fail;
	memcpy(&header, data, sizeof(header));
	header.next_track_id = mpeg4_n32(header.next_track_id);
	data += sizeof(header);
	size -= sizeof(header);
	mlog_level_dump("rate:              %g\n", mpeg4_fix_point(mpeg4_n32(header.rate), 16, 16));
	mlog_level_dump("volume:            %g\n", mpeg4_fix_point(mpeg4_n16(header.volume), 8, 8));
	mlog_level_dump("matrix:\n");
	mpeg4$define(inner, matrix, dump)(mlog, &header.matrix, level + 1);
	mlog_level_dump("next_track_id:     %u (%08x)\n", header.next_track_id, header.next_track_id);
	if (!size) return inst;
	label_fail:
	return NULL;
}
