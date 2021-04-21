#include "box.include.h"
#include "inner.fullbox.h"
#include "inner.timespec.h"
#include <memory.h>

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
	uint32_t media_rate_integer;
	uint32_t media_rate_fraction;
} edit_list_t;

static inline edit_list_t* mpeg4$define(inner, edit_list_rate, get)(edit_list_t *restrict r, const uint8_t *restrict *restrict data, uint64_t *restrict size)
{
	edit_list_rate_t rate;
	if (*size >= sizeof(rate))
	{
		memcpy(&rate, *data, sizeof(rate));
		*data += sizeof(rate);
		*size -= sizeof(rate);
		r->media_rate_integer = (uint32_t) mpeg4_n16(rate.media_rate_integer);
		r->media_rate_fraction = (uint32_t) mpeg4_n16(rate.media_rate_fraction);
		return r;
	}
	return NULL;
}

static inline edit_list_t* mpeg4$define(inner, edit_list_32, get)(edit_list_t *restrict r, const uint8_t *restrict *restrict data, uint64_t *restrict size)
{
	edit_list_time32_t t;
	if (*size >= sizeof(t))
	{
		memcpy(&t, *data, sizeof(t));
		*data += sizeof(t);
		*size -= sizeof(t);
		r->segment_duration = mpeg4_n32(t.segment_duration);
		r->media_time = mpeg4_n32(t.media_time);
		return mpeg4$define(inner, edit_list_rate, get)(r, data, size);
	}
	return NULL;
}

static inline edit_list_t* mpeg4$define(inner, edit_list_64, get)(edit_list_t *restrict r, const uint8_t *restrict *restrict data, uint64_t *restrict size)
{
	edit_list_time64_t t;
	if (*size >= sizeof(t))
	{
		memcpy(&t, *data, sizeof(t));
		*data += sizeof(t);
		*size -= sizeof(t);
		r->segment_duration = mpeg4_n64(t.segment_duration);
		r->media_time = mpeg4_n64(t.media_time);
		return mpeg4$define(inner, edit_list_rate, get)(r, data, size);
	}
	return NULL;
}

static void mpeg4$define(inner, edit_list_t, dump)(mlog_s *restrict mlog, uint32_t level, edit_list_t *restrict r, mpeg4_dump_data_t *restrict unidata)
{
	char buffer[64];
	mlog_level_dump("segment duration:    %s (%lu)\n", mpeg4$define(inner, duration, string)(buffer, (double) r->segment_duration / unidata->timescale), r->segment_duration);
	mlog_level_dump("media time:          %s (%ld)\n", (r->media_time >= 0)?mpeg4$define(inner, duration, string)(buffer, (double) r->media_time / unidata->timescale):"-", r->media_time);
	mlog_level_dump("media rate integer:  %u\n", r->media_rate_integer);
	mlog_level_dump("media rate fraction: %u\n", r->media_rate_fraction);
}

mpeg4$define$dump(box, elst)
{
	edit_list_t edit;
	inner_fullbox_t fullbox;
	uint32_t entry_count;
	if (!mpeg4$define(inner, fullbox, get)(&fullbox, &data, &size))
		goto label_fail;
	mlog_level_dump("version = %u, flags = %06x\n", fullbox.version, fullbox.flags);
	if (fullbox.version > 1)
		goto label_fail;
	if (size < sizeof(entry_count))
		goto label_fail;
	entry_count = mpeg4_n32(*(const uint32_t *) data);
	data += sizeof(entry_count);
	size -= sizeof(entry_count);
	mlog_level_dump("entry count: %u\n", entry_count);
	level += 1;
	if (!fullbox.version)
	{
		while (entry_count && size)
		{
			if (!mpeg4$define(inner, edit_list_32, get)(&edit, &data, &size))
				goto label_fail;
			mpeg4$define(inner, edit_list_t, dump)(mlog, level, &edit, unidata);
			--entry_count;
		}
	}
	else
	{
		while (entry_count && size)
		{
			if (!mpeg4$define(inner, edit_list_64, get)(&edit, &data, &size))
				goto label_fail;
			mpeg4$define(inner, edit_list_t, dump)(mlog, level, &edit, unidata);
			--entry_count;
		}
	}
	if (size || entry_count)
		goto label_fail;
	return inst;
	label_fail:
	return NULL;
}
