#include "box.include.h"
#include "inner.data.h"

typedef struct bit_rate_t {
	uint32_t buffer_size_db;
	uint32_t max_bitrate;
	uint32_t avg_bitrate;
} __attribute__ ((packed)) bit_rate_t;

static mpeg4$define$dump(stsd_codec_btrt)
{
	bit_rate_t bit_rate;
	uint32_t level = unidata->dump_level;
	if (!mpeg4$define(inner, data, get)(&bit_rate, sizeof(bit_rate), &data, &size))
		goto label_fail;
	mlog_level_dump("buffer size db: %u\n", mpeg4_n32(bit_rate.buffer_size_db));
	mlog_level_dump("max bitrate:    %u\n", mpeg4_n32(bit_rate.max_bitrate));
	mlog_level_dump("avg bitrate:    %u\n", mpeg4_n32(bit_rate.avg_bitrate));
	if (!size) return atom;
	label_fail:
	return NULL;
}

static const mpeg4$define$alloc(stsd_codec_btrt)
{
	mpeg4_atom_s *restrict r;
	r = mpeg4_atom_alloc_empty();
	if (r) r->interface.dump = mpeg4$define(atom, stsd_codec_btrt, dump);
	return r;
}

mpeg4$define$find(stsd_codec_btrt)
{
	static const mpeg4_box_type_t type = { .c = "btrt" };
	static const mpeg4_box_type_t extra = { .c = "stsd" };
	return mpeg4_find_atom(inst, mpeg4$define(atom, stsd_codec_btrt, alloc), type.v, extra.v);
}
