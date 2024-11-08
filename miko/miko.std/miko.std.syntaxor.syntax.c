#include "miko.std.syntaxor.h"
#include <string.h>

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
			miko_std_syntax_scope_append_vlist(r->data.scope, vlist_start, vlist_stop))
			return r;
		refer_free(r);
	}
	return NULL;
}

miko_std_syntax_s* miko_std_syntax_create_copy(const miko_std_syntax_s *restrict syntax)
{
	miko_std_syntax_s *restrict r;
	miko_std_syntax_s *restrict v;
	vattr_vlist_t *restrict vlist;
	if ((r = miko_std_syntax_alloc(&syntax->id, &syntax->source)))
	{
		r->data.syntax = (refer_nstring_t) refer_save(syntax->data.syntax);
		r->data.value = (refer_nstring_t) refer_save(syntax->data.value);
		if (syntax->data.scope)
		{
			if (!(r->data.scope = vattr_alloc()))
				goto label_fail;
			for (vlist = syntax->data.scope->vattr; vlist; vlist = vlist->vattr_next)
			{
				if (!(v = miko_std_syntax_create_copy((const miko_std_syntax_s *) vlist->value)))
					goto label_fail;
				if (!vattr_insert_tail(r->data.scope, vlist->vslot->key, v))
					goto label_fail_insert;
				refer_free(v);
			}
		}
		return r;
		label_fail_insert:
		refer_free(v);
		label_fail:
		refer_free(r);
	}
	return NULL;
}

miko_std_syntax_source_t* miko_std_syntax_fetch_source_by_vlist(miko_std_syntax_source_t *restrict source, vattr_vlist_t *vlist_first, vattr_vlist_t *vlist_tail)
{
	const miko_std_syntax_s *syntax_first;
	const miko_std_syntax_s *syntax_tail;
	if (vlist_first && (syntax_first = (const miko_std_syntax_s *) vlist_first->value) &&
		vlist_first && (syntax_tail = (const miko_std_syntax_s *) vlist_first->value) &&
		syntax_first->source.source == syntax_tail->source.source)
	{
		source->source = syntax_first->source.source;
		source->offset = syntax_first->source.offset;
		source->tail = syntax_tail->source.tail;
		return source;
	}
	return NULL;
}

vattr_s* miko_std_syntax_scope_append_vlist(vattr_s *restrict scope, vattr_vlist_t *restrict vlist_start, vattr_vlist_t *restrict vlist_stop)
{
	vattr_vlist_t *restrict vlist;
	if (scope && vlist_start && (!vlist_stop || vlist_start->vslot->vattr == vlist_stop->vslot->vattr))
	{
		for (vlist = vlist_start; vlist && vlist != vlist_stop; vlist = vlist->vattr_next)
		{
			if (!vattr_insert_tail(scope, vlist->vslot->key, vlist->value))
				goto label_fail;
		}
		return scope;
	}
	label_fail:
	return NULL;
}

vattr_s* miko_std_syntax_scope_append_vlist_copy(vattr_s *restrict scope, vattr_vlist_t *restrict vlist_start, vattr_vlist_t *restrict vlist_stop)
{
	miko_std_syntax_s *restrict syntax;
	vattr_vlist_t *restrict vlist;
	syntax = NULL;
	if (scope && vlist_start && (!vlist_stop || vlist_start->vslot->vattr == vlist_stop->vslot->vattr))
	{
		for (vlist = vlist_start; vlist && vlist != vlist_stop; vlist = vlist->vattr_next)
		{
			if (!(syntax = miko_std_syntax_create_copy((const miko_std_syntax_s *) vlist->value)))
				goto label_fail;
			if (!vattr_insert_tail(scope, vlist->vslot->key, syntax))
				goto label_fail;
			refer_free(syntax);
			syntax = NULL;
		}
		return scope;
	}
	label_fail:
	refer_ck_free(syntax);
	return NULL;
}

vattr_s* miko_std_syntax_scope_delete_vlist(vattr_vlist_t *restrict vlist_start, vattr_vlist_t *restrict vlist_stop)
{
	vattr_s *restrict scope;
	vattr_vlist_t *restrict vlist;
	if (vlist_start && (scope = vlist_start->vslot->vattr) &&
		(!vlist_stop || vlist_stop->vslot->vattr == scope))
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

vattr_s* miko_std_syntax_scope_replace_vlist2syntax(vattr_vlist_t *restrict vlist_start, vattr_vlist_t *restrict vlist_stop, const miko_std_syntax_s *restrict syntax)
{
	vattr_s *restrict scope;
	vattr_vlist_t *restrict vlist;
	if (syntax && syntax->id.name && vlist_start && (scope = vlist_start->vslot->vattr) &&
		(!vlist_stop || vlist_stop->vslot->vattr == scope))
	{
		if ((vlist = vattr_insert_tail(scope, syntax->id.name, syntax)))
		{
			if (vattr_moveto_vattr_last(vlist_start, vlist) &&
				miko_std_syntax_scope_delete_vlist(vlist_start, vlist_stop))
				return scope;
			vattr_delete_vlist(vlist);
		}
	}
	return NULL;
}

vattr_s* miko_std_syntax_scope_replace_vlist2vlist(vattr_vlist_t *restrict target_start, vattr_vlist_t *restrict target_stop, vattr_vlist_t *restrict source_start, vattr_vlist_t *restrict source_stop)
{
	vattr_s *restrict target;
	vattr_s *restrict source;
	vattr_vlist_t *restrict vlist;
	vattr_vlist_t *restrict vlist_insert;
	vattr_vlist_t *restrict vlist_insert_start;
	if (target_start && (target = target_start->vslot->vattr) && (!target_stop || target_stop->vslot->vattr == target) &&
		source_start && (source = source_start->vslot->vattr) && (!source_stop || source_stop->vslot->vattr == source))
	{
		vlist_insert = NULL;
		vlist_insert_start = NULL;
		for (vlist = source_start; vlist && vlist != source_stop; vlist = vlist->vattr_next)
		{
			if (!(vlist_insert = vattr_insert_tail(target, vlist->vslot->key, vlist->value)))
				goto label_fail;
			if (!vattr_moveto_vattr_last(target_start, vlist_insert))
				goto label_fail;
			if (!vlist_insert_start)
				vlist_insert_start = vlist_insert;
			vlist_insert = NULL;
		}
		if (miko_std_syntax_scope_delete_vlist(target_start, target_stop))
			return target;
		label_fail:
		if (vlist_insert)
			vattr_delete_vlist(vlist_insert);
		if (vlist_insert_start)
			miko_std_syntax_scope_delete_vlist(vlist_insert_start, target_start);
	}
	return NULL;
}

vattr_vlist_t* miko_std_syntax_scope_find_id(vattr_vlist_t *restrict vlist_start, vattr_vlist_t *restrict vlist_stop, miko_index_t id_index)
{
	vattr_vlist_t *restrict vlist;
	for (vlist = vlist_start; vlist && vlist != vlist_stop; vlist = vlist->vattr_next)
	{
		if (((const miko_std_syntax_s *) vlist->value)->id.index == id_index)
			return vlist;
	}
	return NULL;
}

vattr_vlist_t* miko_std_syntax_scope_find_syntax(vattr_vlist_t *restrict vlist_start, vattr_vlist_t *restrict vlist_stop, miko_index_t id_index, const char *restrict syntax)
{
	const miko_std_syntax_s *restrict v;
	vattr_vlist_t *restrict vlist;
	for (vlist = vlist_start; vlist && vlist != vlist_stop; vlist = vlist->vattr_next)
	{
		v = (const miko_std_syntax_s *) vlist->value;
		if (v->id.index == id_index && v->data.syntax && !strcmp(v->data.syntax->string, syntax))
			return vlist;
	}
	return NULL;
}

const miko_std_syntax_s* miko_std_syntax_verify(const miko_std_syntax_s *restrict syntax, miko_index_t id_index, const char *restrict data_syntax)
{
	if (syntax && syntax->id.index == id_index && (!data_syntax || (syntax->data.syntax &&
		!strcmp(syntax->data.syntax->string, data_syntax))))
		return syntax;
	return NULL;
}
