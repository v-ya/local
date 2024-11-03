#include "miko.source.h"

static refer_nstring_t miko_source_create_lines(refer_nstring_t source, rbtree_t *restrict *restrict pos)
{
	struct refer_nstring_s *restrict lines;
	char *restrict p;
	uintptr_t i, n, line_pos, line_row;
	if ((lines = (struct refer_nstring_s *) refer_dump_data(source, sizeof(refer_nstring_t) + 1 + source->length)))
	{
		p = lines->string;
		n = lines->length;
		line_pos = line_row = 0;
		for (i = 0; i < n; ++i)
		{
			if (p[i] == '\n')
			{
				if (!rbtree_insert(pos, NULL, (uint64_t) line_pos, (const void *) line_row, NULL))
					goto label_fail;
				line_pos = i + 1;
				line_row += 1;
				p[i] = 0;
			}
			else if (p[i] == '\r')
				p[i] = 0;
			else if (p[i] == '\t' || p[i] == '\v')
				p[i] = ' ';
		}
		if (i > line_pos)
		{
			if (!rbtree_insert(pos, NULL, (uint64_t) line_pos, (const void *) line_row, NULL))
				goto label_fail;
		}
		return lines;
		label_fail:
		refer_free(lines);
		rbtree_clear(pos);
	}
	return NULL;
}

static void miko_source_impl_free_func(miko_source_impl_s *restrict r)
{
	refer_ck_free(r->source.name);
	refer_ck_free(r->source.source);
	refer_ck_free(r->lines);
	rbtree_clear(&r->pos);
}

miko_source_s* miko_source_alloc(const char *restrict name, refer_nstring_t source)
{
	miko_source_impl_s *restrict r;
	if (source && (r = (miko_source_impl_s *) refer_alloz(sizeof(miko_source_impl_s))))
	{
		refer_hook_free(r, source_impl);
		r->source.source = (refer_nstring_t) refer_save(source);
		if ((!name || (r->source.name = refer_dump_string(name))) &&
			(r->lines = miko_source_create_lines(source, &r->pos)))
			return &r->source;
		refer_free(r);
	}
	return NULL;
}

static miko_source_pos_t* miko_source_find_line(const miko_source_impl_s *restrict r, miko_source_pos_t *restrict pos, uintptr_t pos_at_source)
{
	rbtree_t *restrict last;
	rbtree_t *restrict next;
	const char *restrict pline;
	uintptr_t line_pos_start;
	uintptr_t line_pos_stop;
	uintptr_t space_pos_stop;
	last = NULL;
	next = r->pos;
	while (next)
	{
		last = next;
		if (pos_at_source < next->key_index)
			next = next->l;
		else next = next->r;
	}
	if (last)
	{
		if (pos_at_source < last->key_index)
			last = rbtree_last(next = last);
		else next = rbtree_next(last);
		pline = r->lines->string;
		line_pos_start = (uintptr_t) last->key_index;
		line_pos_stop = next?((uintptr_t) next->key_index):r->lines->length;
		space_pos_stop = line_pos_start;
		while (line_pos_stop > line_pos_start && !pline[line_pos_stop - 1])
			line_pos_stop -= 1;
		while (space_pos_stop < line_pos_stop && pline[space_pos_stop] == ' ')
			space_pos_stop += 1;
		if (line_pos_start < line_pos_stop)
		{
			if (pos_at_source < line_pos_start)
				pos_at_source = line_pos_start;
			if (pos_at_source >= line_pos_stop)
				pos_at_source = line_pos_stop - 1;
		}
		else pos_at_source = line_pos_start;
		pos->line_string = pline + line_pos_start;
		pos->line_length = line_pos_stop - line_pos_start;
		pos->row_of_source = (uintptr_t) last->value;
		pos->col_of_line = pos_at_source - line_pos_start;
		pos->space_of_line = space_pos_stop - line_pos_start;
		return pos;
	}
	return NULL;
}

miko_source_pos_t* miko_source_find_by_pos(const miko_source_s *restrict r, miko_source_pos_t *restrict pos, uintptr_t pos_at_source)
{
	if (r && pos)
		return miko_source_find_line((const miko_source_impl_s *) r, pos, pos_at_source);
	return NULL;
}
