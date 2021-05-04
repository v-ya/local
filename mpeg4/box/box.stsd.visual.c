#include "box.container.h"
#include "inner.sample_entry.h"
#include "inner.type.h"
#include "inner.data.h"
#include <string.h>

typedef struct mpeg4_stuff__stsd_visual_s {
	mpeg4_stuff_t stuff;
	inner_visual_sample_t sample;
} mpeg4_stuff__stsd_visual_s;

static mpeg4$define$dump(stsd_visual)
{
	inner_visual_sample_t sample;
	uint32_t level = unidata->dump_level;
	if (!mpeg4$define(inner, visual_sample, get)(&sample, &data, &size))
		goto label_fail;
	mlog_level_dump("data reference index: %u\n", sample.data_reference_index);
	mlog_level_dump("width:                %u pixels\n", sample.width);
	mlog_level_dump("height:               %u pixels\n", sample.height);
	mlog_level_dump("horizre solution:     %g ppi\n", sample.horizre_solution);
	mlog_level_dump("vertre solution:      %g ppi\n", sample.vertre_solution);
	mlog_level_dump("frame count:          %u\n", sample.frame_count);
	mlog_level_dump("compressor name:      %s\n", sample.compressor_name);
	mlog_level_dump("depth:                %u\n", sample.depth);
	return mpeg4$define(atom, container, dump)(atom, mlog, data, size, unidata);
	label_fail:
	return NULL;
}

static mpeg4$define$stuff$init(stsd_visual, mpeg4_stuff__stsd_visual_s)
{
	r->sample.horizre_solution = 72;
	r->sample.vertre_solution = 72;
	r->sample.frame_count = 1;
	r->sample.depth = 24;
	return &r->stuff;
}

static mpeg4$define$create(stsd_visual)
{
	return mpeg4_stuff_alloc(atom, inst, type,
		sizeof(mpeg4_stuff__stsd_visual_s),
		(mpeg4_stuff_init_f) mpeg4$define(stuff, stsd_visual, init),
		NULL);
}

static mpeg4$define$parse(stsd_visual)
{
	inner_visual_sample_t sample;
	if (!mpeg4$define(inner, visual_sample, get)(&sample, &data, &size))
		goto label_fail;
	if (!mpeg4$stuff$method$call(stuff, set$data_reference_index, sample.data_reference_index))
		goto label_fail;
	if (!mpeg4$stuff$method$call(stuff, set$pixel_resolution, sample.width, sample.height))
		goto label_fail;
	if (!mpeg4$stuff$method$call(stuff, set$ppi_resolution, sample.horizre_solution, sample.vertre_solution))
		goto label_fail;
	if (!mpeg4$stuff$method$call(stuff, set$frame_per_sample, sample.frame_count))
		goto label_fail;
	if (!mpeg4$stuff$method$call(stuff, set$depth, sample.depth))
		goto label_fail;
	if (!mpeg4$stuff$method$call(stuff, set$name, sample.compressor_name))
		goto label_fail;
	return mpeg4$define(atom, container, parse)(stuff, data, size);
	label_fail:
	return NULL;
}

static mpeg4$define$calc(stsd_visual)
{
	if (mpeg4$define(atom, container, calc)(stuff))
		return mpeg4_stuff_calc_okay(stuff, stuff->info.inner_size + sizeof(inner_visual_sample_entry_t));
	return NULL;
}

static mpeg4$define$build(stsd_visual)
{
	data = mpeg4$define(inner, visual_sample, set)(data, &((mpeg4_stuff__stsd_visual_s *) stuff)->sample);
	return mpeg4$define(atom, container, build)(stuff, data);
}

static inner_method_set_simple_param(stsd_visual, data_reference_index, mpeg4_stuff__stsd_visual_s, uint16_t, sample.data_reference_index);

static const mpeg4_stuff_t* mpeg4$define(stuff, stsd_visual, set$pixel_resolution)(mpeg4_stuff__stsd_visual_s *restrict r, uint16_t width, uint16_t height)
{
	r->sample.width = width;
	r->sample.height = height;
	return &r->stuff;
}

static const mpeg4_stuff_t* mpeg4$define(stuff, stsd_visual, set$ppi_resolution)(mpeg4_stuff__stsd_visual_s *restrict r, double horizre, double vertre)
{
	if (horizre < 0) horizre = 0;
	if (vertre < 0) vertre = 0;
	r->sample.horizre_solution = horizre;
	r->sample.vertre_solution = vertre;
	return &r->stuff;
}

static inner_method_set_simple_param(stsd_visual, frame_per_sample, mpeg4_stuff__stsd_visual_s, uint16_t, sample.frame_count);
static inner_method_set_simple_param(stsd_visual, depth, mpeg4_stuff__stsd_visual_s, uint16_t, sample.depth);

static const mpeg4_stuff_t* mpeg4$define(stuff, stsd_visual, set$name)(mpeg4_stuff__stsd_visual_s *restrict r, const char *restrict name)
{
	if (name)
	{
		size_t n = strlen(name) + 1;
		if (n > sizeof(r->sample.compressor_name))
			goto label_fail;
		memcpy(r->sample.compressor_name, name, n);
	}
	else r->sample.compressor_name[0] = 0;
	return &r->stuff;
	label_fail:
	return NULL;
}

static const mpeg4$define$alloc(stsd_visual)
{
	mpeg4_atom$container_s *restrict r;
	r = (mpeg4_atom$container_s *) refer_alloz(sizeof(mpeg4_atom$container_s));
	if (r)
	{
		refer_set_free(r, (refer_free_f) mpeg4_atom_free_default_func);
		r->atom.interface.dump = mpeg4$define(atom, stsd_visual, dump);
		r->atom.interface.create = mpeg4$define(atom, stsd_visual, create);
		r->atom.interface.parse = mpeg4$define(atom, stsd_visual, parse);
		r->atom.interface.calc = mpeg4$define(atom, stsd_visual, calc);
		r->atom.interface.build = mpeg4$define(atom, stsd_visual, build);
		r->inst = inst;
		if (
			mpeg4$inner$type$alloc$layer2(&r->atom, inst, data, "avcC") &&
			mpeg4$inner$type$alloc$layer2(&r->atom, inst, data, "av1C") &&
			mpeg4$stuff$method$set(&r->atom, stsd_visual, set$data_reference_index) &&
			mpeg4$stuff$method$set(&r->atom, stsd_visual, set$pixel_resolution) &&
			mpeg4$stuff$method$set(&r->atom, stsd_visual, set$ppi_resolution) &&
			mpeg4$stuff$method$set(&r->atom, stsd_visual, set$frame_per_sample) &&
			mpeg4$stuff$method$set(&r->atom, stsd_visual, set$depth) &&
			mpeg4$stuff$method$set(&r->atom, stsd_visual, set$name)
		)
		return &r->atom;
		refer_free(r);
	}
	return NULL;
}

mpeg4$define$find(stsd_visual)
{
	static const mpeg4_box_type_t type = { .c = "vide" };
	static const mpeg4_box_type_t extra = { .c = "stsd" };
	return mpeg4_find_atom(inst, mpeg4$define(atom, stsd_visual, alloc), type.v, extra.v);
}
