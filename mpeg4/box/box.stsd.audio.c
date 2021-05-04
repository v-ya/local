#include "box.container.h"
#include "inner.sample_entry.h"
#include "inner.type.h"
#include "inner.data.h"
#include <string.h>

typedef struct mpeg4_stuff__stsd_audio_s {
	mpeg4_stuff_t stuff;
	inner_audio_sample_t sample;
} mpeg4_stuff__stsd_audio_s;

static mpeg4$define$dump(stsd_audio)
{
	inner_audio_sample_t sample;
	uint32_t level = unidata->dump_level;
	if (!mpeg4$define(inner, audio_sample, get)(&sample, &data, &size))
		goto label_fail;
	mlog_level_dump("data reference index: %u\n", sample.data_reference_index);
	mlog_level_dump("entry version:        v%u\n", sample.entry_version);
	mlog_level_dump("channel count:        %u\n", sample.channel_count);
	mlog_level_dump("sample size:          %u bits\n", sample.sample_size);
	mlog_level_dump("sample rate:          %g\n", sample.sample_rate);
	return mpeg4$define(atom, container, dump)(atom, mlog, data, size, unidata);
	label_fail:
	return NULL;
}

static mpeg4$define$stuff$init(stsd_audio, mpeg4_stuff__stsd_audio_s)
{
	r->sample.channel_count = 2;
	r->sample.sample_size = 16;
	return &r->stuff;
}

static mpeg4$define$create(stsd_audio)
{
	return mpeg4_stuff_alloc(atom, inst, type,
		sizeof(mpeg4_stuff__stsd_audio_s),
		(mpeg4_stuff_init_f) mpeg4$define(stuff, stsd_audio, init),
		NULL);
}

static mpeg4$define$parse(stsd_audio)
{
	inner_audio_sample_t sample;
	if (!mpeg4$define(inner, audio_sample, get)(&sample, &data, &size))
		goto label_fail;
	if (!mpeg4$stuff$method$call(stuff, set$data_reference_index, sample.data_reference_index))
		goto label_fail;
	if (!mpeg4$stuff$method$call(stuff, set$minor_version, sample.entry_version))
		goto label_fail;
	if (!mpeg4$stuff$method$call(stuff, set$audio_channel_count, sample.channel_count))
		goto label_fail;
	if (!mpeg4$stuff$method$call(stuff, set$audio_sample_size, sample.sample_size))
		goto label_fail;
	if (!mpeg4$stuff$method$call(stuff, set$rate, sample.sample_rate))
		goto label_fail;
	return mpeg4$define(atom, container, parse)(stuff, data, size);
	label_fail:
	return NULL;
}

static mpeg4$define$calc(stsd_audio)
{
	if (mpeg4$define(atom, container, calc)(stuff))
		return mpeg4_stuff_calc_okay(stuff, stuff->info.inner_size + sizeof(inner_audio_sample_entry_t));
	return NULL;
}

static mpeg4$define$build(stsd_audio)
{
	data = mpeg4$define(inner, audio_sample, set)(data, &((mpeg4_stuff__stsd_audio_s *) stuff)->sample);
	return mpeg4$define(atom, container, build)(stuff, data);
}

static inner_method_set_simple_param(stsd_audio, data_reference_index, mpeg4_stuff__stsd_audio_s, uint16_t, sample.data_reference_index);

static const mpeg4_stuff_t* mpeg4$define(stuff, stsd_audio, set$minor_version)(mpeg4_stuff__stsd_audio_s *restrict r, uint32_t minor_version)
{
	if (minor_version <= 1)
	{
		r->sample.entry_version = (uint16_t) minor_version;
		return &r->stuff;
	}
	return NULL;
}

static inner_method_set_simple_param(stsd_audio, audio_channel_count, mpeg4_stuff__stsd_audio_s, uint16_t, sample.channel_count);
static inner_method_set_simple_param(stsd_audio, audio_sample_size, mpeg4_stuff__stsd_audio_s, uint16_t, sample.sample_size);
static inner_method_set_simple_param(stsd_audio, rate, mpeg4_stuff__stsd_audio_s, double, sample.sample_rate);

static const mpeg4$define$alloc(stsd_audio)
{
	mpeg4_atom$container_s *restrict r;
	r = (mpeg4_atom$container_s *) refer_alloz(sizeof(mpeg4_atom$container_s));
	if (r)
	{
		refer_set_free(r, (refer_free_f) mpeg4_atom_free_default_func);
		r->atom.interface.dump = mpeg4$define(atom, stsd_audio, dump);
		r->atom.interface.create = mpeg4$define(atom, stsd_audio, create);
		r->atom.interface.parse = mpeg4$define(atom, stsd_audio, parse);
		r->atom.interface.calc = mpeg4$define(atom, stsd_audio, calc);
		r->atom.interface.build = mpeg4$define(atom, stsd_audio, build);
		r->inst = inst;
		if (
			mpeg4$inner$type$alloc$layer2(&r->atom, inst, stsd_codec_btrt, "btrt") &&
			mpeg4$inner$type$alloc$layer2(&r->atom, inst, stsd_codec_esds, "esds") &&
			mpeg4$stuff$method$set(&r->atom, stsd_audio, set$data_reference_index) &&
			mpeg4$stuff$method$set(&r->atom, stsd_audio, set$minor_version) &&
			mpeg4$stuff$method$set(&r->atom, stsd_audio, set$audio_channel_count) &&
			mpeg4$stuff$method$set(&r->atom, stsd_audio, set$audio_sample_size) &&
			mpeg4$stuff$method$set(&r->atom, stsd_audio, set$rate)
		)
		return &r->atom;
		refer_free(r);
	}
	return NULL;
}

mpeg4$define$find(stsd_audio)
{
	static const mpeg4_box_type_t type = { .c = "soun" };
	static const mpeg4_box_type_t extra = { .c = "stsd" };
	return mpeg4_find_atom(inst, mpeg4$define(atom, stsd_audio, alloc), type.v, extra.v);
}
