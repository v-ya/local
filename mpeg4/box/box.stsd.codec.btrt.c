#include "box.include.h"
#include "inner.data.h"

typedef struct bitrate_t {
	uint32_t buffer_size_db;
	uint32_t max_bitrate;
	uint32_t avg_bitrate;
} __attribute__ ((packed)) bitrate_t;

typedef struct mpeg4_stuff__stsd_bitrate_s {
	mpeg4_stuff_t stuff;
	bitrate_t bitrate;
} mpeg4_stuff__stsd_bitrate_s;

static mpeg4$define$dump(stsd_codec_btrt)
{
	bitrate_t bitrate;
	uint32_t level = unidata->dump_level;
	if (!mpeg4$define(inner, data, get)(&bitrate, sizeof(bitrate), &data, &size))
		goto label_fail;
	mlog_level_dump("buffer size db: %u\n", mpeg4_n32(bitrate.buffer_size_db));
	mlog_level_dump("max bitrate:    %u\n", mpeg4_n32(bitrate.max_bitrate));
	mlog_level_dump("avg bitrate:    %u\n", mpeg4_n32(bitrate.avg_bitrate));
	if (!size) return atom;
	label_fail:
	return NULL;
}

static mpeg4$define$create(stsd_codec_btrt)
{
	return mpeg4_stuff_alloc(atom, inst, type, sizeof(mpeg4_stuff__stsd_bitrate_s), NULL, NULL);
}

static mpeg4$define$parse(stsd_codec_btrt)
{
	bitrate_t bitrate;
	if (!mpeg4$define(inner, data, get)(&bitrate, sizeof(bitrate), &data, &size))
		goto label_fail;
	if (!mpeg4$stuff$method$call(stuff, set$bitrate,
		mpeg4_n32(bitrate.buffer_size_db),
		mpeg4_n32(bitrate.max_bitrate),
		mpeg4_n32(bitrate.avg_bitrate)))
		goto label_fail;
	if (!size) return stuff;
	label_fail:
	return NULL;
}

static mpeg4$define$calc(stsd_codec_btrt)
{
	return mpeg4_stuff_calc_okay(stuff, sizeof(bitrate_t));
}

static mpeg4$define$build(stsd_codec_btrt)
{
	const bitrate_t *restrict r = &((mpeg4_stuff__stsd_bitrate_s *) stuff)->bitrate;
	bitrate_t bitrate;
	bitrate.buffer_size_db = mpeg4_n32(r->buffer_size_db);
	bitrate.max_bitrate = mpeg4_n32(r->max_bitrate);
	bitrate.avg_bitrate = mpeg4_n32(r->avg_bitrate);
	mpeg4$define(inner, data, set)(data, &bitrate, sizeof(bitrate));
	return stuff;
}

static const mpeg4_stuff_t* mpeg4$define(stuff, stsd_codec_btrt, set$bitrate)(mpeg4_stuff__stsd_bitrate_s *restrict r, uint32_t buffer_size_db, uint32_t max_bitrate, uint32_t avg_bitrate)
{
	r->bitrate.buffer_size_db = buffer_size_db;
	r->bitrate.max_bitrate = max_bitrate;
	r->bitrate.avg_bitrate = avg_bitrate;
	return &r->stuff;
}

static const mpeg4$define$alloc(stsd_codec_btrt)
{
	mpeg4_atom_s *restrict r;
	r = mpeg4_atom_alloc_empty();
	if (r)
	{
		r->interface.dump = mpeg4$define(atom, stsd_codec_btrt, dump);
		r->interface.create = mpeg4$define(atom, stsd_codec_btrt, create);
		r->interface.parse = mpeg4$define(atom, stsd_codec_btrt, parse);
		r->interface.calc = mpeg4$define(atom, stsd_codec_btrt, calc);
		r->interface.build = mpeg4$define(atom, stsd_codec_btrt, build);
		if (mpeg4$stuff$method$set(r, stsd_codec_btrt, set$bitrate))
			return r;
		refer_free(r);
	}
	return NULL;
}

mpeg4$define$find(stsd_codec_btrt)
{
	static const mpeg4_box_type_t type = { .c = "btrt" };
	static const mpeg4_box_type_t extra = { .c = "stsd" };
	return mpeg4_find_atom(inst, mpeg4$define(atom, stsd_codec_btrt, alloc), type.v, extra.v);
}
