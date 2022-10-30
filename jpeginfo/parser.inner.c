#include "parser.h"

jpeg_parser_target_t* jpeg_parser_scan_segment(jpeg_parser_target_t *restrict t, enum jpeg_segment_type_t *restrict rt)
{
	const uint8_t *restrict p;
	uintptr_t i, n;
	*rt = jpeg_segment_type__nul;
	p = t->data;
	for (i = t->pos, n = t->size; i < n; ++i)
	{
		if (p[i] != jpeg_segment_type__mark)
			continue;
		else
		{
			label_start_mark:
			if (++i >= n) break;
			switch (p[i])
			{
				case jpeg_segment_type__nul:
				case jpeg_segment_type__rst0:
				case jpeg_segment_type__rst1:
				case jpeg_segment_type__rst2:
				case jpeg_segment_type__rst3:
				case jpeg_segment_type__rst4:
				case jpeg_segment_type__rst5:
				case jpeg_segment_type__rst6:
				case jpeg_segment_type__rst7:
					break;
				case jpeg_segment_type__mark:
					goto label_start_mark;
				default:
					*rt = (enum jpeg_segment_type_t) p[i];
					t->pos = i + 1;
					return t;
			}
		}
	}
	t->pos = i;
	return NULL;
}

jpeg_parser_target_t* jpeg_parser_get_segment_length(jpeg_parser_target_t *restrict t, enum jpeg_segment_type_t type, uintptr_t *restrict length)
{
	switch (type)
	{
		case jpeg_segment_type__rst0:
		case jpeg_segment_type__rst1:
		case jpeg_segment_type__rst2:
		case jpeg_segment_type__rst3:
		case jpeg_segment_type__rst4:
		case jpeg_segment_type__rst5:
		case jpeg_segment_type__rst6:
		case jpeg_segment_type__rst7:
		case jpeg_segment_type__soi:
		case jpeg_segment_type__eoi:
			*length = 0;
			return t;
		default:
			if (t->pos + sizeof(uint16_t) <= t->size)
			{
				*length = (uintptr_t) bits_n2be_16(*(uint16_t *) (t->data + t->pos));
				t->pos += sizeof(uint16_t);
				if (*length < sizeof(uint16_t))
					goto label_fail;
				*length -= sizeof(uint16_t);
				if (t->pos + *length <= t->size)
					return t;
			}
			t->pos = t->size;
			label_fail:
			*length = 0;
			return NULL;
	}
}

static void jpeg_parser_segment_free_func(jpeg_parser_segment_s *restrict r)
{
	if (r->name) refer_free(r->name);
	if (r->desc) refer_free(r->desc);
}

static jpeg_parser_segment_s* jpeg_parser_segment_alloc(const char *restrict name, const char *restrict desc, jpeg_parser_segment_parse_f parse)
{
	jpeg_parser_segment_s *restrict r;
	if ((r = (jpeg_parser_segment_s *) refer_alloz(sizeof(jpeg_parser_segment_s))))
	{
		refer_set_free(r, (refer_free_f) jpeg_parser_segment_free_func);
		r->name = (refer_string_t) refer_dump_string(name);
		r->desc = (refer_string_t) refer_dump_string(desc);
		r->parse = parse;
		if (!desc || r->desc)
			return r;
		refer_free(r);
	}
	return NULL;
}

static void jpeg_parser_hashmap_free_func(hashmap_vlist_t *restrict vl)
{
	if (vl->value)
		refer_free(vl->value);
}

jpeg_parser_s* jpeg_parser_add_segment(jpeg_parser_s *restrict p, enum jpeg_segment_type_t type, const char *restrict name, const char *restrict desc, jpeg_parser_segment_parse_f parse)
{
	jpeg_parser_segment_s *restrict seg;
	if ((seg = jpeg_parser_segment_alloc(name, desc, parse)))
	{
		if (hashmap_set_head(&p->type2parser, (uint64_t) type, seg, jpeg_parser_hashmap_free_func))
			return p;
		refer_free(seg);
	}
	return NULL;
}

const jpeg_parser_segment_s* jpeg_parser_get_segment(jpeg_parser_s *restrict p, enum jpeg_segment_type_t type)
{
	return (const jpeg_parser_segment_s *) hashmap_get_head(&p->type2parser, (uint64_t) type);
}

static void jpeg_parser_print_rawdata_line_16(char *restrict hex, char *restrict ascii, const uint8_t *restrict d, uintptr_t n)
{
	static const char hex2c[16] = "0123456789abcdef";
	uintptr_t i;
	for (i = 0; i < n; ++i)
	{
		*hex++ = hex2c[d[i] >> 4];
		*hex++ = hex2c[d[i] & 15];
		*hex++ = ' ';
		*ascii++ = (d[i] >= ' ' && d[i] <= '~')?d[i]:'.';
	}
	while (i < 16)
	{
		*hex++ = ' ';
		*hex++ = ' ';
		*hex++ = ' ';
		++i;
	}
	*hex = 0;
	*ascii = 0;
}

void jpeg_parser_print_rawdata(mlog_s *restrict mlog, const char *restrict name, const void *restrict data, uintptr_t size)
{
	const uint8_t *restrict p;
	uintptr_t n;
	char hex[64], ascii[32];
	if (name) mlog_printf(mlog, "%s [%zu]:\n", name, size);
	p = (const uint8_t *) data;
	while (size)
	{
		n = 16;
		if (size < n) n = size;
		jpeg_parser_print_rawdata_line_16(hex, ascii, p, n);
		mlog_printf(mlog, "%s| %s\n", hex, ascii);
		p += n;
		size -= n;
	}
}
