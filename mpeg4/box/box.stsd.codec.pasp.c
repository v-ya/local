#include "box.include.h"
#include "inner.data.h"

typedef struct pixel_aspect_ratio_t {
	uint32_t h_spacing;
	uint32_t v_spacing;
} __attribute__ ((packed)) pixel_aspect_ratio_t;

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

static const mpeg4$define$alloc(stsd_codec_pasp)
{
	mpeg4_atom_s *restrict r;
	r = mpeg4_atom_alloc_empty();
	if (r) r->interface.dump = mpeg4$define(atom, stsd_codec_pasp, dump);
	return r;
}

mpeg4$define$find(stsd_codec_pasp)
{
	static const mpeg4_box_type_t type = { .c = "pasp" };
	static const mpeg4_box_type_t extra = { .c = "stsd" };
	return mpeg4_find_atom(inst, mpeg4$define(atom, stsd_codec_pasp, alloc), type.v, extra.v);
}
