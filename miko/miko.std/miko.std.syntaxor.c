#include "miko.std.syntaxor.h"

static void miko_std_syntaxor_free_func(miko_std_syntaxor_s *restrict r)
{
	refer_ck_free(r->mlog);
	refer_ck_free(r->tester);
	refer_ck_free(r->syntax);
	refer_ck_free(r->route);
}

miko_std_syntaxor_s* miko_std_syntaxor_alloc(mlog_s *restrict mlog)
{
	miko_std_syntaxor_s *restrict r;
	if ((r = (miko_std_syntaxor_s *) refer_alloz(sizeof(miko_std_syntaxor_s))))
	{
		refer_hook_free(r, syntaxor);
		r->mlog = (mlog_s *) refer_save(mlog);
		if ((r->tester = tparse_tmapping_alloc_single()) &&
			(r->syntax = vattr_alloc()) &&
			(r->route = vattr_alloc()))
			return r;
		refer_free(r);
	}
	return NULL;
}

static miko_index_t miko_std_syntaxor_add_item(miko_std_syntaxor_s *restrict r, const char *restrict id_name, miko_std_syntaxor_chars_t head, miko_std_syntaxor_item_s *restrict item)
{
	miko_std_syntaxor_pool_s *restrict pool;
	uintptr_t i;
	char c[2];
	if ((item->id.name = refer_dump_string(id_name)) &&
		!vattr_get_vslot(r->syntax, id_name) &&
		vattr_insert_tail(r->syntax, id_name, item))
	{
		item->id.index = (uintptr_t) r->syntax->vslot.number;
		c[1] = 0;
		for (i = 0; (c[0] = head[i]); ++i)
		{
			if ((pool = (miko_std_syntaxor_pool_s *) vattr_get_first(r->route, c)))
			{
				label_find:
				if (vattr_insert_tail(pool, id_name, item))
					continue;
			}
			else if ((pool = vattr_alloc()))
			{
				if (!vattr_insert_tail(r->route, c, pool))
					r = NULL;
				refer_free(pool);
				if (r) goto label_find;
			}
			goto label_fail;
		}
		return item->id.index;
	}
	label_fail:
	return 0;
}

miko_index_t miko_std_syntaxor_add_keyword(miko_std_syntaxor_s *restrict r, const char *restrict id_name, miko_std_syntaxor_chars_t head, const char *const restrict keyword[])
{
	miko_std_syntaxor_item_s *restrict item;
	miko_index_t index;
	if (id_name && head && keyword &&
		((item = miko_std_syntaxor_item_create_keyword(keyword))))
	{
		index = miko_std_syntaxor_add_item(r, id_name, head, item);
		refer_free(item);
		return index;
	}
	return 0;
}

miko_std_syntaxor_s* miko_std_syntaxor_okay(miko_std_syntaxor_s *restrict r)
{
	tparse_tmapping_s *restrict tester;
	vattr_vlist_t *restrict vlist;
	tester = r->tester;
	for (vlist = r->route->vattr; vlist; vlist = vlist->vattr_next)
	{
		if (!tester->add(tester, vlist->vslot->key, vlist->value))
			goto label_fail;
	}
	return r;
	label_fail:
	return NULL;
}

void miko_std_syntaxor_print(miko_std_syntaxor_s *restrict r, const miko_source_s *restrict source, uintptr_t pos)
{
	miko_log_s *restrict log;
	miko_log_into_t into;
	if (r->mlog && (log = miko_log_alloc()))
	{
		into = miko_log_into_name(log, miko_std_error__syntaxor);
		miko_log_add_source(log, miko_log_level__error, source, pos);
		miko_log_back(log, into);
		miko_log_print(log, r->mlog);
		refer_free(log);
	}
}

static miko_std_syntax_s* miko_std_syntaxor_initial(miko_std_syntaxor_s *restrict r, miko_std_syntax_s *restrict syntax, miko_std_syntax_source_t *restrict source)
{
	const miko_std_syntaxor_pool_s *restrict pool;
	miko_std_syntaxor_item_s *restrict item;
	tparse_tmapping_s *restrict tester;
	miko_std_syntax_s *restrict value;
	const vattr_vlist_t *restrict vlist;
	const char *restrict p;
	const char *restrict q;
	uintptr_t pos, n;
	uintptr_t skip, length;
	tester = r->tester;
	p = source->source->source->string;
	n = source->source->source->length;
	pos = 0;
	while (pos < n)
	{
		if (!(pool = (const miko_std_syntaxor_pool_s *) tester->test(tester, p[pos])))
			pos += 1;
		else
		{
			item = NULL;
			skip = 0;
			for (vlist = pool->vattr; vlist; vlist = vlist->vattr_next)
			{
				item = (miko_std_syntaxor_item_s *) vlist->value;
				if ((skip = item->test(item, p, n, pos)))
					break;
			}
			if (!skip || !item)
				goto label_fail;
			if (item->data)
				q = item->data(item, &length);
			else q = p + pos, length = skip;
			source->offset = pos;
			source->tail = pos + skip;
			if (!(value = miko_std_syntax_create_syntax(&item->id, source, q, length)))
				goto label_fail;
			if (!vattr_insert_tail(syntax->data.scope, value->id.name, value))
				goto label_fail;
			refer_free(value);
			pos += skip;
		}
	}
	return syntax;
	label_fail:
	miko_std_syntaxor_print(r, source->source, pos);
	return NULL;
}

miko_std_syntax_s* miko_std_syntaxor_create(miko_std_syntaxor_s *restrict r, const miko_source_s *restrict source)
{
	miko_std_syntax_source_t syntax_source;
	miko_std_syntax_s *restrict syntax;
	syntax_source.source = source;
	syntax_source.offset = 0;
	syntax_source.tail = source->source->length;
	if (source && (syntax = miko_std_syntax_create_scope(NULL, &syntax_source)))
	{
		if (miko_std_syntaxor_initial(r, syntax, &syntax_source))
			return syntax;
		refer_free(syntax);
	}
	return NULL;
}
