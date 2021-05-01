#include "box.include.h"
#include "inner.fullbox.h"
#include "inner.data.h"

typedef struct video_media_header_uni_t {
	uint16_t graphicsmode;
	uint16_t opcolor[3];
} __attribute__ ((packed)) video_media_header_uni_t;

typedef struct mpeg4_stuff__video_media_header_t {
	inner_fullbox_t fullbox;
	uint16_t graphicsmode;
	uint16_t opcolor[3];
} mpeg4_stuff__video_media_header_t;

typedef struct mpeg4_stuff__video_media_header_s {
	mpeg4_stuff_t stuff;
	mpeg4_stuff__video_media_header_t pri;
} mpeg4_stuff__video_media_header_s;

static mpeg4$define$dump(vmhd)
{
	inner_fullbox_t fullbox;
	video_media_header_uni_t header;
	uint32_t level = unidata->dump_level;
	if (!mpeg4$define(inner, fullbox, get)(&fullbox, &data, &size))
		goto label_fail;
	mpeg4$define(inner, fullbox, dump)(mlog, &fullbox, NULL, level);
	if (fullbox.version)
		goto label_fail;
	if (!mpeg4$define(inner, data, get)(&header, sizeof(header), &data, &size))
		goto label_fail;
	header.graphicsmode = mpeg4_n16(header.graphicsmode);
	header.opcolor[0] = mpeg4_n16(header.opcolor[0]);
	header.opcolor[1] = mpeg4_n16(header.opcolor[1]);
	header.opcolor[2] = mpeg4_n16(header.opcolor[2]);
	mlog_level_dump("graphicsmode: %04x (%u)\n", header.graphicsmode, header.graphicsmode);
	mlog_level_dump("opcolor:      (%04x (%u), %04x (%u), %04x (%u))\n",
		header.opcolor[0], header.opcolor[0],
		header.opcolor[1], header.opcolor[1],
		header.opcolor[2], header.opcolor[2]);
	if (!size) return atom;
	label_fail:
	return NULL;
}

static mpeg4$define$stuff$init(vmhd, mpeg4_stuff__video_media_header_s)
{
	r->pri.fullbox.flags = 1;
	return &r->stuff;
}

static mpeg4$define$create(vmhd)
{
	return mpeg4_stuff_alloc(atom, inst, type,
		sizeof(mpeg4_stuff__video_media_header_s),
		(mpeg4_stuff_init_f) mpeg4$define(stuff, vmhd, init),
		NULL);
}

static mpeg4$define$parse(vmhd)
{
	inner_fullbox_t fullbox;
	video_media_header_uni_t header;
	if (!mpeg4$define(inner, fullbox, get)(&fullbox, &data, &size))
		goto label_fail;
	if (!mpeg4$stuff$method$call(stuff, set$version_and_flags, fullbox.version, fullbox.flags))
		goto label_fail;
	if (!mpeg4$define(inner, data, get)(&header, sizeof(header), &data, &size))
		goto label_fail;
	if (!mpeg4$stuff$method$call(stuff, set$graphicsmode, mpeg4_n16(header.graphicsmode)))
		goto label_fail;
	if (!mpeg4$stuff$method$call(stuff, set$opcolor,
		mpeg4_n16(header.opcolor[0]),
		mpeg4_n16(header.opcolor[1]),
		mpeg4_n16(header.opcolor[2])))
		goto label_fail;
	if (!size) return stuff;
	label_fail:
	return NULL;
}

static mpeg4$define$calc(vmhd)
{
	return mpeg4_stuff_calc_okay(stuff, sizeof(mpeg4_full_box_suffix_t) + sizeof(video_media_header_uni_t));
}

static mpeg4$define$build(vmhd)
{
	mpeg4_stuff__video_media_header_t *restrict r = &((mpeg4_stuff__video_media_header_s *) stuff)->pri;
	video_media_header_uni_t header;
	header.graphicsmode = mpeg4_n16(r->graphicsmode);
	header.opcolor[0] = mpeg4_n16(r->opcolor[0]);
	header.opcolor[1] = mpeg4_n16(r->opcolor[1]);
	header.opcolor[2] = mpeg4_n16(r->opcolor[2]);
	data = mpeg4$define(inner, fullbox, set)(data, &r->fullbox);
	mpeg4$define(inner, data, set)(data, &header, sizeof(header));
	return stuff;
}

static const mpeg4_stuff_t* mpeg4$define(stuff, vmhd, set$version_and_flags)(mpeg4_stuff__video_media_header_s *restrict r, uint32_t version, uint32_t flags)
{
	if (version) goto label_fail;
	r->pri.fullbox.version = version;
	r->pri.fullbox.flags = flags;
	return &r->stuff;
	label_fail:
	return NULL;
}

static const mpeg4_stuff_t* mpeg4$define(stuff, vmhd, set$graphicsmode)(mpeg4_stuff__video_media_header_s *restrict r, uint16_t graphicsmode)
{
	r->pri.graphicsmode = graphicsmode;
	return &r->stuff;
}

static const mpeg4_stuff_t* mpeg4$define(stuff, vmhd, set$opcolor)(mpeg4_stuff__video_media_header_s *restrict r, uint16_t red, uint16_t green, uint16_t blue)
{
	r->pri.opcolor[0] = red;
	r->pri.opcolor[1] = green;
	r->pri.opcolor[2] = blue;
	return &r->stuff;
}

static const mpeg4$define$alloc(vmhd)
{
	mpeg4_atom_s *restrict r;
	r = mpeg4_atom_alloc_empty();
	if (r)
	{
		r->interface.dump = mpeg4$define(atom, vmhd, dump);
		r->interface.create = mpeg4$define(atom, vmhd, create);
		r->interface.parse = mpeg4$define(atom, vmhd, parse);
		r->interface.calc = mpeg4$define(atom, vmhd, calc);
		r->interface.build = mpeg4$define(atom, vmhd, build);
		if (
			mpeg4$stuff$method$set(r, vmhd, set$version_and_flags) &&
			mpeg4$stuff$method$set(r, vmhd, set$graphicsmode) &&
			mpeg4$stuff$method$set(r, vmhd, set$opcolor)
		) return r;
		refer_free(r);
	}
	return NULL;
}

mpeg4$define$find(vmhd)
{
	static const mpeg4_box_type_t type = { .c = "vmhd" };
	return mpeg4_find_atom(inst, mpeg4$define(atom, vmhd, alloc), type.v, 0);
}
