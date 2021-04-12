#include "box.include.h"
#include "inner.fullbox.h"
#include "inner.timespec.h"
#include "inner.language.h"
#include <memory.h>

typedef struct media_header_uni_t {
	uint16_t language;
	uint16_t pre_defined;
} __attribute__ ((packed)) media_header_uni_t;

mpeg4$define$dump(box, mdhd)
{
	inner_fullbox_t fullbox;
	inner_timespec_t timespec;
	media_header_uni_t header;
	char buffer[64];
	inner_language_t language;
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
	header.language = mpeg4_n16(header.language);
	data += sizeof(header);
	size -= sizeof(header);
	mlog_level_dump("language:          (%s) (%04x)\n", mpeg4$define(inner, language, parse)(&language, header.language)->s, header.language);
	if (!size) return inst;
	label_fail:
	return NULL;
}
