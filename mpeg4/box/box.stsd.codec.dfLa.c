#include "box.include.h"
#include "inner.fullbox.h"
#include "inner.bindata.h"
#include "inner.data.h"

/*
$FLACMetadataBlock:
	bit last_metadata_block_flag;
	uint7_t block_type;
	uint24_t length;
	uint8_t data[length];

$FLACSpecificBox:
	$FullBox fullbox(box.type = 'dfLa', version = 0, flags = 0);
	$FLACMetadataBlock metadata_block[];
*/

typedef struct dfLa_metadata_info_t {
	uint8_t type;
	uint32_t length;
	uintptr_t offset;
} dfLa_metadata_info_t;

typedef struct mpeg4_stuff__stsd_dfLa_t {
	inner_fullbox_t fullbox;
	inner_array_t slot;
	inner_array_t buffer;
} mpeg4_stuff__stsd_dfLa_t;

typedef struct mpeg4_stuff__stsd_dfLa_s {
	mpeg4_stuff_t stuff;
	mpeg4_stuff__stsd_dfLa_t dfLa;
} mpeg4_stuff__stsd_dfLa_s;

static mpeg4$define$dump(stsd_codec_dfLa)
{
	inner_fullbox_t fullbox;
	uint32_t level, i;
	uint32_t length;
	uint8_t type;
	level = unidata->dump_level;
	if (!mpeg4$define(inner, fullbox, get)(&fullbox, &data, &size))
		goto label_fail;
	mpeg4$define(inner, fullbox, dump)(mlog, &fullbox, NULL, level);
	if (fullbox.version)
		goto label_fail;
	i = 0;
	while (size)
	{
		mlog_level_dump("flac_metadata_block[%u]:\n", i);
		level += mlog_level_width;
		if (!mpeg4$define(inner, uint8_t, get)(&type, &data, &size))
			goto label_fail;
		mlog_level_dump("last_metadata_block_flag: %u\n", !!(type & 0x80));
		mlog_level_dump("block_type:               %u\n", type & 0x7f);
		if (!mpeg4$define(inner, uint24_t, get)(&length, &data, &size))
			goto label_fail;
		mlog_level_dump("length:                   %u\n", length);
		if ((uint64_t) length > size)
			goto label_fail;
		mlog_level_dump("block_data:\n");
		if (length)
			mpeg4$define(inner, bindata, dump)(mlog, data, (uint64_t) length, 0, level + mlog_level_width);
		data += length;
		size -= length;
		level -= mlog_level_width;
		++i;
	}
	if (!size) return atom;
	label_fail:
	return NULL;
}

static mpeg4$define$stuff$init(stsd_codec_dfLa, mpeg4_stuff__stsd_dfLa_s)
{
	mpeg4$define(inner, array, init)(&r->dfLa.slot, sizeof(dfLa_metadata_info_t));
	mpeg4$define(inner, array, init)(&r->dfLa.buffer, 1);
	return &r->stuff;
}

static mpeg4$define$stuff$free(stsd_codec_dfLa, mpeg4_stuff__stsd_dfLa_s)
{
	mpeg4$define(inner, array, clear)(&r->dfLa.slot);
	mpeg4$define(inner, array, clear)(&r->dfLa.buffer);
	mpeg4_stuff_free_default_func(&r->stuff);
}

static mpeg4$define$create(stsd_codec_dfLa)
{
	return mpeg4_stuff_alloc(atom, inst, type,
		sizeof(mpeg4_stuff__stsd_dfLa_s),
		(mpeg4_stuff_init_f) mpeg4$define(stuff, stsd_codec_dfLa, init),
		(refer_free_f) mpeg4$define(stuff, stsd_codec_dfLa, free));
}

static mpeg4$define$parse(stsd_codec_dfLa)
{
	inner_fullbox_t fullbox;
	uint32_t length;
	uint8_t type;
	if (!mpeg4$define(inner, fullbox, get)(&fullbox, &data, &size))
		goto label_fail;
	if (!mpeg4$stuff$method$call(stuff, set$version_and_flags, fullbox.version, fullbox.flags))
		goto label_fail;
	while (size)
	{
		if (!mpeg4$define(inner, uint8_t, get)(&type, &data, &size))
			goto label_fail;
		if (!mpeg4$define(inner, uint24_t, get)(&length, &data, &size))
			goto label_fail;
		if ((uint64_t) length > size)
			goto label_fail;
		if (!mpeg4$stuff$method$call(stuff, add$dfLa_metadata, type & 0x7f, data, (uint64_t) length))
			goto label_fail;
		data += length;
		size -= length;
	}
	if (!size) return stuff;
	label_fail:
	return NULL;
}

static mpeg4$define$calc(stsd_codec_dfLa)
{
	const mpeg4_stuff__stsd_dfLa_t *restrict r = &((mpeg4_stuff__stsd_dfLa_s *) stuff)->dfLa;
	return mpeg4_stuff_calc_okay(stuff, sizeof(mpeg4_full_box_suffix_t) + r->slot.number * 4 + r->buffer.number);
}

static mpeg4$define$build(stsd_codec_dfLa)
{
	const mpeg4_stuff__stsd_dfLa_t *restrict r = &((mpeg4_stuff__stsd_dfLa_s *) stuff)->dfLa;
	const dfLa_metadata_info_t *restrict p;
	const uint8_t *restrict buffer;
	uintptr_t n;
	p = (const dfLa_metadata_info_t *) r->slot.array;
	buffer = (const uint8_t *) r->buffer.array;
	n = r->slot.number;
	data = mpeg4$define(inner, fullbox, set)(data, &r->fullbox);
	while (n)
	{
		--n;
		*data++ = p->type | (n?0:0x80);
		data = mpeg4$define(inner, uint24_t, set)(data, p->length);
		data = mpeg4$define(inner, data, set)(data, buffer + p->offset, (size_t) p->length);
	}
	return stuff;
}

static inner_method_set_fullbox(stsd_codec_dfLa, mpeg4_stuff__stsd_dfLa_s, dfLa.fullbox, 0);
static inner_method_get_fullbox(stsd_codec_dfLa, mpeg4_stuff__stsd_dfLa_s, dfLa.fullbox);

static const mpeg4_stuff_t* mpeg4$define(stuff, stsd_codec_dfLa, add$dfLa_metadata)(mpeg4_stuff__stsd_dfLa_s *restrict r, uint8_t type, const void *data, uint64_t size)
{
	dfLa_metadata_info_t *restrict p;
	if (size <= 0xffffff)
	{
		if ((p = (dfLa_metadata_info_t *) mpeg4$define(inner, array, append_point)(&r->dfLa.slot, 1)))
		{
			p->type = type & 0x7f;
			p->length = (uint32_t) size;
			p->offset = r->dfLa.buffer.number;
			if (mpeg4$define(inner, array, append_data)(&r->dfLa.buffer, (uintptr_t) size, data))
				return &r->stuff;
			--r->dfLa.slot.number;
		}
	}
	return NULL;
}

static const mpeg4$define$alloc(stsd_codec_dfLa)
{
	mpeg4_atom_s *restrict r;
	r = mpeg4_atom_alloc_empty();
	if (r)
	{
		r->interface.dump = mpeg4$define(atom, stsd_codec_dfLa, dump);
		r->interface.create = mpeg4$define(atom, stsd_codec_dfLa, create);
		r->interface.parse = mpeg4$define(atom, stsd_codec_dfLa, parse);
		r->interface.calc = mpeg4$define(atom, stsd_codec_dfLa, calc);
		r->interface.build = mpeg4$define(atom, stsd_codec_dfLa, build);
		if (
			mpeg4$stuff$method$set(r, stsd_codec_dfLa, set$version_and_flags) &&
			mpeg4$stuff$method$set(r, stsd_codec_dfLa, get$version_and_flags) &&
			mpeg4$stuff$method$set(r, stsd_codec_dfLa, add$dfLa_metadata)
		) return r;
		return r;
		refer_free(r);
	}
	return NULL;
}

mpeg4$define$find(stsd_codec_dfLa)
{
	static const mpeg4_box_type_t type = { .c = "dfLa" };
	static const mpeg4_box_type_t extra = { .c = "stsd" };
	return mpeg4_find_atom(inst, mpeg4$define(atom, stsd_codec_dfLa, alloc), type.v, extra.v);
}
