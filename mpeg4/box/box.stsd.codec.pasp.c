#include "box.include.h"
#include "inner.data.h"

typedef struct pixel_aspect_ratio_t {
	uint32_t h_spacing;
	uint32_t v_spacing;
} __attribute__ ((packed)) pixel_aspect_ratio_t;

typedef struct mpeg4_stuff__stsd_pixel_aspect_ratio_s {
	mpeg4_stuff_t stuff;
	pixel_aspect_ratio_t pixel_aspect_ratio;
} mpeg4_stuff__stsd_pixel_aspect_ratio_s;

static mpeg4$define$dump(stsd_codec_pasp)
{
	pixel_aspect_ratio_t pixel_aspect_ratio;
	uint32_t level = unidata->dump_level;
	if (!mpeg4$define(inner, data, get)(&pixel_aspect_ratio, sizeof(pixel_aspect_ratio), &data, &size))
		goto label_fail;
	mlog_level_dump("h-spacing: %u\n", mpeg4_n32(pixel_aspect_ratio.h_spacing));
	mlog_level_dump("v-spacing: %u\n", mpeg4_n32(pixel_aspect_ratio.v_spacing));
	if (!size) return atom;
	label_fail:
	return NULL;
}

static mpeg4$define$create(stsd_codec_pasp)
{
	return mpeg4_stuff_alloc(atom, inst, type, sizeof(mpeg4_stuff__stsd_pixel_aspect_ratio_s), NULL, NULL);
}

static mpeg4$define$parse(stsd_codec_pasp)
{
	pixel_aspect_ratio_t pixel_aspect_ratio;
	if (!mpeg4$define(inner, data, get)(&pixel_aspect_ratio, sizeof(pixel_aspect_ratio), &data, &size))
		goto label_fail;
	if (!mpeg4$stuff$method$call(stuff, set$pixel_aspect_ratio,
		mpeg4_n32(pixel_aspect_ratio.h_spacing),
		mpeg4_n32(pixel_aspect_ratio.v_spacing)))
		goto label_fail;
	if (!size) return stuff;
	label_fail:
	return NULL;
}

static mpeg4$define$calc(stsd_codec_pasp)
{
	return mpeg4_stuff_calc_okay(stuff, sizeof(pixel_aspect_ratio_t));
}

static mpeg4$define$build(stsd_codec_pasp)
{
	const pixel_aspect_ratio_t *restrict r = &((mpeg4_stuff__stsd_pixel_aspect_ratio_s *) stuff)->pixel_aspect_ratio;
	pixel_aspect_ratio_t pixel_aspect_ratio;
	pixel_aspect_ratio.h_spacing = mpeg4_n32(r->h_spacing);
	pixel_aspect_ratio.v_spacing = mpeg4_n32(r->v_spacing);
	mpeg4$define(inner, data, set)(data, &pixel_aspect_ratio, sizeof(pixel_aspect_ratio));
	return stuff;
}

static const mpeg4_stuff_t* mpeg4$define(stuff, stsd_codec_pasp, set$pixel_aspect_ratio)(mpeg4_stuff__stsd_pixel_aspect_ratio_s *restrict r, uint32_t h_spacing, uint32_t v_spacing)
{
	r->pixel_aspect_ratio.h_spacing = h_spacing;
	r->pixel_aspect_ratio.v_spacing = v_spacing;
	return &r->stuff;
}

static const mpeg4$define$alloc(stsd_codec_pasp)
{
	mpeg4_atom_s *restrict r;
	r = mpeg4_atom_alloc_empty();
	if (r)
	{
		r->interface.dump = mpeg4$define(atom, stsd_codec_pasp, dump);
		r->interface.create = mpeg4$define(atom, stsd_codec_pasp, create);
		r->interface.parse = mpeg4$define(atom, stsd_codec_pasp, parse);
		r->interface.calc = mpeg4$define(atom, stsd_codec_pasp, calc);
		r->interface.build = mpeg4$define(atom, stsd_codec_pasp, build);
		if (mpeg4$stuff$method$set(r, stsd_codec_pasp, set$pixel_aspect_ratio))
			return r;
		refer_free(r);
	}
	return NULL;
}

mpeg4$define$find(stsd_codec_pasp)
{
	static const mpeg4_box_type_t type = { .c = "pasp" };
	static const mpeg4_box_type_t extra = { .c = "stsd" };
	return mpeg4_find_atom(inst, mpeg4$define(atom, stsd_codec_pasp, alloc), type.v, extra.v);
}
