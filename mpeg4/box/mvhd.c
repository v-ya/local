#include "box.func.h"
#include <memory.h>

typedef struct movie_header_time32_t {
	uint32_t creation_time;
	uint32_t modification_time;
	uint32_t timescale;
	uint32_t duration;
} __attribute__ ((packed)) movie_header_time32_t;

typedef struct movie_header_time64_t {
	uint64_t creation_time;
	uint64_t modification_time;
	uint32_t timescale;
	uint64_t duration;
} __attribute__ ((packed)) movie_header_time64_t;

typedef struct movie_header_uni_t {
	int32_t rate;          // 16.16
	int16_t volume;        // 8.8
	uint16_t reserved1;
	uint32_t reserved2[2];
	int32_t matrix_a;      // 16.16
	int32_t matrix_b;      // 16.16
	int32_t matrix_u;      // 2.30
	int32_t matrix_c;      // 16.16
	int32_t matrix_d;      // 16.16
	int32_t matrix_v;      // 2.30
	int32_t matrix_x;      // 16.16
	int32_t matrix_y;      // 16.16
	int32_t matrix_w;      // 2.30
	uint32_t pre_defined[6];
	uint32_t next_track_id;
} __attribute__ ((packed)) movie_header_uni_t;

mpeg4$define$dump(box, mvhd)
{
	uint64_t creation_time;
	uint64_t modification_time;
	uint64_t duration;
	uint32_t timescale;
	uint32_t version, flags;
	movie_header_uni_t header;
	char buffer[64];
	if (size < sizeof(mpeg4_full_box_suffix_t))
		goto label_fail;
	version = mpeg4_full_box_suffix_parse(*(const mpeg4_full_box_suffix_t *) data, &flags);
	data += sizeof(mpeg4_full_box_suffix_t);
	size -= sizeof(mpeg4_full_box_suffix_t);
	mlog_level_dump("version = %u, flags = %06x\n", version, flags);
	if (version == 0)
	{
		movie_header_time32_t t;
		if (size < sizeof(t))
			goto label_fail;
		memcpy(&t, data, sizeof(t));
		data += sizeof(t);
		size -= sizeof(t);
		creation_time = mpeg4_n32(t.creation_time);
		modification_time = mpeg4_n32(t.modification_time);
		timescale = mpeg4_n32(t.timescale);
		duration = mpeg4_n32(t.duration);
	}
	else if (version == 1)
	{
		movie_header_time64_t t;
		if (size < sizeof(t))
			goto label_fail;
		memcpy(&t, data, sizeof(t));
		data += sizeof(t);
		size -= sizeof(t);
		creation_time = mpeg4_n64(t.creation_time);
		modification_time = mpeg4_n64(t.modification_time);
		timescale = mpeg4_n32(t.timescale);
		duration = mpeg4_n64(t.duration);
	}
	else goto label_fail;
	mlog_level_dump("creation time:     %s (%lu)\n", mpeg4_time1904_string(buffer, creation_time), creation_time);
	mlog_level_dump("modification time: %s (%lu)\n", mpeg4_time1904_string(buffer, modification_time), modification_time);
	mlog_level_dump("timescale:         %u\n", timescale);
	mlog_level_dump("duration:          %s (%lu)\n", mpeg4_duration_string(buffer, (double) duration / timescale), duration);
	if (size < sizeof(header))
		goto label_fail;
	memcpy(&header, data, sizeof(header));
	data += sizeof(header);
	size -= sizeof(header);
	mlog_level_dump("rate:              %g\n", mpeg4_fix_point(mpeg4_n32(header.rate), 16, 16));
	mlog_level_dump("volume:            %g\n", mpeg4_fix_point(mpeg4_n16(header.volume), 8, 8));
	mlog_level_dump("matrix:\n");
	level += 1;
	mlog_level_dump("%12g, %12g, %12g\n",
		mpeg4_fix_point(mpeg4_n32(header.matrix_a), 16, 16),
		mpeg4_fix_point(mpeg4_n32(header.matrix_b), 16, 16),
		mpeg4_fix_point(mpeg4_n32(header.matrix_u), 2, 30));
	mlog_level_dump("%12g, %12g, %12g\n",
		mpeg4_fix_point(mpeg4_n32(header.matrix_c), 16, 16),
		mpeg4_fix_point(mpeg4_n32(header.matrix_d), 16, 16),
		mpeg4_fix_point(mpeg4_n32(header.matrix_v), 2, 30));
	mlog_level_dump("%12g, %12g, %12g\n",
		mpeg4_fix_point(mpeg4_n32(header.matrix_x), 16, 16),
		mpeg4_fix_point(mpeg4_n32(header.matrix_y), 16, 16),
		mpeg4_fix_point(mpeg4_n32(header.matrix_w), 2, 30));
	level -= 1;
	header.next_track_id = mpeg4_n32(header.next_track_id);
	mlog_level_dump("next_track_id:     %u (%08x)\n", header.next_track_id, header.next_track_id);
	if (!size) return inst;
	label_fail:
	return NULL;
}
