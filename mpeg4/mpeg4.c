#include "mpeg4.h"
#include "mpeg4.inst.h"
#include "mpeg4.box.h"
#include "box/box.include.h"

static void mpeg4_free_func(mpeg4_s *restrict r)
{
	if (r->type) rbtree_clear(&r->type);
	if (r->uuid) rbtree_clear(&r->uuid);
}

static mpeg4_s* mpeg4_preset_type(mpeg4_s *restrict r);
mpeg4_s* mpeg4_alloc(void)
{
	mpeg4_s *restrict r;
	if ((r = (mpeg4_s *) refer_alloz(sizeof(mpeg4_s))))
	{
		refer_set_free(r, (refer_free_f) mpeg4_free_func);
		if (mpeg4_preset_type(r))
			return r;
		refer_free(r);
	}
	return NULL;
}

const mpeg4_s* mpeg4_dump(const mpeg4_s *restrict inst, mlog_s *restrict mlog, const uint8_t *restrict data, uint64_t size)
{
	mpeg4_dump_data_t unidata = {
		.dump_samples = 0,
		.timescale = 1
	};
	return mpeg4_dump_level(inst, mlog, data, size, &unidata, 0);
}

static mpeg4_box_type_t mpeg4_preset_type_parse(const char *restrict type)
{
	mpeg4_box_type_t r;
	r.v = 0;
	if ((r.c[0] = type[0]) && (r.c[1] = type[1]) && (r.c[2] = type[2]) && (r.c[3] = type[3]))
		return r;
	return r;
}

static mpeg4_s* mpeg4_preset_type(mpeg4_s *restrict r)
{
	mpeg4_func_box_t func;
	#define _d_type(_type, _name)  \
		func.dump = mpeg4$define(box, _name, dump);\
		if (!mpeg4_set_type(r, mpeg4_preset_type_parse(#_type).v, &func)) goto label_fail;
	#define d_type(_type)  _d_type(_type, _type)
	_d_type(, unknow);
	_d_type(moov, container);
	_d_type(trak, container);
	_d_type(mdia, container);
	_d_type(minf, container);
	_d_type(dinf, container);
	_d_type(stbl, container);
	_d_type(edts, container);
	_d_type(udta, container);
	// _d_type(ilst, container);
	_d_type(dref, container_count);
	_d_type(stsd, container_count);
	_d_type(free, free_skip);
	_d_type(skip, free_skip);
	d_type(ftyp);
	d_type(mdat);
	d_type(mvhd);
	d_type(tkhd);
	d_type(mdhd);
	d_type(hdlr);
	d_type(nmhd);
	d_type(elng);
	_d_type(url\x20, url_);
	_d_type(urn\x20, urn_);
	d_type(stts);
	d_type(ctts);
	d_type(stss);
	d_type(stsz);
	d_type(stsc);
	d_type(stco);
	d_type(co64);
	d_type(elst);
	d_type(meta);
	#undef _d_type
	#undef d_type
	return r;
	label_fail:
	return NULL;
}
