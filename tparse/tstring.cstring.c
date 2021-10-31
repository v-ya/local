#include "tstring.h"

typedef struct tparse_tstring_trigger_check_s {
	tparse_tstring_trigger_s trigger;
	char check;
} tparse_tstring_trigger_check_s;

/*
	\a	0x07	响铃(BEL)
	\b	0x08	退格(BS)
	\e	0x1b	转义(ESC)
	\f	0x0c	换页(FF)
	\n	0x0a	换行(LF)
	\r	0x0d	回车(CR)
	\t	0x09	水平制表(HT)
	\v	0x0b	垂直制表(VT)
	\"	0x22	"
	\'	0x27	'
	\?	0x3f	?
	\\	0x5c	\
	\-	0-	八进制
	\--	0--	八进制
	\---	0---	八进制
	\x--	0x--	十六进制
*/

static uintptr_t tparse_tstring_cstring_escape_parse_octal(const char *restrict data, uintptr_t size, char *restrict rc)
{
	uintptr_t i;
	uint32_t n;
	n = 0;
	if (size > 3) size = 3;
	for (i = 0; i < size && data[i] >= '0' && data[i] <= '7'; ++i)
		n = (n << 3) | (data[i] - '0');
	*rc = (char) n;
	return i;
}

static uintptr_t tparse_tstring_cstring_escape_parse_hex(const char *restrict data, uintptr_t size, char *restrict rc)
{
	uintptr_t i;
	uint32_t n;
	int c;
	n = 0;
	if (size > 2) size = 2;
	for (i = 0; i < size; ++i)
	{
		c = data[i];
		if (c >= '0' && c <= '9')
			c -= '0';
		else if (c >= 'a' && c <= 'f')
			c -= 'a' - 10;
		else if (c >= 'A' && c <= 'F')
			c -= 'A' - 10;
		else break;
		n = (n << 4) | c;
	}
	*rc = (char) n;
	return i;
}

static tparse_tstring_trigger_s* tparse_tstring_cstring_escape_parse_func(tparse_tstring_trigger_s *restrict trigger, tparse_tstring_value_t *restrict value, const char *restrict data, uintptr_t size, uintptr_t *restrict pos)
{
	uintptr_t p, n;
	char c;
	if ((p = *pos) < size)
	{
		switch ((c = data[p]))
		{
			// octal
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
				n = tparse_tstring_cstring_escape_parse_octal(data + p, size - p, &c);
				if (n)
				{
					*pos = p + n;
					goto label_set_without_pos;
				}
				break;
			// special
			case 'a': c = '\a'; goto label_set;
			case 'b': c = '\b'; goto label_set;
			case 'e': c = '\e'; goto label_set;
			case 'f': c = '\f'; goto label_set;
			case 'n': c = '\n'; goto label_set;
			case 'r': c = '\r'; goto label_set;
			case 't': c = '\t'; goto label_set;
			case 'v': c = '\v'; goto label_set;
			// hex
			case 'x':
				if (++p < size)
				{
					n = tparse_tstring_cstring_escape_parse_hex(data + p, size - p, &c);
					if (n)
					{
						*pos = p + n;
						goto label_set_without_pos;
					}
				}
				break;
			// keep
			default:
				label_set:
				*pos = p + 1;
				label_set_without_pos:
				tparse_tstring_value_rollback(value, 1);
				if (!tparse_tstring_value_append(value, &c, 1))
					goto label_fail;
				goto label_okay;
		}
	}
	tparse_tstring_value_rollback(value, 1);
	label_okay:
	return trigger;
	label_fail:
	return NULL;
}

static tparse_tstring_trigger_s* tparse_tstring_cstring_head_parse_func(tparse_tstring_trigger_s *restrict trigger, tparse_tstring_value_t *restrict value, const char *restrict data, uintptr_t size, uintptr_t *restrict pos)
{
	uintptr_t p;
	if ((p = *pos) < size)
	{
		if (data[p] == ((tparse_tstring_trigger_check_s *) trigger)->check)
		{
			*pos = p + 1;
			return trigger;
		}
	}
	return NULL;
}

static tparse_tstring_trigger_s* tparse_tstring_cstring_tail_parse_func(tparse_tstring_trigger_s *restrict trigger, tparse_tstring_value_t *restrict value, const char *restrict data, uintptr_t size, uintptr_t *restrict pos)
{
	tparse_tstring_value_rollback(value, 1);
	tparse_tstring_value_finally(value);
	return trigger;
}

static tparse_tstring_s* tparse_tstring_cstring_set_head_trigger(tparse_tstring_s *restrict ts, char c)
{
	tparse_tstring_trigger_check_s *restrict t;
	t = (tparse_tstring_trigger_check_s *) refer_alloz(sizeof(tparse_tstring_trigger_check_s));
	if (t)
	{
		t->trigger.trigger = tparse_tstring_cstring_head_parse_func;
		t->check = c;
		ts = tparse_tstring_set_trigger(ts, tparse_tstring_trigger_head, NULL, &t->trigger);
		refer_free(t);
		return ts;
	}
	return NULL;
}

static tparse_tstring_s* tparse_tstring_alloc_c_parse(const char *restrict quotes)
{
	tparse_tstring_s *restrict r;
	if ((r = tparse_tstring_alloc_empty()))
	{
		if (tparse_tstring_cstring_set_head_trigger(r, *quotes) &&
			tparse_tstring_set_trigger_func(r, tparse_tstring_trigger_inner, "\\", tparse_tstring_cstring_escape_parse_func) &&
			tparse_tstring_set_trigger_func(r, tparse_tstring_trigger_inner, quotes, tparse_tstring_cstring_tail_parse_func))
			return r;
		refer_free(r);
	}
	return NULL;
}

tparse_tstring_s* tparse_tstring_alloc_c_parse_multi_quotes(void)
{
	return tparse_tstring_alloc_c_parse("\"");
}

tparse_tstring_s* tparse_tstring_alloc_c_parse_single_quotes(void)
{
	return tparse_tstring_alloc_c_parse("\'");
}
