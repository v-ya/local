#include "box.include.h"
#include "inner.fullbox.h"
#include "inner.timespec.h"
#include "inner.language.h"
#include "inner.data.h"

typedef struct media_header_uni_t {
	uint16_t language;
	uint16_t pre_defined;
} __attribute__ ((packed)) media_header_uni_t;

typedef struct mpeg4_stuff__media_header_t {
	inner_fullbox_t fullbox;
	inner_timespec_t timespec;
	inner_language_t language;
} mpeg4_stuff__media_header_t;

typedef struct mpeg4_stuff__media_header_s {
	mpeg4_stuff_t stuff;
	mpeg4_stuff__media_header_t pri;
} mpeg4_stuff__media_header_s;

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

static mpeg4$define$stuff$init(mdhd, mpeg4_stuff__media_header_s)
{
	r->pri.timespec.timescale = 1;
	r->pri.language = (inner_language_t) {.s = "und"};
	return &r->stuff;
}

static mpeg4$define$create(mdhd)
{
	return mpeg4_stuff_alloc(atom, inst, type,
		sizeof(mpeg4_stuff__media_header_s),
		(mpeg4_stuff_init_f) mpeg4$define(stuff, mdhd, init),
		NULL);
}

static mpeg4$define$parse(mdhd)
{
	inner_fullbox_t fullbox;
	inner_timespec_t timespec;
	media_header_uni_t header;
	inner_language_t language;
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
	if (!mpeg4$stuff$method$call(stuff, set$language, mpeg4$define(inner, language, parse)(&language, mpeg4_n16(header.language))->s))
		goto label_fail;
	if (!size) return stuff;
	label_fail:
	return NULL;
}

static mpeg4$define$calc(mdhd)
{
	switch (((mpeg4_stuff__media_header_s *) stuff)->pri.fullbox.version)
	{
		case 0: return mpeg4_stuff_calc_okay(stuff,
				sizeof(mpeg4_full_box_suffix_t) +
				sizeof(inner_timespec_32_t) +
				sizeof(media_header_uni_t));
		case 1: return mpeg4_stuff_calc_okay(stuff,
				sizeof(mpeg4_full_box_suffix_t) +
				sizeof(inner_timespec_64_t) +
				sizeof(media_header_uni_t));
		default: return NULL;
	}
}

static mpeg4$define$build(mdhd)
{
	mpeg4_stuff__media_header_t *restrict r = &((mpeg4_stuff__media_header_s *) stuff)->pri;
	media_header_uni_t header = {
		.pre_defined = 0
	};
	data = mpeg4$define(inner, fullbox, set)(data, &r->fullbox);
	if (r->fullbox.version == 0)
		data = mpeg4$define(inner, timespec32, set)(data, &r->timespec);
	else if (r->fullbox.version == 1)
		data = mpeg4$define(inner, timespec64, set)(data, &r->timespec);
	else goto label_fail;
	header.language = mpeg4_n16(mpeg4$define(inner, language, build)(&r->language));
	mpeg4$define(inner, data, set)(data, &header, sizeof(header));
	return stuff;
	label_fail:
	return NULL;
}

static inner_method_set_fullbox(mdhd, mpeg4_stuff__media_header_s, pri.fullbox, 1);
static inner_method_get_fullbox(mdhd, mpeg4_stuff__media_header_s, pri.fullbox);

static const mpeg4_stuff_t* mpeg4$define(stuff, mdhd, set$creation_time)(mpeg4_stuff__media_header_s *restrict r, uint64_t creation_time)
{
	r->pri.timespec.creation_time = creation_time;
	return &r->stuff;
}

static const mpeg4_stuff_t* mpeg4$define(stuff, mdhd, set$modification_time)(mpeg4_stuff__media_header_s *restrict r, uint64_t modification_time)
{
	r->pri.timespec.modification_time = modification_time;
	return &r->stuff;
}

static const mpeg4_stuff_t* mpeg4$define(stuff, mdhd, set$timescale)(mpeg4_stuff__media_header_s *restrict r, uint32_t timescale)
{
	if (timescale)
	{
		r->pri.timespec.timescale = timescale;
		return &r->stuff;
	}
	return NULL;
}

static const mpeg4_stuff_t* mpeg4$define(stuff, mdhd, set$duration)(mpeg4_stuff__media_header_s *restrict r, uint64_t duration)
{
	r->pri.timespec.duration = duration;
	return &r->stuff;
}

static const mpeg4_stuff_t* mpeg4$define(stuff, mdhd, set$language)(mpeg4_stuff__media_header_s *restrict r, const char language[3])
{
	r->pri.language.s[0] = language[0];
	r->pri.language.s[1] = language[1];
	r->pri.language.s[2] = language[2];
	r->pri.language.s[3] = 0;
	return &r->stuff;
}

static const mpeg4$define$alloc(mdhd)
{
	mpeg4_atom_s *restrict r;
	r = mpeg4_atom_alloc_empty();
	if (r)
	{
		r->interface.dump = mpeg4$define(atom, mdhd, dump);
		r->interface.create = mpeg4$define(atom, mdhd, create);
		r->interface.parse = mpeg4$define(atom, mdhd, parse);
		r->interface.calc = mpeg4$define(atom, mdhd, calc);
		r->interface.build = mpeg4$define(atom, mdhd, build);
		if (
			mpeg4$stuff$method$set(r, mdhd, set$version_and_flags) &&
			mpeg4$stuff$method$set(r, mdhd, get$version_and_flags) &&
			mpeg4$stuff$method$set(r, mdhd, set$creation_time) &&
			mpeg4$stuff$method$set(r, mdhd, set$modification_time) &&
			mpeg4$stuff$method$set(r, mdhd, set$timescale) &&
			mpeg4$stuff$method$set(r, mdhd, set$duration) &&
			mpeg4$stuff$method$set(r, mdhd, set$language)
		) return r;
	}
	return r;
}

mpeg4$define$find(mdhd)
{
	static const mpeg4_box_type_t type = { .c = "mdhd" };
	return mpeg4_find_atom(inst, mpeg4$define(atom, mdhd, alloc), type.v, 0);
}
