#include "miko.std.syntaxor.h"

static void miko_std_syntax_free_func(miko_std_syntax_s *restrict r)
{
	refer_ck_free(r->id.name);
	refer_ck_free(r->data.syntax);
	refer_ck_free(r->data.value);
	refer_ck_free(r->data.scope);
	refer_ck_free(r->source.source);
}

static miko_std_syntax_s* miko_std_syntax_alloc(const miko_std_syntax_id_t *restrict id, const miko_std_syntax_source_t *restrict source)
{
	miko_std_syntax_s *restrict r;
	if ((r = (miko_std_syntax_s *) refer_alloz(sizeof(miko_std_syntax_s))))
	{
		refer_hook_free(r, syntax);
		if (id)
		{
			r->id.name = (refer_string_t) refer_save(id->name);
			r->id.index = id->index;
		}
		if (source)
		{
			r->source.source = (const miko_source_s *) refer_save(source->source);
			r->source.offset = source->offset;
			r->source.tail = source->tail;
		}
		return r;
	}
	return NULL;
}

miko_std_syntax_s* miko_std_syntax_create_syntax(const miko_std_syntax_id_t *restrict id, const miko_std_syntax_source_t *restrict source, const char *restrict syntax, uintptr_t length)
{
	miko_std_syntax_s *restrict r;
	if ((r = miko_std_syntax_alloc(id, source)))
	{
		if ((r->data.syntax = (refer_nstring_t) refer_dump_nstring_with_length(syntax, length)))
			return r;
		refer_free(r);
	}
	return NULL;
}

miko_std_syntax_s* miko_std_syntax_create_scope(const miko_std_syntax_id_t *restrict id, const miko_std_syntax_source_t *restrict source)
{
	miko_std_syntax_s *restrict r;
	if ((r = miko_std_syntax_alloc(id, source)))
	{
		if ((r->data.scope = vattr_alloc()))
			return r;
		refer_free(r);
	}
	return NULL;
}

miko_std_syntax_s* miko_std_syntax_create_scope_by_vlist(const miko_std_syntax_id_t *restrict id, const miko_std_syntax_source_t *restrict source, vattr_vlist_t *restrict vlist_start, vattr_vlist_t *restrict vlist_stop)
{
	miko_std_syntax_s *restrict r;
	if ((r = miko_std_syntax_alloc(id, source)))
	{
		if ((r->data.scope = vattr_alloc()) &&
			miko_std_syntax_scope_append_vlist(r, vlist_start, vlist_stop))
			return r;
		refer_free(r);
	}
	return NULL;
}

miko_std_syntax_source_t* miko_std_syntax_fetch_source_by_vlist(miko_std_syntax_source_t *restrict source, vattr_vlist_t *restrict vlist_start, vattr_vlist_t *restrict vlist_stop)
{
	const miko_std_syntax_s *restrict syntax_start;
	const miko_std_syntax_s *restrict syntax_stop;
	if (vlist_start && (syntax_start = (const miko_std_syntax_s *) vlist_start->value) &&
		vlist_stop && (syntax_stop = (const miko_std_syntax_s *) vlist_stop->value) &&
		syntax_start->source.source == syntax_stop->source.source)
	{
		source->source = syntax_start->source.source;
		source->offset = syntax_start->source.offset;
		source->tail = syntax_stop->source.tail;
		return source;
	}
	return NULL;
}

miko_std_syntax_s* miko_std_syntax_scope_append_vlist(miko_std_syntax_s *restrict scope, vattr_vlist_t *restrict vlist_start, vattr_vlist_t *restrict vlist_stop)
{
	vattr_vlist_t *restrict vlist;
	if (scope->data.scope && vlist_start->vslot->vattr == vlist_stop->vslot->vattr)
	{
		for (vlist = vlist_start; vlist && vlist != vlist_stop; vlist = vlist->vattr_next)
		{
			if (!vattr_insert_tail(scope->data.scope, vlist->vslot->key, vlist->value))
				goto label_fail;
		}
		return scope;
	}
	label_fail:
	return NULL;
}

miko_std_syntax_s* miko_std_syntax_scope_delete_vlist(miko_std_syntax_s *restrict scope, vattr_vlist_t *restrict vlist_start, vattr_vlist_t *restrict vlist_stop)
{
	vattr_vlist_t *restrict vlist;
	if (vlist_start->vslot->vattr == scope->data.scope && vlist_stop->vslot->vattr == scope->data.scope)
	{
		for (vlist = vlist_start; vlist && vlist != vlist_stop; vlist = vlist_start)
		{
			vlist_start = vlist->vattr_next;
			vattr_delete_vlist(vlist);
		}
		return scope;
	}
	return NULL;
}

miko_std_syntax_s* miko_std_syntax_scope_replace_vlist2syntax(miko_std_syntax_s *restrict scope, vattr_vlist_t *restrict vlist_start, vattr_vlist_t *restrict vlist_stop, const miko_std_syntax_s *restrict syntax)
{
	vattr_vlist_t *restrict vlist;
	if (scope->data.scope && syntax && syntax->id.name &&
		vlist_start->vslot->vattr == scope->data.scope &&
		vlist_stop->vslot->vattr == scope->data.scope)
	{
		if (!(vlist = vattr_insert_tail(scope->data.scope, syntax->id.name, syntax)))
			goto label_fail;
		if (!vattr_moveto_vattr_last(vlist_start, vlist))
			goto label_fail;
		if (miko_std_syntax_scope_delete_vlist(scope, vlist_start, vlist_stop))
			return scope;
		vattr_delete_vlist(vlist);
	}
	label_fail:
	return NULL;
}
