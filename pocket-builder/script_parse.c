#include "script_parse.h"
#include <json.h>
#include <stdlib.h>
#include <string.h>

static const uint8_t kmap[256] = {
	['-'] = 1,
	['.'] = 1,
	['0' ... '9'] = 1,
	['A' ... 'Z'] = 1,
	['a' ... 'z'] = 1,
	['_'] = 1
};

int parse_is_key(const char *restrict s)
{
	return (int) kmap[*(const uint8_t *) s];
}

const char* parse_key(exbuffer_t *restrict buffer, const char *restrict *restrict p)
{
	register const uint8_t *restrict s;
	register uint8_t *restrict d;
	uintptr_t n;
	s = (const uint8_t *) *p;
	while (kmap[*s]) ++s;
	n = (uintptr_t) s - (uintptr_t) *p;
	if ((d = exbuffer_need(buffer, n + 1)))
	{
		memcpy(d, *p, n);
		d[n] = 0;
		*p = (const char *) s;
		return (const char *) d;
	}
	return NULL;
}

static const char* parse_value_array(exbuffer_t *restrict buffer, const char *restrict s)
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
	buffer->used = 0;
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
				if (!exbuffer_need(buffer, buffer->used = (i + 1) * sizeof(_type)))\
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
		if (!exbuffer_need(buffer, buffer->used = (want) * sizeof(_type)))\
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

exbuffer_t* parse_value(exbuffer_t *restrict buffer, const char *restrict *restrict p, uintptr_t *restrict align)
{
	exbuffer_t *r;
	register const char *restrict s;
	r = NULL;
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
				exbuffer_clear(buffer);
				if ((s = json_decode(*p = s, &v)) && exbuffer_append(buffer, v->value.string, strlen(v->value.string) + 1))
					r = buffer;
			}
			else if (*s == '@')
			{
				// load text
				if (*++s == '\"' && (s = json_decode(*p = s, &v)) && parse_load_text(buffer, v->value.string))
					r = buffer;
			}
			else if (*s == '&')
			{
				// load file
				if (*++s == '\"' && (s = json_decode(*p = s, &v)) && parse_load_file(buffer, v->value.string))
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

#include <fsys.h>

exbuffer_t* parse_load_file(exbuffer_t *restrict buffer, const char *restrict path)
{
	exbuffer_t *r;
	fsys_file_s *fp;
	uint64_t size;
	uintptr_t n;
	r = NULL;
	buffer->used = 0;
	fp = fsys_file_alloc(path, fsys_file_flag_read);
	if (fp)
	{
		if (fsys_file_attr_size(fp, &size) && (n = (uintptr_t) size) && exbuffer_need(buffer, n))
		{
			buffer->used = n;
			if (fsys_file_read(fp, buffer->data, n, &n) && n == buffer->used)
				r = buffer;
		}
		refer_free(fp);
	}
	return r;
}

exbuffer_t* parse_load_text(exbuffer_t *restrict buffer, const char *restrict path)
{
	char stail = 0;
	if (parse_load_file(buffer, path) && exbuffer_append(buffer, &stail, 1))
		return buffer;
	return NULL;
}
