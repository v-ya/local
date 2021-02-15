#include "script_parse.h"
#include <json.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const uint8_t kmap[256] = {
	['0' ... '9'] = 1,
	['A' ... 'Z'] = 1,
	['a' ... 'z'] = 1
};

const char* parse_key(buffer_t *restrict buffer, const char *restrict *restrict p)
{
	register const uint8_t *restrict s;
	register uint8_t *restrict d;
	uint32_t n = 32;
	if (buffer_need(buffer, n))
	{
		s = (const uint8_t *) *p;
		d = buffer->data;
		while (--n && kmap[*s])
			*d++ = *s++;
		*p = (const char *) s;
		if (!kmap[*s])
		{
			*d = 0;
			return (const char *) buffer->data;
		}
	}
	return NULL;
}

static const char* parse_value_array(buffer_t *restrict buffer, const char *restrict s)
{
	uintptr_t want, i;
	want = ~(uintptr_t) 0;
	i = 0;
	buffer->used = 0;
	if (*s == '(')
	{
		uintptr_t n;
		++s;
		skip_space(s);
		n = *(uint8_t *) s++;
		// 'f' 102
		// 's' 115
		// 'u' 117
		while (*s >= '0' && *s <= '9')
		{
			n = n * 10 + (uintptr_t) (*s++ - '0');
		}
		skip_space(s);
		if (*s == '[')
		{
			++s;
			skip_space(s);
			want = (uintptr_t) strtoull(s, (char **) &s, 0);
			skip_space(s);
			if (*s != ']' || !~want)
				goto label_fail;
			++s;
			skip_space(s);
		}
		if (*s == ')')
		{
			++s;
			skip_space(s);
			if (*s == '{')
			{
				++s;
				skip_space(s);
				switch (n)
				{
					case 1178: goto label_u8;
					case 1158: goto label_s8;
					case 11716: goto label_u16;
					case 11516: goto label_s16;
					case 11732: goto label_u32;
					case 11532: goto label_s32;
					case 11764: goto label_u64;
					case 11564: goto label_s64;
					case 10232: goto label_f32;
					case 10264: goto label_f64;
				}
			}
		}
	}
	label_fail:
	return NULL;
	label_end:
	if (*s != '}') goto label_fail;
	return s + 1;
	// mapping type
	#define check_want(_sl)  if (~want && ((want << _sl) >> _sl) != want) goto label_fail
	#define get_data(_type, _func, ...) \
		if (*s != '}')\
		{\
			while (i < want)\
			{\
				if (!buffer_need(buffer, (i + 1) * sizeof(_type)))\
					goto label_fail;\
				((_type *) buffer->data)[i++] = (_type) _func(s, (char **) &s, ##__VA_ARGS__);\
				skip_space(s);\
				if (*s == '}') break;\
				if (*s != ',') goto label_fail;\
				++s;\
				skip_space(s);\
			}\
		}\
		if (~want && i < want) goto label_fill_##_type;\
		goto label_end
	label_u8:
	get_data(uint8_t, strtoul, 0);
	label_s8:
	get_data(int8_t, strtol, 0);
	goto label_end;
	label_u16:
	check_want(1);
	get_data(uint16_t, strtoul, 0);
	label_s16:
	check_want(1);
	get_data(int16_t, strtol, 0);
	label_u32:
	check_want(2);
	get_data(uint32_t, strtoul, 0);
	label_s32:
	check_want(2);
	get_data(int32_t, strtol, 0);
	label_u64:
	check_want(3);
	get_data(uint64_t, strtoull, 0);
	label_s64:
	check_want(3);
	get_data(int64_t, strtoll, 0);
	label_f32:
	check_want(2);
	get_data(float, strtof);
	label_f64:
	check_want(3);
	get_data(double, strtod);
	#undef check_want
	#undef get_data
	#define fill_zero_tail(_type) \
		if (!buffer_need(buffer, (want) * sizeof(_type)))\
			goto label_fail;\
		memset((_type *) buffer->data + i, 0, sizeof(_type) * (want - i));\
		goto label_end
	label_fill_uint8_t:
	label_fill_int8_t:
	fill_zero_tail(uint8_t);
	label_fill_uint16_t:
	label_fill_int16_t:
	fill_zero_tail(uint16_t);
	label_fill_uint32_t:
	label_fill_int32_t:
	label_fill_float:
	fill_zero_tail(uint32_t);
	label_fill_uint64_t:
	label_fill_int64_t:
	label_fill_double:
	fill_zero_tail(uint64_t);
	#undef fill_zero_tail
}

buffer_t* parse_value(buffer_t *restrict buffer, const char *restrict *restrict p, uintptr_t *restrict align)
{
	buffer_t *r;
	register const char *restrict s;
	r = NULL;
	buffer->used = 0;
	if (*(s = *p) == '=')
	{
		++s;
		if (*s >= '0' && *s <= '9')
		{
			register uintptr_t a;
			a = 0;
			do {
				a = a * 10 + (uintptr_t) (*s++ - '0');
			} while (*s >= '0' && *s <= '9');
			*align = a;
		}
		if (*s == '>')
		{
			json_inode_t *v;
			++s;
			skip_space(s);
			v = NULL;
			if (*s == '\"')
			{
				// string
				if ((s = json_decode(*p = s, &v)) && buffer_need(buffer, strlen(v->value.string) + 1))
				{
					memcpy(buffer->data, v->value.string, buffer->used);
					r = buffer;
				}
			}
			else if (*s == '@')
			{
				// load text
				if (*++s == '\"' && (s = json_decode(*p = s, &v)) && load_text(buffer, v->value.string))
					r = buffer;
			}
			else if (*s == '&')
			{
				// load file
				if (*++s == '\"' && (s = json_decode(*p = s, &v)) && load_file(buffer, v->value.string))
					r = buffer;
			}
			else if (*s == '(')
			{
				// array
				if ((s = parse_value_array(buffer, *p = s)))
					r = buffer;
			}
			if (v) json_free(v);
		}
		if (s) *p = s;
	}
	return r;
}

buffer_t* load_file(buffer_t *restrict buffer, const char *restrict path)
{
	buffer_t *r;
	FILE *fp;
	size_t n;
	r = NULL;
	buffer->used = 0;
	fp = fopen(path, "rb");
	if (fp)
	{
		fseek(fp, 0, SEEK_END);
		n = ftell(fp);
		if (n && buffer_need(buffer, n))
		{
			fseek(fp, 0, SEEK_SET);
			if (fread(buffer->data, 1, n, fp) == n)
				r = buffer;
		}
		else r = buffer;
		fclose(fp);
	}
	return r;
}

buffer_t* load_text(buffer_t *restrict buffer, const char *restrict path)
{
	if (load_file(buffer, path) && buffer_need(buffer, buffer->used + 1))
	{
		buffer->data[buffer->used - 1] = 0;
		return buffer;
	}
	return NULL;
}
