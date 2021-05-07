#include "box.include.h"
#include "inner.bindata.h"
#include "inner.data.h"

/*
	bit marker = 1;
	uint7_t version = 1;
	uint3_t seq_profile;
	uint5_t seq_level_idx_0;
	bit seq_tier_0;
	bit high_bitdepth;
	bit twelve_bit;
	bit monochrome;
	bit chroma_subsampling_x;
	bit chroma_subsampling_y;
	uint2_t chroma_sample_position;
	bit3_t reserved = 0;
	bit initial_presentation_delay_present;
	if (initial_presentation_delay_present)
		uint4_t initial_presentation_delay_minus_1;
	else
		bit4_t reserved = 0;
	uint8_t config_obus[];
*/

typedef struct av1_config_t {
	uint8_t version;
	uint8_t seq_profile;
	uint8_t seq_level_idx_0;
	uint8_t flag;
	uint8_t chroma_sample_position;
	uint8_t initial_presentation_delay;
	uint8_t *config_obus;
	uintptr_t config_obus_size;
} av1_config_t;

typedef struct mpeg4_stuff__stsd_av1C_s {
	mpeg4_stuff_t stuff;
	av1_config_t av1_config;
} mpeg4_stuff__stsd_av1C_s;

static mpeg4$define$dump(stsd_codec_av1C)
{
	uint32_t level;
	uint8_t b;
	level = unidata->dump_level;
	if (!mpeg4$define(inner, uint8_t, get)(&b, &data, &size))
		goto label_fail;
	mlog_level_dump("marker:                             %u\n", !!(b & 0x80));
	mlog_level_dump("version:                            %u\n", b & 0x7f);
	if (b != 0x81)
		goto label_fail;
	if (!mpeg4$define(inner, uint8_t, get)(&b, &data, &size))
		goto label_fail;
	mlog_level_dump("seq_profile:                        %u\n", (b >> 5) & 0x07);
	mlog_level_dump("seq_level_idx_0:                    %u\n", b & 0x1f);
	if (!mpeg4$define(inner, uint8_t, get)(&b, &data, &size))
		goto label_fail;
	mlog_level_dump("seq_tier_0:                         %u\n", !!(b & 0x80));
	mlog_level_dump("high_bitdepth:                      %u\n", !!(b & 0x40));
	mlog_level_dump("twelve_bit:                         %u\n", !!(b & 0x20));
	mlog_level_dump("monochrome:                         %u\n", !!(b & 0x10));
	mlog_level_dump("chroma_subsampling_x:               %u\n", !!(b & 0x08));
	mlog_level_dump("chroma_subsampling_y:               %u\n", !!(b & 0x04));
	mlog_level_dump("chroma_sample_position:             %u\n", b & 0x03);
	if (!mpeg4$define(inner, uint8_t, get)(&b, &data, &size))
		goto label_fail;
	mlog_level_dump("initial_presentation_delay_present: %u\n", !!(b & 0x10));
	mlog_level_dump("initial_presentation_delay:         %u\n", (b & 0x10)?((b & 0x0f) + 1):0);
	mlog_level_dump("config_obus_size:                   %lu\n", size);
	mlog_level_dump("config_obus:\n");
	mpeg4$define(inner, bindata, dump)(mlog, data, size, 0, level + mlog_level_width);
	return atom;
	label_fail:
	return NULL;
}

static mpeg4$define$stuff$init(stsd_codec_av1C, mpeg4_stuff__stsd_av1C_s)
{
	r->av1_config.version = 0x81;
	return &r->stuff;
}

static mpeg4$define$stuff$free(stsd_codec_av1C, mpeg4_stuff__stsd_av1C_s)
{
	mpeg4$define(inner, block, set)(&r->av1_config.config_obus, &r->av1_config.config_obus_size, NULL, 0);
	mpeg4_stuff_free_default_func(&r->stuff);
}

static mpeg4$define$create(stsd_codec_av1C)
{
	return mpeg4_stuff_alloc(atom, inst, type,
		sizeof(mpeg4_stuff__stsd_av1C_s),
		(mpeg4_stuff_init_f) mpeg4$define(stuff, stsd_codec_av1C, init),
		(refer_free_f) mpeg4$define(stuff, stsd_codec_av1C, free));
}

static mpeg4$define$parse(stsd_codec_av1C)
{
	uint8_t b;
	if (!mpeg4$define(inner, uint8_t, get)(&b, &data, &size))
		goto label_fail;
	if (b != 0x81)
		goto label_fail;
	if (!mpeg4$define(inner, uint8_t, get)(&b, &data, &size))
		goto label_fail;
	if (!mpeg4$stuff$method$call(stuff, set$av1C_seq,
		(b >> 5) & 0x07,
		b & 0x1f))
		goto label_fail;
	if (!mpeg4$define(inner, uint8_t, get)(&b, &data, &size))
		goto label_fail;
	if (!mpeg4$stuff$method$call(stuff, set$av1C_flag,
		!!(b & 0x80),
		!!(b & 0x40),
		!!(b & 0x20),
		!!(b & 0x10),
		!!(b & 0x08),
		!!(b & 0x04),
		b & 0x03))
		goto label_fail;
	if (!mpeg4$define(inner, uint8_t, get)(&b, &data, &size))
		goto label_fail;
	if (!mpeg4$stuff$method$call(stuff, set$av1C_initial_presentation_delay,
		(b & 0x10)?((b & 0x0f) + 1):0))
		goto label_fail;
	if (!size || mpeg4$stuff$method$call(stuff, set$data, data, size))
		return stuff;
	label_fail:
	return NULL;
}

static mpeg4$define$calc(stsd_codec_av1C)
{
	return mpeg4_stuff_calc_okay(stuff, ((mpeg4_stuff__stsd_av1C_s *) stuff)->av1_config.config_obus_size + 4);
}

static mpeg4$define$build(stsd_codec_av1C)
{
	const av1_config_t *restrict r;
	r = &((mpeg4_stuff__stsd_av1C_s *) stuff)->av1_config;
	*data++ = r->version;
	*data++ = (r->seq_profile << 5) | (r->seq_level_idx_0 & 0x1f);
	*data++ = r->flag;
	*data++ = r->initial_presentation_delay?((r->initial_presentation_delay - 1) | 0x10):0;
	if (r->config_obus_size)
		mpeg4$define(inner, data, set)(data, r->config_obus, r->config_obus_size);
	return stuff;
}

static const mpeg4_stuff_t* mpeg4$define(stuff, stsd_codec_av1C, set$av1C_seq)(mpeg4_stuff__stsd_av1C_s *restrict r, uint8_t seq_profile, uint8_t seq_level_idx_0)
{
	if (seq_profile < 8 && seq_level_idx_0 < 32)
	{
		r->av1_config.seq_profile = seq_profile;
		r->av1_config.seq_level_idx_0 = seq_level_idx_0;
		return &r->stuff;
	}
	return NULL;
}

static const mpeg4_stuff_t* mpeg4$define(stuff, stsd_codec_av1C, set$av1C_flag)(mpeg4_stuff__stsd_av1C_s *restrict r, uint8_t seq_tier_0, uint8_t high_bitdepth, uint8_t twelve_bit, uint8_t monochrome, uint8_t chroma_subsampling_x, uint8_t chroma_subsampling_y, uint8_t chroma_sample_position)
{
	if (chroma_sample_position < 4)
	{
		register uint8_t flag = 0;
		flag |= !!seq_tier_0;
		flag <<= 1;
		flag |= !!high_bitdepth;
		flag <<= 1;
		flag |= !!twelve_bit;
		flag <<= 1;
		flag |= !!monochrome;
		flag <<= 1;
		flag |= !!chroma_subsampling_x;
		flag <<= 1;
		flag |= !!chroma_subsampling_y;
		flag <<= 2;
		flag |= chroma_sample_position;
		r->av1_config.flag = flag;
		return &r->stuff;
	}
	return NULL;
}

static const mpeg4_stuff_t* mpeg4$define(stuff, stsd_codec_av1C, set$av1C_initial_presentation_delay)(mpeg4_stuff__stsd_av1C_s *restrict r, uint8_t initial_presentation_delay)
{
	if (initial_presentation_delay <= 16)
	{
		r->av1_config.initial_presentation_delay = initial_presentation_delay;
		return &r->stuff;
	}
	return NULL;
}

static const mpeg4_stuff_t* mpeg4$define(stuff, stsd_codec_av1C, set$data)(mpeg4_stuff__stsd_av1C_s *restrict r, const void *data, uint64_t size)
{
	if (mpeg4$define(inner, block, set)(&r->av1_config.config_obus, &r->av1_config.config_obus_size, data, size))
		return &r->stuff;
	return NULL;
}

static const mpeg4_stuff_t* mpeg4$define(stuff, stsd_codec_av1C, get$data)(mpeg4_stuff__stsd_av1C_s *restrict r, uint64_t offset, void *data, uint64_t size, uint64_t *restrict rsize)
{
	mpeg4$define(inner, block, get)(r->av1_config.config_obus, r->av1_config.config_obus_size, offset, data, size, rsize);
	return &r->stuff;
}

static const mpeg4$define$alloc(stsd_codec_av1C)
{
	mpeg4_atom_s *restrict r;
	r = mpeg4_atom_alloc_empty();
	if (r)
	{
		r->interface.dump = mpeg4$define(atom, stsd_codec_av1C, dump);
		r->interface.create = mpeg4$define(atom, stsd_codec_av1C, create);
		r->interface.parse = mpeg4$define(atom, stsd_codec_av1C, parse);
		r->interface.calc = mpeg4$define(atom, stsd_codec_av1C, calc);
		r->interface.build = mpeg4$define(atom, stsd_codec_av1C, build);
		if (
			mpeg4$stuff$method$set(r, stsd_codec_av1C, set$av1C_seq) &&
			mpeg4$stuff$method$set(r, stsd_codec_av1C, set$av1C_flag) &&
			mpeg4$stuff$method$set(r, stsd_codec_av1C, set$av1C_initial_presentation_delay) &&
			mpeg4$stuff$method$set(r, stsd_codec_av1C, set$data) &&
			mpeg4$stuff$method$set(r, stsd_codec_av1C, get$data)
		) return r;
		refer_free(r);
	}
	return NULL;
}

mpeg4$define$find(stsd_codec_av1C)
{
	static const mpeg4_box_type_t type = { .c = "av1C" };
	static const mpeg4_box_type_t extra = { .c = "stsd" };
	return mpeg4_find_atom(inst, mpeg4$define(atom, stsd_codec_av1C, alloc), type.v, extra.v);
}
