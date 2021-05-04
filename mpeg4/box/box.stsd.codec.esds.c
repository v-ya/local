#include "box.include.h"
#include "inner.fullbox.h"
#include "inner.bindata.h"
#include "inner.data.h"

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
	else ...

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
*/

typedef const uint8_t* (*esds_parse_f)(const uint8_t *restrict data, uint32_t size, uint8_t tag, const void *pri);

static inline uint8_t* esds_tag_get(uint8_t *restrict tag, const uint8_t *restrict *restrict data, uint32_t *restrict size)
{
	if (*size)
	{
		*tag = *((*data)++);
		--*size;
		return tag;
	}
	return NULL;
}

static inline uint32_t* esds_length_get(uint32_t *restrict length, const uint8_t *restrict *restrict data, uint32_t *restrict size)
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

static const uint8_t* esds_parse(const uint8_t *restrict data, uint32_t size, esds_parse_f parse, const void *pri)
{
	uint32_t n;
	uint8_t tag;
	while (size)
	{
		if (!esds_tag_get(&tag, &data, &size))
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

static const uint8_t* esds_parse_dump_func(const uint8_t *restrict data, uint32_t size, uint8_t tag, esds_parse_dump_t *restrict p);

static const uint8_t* esds_parse_dump_esd(const uint8_t *restrict data, uint32_t size, esds_parse_dump_t *restrict p)
{
	mlog_s *restrict mlog = p->mlog;
	uint32_t level = p->unidata.dump_level;
	uint16_t es_id;
	uint8_t flag;
	if (size < 3)
		goto label_fail;
	es_id = mpeg4_n16(*(uint16_t *) data);
	flag = data[2];
	data += 3;
	size -= 3;
	mlog_level_dump("es_id:            %u\n", es_id);
	mlog_level_dump("flag:             %02x\n", flag);
	level += mlog_level_width;
	mlog_level_dump("stream_dependence_flag: %u\n", !!(flag & 0x80));
	mlog_level_dump("url_flag:               %u\n", !!(flag & 0x40));
	mlog_level_dump("stream_priority:        %u\n", flag & 0x1f);
	level -= mlog_level_width;
	if (flag & 0x80)
	{
		if (size < 2)
			goto label_fail;
		es_id = mpeg4_n32(*(uint32_t *) data);
		data += 2;
		size -= 2;
		mlog_level_dump("depends_on_es_id: %u\n", es_id);
	}
	if (flag & 0x40)
	{
		mlog_level_dump("url:\n");
		if (size) mpeg4$define(inner, bindata, dump)(mlog, data, (uint64_t) size, 0, level + mlog_level_width);
	}
	else if (size)
	{
		if (!(data = esds_parse(data, size, (esds_parse_f) esds_parse_dump_func, p)))
			goto label_fail;
	}
	return data;
	label_fail:
	return NULL;
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

static const uint8_t* esds_parse_dump_dcd(const uint8_t *restrict data, uint32_t size, esds_parse_dump_t *restrict p)
{
	mlog_s *restrict mlog = p->mlog;
	uint32_t level = p->unidata.dump_level;
	uint8_t b;
	uint32_t l;
	if (size < 13)
		goto label_fail;
	size -= 13;
	b = *data++;
	mlog_level_dump("object_profile_indication: 0x%02x %s\n", b, esds_parse_get_profile_indication_string(b));
	b = *data++;
	mlog_level_dump("stream_type:               0x%02x %s\n", b >> 2, esds_parse_get_stream_type_string(b >> 2));
	mlog_level_dump("up_stream:                 %u\n", !!(b & 2));
	l = *data++;
	l = (l << 8) | *data++;
	l = (l << 8) | *data++;
	mlog_level_dump("buffer_size_db:            %u\n", l);
	l = *data++;
	l = (l << 8) | *data++;
	l = (l << 8) | *data++;
	l = (l << 8) | *data++;
	mlog_level_dump("max_bitrate:               %u\n", l);
	l = *data++;
	l = (l << 8) | *data++;
	l = (l << 8) | *data++;
	l = (l << 8) | *data++;
	mlog_level_dump("avg_bitrate:               %u\n", l);
	if (size && !(data = esds_parse(data, size, (esds_parse_f) esds_parse_dump_func, p)))
		goto label_fail;
	return data;
	label_fail:
	return NULL;
}

static const uint8_t* esds_parse_dump_func(const uint8_t *restrict data, uint32_t size, uint8_t tag, esds_parse_dump_t *restrict p)
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
	static const char *tag_unknow = "unknow";
	mlog_s *restrict mlog = p->mlog;
	uint32_t level = p->unidata.dump_level;
	mlog_level_dump("(%02u) %s %u\n", tag,
		(tag < (sizeof(tag2string) / sizeof(*tag2string)))?
			(tag2string[tag]?
				tag2string[tag]:
				tag_unknow):
			tag_unknow,
		size);
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
			mpeg4$define(inner, bindata, dump)(mlog, data, (uint64_t) size, 0, level + mlog_level_width);
			break;
	}
	p->unidata.dump_level -= mlog_level_width;
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

static const mpeg4$define$alloc(stsd_codec_esds)
{
	mpeg4_atom_s *restrict r;
	r = mpeg4_atom_alloc_empty();
	if (r)
	{
		r->interface.dump = mpeg4$define(atom, stsd_codec_esds, dump);
	}
	return r;
}

mpeg4$define$find(stsd_codec_esds)
{
	static const mpeg4_box_type_t type = { .c = "esds" };
	static const mpeg4_box_type_t extra = { .c = "stsd" };
	return mpeg4_find_atom(inst, mpeg4$define(atom, stsd_codec_esds, alloc), type.v, extra.v);
}
