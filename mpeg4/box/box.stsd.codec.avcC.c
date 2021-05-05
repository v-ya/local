#include "box.include.h"
#include "inner.bindata.h"
#include "inner.data.h"

/*
	uint8_t configuration_version = 1;
	uint8_t avc_profile_indication;
	uint8_t profile_compatibility;
	uint8_t avc_level_indication;
	bit6_t reserved = 0b111111;
	uint2_t length_size_minus_1;
	bit3_t reserved = 0b111;
	uint5_t num_of_sequence_parameter_sets;
	for (i = 0; i < num_of_sequence_parameter_sets; ++i)
	{
		uint16_t sequence_parameter_set_length;
		uint8_t sequence_parameter_set_nal_unit[sequence_parameter_set_length];
	}
	uint8_t num_of_picture_parameter_sets;
	for (i = 0; i < num_of_picture_parameter_sets; ++i)
	{
		uint16_t picture_parameter_set_length;
		uint8_t picture_parameter_set_nal_unit[picture_parameter_set_length];
	}
	if (profile_idc == 100 || profile_idc == 110 || profile_idc == 122 || profile_idc == 144)
	{
		bit6_t reserved = 0b111111;
		uint2_t chroma_format;
		bit5_t reserved = 0b11111;
		uint3_t bit_depth_luma_minus_8;
		bit5_t reserved = 0b11111;
		uint3_t bit_depth_chroma_minus_8;
		uint8_t num_of_sequence_parameter_set_ext;
		for (i = 0; i < num_of_sequence_parameter_set_ext; ++i)
		{
			uint16_t sequence_parameter_set_ext_length;
			uint8_t sequence_parameter_set_ext_nal_unit[sequence_parameter_set_ext_length];
		}
	}
*/

typedef struct avcC_data_t {
	struct avcC_data_t *next;
	uintptr_t offset;
	uintptr_t length;
} avcC_data_t;

typedef struct avcC_indication_t {
	uint8_t configuration_version;
	uint8_t avc_profile_indication;
	uint8_t profile_compatibility;
	uint8_t avc_level_indication;
} avcC_indication_t;

typedef struct avc_config_t {
	avcC_indication_t indication;
	uint8_t length_size;
	uint8_t chroma_format;
	uint8_t bit_depth_luma;
	uint8_t bit_depth_chroma;
	uintptr_t enable_extra;
	uintptr_t n_sps;
	uintptr_t n_pps;
	uintptr_t n_spse;
	avcC_data_t *sps;
	avcC_data_t *pps;
	avcC_data_t *spse;
	avcC_data_t **tail_sps;
	avcC_data_t **tail_pps;
	avcC_data_t **tail_spse;
	inner_array_t slot;
	inner_array_t buffer;
} avc_config_t;

typedef struct mpeg4_stuff__stsd_avcC_s {
	mpeg4_stuff_t stuff;
	avc_config_t avc_config;
} mpeg4_stuff__stsd_avcC_s;

static mpeg4$define$dump(stsd_codec_avcC)
{
	avcC_indication_t indication;
	uint32_t level, i, n;
	uint16_t length;
	uint8_t b;
	level = unidata->dump_level;
	if (!mpeg4$define(inner, data, get)(&indication, sizeof(indication), &data, &size))
		goto label_fail;
	mlog_level_dump("configuration_version:             %u\n", indication.configuration_version);
	if (indication.configuration_version != 1)
		goto label_fail;
	mlog_level_dump("avc_profile_indication:            %u\n", indication.avc_profile_indication);
	mlog_level_dump("profile_compatibility:             %u\n", indication.profile_compatibility);
	mlog_level_dump("avc_level_indication:              %u\n", indication.avc_level_indication);
	if (!mpeg4$define(inner, uint8_t, get)(&b, &data, &size))
		goto label_fail;
	mlog_level_dump("length_size:                       %u\n", (b & 3) + 1);
	if (!mpeg4$define(inner, uint8_t, get)(&b, &data, &size))
		goto label_fail;
	n = b & 0x1f;
	mlog_level_dump("num_of_sequence_parameter_sets:    %u\n", n);
	if (n)
	{
		level += mlog_level_width;
		for (i = 0; i < n; ++i)
		{
			if (!mpeg4$define(inner, uint16_t, get)(&length, &data, &size))
				goto label_fail;
			mlog_level_dump("sps[%u]: %u bytes\n", i, length);
			if ((uint64_t) length > size)
				goto label_fail;
			mpeg4$define(inner, bindata, dump)(mlog, data, (uint64_t) length, 0, level + mlog_level_width);
			data += length;
			size -= length;
		}
		level -= mlog_level_width;
	}
	if (!mpeg4$define(inner, uint8_t, get)(&b, &data, &size))
		goto label_fail;
	n = b;
	mlog_level_dump("num_of_picture_parameter_sets:     %u\n", n);
	if (n)
	{
		level += mlog_level_width;
		for (i = 0; i < n; ++i)
		{
			if (!mpeg4$define(inner, uint16_t, get)(&length, &data, &size))
				goto label_fail;
			mlog_level_dump("pps[%u]: %u bytes\n", i, length);
			if ((uint64_t) length > size)
				goto label_fail;
			mpeg4$define(inner, bindata, dump)(mlog, data, (uint64_t) length, 0, level + mlog_level_width);
			data += length;
			size -= length;
		}
		level -= mlog_level_width;
	}
	if (size)
	{
		if (!mpeg4$define(inner, uint8_t, get)(&b, &data, &size))
			goto label_fail;
		mlog_level_dump("chroma_format:                     %u\n", b & 3);
		if (!mpeg4$define(inner, uint8_t, get)(&b, &data, &size))
			goto label_fail;
		mlog_level_dump("bit_depth_luma:                    %u\n", (b & 7) + 8);
		if (!mpeg4$define(inner, uint8_t, get)(&b, &data, &size))
			goto label_fail;
		mlog_level_dump("bit_depth_chroma:                  %u\n", (b & 7) + 8);
		if (!mpeg4$define(inner, uint8_t, get)(&b, &data, &size))
			goto label_fail;
		n = b;
		mlog_level_dump("num_of_sequence_parameter_set_ext: %u\n", n);
		if (n)
		{
			level += mlog_level_width;
			for (i = 0; i < n; ++i)
			{
				if (!mpeg4$define(inner, uint16_t, get)(&length, &data, &size))
					goto label_fail;
				mlog_level_dump("spse[%u]: %u bytes\n", i, length);
				if ((uint64_t) length > size)
					goto label_fail;
				mpeg4$define(inner, bindata, dump)(mlog, data, (uint64_t) length, 0, level + mlog_level_width);
				data += length;
				size -= length;
			}
			level -= mlog_level_width;
		}
	}
	if (!size) return atom;
	label_fail:
	return NULL;
}

static mpeg4$define$stuff$init(stsd_codec_avcC, mpeg4_stuff__stsd_avcC_s)
{
	r->avc_config.indication.configuration_version = 1;
	r->avc_config.length_size = 4;
	r->avc_config.bit_depth_luma = 8;
	r->avc_config.bit_depth_chroma = 8;
	r->avc_config.tail_sps = &r->avc_config.sps;
	r->avc_config.tail_pps = &r->avc_config.pps;
	r->avc_config.tail_spse = &r->avc_config.spse;
	mpeg4$define(inner, array, init)(&r->avc_config.slot, sizeof(avcC_data_t));
	mpeg4$define(inner, array, init)(&r->avc_config.buffer, 1);
	return &r->stuff;
}

static mpeg4$define$stuff$free(stsd_codec_avcC, mpeg4_stuff__stsd_avcC_s)
{
	mpeg4$define(inner, array, clear)(&r->avc_config.slot);
	mpeg4$define(inner, array, clear)(&r->avc_config.buffer);
	mpeg4_stuff_free_default_func(&r->stuff);
}

static mpeg4$define$create(stsd_codec_avcC)
{
	return mpeg4_stuff_alloc(atom, inst, type,
		sizeof(mpeg4_stuff__stsd_avcC_s),
		(mpeg4_stuff_init_f) mpeg4$define(stuff, stsd_codec_avcC, init),
		(refer_free_f) mpeg4$define(stuff, stsd_codec_avcC, free));
}

static mpeg4$define$parse(stsd_codec_avcC)
{
	avcC_indication_t indication;
	uint32_t n;
	uint16_t length;
	uint8_t b;
	if (!mpeg4$define(inner, data, get)(&indication, sizeof(indication), &data, &size))
		goto label_fail;
	if (indication.configuration_version != 1)
		goto label_fail;
	if (!mpeg4$define(inner, uint8_t, get)(&b, &data, &size))
		goto label_fail;
	if (!mpeg4$stuff$method$call(stuff, set$avcC_indication,
		indication.avc_profile_indication,
		indication.profile_compatibility,
		indication.avc_level_indication,
		(b & 3) + 1))
		goto label_fail;
	if (!mpeg4$define(inner, uint8_t, get)(&b, &data, &size))
		goto label_fail;
	n = b & 0x1f;
	while (n)
	{
		if (!mpeg4$define(inner, uint16_t, get)(&length, &data, &size))
			goto label_fail;
		if ((uint64_t) length > size)
			goto label_fail;
		if (!mpeg4$stuff$method$call(stuff, add$avcC_sps, data, (uint64_t) length))
			goto label_fail;
		data += length;
		size -= length;
		--n;
	}
	if (!mpeg4$define(inner, uint8_t, get)(&b, &data, &size))
		goto label_fail;
	n = b;
	while (n)
	{
		if (!mpeg4$define(inner, uint16_t, get)(&length, &data, &size))
			goto label_fail;
		if ((uint64_t) length > size)
			goto label_fail;
		if (!mpeg4$stuff$method$call(stuff, add$avcC_pps, data, (uint64_t) length))
			goto label_fail;
		data += length;
		size -= length;
		--n;
	}
	if (size)
	{
		uint8_t chroma_format, bit_depth_luma, bit_depth_chroma;
		if (!mpeg4$define(inner, uint8_t, get)(&b, &data, &size))
			goto label_fail;
		chroma_format = b & 3;
		if (!mpeg4$define(inner, uint8_t, get)(&b, &data, &size))
			goto label_fail;
		bit_depth_luma = (b & 7) + 8;
		if (!mpeg4$define(inner, uint8_t, get)(&b, &data, &size))
			goto label_fail;
		bit_depth_chroma = (b & 7) + 8;
		if (!mpeg4$stuff$method$call(stuff, set$avcC_extra, chroma_format, bit_depth_luma, bit_depth_chroma))
			goto label_fail;
		if (!mpeg4$define(inner, uint8_t, get)(&b, &data, &size))
			goto label_fail;
		n = b;
		while (n)
		{
			if (!mpeg4$define(inner, uint16_t, get)(&length, &data, &size))
				goto label_fail;
			if ((uint64_t) length > size)
				goto label_fail;
			if (!mpeg4$stuff$method$call(stuff, add$avcC_extra_spse, data, (uint64_t) length))
				goto label_fail;
			data += length;
			size -= length;
			--n;
		}
	}
	if (!size) return stuff;
	label_fail:
	return NULL;
}

static mpeg4$define$calc(stsd_codec_avcC)
{
	const avc_config_t *restrict r;
	const avcC_data_t *restrict p;
	uint64_t size;
	r = &((mpeg4_stuff__stsd_avcC_s *) stuff)->avc_config;
	size = sizeof(avcC_indication_t)  // configuration_version && avc_profile_indication && profile_compatibility && avc_level_indication
		+ 1                       // length_size_minus_1
		+ 1                       // num_of_sequence_parameter_sets
		+ 1;                      // num_of_picture_parameter_sets
	if ((p = r->sps)) while (p)
	{
		size += (uint64_t) p->length + 2;
		p = p->next;
	}
	if ((p = r->pps)) while (p)
	{
		size += (uint64_t) p->length + 2;
		p = p->next;
	}
	if (r->enable_extra)
	{
		size += 1    // chroma_format
			+ 1  // bit_depth_luma_minus_8
			+ 1  // bit_depth_chroma_minus_8
			+ 1; // num_of_sequence_parameter_set_ext
		if ((p = r->spse)) while (p)
		{
			size += (uint64_t) p->length + 2;
			p = p->next;
		}
	}
	return mpeg4_stuff_calc_okay(stuff, size);
}

static mpeg4$define$build(stsd_codec_avcC)
{
	const avc_config_t *restrict r;
	const avcC_data_t *restrict p;
	const uint8_t *restrict buffer;
	r = &((mpeg4_stuff__stsd_avcC_s *) stuff)->avc_config;
	buffer = (const uint8_t *) r->buffer.array;
	data = mpeg4$define(inner, data, set)(data, &r->indication, sizeof(r->indication));
	*data++ = (r->length_size - 1) | 0xfc;
	*data++ = (uint8_t) r->n_sps | 0xe0;
	if ((p = r->sps)) while (p)
	{
		data = mpeg4$define(inner, uint16_t, set)(data, (uint16_t) p->length);
		data = mpeg4$define(inner, data, set)(data, buffer + p->offset, p->length);
		p = p->next;
	}
	*data++ = (uint8_t) r->n_pps;
	if ((p = r->pps)) while (p)
	{
		data = mpeg4$define(inner, uint16_t, set)(data, (uint16_t) p->length);
		data = mpeg4$define(inner, data, set)(data, buffer + p->offset, p->length);
		p = p->next;
	}
	if (r->enable_extra)
	{
		*data++ = r->chroma_format | 0xfc;
		*data++ = (r->bit_depth_luma - 8) | 0xf8;
		*data++ = (r->bit_depth_chroma - 8) | 0xf8;
		*data++ = (uint8_t) r->n_spse;
		if ((p = r->spse)) while (p)
		{
			data = mpeg4$define(inner, uint16_t, set)(data, (uint16_t) p->length);
			data = mpeg4$define(inner, data, set)(data, buffer + p->offset, p->length);
			p = p->next;
		}
	}
	return stuff;
}

static const mpeg4_stuff_t* mpeg4$define(stuff, stsd_codec_avcC, set$avcC_indication)(mpeg4_stuff__stsd_avcC_s *restrict r, uint8_t avc_profile_indication, uint8_t profile_compatibility, uint8_t avc_level_indication, uint8_t length_size)
{
	if (length_size == 1 || length_size == 2 || length_size == 4)
	{
		r->avc_config.indication.avc_profile_indication = avc_profile_indication;
		r->avc_config.indication.profile_compatibility = profile_compatibility;
		r->avc_config.indication.avc_level_indication = avc_level_indication;
		r->avc_config.length_size = length_size;
		return &r->stuff;
	}
	return NULL;
}

static const mpeg4_stuff_t* mpeg4$define(stuff, stsd_codec_avcC, set$avcC_extra)(mpeg4_stuff__stsd_avcC_s *restrict r, uint8_t chroma_format, uint8_t bit_depth_luma, uint8_t bit_depth_chroma)
{
	// chroma_format:    [0, 3)
	// bit_depth_luma:   [8, 15)
	// bit_depth_chroma: [8, 15)
	if (chroma_format < 4 &&
		bit_depth_luma >= 8 && bit_depth_luma < 16 &&
		bit_depth_chroma >= 8 && bit_depth_chroma < 16)
	{
		r->avc_config.chroma_format = chroma_format;
		r->avc_config.bit_depth_luma = bit_depth_luma;
		r->avc_config.bit_depth_chroma = bit_depth_chroma;
		r->avc_config.enable_extra = 1;
		return &r->stuff;
	}
	return NULL;
}

static const mpeg4_stuff_t* mpeg4$define(stuff, stsd_codec_avcC, add$avcC_sps)(mpeg4_stuff__stsd_avcC_s *restrict r, const void *data, uint64_t size)
{
	avcC_data_t *restrict p;
	// n_sps: [0, 31]
	if (r->avc_config.n_sps < 31 && size && size <= 0xffff)
	{
		if ((p = (avcC_data_t *) mpeg4$define(inner, array, append_point)(&r->avc_config.slot, 1)))
		{
			p->next = NULL;
			p->offset = r->avc_config.buffer.number;
			p->length = (uintptr_t) size;
			if (mpeg4$define(inner, array, append_data)(&r->avc_config.buffer, (uintptr_t) size, data))
			{
				*r->avc_config.tail_sps = p;
				r->avc_config.tail_sps = &p->next;
				++r->avc_config.n_sps;
				return &r->stuff;
			}
			--r->avc_config.slot.number;
		}
	}
	return NULL;
}

static const mpeg4_stuff_t* mpeg4$define(stuff, stsd_codec_avcC, add$avcC_pps)(mpeg4_stuff__stsd_avcC_s *restrict r, const void *data, uint64_t size)
{
	avcC_data_t *restrict p;
	// n_pps: [0, 255]
	if (r->avc_config.n_pps < 255 && size && size <= 0xffff)
	{
		if ((p = (avcC_data_t *) mpeg4$define(inner, array, append_point)(&r->avc_config.slot, 1)))
		{
			p->next = NULL;
			p->offset = r->avc_config.buffer.number;
			p->length = (uintptr_t) size;
			if (mpeg4$define(inner, array, append_data)(&r->avc_config.buffer, (uintptr_t) size, data))
			{
				*r->avc_config.tail_pps = p;
				r->avc_config.tail_pps = &p->next;
				++r->avc_config.n_pps;
				return &r->stuff;
			}
			--r->avc_config.slot.number;
		}
	}
	return NULL;
}

static const mpeg4_stuff_t* mpeg4$define(stuff, stsd_codec_avcC, add$avcC_extra_spse)(mpeg4_stuff__stsd_avcC_s *restrict r, const void *data, uint64_t size)
{
	avcC_data_t *restrict p;
	// n_spse: [0, 255]
	if (r->avc_config.n_spse < 255 && size && size <= 0xffff)
	{
		if ((p = (avcC_data_t *) mpeg4$define(inner, array, append_point)(&r->avc_config.slot, 1)))
		{
			p->next = NULL;
			p->offset = r->avc_config.buffer.number;
			p->length = (uintptr_t) size;
			if (mpeg4$define(inner, array, append_data)(&r->avc_config.buffer, (uintptr_t) size, data))
			{
				*r->avc_config.tail_spse = p;
				r->avc_config.tail_spse = &p->next;
				++r->avc_config.n_spse;
				r->avc_config.enable_extra = 1;
				return &r->stuff;
			}
			--r->avc_config.slot.number;
		}
	}
	return NULL;
}

static const mpeg4$define$alloc(stsd_codec_avcC)
{
	mpeg4_atom_s *restrict r;
	r = mpeg4_atom_alloc_empty();
	if (r)
	{
		r->interface.dump = mpeg4$define(atom, stsd_codec_avcC, dump);
		r->interface.create = mpeg4$define(atom, stsd_codec_avcC, create);
		r->interface.parse = mpeg4$define(atom, stsd_codec_avcC, parse);
		r->interface.calc = mpeg4$define(atom, stsd_codec_avcC, calc);
		r->interface.build = mpeg4$define(atom, stsd_codec_avcC, build);
		if (
			mpeg4$stuff$method$set(r, stsd_codec_avcC, set$avcC_indication) &&
			mpeg4$stuff$method$set(r, stsd_codec_avcC, set$avcC_extra) &&
			mpeg4$stuff$method$set(r, stsd_codec_avcC, add$avcC_sps) &&
			mpeg4$stuff$method$set(r, stsd_codec_avcC, add$avcC_pps) &&
			mpeg4$stuff$method$set(r, stsd_codec_avcC, add$avcC_extra_spse)
		) return r;
		refer_free(r);
	}
	return NULL;
}

mpeg4$define$find(stsd_codec_avcC)
{
	static const mpeg4_box_type_t type = { .c = "avcC" };
	static const mpeg4_box_type_t extra = { .c = "stsd" };
	return mpeg4_find_atom(inst, mpeg4$define(atom, stsd_codec_avcC, alloc), type.v, extra.v);
}
