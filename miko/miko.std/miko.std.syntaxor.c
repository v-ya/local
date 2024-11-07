#include "miko.std.syntaxor.h"
#include <memory.h>

static void miko_std_syntaxor_free_func(miko_std_syntaxor_s *restrict r)
{
	refer_ck_free(r->mlog);
	refer_ck_free(r->tester);
	refer_ck_free(r->syntax);
	refer_ck_free(r->route);
	refer_ck_free(r->scope_start);
	refer_ck_free(r->scope_stop);
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
			(r->route = vattr_alloc()) &&
			(r->scope_start = vattr_alloc()) &&
			(r->scope_stop = vattr_alloc()))
			return r;
		refer_free(r);
	}
	return NULL;
}

miko_index_t miko_std_syntaxor_name2index(const miko_std_syntaxor_s *restrict r, const char *restrict id_name)
{
	const miko_std_syntaxor_item_s *restrict item;
	if (id_name && (item = (const miko_std_syntaxor_item_s *) vattr_get_first(r->syntax, id_name)))
		return item->id.index;
	return 0;
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
		if (head && item->test)
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
		}
		return item->id.index;
	}
	label_fail:
	return 0;
}

miko_index_t miko_std_syntaxor_add_none(miko_std_syntaxor_s *restrict r, const char *restrict id_name)
{
	miko_std_syntaxor_item_s *restrict item;
	miko_index_t index;
	if (id_name && ((item = (miko_std_syntaxor_item_s *) refer_alloz(sizeof(miko_std_syntaxor_item_s)))))
	{
		index = miko_std_syntaxor_add_item(r, id_name, NULL, item);
		refer_free(item);
		return index;
	}
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

miko_index_t miko_std_syntaxor_add_tword(miko_std_syntaxor_s *restrict r, const char *restrict id_name, miko_std_syntaxor_chars_t head, miko_std_syntaxor_chars_t inner, miko_std_syntaxor_chars_t tail)
{
	miko_std_syntaxor_item_s *restrict item;
	miko_index_t index;
	if (id_name && head && inner && tail &&
		((item = miko_std_syntaxor_item_create_tword(head, inner, tail))))
	{
		index = miko_std_syntaxor_add_item(r, id_name, head, item);
		refer_free(item);
		return index;
	}
	return 0;
}

miko_index_t miko_std_syntaxor_add_tstring(miko_std_syntaxor_s *restrict r, const char *restrict id_name, miko_std_syntaxor_chars_t head, tparse_tstring_s* (*tstring_create_func)(void))
{
	miko_std_syntaxor_item_s *restrict item;
	tparse_tstring_s *restrict tstring;
	miko_index_t index;
	if (id_name && head && tstring_create_func && (tstring = tstring_create_func()))
	{
		if ((item = miko_std_syntaxor_item_create_tstring(tstring)))
		{
			index = miko_std_syntaxor_add_item(r, id_name, head, item);
			refer_free(tstring);
			refer_free(item);
			return index;
		}
		refer_free(tstring);
	}
	return 0;
}

miko_std_syntaxor_s* miko_std_syntaxor_set_scope(miko_std_syntaxor_s *restrict r, const char *restrict id_name, const char *restrict syntax_start, const char *restrict syntax_stop, const char *restrict syntax_scope, uintptr_t must_match, const char *const restrict allow_syntax_name[])
{
	const miko_std_syntaxor_item_s *restrict item;
	miko_std_syntaxor_scope_s *restrict scope;
	miko_index_t index;
	uintptr_t i;
	if (id_name && syntax_start && syntax_stop && allow_syntax_name &&
		(item = (const miko_std_syntaxor_item_s *) vattr_get_first(r->syntax, id_name)) &&
		!vattr_get_vslot(r->scope_start, syntax_start) && !vattr_get_vslot(r->scope_stop, syntax_stop) &&
		(scope = miko_std_syntaxor_scope_alloc(&item->id, syntax_scope, !!must_match)))
	{
		for (i = 0; allow_syntax_name[i]; ++i)
		{
			if (!(index = miko_std_syntaxor_name2index(r, allow_syntax_name[i])))
				goto label_fail;
			if (!miko_std_syntaxor_scope_add_allow(scope, index))
				goto label_fail;
		}
		if (vattr_insert_tail(r->scope_start, syntax_start, scope) &&
			vattr_insert_tail(r->scope_stop, syntax_stop, scope))
		{
			refer_free(scope);
			return r;
		}
		label_fail:
		vattr_delete(r->scope_start, syntax_start);
		vattr_delete(r->scope_stop, syntax_stop);
		refer_free(scope);
	}
	return NULL;
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

static void miko_std_syntaxor_print_1_source_pos(miko_std_syntaxor_s *restrict r, const miko_source_s *restrict source, uintptr_t pos)
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

static void miko_std_syntaxor_print_2_syntax(miko_std_syntaxor_s *restrict r, const miko_std_syntax_s *restrict s1, const miko_std_syntax_s *restrict s2)
{
	miko_log_s *restrict log;
	miko_log_into_t into;
	if (r->mlog && (log = miko_log_alloc()))
	{
		into = miko_log_into_name(log, miko_std_error__syntaxor);
		if (s1) miko_log_add_source(log, miko_log_level__error, s1->source.source, s1->source.offset);
		if (s2) miko_log_add_source(log, miko_log_level__error, s2->source.source, s2->source.offset);
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
	value = NULL;
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
			source->offset = pos;
			source->tail = pos + skip;
			if (!(value = miko_std_syntax_create_syntax(&item->id, source, p + pos, skip)))
				goto label_fail;
			if (!item->data)
				value->data.value = (refer_nstring_t) refer_save(value->data.syntax);
			else
			{
				q = item->data(item, &length);
				if (!(value->data.value = refer_dump_nstring_with_length(q, length)))
					goto label_fail;
			}
			if (!vattr_insert_tail(syntax->data.scope, value->id.name, value))
				goto label_fail;
			refer_free(value);
			value = NULL;
			pos += skip;
		}
	}
	return syntax;
	label_fail:
	refer_ck_free(value);
	miko_std_syntaxor_print_1_source_pos(r, source->source, pos);
	return NULL;
}

static miko_std_syntaxor_scope_t* miko_std_syntaxor_scope_find_tstack(tparse_tstack_s *restrict tstack, miko_std_syntaxor_scope_t *restrict stack, const miko_std_syntaxor_scope_s *restrict scope)
{
	miko_std_syntaxor_scope_t *restrict layer;
	uintptr_t i;
	for (i = 0; (layer = (miko_std_syntaxor_scope_t *) tparse_tstack_layer(tstack, i, NULL)); ++i)
	{
		if (layer->scope == scope)
		{
			memcpy(stack, layer, sizeof(*stack));
			for (i += 1; i; --i)
				tparse_tstack_pop(tstack);
			return stack;
		}
		if (layer->scope->must_match)
			break;
	}
	if (layer) memcpy(stack, layer, sizeof(*stack));
	else memset(stack, 0, sizeof(*stack));
	return NULL;
}

static miko_std_syntax_s* miko_std_syntaxor_scope(miko_std_syntaxor_s *restrict r, miko_std_syntax_s *restrict syntax)
{
	tparse_tstack_s *restrict tstack;
	const miko_std_syntaxor_scope_s *restrict scope;
	miko_std_syntaxor_scope_t *restrict layer;
	miko_std_syntax_s *restrict inode;
	vattr_vlist_t *restrict vlist_next;
	vattr_vlist_t *restrict vlist;
	refer_nstring_t syntax_nstr;
	miko_std_syntaxor_scope_t stack;
	stack.scope = NULL;
	stack.vlist_start = NULL;
	inode = NULL;
	vlist = syntax->data.scope->vattr;
	if ((tstack = tparse_tstack_alloc()))
	{
		while (vlist)
		{
			inode = (miko_std_syntax_s *) vlist->value;
			if ((syntax_nstr = inode->data.syntax))
			{
				if ((scope = (const miko_std_syntaxor_scope_s *) vattr_get_first(
					r->scope_stop, syntax_nstr->string)))
				{
					// find stop
					if (!miko_std_syntaxor_scope_find_tstack(tstack, &stack, scope))
						goto label_fail;
					if (miko_std_syntaxor_scope_allow_test(scope, stack.vlist_start, vlist))
					{
						// replace scope
						vlist_next = vlist->vattr_next;
						if (!miko_std_syntaxor_scope_replace(scope, syntax, stack.vlist_start, vlist))
							goto label_fail;
						vlist = vlist_next;
						continue;
					}
					else if (scope->must_match)
						goto label_fail;
				}
				else if ((scope = (const miko_std_syntaxor_scope_s *) vattr_get_first(
					r->scope_start, syntax_nstr->string)))
				{
					// find start
					stack.scope = scope;
					stack.vlist_start = vlist;
					if (!(layer = (miko_std_syntaxor_scope_t *) tparse_tstack_push(
						tstack, sizeof(stack), NULL)))
						goto label_fail;
					memcpy(layer, &stack, sizeof(stack));
				}
			}
			vlist = vlist->vattr_next;
		}
		miko_std_syntaxor_scope_find_tstack(tstack, &stack, NULL);
		if (!tparse_tstack_layer_number(tstack))
		{
			refer_free(tstack);
			return syntax;
		}
		label_fail:
		// print no-match log
		miko_std_syntaxor_print_2_syntax(r,
			stack.vlist_start?((const miko_std_syntax_s *) stack.vlist_start->value):NULL,
			vlist?((const miko_std_syntax_s *) vlist->value):NULL);
		refer_free(tstack);
	}
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
		if (miko_std_syntaxor_initial(r, syntax, &syntax_source) &&
			miko_std_syntaxor_scope(r, syntax))
			return syntax;
		refer_free(syntax);
	}
	return NULL;
}
