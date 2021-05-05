#include "box.include.h"
#include "inner.fullbox.h"
#include "inner.bindata.h"
#include "inner.data.h"
#include <memory.h>

/*
ES_Descriptor:
	uint8_t tag = 0x03;
	uvar28_t length;
	uint16_t es_id;
		bit1_t stream_dependence_flag;
		bit1_t url_flag;
		bit1_t reserved = 1;
		uint5_t stream_priority;
	if (stream_dependence_flag)
		uint16_t depends_on_es_id;
	if (url_flag)
		string url;
	else ... {
		DecoderConfigDescriptor dcd;
		SLConfigDescriptor slcd;
	}

DecoderConfigDescriptor:
	uint8_t tag = 0x04;
	uvar28_t length;
	uint8_t object_profile_indication;
		uint6_t stream_type
		bit1_t up_stream;
		bit1_t reserved = 1;
	uint24_t buffer_size_db;
	uint32_t max_bitrate;
	uint32_t avg_bitrate;
	DecoderSpecificInfo dec_specific_info[];

DecoderSpecificInfo:
	uint8_t tag = 0x05;
	uvar28_t length;
	uint8_t data[length];

SLConfigDescriptor:
	uint8_t tag = 0x06;
	uvar28_t length;
	...
*/

typedef struct decoder_specific_info_t {
	uint32_t length;
	uint32_t align_size;
	uint8_t data[];
} decoder_specific_info_t;

typedef struct decoder_config_descriptor_t {
	uint32_t length;
	uint8_t object_profile_indication;
	uint8_t stream_info;
	uint32_t buffer_size_db;
	uint32_t max_bitrate;
	uint32_t avg_bitrate;
	uint32_t dsi_number;
	inner_array_t dsi;
} decoder_config_descriptor_t;

typedef struct sl_config_descriptor_t {
	uint32_t length;
} sl_config_descriptor_t;

typedef struct es_descriptor_t {
	uint32_t length;
	uint16_t es_id;
	decoder_config_descriptor_t dcd;
	sl_config_descriptor_t slcd;
} es_descriptor_t;

typedef struct mpeg4_stuff__stsd_esds_s {
	mpeg4_stuff_t stuff;
	inner_fullbox_t fullbox;
	es_descriptor_t es_descriptor;
} mpeg4_stuff__stsd_esds_s;

typedef const uint8_t* (*esds_parse_f)(const uint8_t *restrict data, uint64_t size, uint8_t tag, const void *pri);

static const char* esds_parse_get_tag_string(uint8_t tag)
{
	static const char *tag2string[] = {
		[0] = NULL,
		[1] = NULL,
		[2] = NULL,
		[3] = "ES_Descriptor",
		[4] = "DecoderConfigDescriptor",
		[5] = "DecoderSpecificInfo",
		[6] = "SLConfigDescriptor"
	};
	if (tag < (sizeof(tag2string) / sizeof(*tag2string)) && tag2string[tag])
		return tag2string[tag];
	return "unknow";
}

static const char* esds_parse_get_profile_indication_string(uint8_t object_profile_indication)
{
	static const char *opi2string[256] = {
		[0x00] = "!Forbidden",
		[0x01] = "Systems ISO/IEC 14496-1 (a)",
		[0x02] = "Systems ISO/IEC 14496-1 (b)",
		[0x03] = "Interaction Stream",
		[0x04] = "Extended BIFS (h)",
		[0x05] = "AFX Stream (i)",
		[0x06] = "Font Data Stream",
		[0x07] = "Synthetised Texture",
		[0x08] = "Text Stream",
		[0x09] = "LASeR Stream",
		[0x0a] = "Simple Aggregation Format (SAF) Stream",
		[0x20] = "Visual ISO/IEC 14496-2 (c)",
		[0x21] = "Visual ITU-T Recommendation H.264 | ISO/IEC 14496-10 (g)",
		[0x22] = "Parameter Sets for ITU-T Recommendation H.264 | ISO/IEC 14496-10 (g)",
		[0x23] = "Visual ISO/IEC 23008-2 | ITU-T Recommendation H.265",
		[0x40] = "Audio ISO/IEC 14496-3 AAC Main",
		[0x41] = "Audio ISO/IEC 14496-3 AAC LC",
		[0x60] = "Visual ISO/IEC 13818-2 MPEG-2 Simple Profile",
		[0x61] = "Visual ISO/IEC 13818-2 MPEG-2 Main Profile",
		[0x62] = "Visual ISO/IEC 13818-2 MPEG-2 SNR Profile",
		[0x63] = "Visual ISO/IEC 13818-2 MPEG-2 Spatial Profile",
		[0x64] = "Visual ISO/IEC 13818-2 MPEG-2 High Profile",
		[0x65] = "Visual ISO/IEC 13818-2 MPEG-2 422 Profile",
		[0x66] = "Audio ISO/IEC 13818-7 AAC Main Profile",
		[0x67] = "Audio ISO/IEC 13818-7 AAC LowComplexity Profile",
		[0x68] = "Audio ISO/IEC 13818-7 AAC Scaleable Sampling Rate Profile",
		[0x69] = "Audio ISO/IEC 13818-3 MP2/MP3",
		[0x6a] = "Visual ISO/IEC 11172-2 MPEG-1",
		[0x6b] = "Audio ISO/IEC 11172-3 MP3",
		[0x6c] = "Visual ISO/IEC 10918-1 MJPEG",
		[0x6d] = "Portable Network Graphics (f)",
		[0x6e] = "Visual ISO/IEC 15444-1 (JPEG 2000)",
		[0xc0] = "VP9 (nonstandard)",
		[0xc1] = "FLAC (nonstandard)"
	};
	if (opi2string[object_profile_indication])
		return opi2string[object_profile_indication];
	return "unknow";
}

static const char* esds_parse_get_stream_type_string(uint8_t stream_type)
{
	static const char *opi2string[64] = {
		[0x00] = "!Forbidden",
		[0x01] = "Object Descriptor Stream",
		[0x02] = "Clock Reference Stream",
		[0x03] = "Scene Description Stream",
		[0x04] = "Visual Stream",
		[0x05] = "Audio Stream",
		[0x06] = "MPEG7 Stream",
		[0x0a] = "ObjectContentInfo Stream",
	};
	if (opi2string[stream_type &= 0x3f])
		return opi2string[stream_type];
	return "unknow";
}

static inline uint32_t* esds_length_get(uint32_t *restrict length, const uint8_t *restrict *restrict data, uint64_t *restrict size)
{
	register const uint8_t *restrict p = *data;
	register uint32_t n = 0;
	register uint8_t c;
	switch (*size)
	{
		default:
			if (!((c = *p++) & 0x80))
				goto label_ok;
			n = (n << 7) | (c & 0x7f);
			// fall through
		case 3:
			if (!((c = *p++) & 0x80))
				goto label_ok;
			n = (n << 7) | (c & 0x7f);
			// fall through
		case 2:
			if (!((c = *p++) & 0x80))
				goto label_ok;
			n = (n << 7) | (c & 0x7f);
			// fall through
		case 1:
			if (((c = *p++) & 0x80))
				goto label_fail;
			label_ok:
			n = (n << 7) | (c & 0x7f);
			*length = n;
			*size -= (uint32_t) ((uintptr_t) p - (uintptr_t) *data);
			*data = p;
			return length;
		case 0:
			label_fail:
			return NULL;
	}
}

static inline uint8_t* esds_length_set(uint8_t *restrict data, uint32_t length)
{
	*data++ = (uint8_t) ((length >> 21) | 0x80);
	*data++ = (uint8_t) ((length >> 14) | 0x80);
	*data++ = (uint8_t) ((length >> 7) | 0x80);
	*data++ = (uint8_t) (length & 0x7f);
	return data;
}

static const uint8_t* esds_parse(const uint8_t *restrict data, uint64_t size, esds_parse_f parse, const void *pri)
{
	uint32_t n;
	uint8_t tag;
	while (size)
	{
		if (!mpeg4$define(inner, uint8_t, get)(&tag, &data, &size))
			goto label_fail;
		if (!esds_length_get(&n, &data, &size))
			goto label_fail;
		if (n > size)
			goto label_fail;
		if (!parse(data, n, tag, pri))
			goto label_fail;
		data += n;
		size -= n;
	}
	return data;
	label_fail:
	return NULL;
}

typedef struct esds_parse_dump_t {
	mlog_s *mlog;
	mpeg4_atom_dump_t unidata;
} esds_parse_dump_t;

static const uint8_t* esds_parse_dump_func(const uint8_t *restrict data, uint64_t size, uint8_t tag, esds_parse_dump_t *restrict p);

static esds_parse_dump_t* esds_parse_dump_esd(const uint8_t *restrict data, uint64_t size, esds_parse_dump_t *restrict p)
{
	mlog_s *restrict mlog = p->mlog;
	uint32_t level = p->unidata.dump_level;
	uint16_t es_id;
	uint8_t flag, n;
	if (!mpeg4$define(inner, uint16_t, get)(&es_id, &data, &size))
		goto label_fail;
	mlog_level_dump("es_id:            %u\n", es_id);
	if (!mpeg4$define(inner, uint8_t, get)(&flag, &data, &size))
		goto label_fail;
	mlog_level_dump("flag:             %02x\n", flag);
	level += mlog_level_width;
	mlog_level_dump("stream_dependence_flag: %u\n", !!(flag & 0x80));
	mlog_level_dump("url_flag:               %u\n", !!(flag & 0x40));
	mlog_level_dump("stream_priority:        %u\n", flag & 0x1f);
	level -= mlog_level_width;
	if (flag & 0x80)
	{
		if (!mpeg4$define(inner, uint16_t, get)(&es_id, &data, &size))
			goto label_fail;
		mlog_level_dump("depends_on_es_id: %u\n", es_id);
	}
	if (flag & 0x40)
	{
		mlog_level_dump("url:\n");
		if (!size) goto label_fail;
		--size;
		n = *data++;
		if ((uint64_t) n > size)
			goto label_fail;
		if (n) mpeg4$define(inner, bindata, dump)(mlog, data, (uint64_t) n, 0, level + mlog_level_width);
		data += n;
		size -= n;
	}
	if (size)
	{
		if (!esds_parse(data, size, (esds_parse_f) esds_parse_dump_func, p))
			goto label_fail;
	}
	return p;
	label_fail:
	return NULL;
}

static esds_parse_dump_t* esds_parse_dump_dcd(const uint8_t *restrict data, uint64_t size, esds_parse_dump_t *restrict p)
{
	mlog_s *restrict mlog = p->mlog;
	uint32_t level = p->unidata.dump_level;
	uint8_t b;
	uint32_t l;
	if (!mpeg4$define(inner, uint8_t, get)(&b, &data, &size))
		goto label_fail;
	mlog_level_dump("object_profile_indication: 0x%02x %s\n", b, esds_parse_get_profile_indication_string(b));
	if (!mpeg4$define(inner, uint8_t, get)(&b, &data, &size))
		goto label_fail;
	mlog_level_dump("stream_type:               0x%02x %s\n", b >> 2, esds_parse_get_stream_type_string(b >> 2));
	mlog_level_dump("up_stream:                 %u\n", !!(b & 2));
	if (!mpeg4$define(inner, uint24_t, get)(&l, &data, &size))
		goto label_fail;
	mlog_level_dump("buffer_size_db:            %u\n", l);
	if (!mpeg4$define(inner, uint32_t, get)(&l, &data, &size))
		goto label_fail;
	mlog_level_dump("max_bitrate:               %u\n", l);
	if (!mpeg4$define(inner, uint32_t, get)(&l, &data, &size))
		goto label_fail;
	mlog_level_dump("avg_bitrate:               %u\n", l);
	if (size && !esds_parse(data, size, (esds_parse_f) esds_parse_dump_func, p))
		goto label_fail;
	return p;
	label_fail:
	return NULL;
}

static const uint8_t* esds_parse_dump_func(const uint8_t *restrict data, uint64_t size, uint8_t tag, esds_parse_dump_t *restrict p)
{
	mlog_s *restrict mlog = p->mlog;
	uint32_t level = p->unidata.dump_level;
	mlog_level_dump("(%02u) %s %u\n", tag, esds_parse_get_tag_string(tag), size);
	p->unidata.dump_level += mlog_level_width;
	switch (tag)
	{
		case 3:
			if (!esds_parse_dump_esd(data, size, p))
				goto label_fail;
			break;
		case 4:
			if (!esds_parse_dump_dcd(data, size, p))
				goto label_fail;
			break;
		default:
			mpeg4$define(inner, bindata, dump)(mlog, data, size, 0, level + mlog_level_width);
			break;
	}
	p->unidata.dump_level -= mlog_level_width;
	return data;
	label_fail:
	return NULL;
}

static const uint8_t* esds_parse_parse_func(const uint8_t *restrict data, uint64_t size, uint8_t tag, mpeg4_stuff_t *restrict stuff);

static mpeg4_stuff_t* esds_parse_parse_esd(const uint8_t *restrict data, uint64_t size, mpeg4_stuff_t *restrict stuff)
{
	uint16_t es_id;
	uint8_t flag, n;
	if (!mpeg4$define(inner, uint16_t, get)(&es_id, &data, &size))
		goto label_fail;
	if (!mpeg4$stuff$method$call(stuff, set$es_id, es_id))
		goto label_fail;
	if (!mpeg4$define(inner, uint8_t, get)(&flag, &data, &size))
		goto label_fail;
	if (flag & 0x80)
	{
		if (!mpeg4$define(inner, uint16_t, get)(&es_id, &data, &size))
			goto label_fail;
	}
	if (flag & 0x40)
	{
		if (!size) goto label_fail;
		--size;
		n = *data++;
		if ((uint64_t) n > size)
			goto label_fail;
		data += n;
		size -= n;
	}
	if (size)
	{
		if (!esds_parse(data, size, (esds_parse_f) esds_parse_parse_func, stuff))
			goto label_fail;
	}
	return stuff;
	label_fail:
	return NULL;
}

static mpeg4_stuff_t* esds_parse_parse_dcd(const uint8_t *restrict data, uint64_t size, mpeg4_stuff_t *restrict stuff)
{
	uint32_t buffer_size_db, max_bitrate, avg_bitrate;
	uint8_t opi, st;
	if (!mpeg4$define(inner, uint8_t, get)(&opi, &data, &size))
		goto label_fail;
	if (!mpeg4$define(inner, uint8_t, get)(&st, &data, &size))
		goto label_fail;
	if (!mpeg4$stuff$method$call(stuff, set$decoder_config_profile, opi, st >> 2))
		goto label_fail;
	if (!mpeg4$define(inner, uint24_t, get)(&buffer_size_db, &data, &size))
		goto label_fail;
	if (!mpeg4$define(inner, uint32_t, get)(&max_bitrate, &data, &size))
		goto label_fail;
	if (!mpeg4$define(inner, uint32_t, get)(&avg_bitrate, &data, &size))
		goto label_fail;
	if (!mpeg4$stuff$method$call(stuff, set$bitrate, buffer_size_db, max_bitrate, avg_bitrate))
		goto label_fail;
	if (size && !esds_parse(data, size, (esds_parse_f) esds_parse_parse_func, stuff))
		goto label_fail;
	return stuff;
	label_fail:
	return NULL;
}

static mpeg4_stuff_t* esds_parse_parse_dsi(const uint8_t *restrict data, uint64_t size, mpeg4_stuff_t *restrict stuff)
{
	if (mpeg4$stuff$method$call(stuff, add$data, data, size, NULL))
		return stuff;
	return NULL;
}

static const uint8_t* esds_parse_parse_func(const uint8_t *restrict data, uint64_t size, uint8_t tag, mpeg4_stuff_t *restrict stuff)
{
	switch (tag)
	{
		case 3:
			if (!esds_parse_parse_esd(data, size, stuff))
				goto label_fail;
			break;
		case 4:
			if (!esds_parse_parse_dcd(data, size, stuff))
				goto label_fail;
			break;
		case 5:
			if (!esds_parse_parse_dsi(data, size, stuff))
				goto label_fail;
			break;
		default:
			break;
	}
	return data;
	label_fail:
	return NULL;
}

static mpeg4$define$dump(stsd_codec_esds)
{
	esds_parse_dump_t p = {
		.mlog = mlog,
		.unidata = *unidata
	};
	inner_fullbox_t fullbox;
	uint32_t level = unidata->dump_level;
	if (!mpeg4$define(inner, fullbox, get)(&fullbox, &data, &size))
		goto label_fail;
	mpeg4$define(inner, fullbox, dump)(mlog, &fullbox, NULL, level);
	if (fullbox.version)
		goto label_fail;
	if (esds_parse(data, size, (esds_parse_f) esds_parse_dump_func, &p))
		return atom;
	label_fail:
	return NULL;
}

static mpeg4$define$stuff$init(stsd_codec_esds, mpeg4_stuff__stsd_esds_s)
{
	mpeg4$define(inner, array, init)(&r->es_descriptor.dcd.dsi, sizeof(decoder_specific_info_t));
	return &r->stuff;
}

static mpeg4$define$stuff$free(stsd_codec_esds, mpeg4_stuff__stsd_esds_s)
{
	mpeg4$define(inner, array, clear)(&r->es_descriptor.dcd.dsi);
	mpeg4_stuff_free_default_func(&r->stuff);
}

static mpeg4$define$create(stsd_codec_esds)
{
	return mpeg4_stuff_alloc(atom, inst, type,
		sizeof(mpeg4_stuff__stsd_esds_s),
		(mpeg4_stuff_init_f) mpeg4$define(stuff, stsd_codec_esds, init),
		(refer_free_f) mpeg4$define(stuff, stsd_codec_esds, free));
}

static mpeg4$define$parse(stsd_codec_esds)
{
	inner_fullbox_t fullbox;
	if (!mpeg4$define(inner, fullbox, get)(&fullbox, &data, &size))
		goto label_fail;
	if (!mpeg4$stuff$method$call(stuff, set$version_and_flags, fullbox.version, fullbox.flags))
		goto label_fail;
	if (esds_parse(data, size, (esds_parse_f) esds_parse_parse_func, stuff))
		return stuff;
	label_fail:
	return NULL;
}

static mpeg4$define$calc(stsd_codec_esds)
{
	es_descriptor_t *restrict r = &((mpeg4_stuff__stsd_esds_s *) stuff)->es_descriptor;
	const decoder_specific_info_t *restrict dsi;
	uint32_t length, n;
	// calc esd
		// calc dcd
		length = 0;
			// calc dsi[]
			dsi = (const decoder_specific_info_t *) r->dcd.dsi.array;
			n = r->dcd.dsi_number;
			while (n)
			{
				length += 1            // tag
					+ 4            // length
					+ dsi->length; // data
				dsi = (const decoder_specific_info_t *) (dsi->data + dsi->align_size);
				--n;
			}
		r->dcd.length = length // dsi[]
				+ 1    // object_profile_indication
				+ 1    // stream_type && up_stream
				+ 3    // buffer_size_db
				+ 4    // max_bitrate
				+ 4;   // avg_bitrate
		length = 1               // tag
			+ 4              // length
			+ r->dcd.length; // dcd
		// calc slcd
		r->slcd.length = 1; // 0x02
		length += 1               // tag
			+ 4               // length
			+ r->slcd.length; // slcd
	r->length = length   // dcd && slcd
			+ 2  // es_id
			+ 1; // stream_dependence_flag && url_flag && stream_priority
	length = 1           // tag
		+ 4          // length
		+ r->length; // esd
	return mpeg4_stuff_calc_okay(stuff, (uint64_t) length + sizeof(mpeg4_full_box_suffix_t));
}

static mpeg4$define$build(stsd_codec_esds)
{
	const es_descriptor_t *restrict r = &((mpeg4_stuff__stsd_esds_s *) stuff)->es_descriptor;
	const decoder_specific_info_t *restrict dsi;
	uint32_t n;
	data = mpeg4$define(inner, fullbox, set)(data, &((mpeg4_stuff__stsd_esds_s *) stuff)->fullbox);
	// esd
	*data++ = 0x03;
	data = esds_length_set(data, r->length);
	data = mpeg4$define(inner, uint16_t, set)(data, r->es_id);
	*data++ = 0x00;
	// dcd
	*data++ = 0x04;
	data = esds_length_set(data, r->dcd.length);
	*data++ = r->dcd.object_profile_indication;
	*data++ = r->dcd.stream_info;
	data = mpeg4$define(inner, uint24_t, set)(data, r->dcd.buffer_size_db);
	data = mpeg4$define(inner, uint32_t, set)(data, r->dcd.max_bitrate);
	data = mpeg4$define(inner, uint32_t, set)(data, r->dcd.avg_bitrate);
	// dsi[]
	dsi = (const decoder_specific_info_t *) r->dcd.dsi.array;
	n = r->dcd.dsi_number;
	while (n)
	{
		*data++ = 0x05;
		data = esds_length_set(data, dsi->length);
		if (dsi->length)
			memcpy(data, dsi->data, dsi->length);
		data += dsi->length;
		dsi = (const decoder_specific_info_t *) (dsi->data + dsi->align_size);
		--n;
	}
	// slcd
	*data++ = 0x06;
	data = esds_length_set(data, r->slcd.length);
	*data++ = 0x02;
	return stuff;
}

static inner_method_set_fullbox(stsd_codec_esds, mpeg4_stuff__stsd_esds_s, fullbox, 0);
static inner_method_get_fullbox(stsd_codec_esds, mpeg4_stuff__stsd_esds_s, fullbox);
static inner_method_set_simple_param(stsd_codec_esds, es_id, mpeg4_stuff__stsd_esds_s, uint16_t, es_descriptor.es_id);

static const mpeg4_stuff_t* mpeg4$define(stuff, stsd_codec_esds, set$decoder_config_profile)(mpeg4_stuff__stsd_esds_s *restrict r, uint8_t object_profile_indication, uint8_t stream_type)
{
	r->es_descriptor.dcd.object_profile_indication = object_profile_indication;
	r->es_descriptor.dcd.stream_info = stream_type << 2;
	return &r->stuff;
}

static const mpeg4_stuff_t* mpeg4$define(stuff, stsd_codec_esds, set$bitrate)(mpeg4_stuff__stsd_esds_s *restrict r, uint32_t buffer_size_db, uint32_t max_bitrate, uint32_t avg_bitrate)
{
	r->es_descriptor.dcd.buffer_size_db = buffer_size_db;
	r->es_descriptor.dcd.max_bitrate = max_bitrate;
	r->es_descriptor.dcd.avg_bitrate = avg_bitrate;
	return &r->stuff;
}

static const mpeg4_stuff_t* mpeg4$define(stuff, stsd_codec_esds, add$data)(mpeg4_stuff__stsd_esds_s *restrict r, const void *data, uint64_t size, uint64_t *restrict offset)
{
	decoder_specific_info_t *restrict dsi;
	uint32_t align_size;
	if (offset) *offset = (uint64_t) r->es_descriptor.dcd.dsi_number;
	if (size > 0xffff)
		goto label_fail;
	align_size = ((uint32_t) size + (sizeof(decoder_specific_info_t) - 1)) & ~(sizeof(decoder_specific_info_t) - 1);
	if ((dsi = (decoder_specific_info_t *) mpeg4$define(inner, array, append_point)(
		&r->es_descriptor.dcd.dsi,
		align_size / sizeof(decoder_specific_info_t) + 1)))
	{
		dsi->length = (uint32_t) size;
		dsi->align_size = align_size;
		memcpy(dsi->data, data, size);
		++r->es_descriptor.dcd.dsi_number;
		return &r->stuff;
	}
	label_fail:
	return NULL;
}

static const mpeg4$define$alloc(stsd_codec_esds)
{
	mpeg4_atom_s *restrict r;
	r = mpeg4_atom_alloc_empty();
	if (r)
	{
		r->interface.dump = mpeg4$define(atom, stsd_codec_esds, dump);
		r->interface.create = mpeg4$define(atom, stsd_codec_esds, create);
		r->interface.parse = mpeg4$define(atom, stsd_codec_esds, parse);
		r->interface.calc = mpeg4$define(atom, stsd_codec_esds, calc);
		r->interface.build = mpeg4$define(atom, stsd_codec_esds, build);
		if (
			mpeg4$stuff$method$set(r, stsd_codec_esds, set$version_and_flags) &&
			mpeg4$stuff$method$set(r, stsd_codec_esds, get$version_and_flags) &&
			mpeg4$stuff$method$set(r, stsd_codec_esds, set$es_id) &&
			mpeg4$stuff$method$set(r, stsd_codec_esds, set$decoder_config_profile) &&
			mpeg4$stuff$method$set(r, stsd_codec_esds, set$bitrate) &&
			mpeg4$stuff$method$set(r, stsd_codec_esds, add$data)
		) return r;
		refer_free(r);
	}
	return NULL;
}

mpeg4$define$find(stsd_codec_esds)
{
	static const mpeg4_box_type_t type = { .c = "esds" };
	static const mpeg4_box_type_t extra = { .c = "stsd" };
	return mpeg4_find_atom(inst, mpeg4$define(atom, stsd_codec_esds, alloc), type.v, extra.v);
}
