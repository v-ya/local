#include "box.include.h"
#include "inner.fullbox.h"
#include "inner.timespec.h"
#include "inner.language.h"
#include "inner.data.h"

typedef struct media_header_uni_t {
	uint16_t language;
	uint16_t pre_defined;
} __attribute__ ((packed)) media_header_uni_t;

static mpeg4$define$dump(mdhd)
{
	inner_fullbox_t fullbox;
	inner_timespec_t timespec;
	media_header_uni_t header;
	char buffer[64];
	inner_language_t language;
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
	header.language = mpeg4_n16(header.language);
	mlog_level_dump("language:          (%s) (%04x)\n", mpeg4$define(inner, language, parse)(&language, header.language)->s, header.language);
	if (!size) return atom;
	label_fail:
	return NULL;
}

static const mpeg4$define$alloc(mdhd)
{
	mpeg4_atom_s *restrict r;
	r = mpeg4_atom_alloc_empty();
	if (r)
	{
		r->interface.dump = mpeg4$define(atom, mdhd, dump);
	}
	return r;
}

mpeg4$define$find(mdhd)
{
	static const mpeg4_box_type_t type = { .c = "mdhd" };
	return mpeg4_find_atom(inst, mpeg4$define(atom, mdhd, alloc), type.v, 0);
}
