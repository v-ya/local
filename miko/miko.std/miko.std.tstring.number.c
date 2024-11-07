#include "miko.std.tstring.h"

typedef struct miko_std_tstring_number_trigger_head_s miko_std_tstring_number_trigger_head_s;

struct miko_std_tstring_number_trigger_head_s {
	tparse_tstring_trigger_s trigger;
	tparse_tword_edge_s *hex;
	tparse_tword_edge_s *binary;
	tparse_tword_edge_s *octal;
	tparse_tword_edge_s *integer;
};

// head

static void miko_std_tstring_number_initial_hit(tparse_tword_edge_s *restrict edge, miko_std_syntaxor_chars_t hit)
{
	uintptr_t i;
	for (i = 0; hit[i]; ++i)
		edge->edge[((const uint8_t *) hit)[i]] =
			tparse_tword_edge_head |
			tparse_tword_edge_inner |
			tparse_tword_edge_tail;
}

static void miko_std_tstring_number_trigger_head_free_func(miko_std_tstring_number_trigger_head_s *restrict r)
{
	refer_ck_free(r->hex);
	refer_ck_free(r->binary);
	refer_ck_free(r->octal);
	refer_ck_free(r->integer);
}

static tparse_tstring_trigger_s* miko_std_tstring_number_trigger_head_alloc(tparse_tstring_trigger_f trigger)
{
	miko_std_tstring_number_trigger_head_s *restrict r;
	if (trigger && (r = (miko_std_tstring_number_trigger_head_s *) refer_alloz(sizeof(miko_std_tstring_number_trigger_head_s))))
	{
		refer_hook_free(r, tstring_number_trigger_head);
		r->trigger.trigger = trigger;
		if ((r->hex = tparse_tword_edge_alloc()) &&
			(r->binary = tparse_tword_edge_alloc()) &&
			(r->octal = tparse_tword_edge_alloc()) &&
			(r->integer = tparse_tword_edge_alloc()))
		{
			miko_std_tstring_number_initial_hit(r->hex, "0123456789ABCDEFabcdef_");
			miko_std_tstring_number_initial_hit(r->binary, "01_");
			miko_std_tstring_number_initial_hit(r->octal, "01234567_");
			miko_std_tstring_number_initial_hit(r->integer, "0123456789_");
			return &r->trigger;
		}
		refer_free(r);
	}
	return NULL;
}

// trigger

static uintptr_t miko_std_tstring_number_skip_integer(const tparse_tword_edge_s *restrict integer, const char *restrict text, uintptr_t size, uintptr_t pos)
{
	uintptr_t p;
	if ((p = pos) < size && (text[p] == '+' || text[p] == '-'))
		p += 1;
	if (p < size && text[p] >= '0' && text[p] <= '9')
		return tparse_tword_skip(integer, text, size, p);
	return pos;
}

static tparse_tstring_trigger_s* miko_std_tstring_number_trigger_head_func(miko_std_tstring_number_trigger_head_s *restrict trigger, tparse_tstring_value_t *restrict value, const char *restrict data, uintptr_t size, uintptr_t *restrict pos)
{
	uintptr_t p, q, i;
	if ((p = *pos) < size)
	{
		data += p;
		size -= p;
		if (data[0] == '0' && size > 1)
		{
			// hex || binary || octal
			q = 2;
			if (data[1] == 'X' || data[1] == 'x')
			{
				// hex
				if ((p = tparse_tword_skip(trigger->hex, data, size, q)) > q)
					goto label_skipped;
				goto label_fail;
			}
			else if (data[1] == 'B' || data[1] == 'b')
			{
				// binary
				if ((p = tparse_tword_skip(trigger->binary, data, size, q)) > q)
					goto label_skipped;
				goto label_fail;
			}
			else if (data[1] >= '0' && data[1] <= '7')
			{
				// octal
				if ((p = tparse_tword_skip(trigger->octal, data, size, q)) > q)
					goto label_skipped;
				goto label_fail;
			}
		}
		// integer || floating
		q = 0;
		if ((p = miko_std_tstring_number_skip_integer(trigger->integer, data, size, q)) <= q)
			goto label_fail;
		if (p < size && data[p] == '.')
		{
			// floating
			p = tparse_tword_skip(trigger->integer, data, size, p + 1);
			// check 1.0e-1
			if (p < size && (data[p] == 'E' || data[p] == 'e'))
			{
				q = p + 1;
				if ((p = miko_std_tstring_number_skip_integer(trigger->integer, data, size, q)) <= q)
					goto label_fail;
			}
		}
		if (p < size && (data[p] == 'D' || data[p] == 'd' || data[p] == 'F' || data[p] == 'f'))
			p += 1;
		label_skipped:
		if (p > 0)
		{
			for (i = 0; i < p; ++i)
			{
				if (data[i] != '_' && !tparse_tstring_value_append(value, data + i, 1))
					goto label_fail;
			}
			*pos += p;
			return &trigger->trigger;
		}
	}
	label_fail:
	return NULL;
}

static tparse_tstring_trigger_s* miko_std_tstring_number_trigger_tail_func(tparse_tstring_trigger_s *restrict trigger, tparse_tstring_value_t *restrict value, const char *restrict data, uintptr_t size, uintptr_t *restrict pos)
{
	uintptr_t p;
	if ((p = *pos) >= size || !(
		(data[p] >= '0' && data[p] <= '9') ||
		(data[p] >= 'A' && data[p] <= 'Z') ||
		(data[p] >= 'a' && data[p] <= 'z') ||
		data[p] == '_'))
	{
		tparse_tstring_value_finally(value);
		return trigger;
	}
	return NULL;
}

tparse_tstring_s* miko_std_tstring_create_number(void)
{
	tparse_tstring_trigger_s *restrict trigger;
	tparse_tstring_s *restrict r;
	tparse_tstring_s *result;
	result = NULL;
	if ((r = tparse_tstring_alloc_empty(NULL)))
	{
		if ((trigger = miko_std_tstring_number_trigger_head_alloc(
			(tparse_tstring_trigger_f) miko_std_tstring_number_trigger_head_func)))
		{
			if (tparse_tstring_set_trigger(r, tparse_tstring_trigger_head, NULL, trigger) &&
				tparse_tstring_set_trigger_func(r, tparse_tstring_trigger_tail, NULL,
					miko_std_tstring_number_trigger_tail_func))
			{
				result = r;
				r = NULL;
			}
			refer_free(trigger);
		}
		refer_ck_free(r);
	}
	return result;
}
