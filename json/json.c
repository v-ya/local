#include "json.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define json_char_type_undef	0
#define json_char_type_space	1
#define json_char_type_number	2
#define json_char_type_alpha	3
#define json_char_type_special	4
#define json_char_type_numsym	5

static uint8_t json_char_type[256] = {
	[' '] = json_char_type_space,
	['\t'] = json_char_type_space,
	['\r'] = json_char_type_space,
	['\n'] = json_char_type_space,
	['0'...'9'] = json_char_type_number,
	['a'...'z'] = json_char_type_alpha,
	['A'...'Z'] = json_char_type_alpha,
	['"'] = json_char_type_special,
	['['] = json_char_type_special,
	['{'] = json_char_type_special,
	['.'] = json_char_type_numsym,
	['+'] = json_char_type_numsym,
	['-'] = json_char_type_numsym
};

static uint8_t json_char_hex[256] = {
	['0'] = 0,
	['1'] = 1,
	['2'] = 2,
	['3'] = 3,
	['4'] = 4,
	['5'] = 5,
	['6'] = 6,
	['7'] = 7,
	['8'] = 8,
	['9'] = 9,
	['A'] = 10,
	['B'] = 11,
	['C'] = 12,
	['D'] = 13,
	['E'] = 14,
	['F'] = 15,
	['a'] = 10,
	['b'] = 11,
	['c'] = 12,
	['d'] = 13,
	['e'] = 14,
	['f'] = 15
};

static const char* json_decode_number(const char *s, json_inode_t **pji)
{
	register const char *t;
	register json_inode_t *ji;
	// test is integer or floating
	t = s;
	if (*t == '+' || *t == '-') t++;
	while (json_char_type[*(uint8_t*)t] == json_char_type_number) t++;
	if (*t == '.' || *t == 'e' || *t == 'E')
	{
		// floating
		ji = (json_inode_t*) malloc(sizeof(json_inode_type_t) + sizeof(double));
		if (!ji) return NULL;
		ji->type = json_inode_floating;
		ji->value.floating = strtod(s, (char **) &s);
		*pji = ji;
		return s;
	}
	else
	{
		// integer
		ji = (json_inode_t*) malloc(sizeof(json_inode_type_t) + sizeof(int64_t));
		if (!ji) return NULL;
		ji->type = json_inode_integer;
		ji->value.integer = strtol(s, (char **) &s, 0);
		*pji = ji;
		return s;
	}
}

static const char* json_decode_alpha(register const char *s, json_inode_t **pji)
{
	static char *s_null = "null", *s_true = "true", *s_false = "false";
	register json_inode_t *ji;
	switch (*s)
	{
		case 'n':
			if (!memcmp(s, s_null, 4) && json_char_type[((uint8_t*)s)[4]] != json_char_type_alpha)
			{
				ji = (json_inode_t*) malloc(sizeof(json_inode_type_t));
				if (!ji) return NULL;
				ji->type = json_inode_null;
				*pji = ji;
				return s + 4;
			}
			return NULL;
		case 't':
			if (!memcmp(s, s_true, 4) && json_char_type[((uint8_t*)s)[4]] != json_char_type_alpha)
			{
				ji = (json_inode_t*) malloc(sizeof(json_inode_type_t) + sizeof(uint64_t));
				if (!ji) return NULL;
				ji->type = json_inode_boolean;
				ji->value.boolean = 1;
				*pji = ji;
				return s + 4;
			}
			return NULL;
		case 'f':
			if (!memcmp(s, s_false, 5) && json_char_type[((uint8_t*)s)[5]] != json_char_type_alpha)
			{
				ji = (json_inode_t*) malloc(sizeof(json_inode_type_t) + sizeof(uint64_t));
				if (!ji) return NULL;
				ji->type = json_inode_boolean;
				ji->value.boolean = 0;
				*pji = ji;
				return s + 5;
			}
			return NULL;
	}
	return NULL;
}

static const char* json_decode_string(register const char *s, json_inode_t **pji)
{
	register uint64_t size;
	register char *t;
	json_inode_t *ji;
	if (*s == '"')
	{
		s++;
		t = (char *) s;
		size = 0;
		while (1)
		{
			switch (*s)
			{
				case 0:
					return NULL;
				case '"':
					size++;
					goto CalcLengthEnd;
				case '\\':
					s++;
					switch (*s)
					{
						case '"':
						case '\\':
						case '/':
						case 'n':
						case 'r':
						case 't':
						case 'b':
						case 'f':
							size++;
							s++;
							continue;
						case 'u':
							s++;
							if (*s) s++;
							if (*s) s++;
							if (*s) s++;
							if (*s) s++;
							size += 2;
							continue;
					}
					continue;
				default:
					size++;
					s++;
					continue;
			}
		}
		CalcLengthEnd:
		s = t;
		ji = (json_inode_t*) malloc(sizeof(json_inode_type_t) + size);
		if (!ji) return NULL;
		ji->type = json_inode_string;
		t = ji->value.string;
		size += (uint64_t) t;
		while((uint64_t)t < size)
		{
			switch (*s)
			{
				case '"':
					s++;
					*t++ = 0;
					goto CopyEnd;
				case '\\':
					s++;
					switch (*s)
					{
						case '"':
						case '\\':
						case '/':
							*t++ = *s++;
							continue;
						case 'n':
							s++;
							*t++ = '\n';
							continue;
						case 'r':
							s++;
							*t++ = '\r';
							continue;
						case 't':
							s++;
							*t++ = '\t';
							continue;
						case 'b':
							s++;
							*t++ = '\b';
							continue;
						case 'f':
							s++;
							*t++ = '\f';
							continue;
						case 'u':
							t[0] = (json_char_hex[((uint8_t*)s)[1]]<<4)|json_char_hex[((uint8_t*)s)[2]];
							t[1] = (json_char_hex[((uint8_t*)s)[3]]<<4)|json_char_hex[((uint8_t*)s)[4]];
							s += 5;
							t += 2;
							continue;
					}
					continue;
				default:
					*t++ = *s++;
					continue;
			}
		}
		CopyEnd:
		*pji = ji;
		return s;
	}
	return NULL;
}

static void json_hashmap_free_func(hashmap_vlist_t *vl)
{
	if (vl->value) json_free((json_inode_t*)vl->value);
}

static const char* json_decode_array(register const char *s, json_inode_t **pji)
{
	register uint64_t index;
	register hashmap_t *hm;
	json_inode_t *ji, *jic;
	if (*s == '[')
	{
		index = 0;
		jic = NULL;
		s++;
		ji = (json_inode_t*) malloc(sizeof(json_inode_type_t) + sizeof(hashmap_t));
		if (!ji) goto Err;
		ji->type = json_inode_array;
		hm = &ji->value.array;
		if (!hashmap_init(hm)) goto Err;
		while(1)
		{
			// ]
			while(json_char_type[*(uint8_t*)s] == json_char_type_space) s++;
			if (!*s) goto Err;
			if (*s == ']') break;
			// get value
			s = json_decode(s, &jic);
			if (!s || !jic) goto Err;
			if (!hashmap_set_head(hm, index++, jic, json_hashmap_free_func)) goto Err;
			// clear
			jic = NULL;
			// ,
			while(json_char_type[*(uint8_t*)s] == json_char_type_space) s++;
			if (*s == ',') s++;
			else if (*s != ']') goto Err;
		}
		*pji = ji;
		return s + 1;
	}
	return NULL;
	Err:
	if (jic) json_free(jic);
	if (ji) json_free(ji);
	return NULL;
}

static const char* json_decode_object(register const char *s, json_inode_t **pji)
{
	register hashmap_t *hm;
	json_inode_t *ji, *jik, *jic;
	if (*s == '{')
	{
		jik = jic = NULL;
		s++;
		ji = (json_inode_t*) malloc(sizeof(json_inode_type_t) + sizeof(hashmap_t));
		if (!ji) goto Err;
		ji->type = json_inode_object;
		hm = &ji->value.object;
		if (!hashmap_init(hm)) goto Err;
		while(1)
		{
			// }
			while(json_char_type[*(uint8_t*)s] == json_char_type_space) s++;
			if (!*s) goto Err;
			if (*s == '}') break;
			else if (*s != '"') goto Err;
			// get key
			s = json_decode_string(s, &jik);
			if (!s || !jik) goto Err;
			// :
			while(json_char_type[*(uint8_t*)s] == json_char_type_space) s++;
			if (*s != ':') goto Err;
			s++;
			while(json_char_type[*(uint8_t*)s] == json_char_type_space) s++;
			// get value
			s = json_decode(s, &jic);
			if (!s || !jic) goto Err;
			if (!hashmap_set_name(hm, jik->value.string, jic, json_hashmap_free_func)) goto Err;
			// clear
			json_free(jik);
			jik = jic = NULL;
			// ,
			while(json_char_type[*(uint8_t*)s] == json_char_type_space) s++;
			if (*s == ',') s++;
			else if (*s != '}') goto Err;
		}
		*pji = ji;
		return s + 1;
	}
	return NULL;
	Err:
	if (jik) json_free(jik);
	if (jic) json_free(jic);
	if (ji) json_free(ji);
	return NULL;
}

void json_free(register json_inode_t *json)
{
	if (json) switch (json->type)
	{
		case json_inode_array:
		case json_inode_object:
			hashmap_uini(&json->value.array);
		// fall through
		default:
			free(json);
	}
}

const char* json_decode(register const char *s, register json_inode_t **pji)
{
	if (*pji)
	{
		json_free(*pji);
		*pji = NULL;
	}
	while(json_char_type[*(uint8_t*)s] == json_char_type_space) s++;
	switch (json_char_type[*(uint8_t*)s])
	{
		case json_char_type_number:
		case json_char_type_numsym:
			// integer, floating
			return json_decode_number(s, pji);
		case json_char_type_alpha:
			// null, boolean
			return json_decode_alpha(s, pji);
		case json_char_type_special:
			// string, array, object
			switch (*s)
			{
				case '"':
					return json_decode_string(s, pji);
				case '[':
					return json_decode_array(s, pji);
				case '{':
					return json_decode_object(s, pji);
			}
			return NULL;
		default:
			return NULL;
	}
}

static uint64_t json_length_string(register json_inode_t *ji)
{
	register char *s;
	register uint64_t length;
	s = ji->value.string;
	length = 2;
	while(1)
	{
		switch(*s)
		{
			case 0:
				return length;
			case '"':
			case '\\':
			case '\n':
			case '\r':
			case '\t':
			case '\b':
			case '\f':
				s++;
				length += 2;
				continue;
			default:
				s++;
				length++;
				continue;
		}
	}
}

static void json_length_array_hashmap_call_func(register hashmap_vlist_t *vl, register uint64_t *length)
{
	if (vl->value) *length += json_length((json_inode_t*)vl->value);
}

static uint64_t json_length_array(register json_inode_t *ji)
{
	register hashmap_t *hm;
	uint64_t length;
	hm = &ji->value.array;
	if (hm->number)
	{
		length = hm->number + 1;
		hashmap_call(hm, (hashmap_func_call_t) json_length_array_hashmap_call_func, &length);
		return length;
	}
	else return 2;
}

static void json_length_object_hashmap_call_func(register hashmap_vlist_t *vl, register uint64_t *length)
{
	if (vl->value) *length += json_length_string((json_inode_t*)(vl->name - sizeof(json_inode_type_t))) + json_length((json_inode_t*)vl->value) + 1;
}

static uint64_t json_length_object(register json_inode_t *ji)
{
	register hashmap_t *hm;
	uint64_t length;
	hm = &ji->value.object;
	if (hm->number)
	{
		length = hm->number + 1;
		hashmap_call(hm, (hashmap_func_call_t) json_length_object_hashmap_call_func, &length);
		return length;
	}
	else return 2;
}

int json_make_floating(register char *buf, double f)
{
	register uint64_t z;
	register int i;
	double e;
	if (f < 0)
	{
		i = 1;
		*buf = '-';
	}
	else i = 0;
	z = f = fabs(f) + 0.5e-12;
	f -= z;
	i += sprintf(buf + i, "%lu", z);
	if (f > 1e-12)
	{
		e = 1e-12;
		buf += i;
		*buf++ = '.';
		++i;
		while (f > e)
		{
			f *= 10;
			e *= 10;
			*buf++ = (uint32_t)f + '0';
			f -= (uint32_t)f;
			++i;
		}
	}
	return i;
}

uint64_t json_length(register json_inode_t *ji)
{
	char buffer[64];
	switch (ji->type)
	{
		case json_inode_null:
			return 4;
		case json_inode_boolean:
			return 4 + !ji->value.boolean;
		case json_inode_integer:
			return sprintf(buffer, "%ld", ji->value.integer);
		case json_inode_floating:
			return json_make_floating(buffer, ji->value.floating);
		case json_inode_string:
			return json_length_string(ji);
		case json_inode_array:
			return json_length_array(ji);
		case json_inode_object:
			return json_length_object(ji);
		default:
			return 0;
	}
}

static char* json_encode_string(register json_inode_t *ji, register char *s)
{
	register char *ss;
	ss = ji->value.string;
	*s++ = '"';
	while(1)
	{
		switch(*ss)
		{
			case 0:
				*s++ = '"';
				return s;
			case '"':
			case '\\':
				*s++ = '\\';
				*s++ = *ss++;
				continue;
			case '\n':
				*s++ = '\\';
				ss++;
				*s++ = 'n';
				continue;
			case '\r':
				*s++ = '\\';
				ss++;
				*s++ = 'r';
				continue;
			case '\t':
				*s++ = '\\';
				ss++;
				*s++ = 't';
				continue;
			case '\b':
				*s++ = '\\';
				ss++;
				*s++ = 'b';
				continue;
			case '\f':
				*s++ = '\\';
				ss++;
				*s++ = 'f';
				continue;
			default:
				*s++ = *ss++;
				continue;
		}
	}
}

static char* json_encode_array(register json_inode_t *ji, register char *s)
{
	register hashmap_t *hm;
	register uint64_t i, n;
	hm = &ji->value.array;
	*s++ = '[';
	if (hm->number)
	{
		i = 0;
		n = 1;
		while (n)
		{
			ji = (json_inode_t*) hashmap_get_head(hm, i);
			if (ji)
			{
				s = json_encode_intext(ji, s);
				n--;
			}
			i++;
		}
		n = hm->number - 1;
		while (n)
		{
			ji = (json_inode_t*) hashmap_get_head(hm, i);
			if (ji)
			{
				*s++ = ',';
				s = json_encode_intext(ji, s);
				n--;
			}
			i++;
		}
	}
	*s++ = ']';
	return s;
}

static void json_encode_object_hashmap_call_func(register hashmap_vlist_t *vl, register char **ps)
{
	if (vl->value)
	{
		if (ps[1]) ps[1] = NULL;
		else *(*ps)++ = ',';
		*ps = json_encode_string((json_inode_t*)(vl->name - sizeof(json_inode_type_t)), *ps);
		*(*ps)++ = ':';
		*ps = json_encode_intext((json_inode_t*)vl->value, *ps);
	}
}

static char* json_encode_object(register json_inode_t *ji, register char *s)
{
	register hashmap_t *hm;
	struct {
		char *s;
		uint64_t isfirst;
	} v;
	hm = &ji->value.object;
	*s++ = '{';
	if (hm->number)
	{
		v.s = s;
		v.isfirst = -1;
		hashmap_call(hm, (hashmap_func_call_t) json_encode_object_hashmap_call_func, &v);
		s = v.s;
	}
	*s++ = '}';
	return s;
}

char* json_encode_intext(register json_inode_t *ji, register char *s)
{
	static char *s_null = "null", *s_true = "true", *s_false = "false";
	switch (ji->type)
	{
		case json_inode_null:
			*(uint32_t*)s = *(uint32_t*) s_null;
			return s + 4;
		case json_inode_boolean:
			if (ji->value.boolean)
			{
				*(uint32_t*)s = *(uint32_t*) s_true;
				return s + 4;
			}
			else
			{
				
				*(uint32_t*)s = *(uint32_t*) s_false;
				s[4] = 'e';
				return s + 5;
			}
		case json_inode_integer:
			return s + sprintf(s, "%ld", ji->value.integer);
		case json_inode_floating:
			return s + json_make_floating(s, ji->value.floating);
		case json_inode_string:
			return json_encode_string(ji, s);
		case json_inode_array:
			return json_encode_array(ji, s);
		case json_inode_object:
			return json_encode_object(ji, s);
		default:
			return s;
	}
}

char* json_encode(register json_inode_t *ji, register char *s)
{
	s = json_encode_intext(ji, s);
	*s = 0;
	return s;
}

json_inode_t* json_create_null(void)
{
	register json_inode_t *ji;
	ji = (json_inode_t*) malloc(sizeof(json_inode_type_t));
	if (ji) ji->type = json_inode_null;
	return ji;
}

json_inode_t* json_create_boolean(register uint64_t boolean)
{
	register json_inode_t *ji;
	ji = (json_inode_t*) malloc(sizeof(json_inode_type_t) + sizeof(uint64_t));
	if (ji)
	{
		ji->type = json_inode_boolean;
		ji->value.boolean = boolean;
	}
	return ji;
}

json_inode_t* json_create_integer(register int64_t integer)
{
	register json_inode_t *ji;
	ji = (json_inode_t*) malloc(sizeof(json_inode_type_t) + sizeof(int64_t));
	if (ji)
	{
		ji->type = json_inode_integer;
		ji->value.integer = integer;
	}
	return ji;
}

json_inode_t* json_create_floating(register double floating)
{
	register json_inode_t *ji;
	ji = (json_inode_t*) malloc(sizeof(json_inode_type_t) + sizeof(double));
	if (ji)
	{
		ji->type = json_inode_floating;
		ji->value.floating = floating;
	}
	return ji;
}

json_inode_t* json_create_string(register const char *string)
{
	register json_inode_t *ji;
	register uint64_t size;
	size = strlen(string) + 1;
	ji = (json_inode_t*) malloc(sizeof(json_inode_type_t) + size);
	if (ji)
	{
		ji->type = json_inode_string;
		memcpy(ji->value.string, string, size);
	}
	return ji;
}

json_inode_t* json_create_array(void)
{

	register json_inode_t *ji;
	ji = (json_inode_t*) malloc(sizeof(json_inode_type_t) + sizeof(hashmap_t));
	if (ji)
	{
		ji->type = json_inode_array;
		if (!hashmap_init(&ji->value.array))
		{
			free(ji);
			ji = NULL;
		}
	}
	return ji;
}

json_inode_t* json_create_object(void)
{

	register json_inode_t *ji;
	ji = (json_inode_t*) malloc(sizeof(json_inode_type_t) + sizeof(hashmap_t));
	if (ji)
	{
		ji->type = json_inode_object;
		if (!hashmap_init(&ji->value.array))
		{
			free(ji);
			ji = NULL;
		}
	}
	return ji;
}

json_inode_t* json_array_find(register json_inode_t *ji, register uint64_t index)
{
	if (ji && ji->type == json_inode_array) return (json_inode_t*) hashmap_get_head(&ji->value.array, index);
	return NULL;
}

json_inode_t* json_object_find(register json_inode_t *ji, register const char *key)
{
	if (ji && ji->type == json_inode_object) return (json_inode_t*) hashmap_get_name(&ji->value.object, key);
	return NULL;
}

json_inode_t* json_array_get(register json_inode_t *ji, register uint64_t index)
{
	register hashmap_vlist_t *vl;
	register json_inode_t *r = NULL;
	if (ji && ji->type == json_inode_array)
	{
		vl = hashmap_find_head(&ji->value.array, index);
		if (vl)
		{
			r = (json_inode_t*) vl->value;
			vl->value = NULL;
			hashmap_delete_head(&ji->value.array, index);
		}
	}
	return r;
}

json_inode_t* json_object_get(register json_inode_t *ji, register const char *key)
{
	register hashmap_vlist_t *vl;
	register json_inode_t *r = NULL;
	if (ji && ji->type == json_inode_object)
	{
		vl = hashmap_find_name(&ji->value.object, key);
		if (vl)
		{
			r = (json_inode_t*) vl->value;
			vl->value = NULL;
			hashmap_delete_name(&ji->value.object, key);
		}
	}
	return r;
}

json_inode_t* json_array_set(register json_inode_t *ji, register uint64_t index, register json_inode_t *value)
{
	if (ji && value && ji->type == json_inode_array) return hashmap_set_head(&ji->value.array, index, value, json_hashmap_free_func)?value:NULL;
	return NULL;
}

json_inode_t* json_object_set(register json_inode_t *ji, register const char *key, register json_inode_t *value)
{
	if (ji && value && ji->type == json_inode_object) return hashmap_set_name(&ji->value.array, key, value, json_hashmap_free_func)?value:NULL;
	return NULL;
}

void json_array_delete(register json_inode_t *ji, register uint64_t index)
{
	if (ji && ji->type == json_inode_array) hashmap_delete_head(&ji->value.array, index);
}

void json_object_delete(register json_inode_t *ji, register const char *key)
{
	if (ji && ji->type == json_inode_object) hashmap_delete_name(&ji->value.object, key);
}

json_inode_t* json_load(const char *path)
{
	register char *s_json;
	register size_t size;
	register FILE *fp;
	json_inode_t *json;
	json = NULL;
	if (path)
	{
		fp = fopen(path, "r");
		if (fp)
		{
			fseek(fp, 0, SEEK_END);
			size = ftell(fp);
			s_json = (char *) malloc(size + 1);
			if (s_json)
			{
				fseek(fp, 0, SEEK_SET);
				if (fread(s_json, 1, size, fp) == size)
				{
					s_json[size] = 0;
					json_decode(path, &json);
				}
				free(s_json);
			}
			fclose(fp);
		}
	}
	return json;
}

json_inode_t* json_find(register json_inode_t *ji, register const char *jpath)
{
	register char *s;
	register uint64_t length;
	char *psf, *ps, c;
	length = strlen(jpath) + 1;
	s = (char *) malloc(length);
	if (!s) return NULL;
	memcpy(s, jpath, length);
	jpath = psf = s;
	if (*jpath != '.' && *jpath != '[') goto L_object;
	while (ji && jpath && *jpath)
	{
		switch (*jpath)
		{
			case '.':
				++jpath;
				L_object:
				s = strpbrk(jpath, ".[");
				if (s)
				{
					c = *s;
					*s = 0;
					ji = json_object_find(ji, jpath);
					*s = c;
				}
				else ji = json_object_find(ji, jpath);
				jpath = s;
				break;
			case '[':
				++jpath;
				length = strtol(jpath, &ps, 0);
				s = ps;
				if (*s == ']')
				{
					++s;
					ji = json_array_find(ji, length);
					jpath = s;
					break;
				}
			// fall through
			default:
				ji = NULL;
				goto End;
		}
	}
	End:
	free(psf);
	return ji;
}

json_inode_t* json_get_null(register json_inode_t *ji, register const char *jpath)
{
	ji = json_find(ji, jpath);
	if (ji && ji->type == json_inode_null) return ji;
	return NULL;
}

json_inode_t* json_get_boolean(register json_inode_t *ji, register const char *jpath, register uint64_t *v)
{
	ji = json_find(ji, jpath);
	if (ji && ji->type == json_inode_boolean)
	{
		if (v) *v = ji->value.boolean;
		return ji;
	}
	return NULL;
}

json_inode_t* json_get_integer(register json_inode_t *ji, register const char *jpath, register int64_t *v)
{
	ji = json_find(ji, jpath);
	if (ji && ji->type == json_inode_integer)
	{
		if (v) *v = ji->value.integer;
		return ji;
	}
	return NULL;
}

json_inode_t* json_get_floating(register json_inode_t *ji, register const char *jpath, register double *v)
{
	ji = json_find(ji, jpath);
	if (ji && ji->type == json_inode_floating)
	{
		if (v) *v = ji->value.floating;
		return ji;
	}
	return NULL;
}

json_inode_t* json_get_string(register json_inode_t *ji, register const char *jpath, register const char **v)
{
	ji = json_find(ji, jpath);
	if (ji && ji->type == json_inode_string)
	{
		if (v) *v = ji->value.string;
		return ji;
	}
	return NULL;
}

json_inode_t* json_get_array(register json_inode_t *ji, register const char *jpath, register hashmap_t **v)
{
	ji = json_find(ji, jpath);
	if (ji && ji->type == json_inode_array)
	{
		if (v) *v = &ji->value.array;
		return ji;
	}
	return NULL;
}

json_inode_t* json_get_object(register json_inode_t *ji, register const char *jpath, register hashmap_t **v)
{
	ji = json_find(ji, jpath);
	if (ji && ji->type == json_inode_object)
	{
		if (v) *v = &ji->value.object;
		return ji;
	}
	return NULL;
}

static void json_copy_hashmap_func(register hashmap_vlist_t *vl, register hashmap_t *hm)
{
	register json_inode_t *v;
	if (vl->name)
	{
		// object
		v = json_copy((json_inode_t*) vl->value);
		if (v && !hashmap_set_name(hm, vl->name, v, json_hashmap_free_func)) json_free(v);
	}
	else
	{
		// array
		v = json_copy((json_inode_t*) vl->value);
		if (v && !hashmap_set_head(hm, vl->head, v, json_hashmap_free_func)) json_free(v);
	}
}

json_inode_t* json_copy(register json_inode_t *ji)
{
	register json_inode_t *r;
	register uint64_t size;
	if (ji) switch (ji->type)
	{
		case json_inode_null:
			r = (json_inode_t*) malloc(sizeof(json_inode_type_t));
			if (r)
			{
				r->type = json_inode_null;
				return r;
			}
			return NULL;
		case json_inode_boolean:
			r = (json_inode_t*) malloc(sizeof(json_inode_type_t) + sizeof(uint64_t));
			if (r)
			{
				r->type = json_inode_boolean;
				r->value.boolean = ji->value.boolean;
				return r;
			}
			return NULL;
		case json_inode_integer:
			r = (json_inode_t*) malloc(sizeof(json_inode_type_t) + sizeof(int64_t));
			if (r)
			{
				r->type = json_inode_integer;
				r->value.integer = ji->value.integer;
				return r;
			}
			return NULL;
		case json_inode_floating:
			r = (json_inode_t*) malloc(sizeof(json_inode_type_t) + sizeof(double));
			if (r)
			{
				r->type = json_inode_floating;
				r->value.floating = ji->value.floating;
				return r;
			}
			return NULL;
		case json_inode_string:
			size = sizeof(json_inode_type_t) + strlen(ji->value.string) + 1;
			r = (json_inode_t*) malloc(size);
			if (r)
			{
				memcpy(r, ji, size);
				return r;
			}
			return NULL;
		case json_inode_array:
		case json_inode_object:
			r = (json_inode_t*) malloc(sizeof(json_inode_type_t) + sizeof(hashmap_t));
			if (r)
			{
				r->type = ji->type;
				if (hashmap_init(&r->value.array))
				{
					hashmap_call(&ji->value.object, (hashmap_func_call_t) json_copy_hashmap_func, &r->value.object);
					return r;
				}
				free(r);
			}
			return NULL;
	}
	return NULL;
}

