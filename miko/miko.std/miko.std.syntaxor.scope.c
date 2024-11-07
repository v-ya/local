#include "miko.std.syntaxor.h"

static void miko_std_syntaxor_scope_free_func(miko_std_syntaxor_scope_s *restrict r)
{
	refer_ck_free(r->id.name);
	refer_ck_free(r->syntax);
	rbtree_clear(&r->allow);
}

miko_std_syntaxor_scope_s* miko_std_syntaxor_scope_alloc(const miko_std_syntax_id_t *restrict id, const char *restrict syntax_scope, uintptr_t must_match)
{
	miko_std_syntaxor_scope_s *restrict r;
	if (id && id->name && id->index && (r = (miko_std_syntaxor_scope_s *) refer_alloz(sizeof(miko_std_syntaxor_scope_s))))
	{
		refer_hook_free(r, syntaxor_scope);
		r->id.name = (refer_string_t) refer_save(id->name);
		r->id.index = id->index;
		r->must_match = must_match;
		if ((!syntax_scope || (r->syntax = refer_dump_nstring(syntax_scope))))
			return r;
		refer_free(r);
	}
	return NULL;
}

miko_std_syntaxor_scope_s* miko_std_syntaxor_scope_add_allow(miko_std_syntaxor_scope_s *restrict r, miko_index_t index)
{
	if (rbtree_find(&r->allow, NULL, (uint64_t) index) ||
		rbtree_insert(&r->allow, NULL, (uint64_t) index, NULL, NULL))
		return r;
	return NULL;
}

const miko_std_syntaxor_scope_s* miko_std_syntaxor_scope_allow_test(const miko_std_syntaxor_scope_s *restrict r, vattr_vlist_t *restrict vlist_start, vattr_vlist_t *restrict vlist_stop)
{
	const miko_std_syntax_s *restrict syntax;
	vattr_vlist_t *restrict vlist;
	if (vlist_start->vslot->vattr == vlist_stop->vslot->vattr)
	{
		vlist_stop = vlist_stop->vattr_next;
		for (vlist = vlist_start; vlist && vlist != vlist_stop; vlist = vlist->vattr_next)
		{
			syntax = (const miko_std_syntax_s *) vlist->value;
			if (!rbtree_find(&r->allow, NULL, (uint64_t) syntax->id.index))
				goto label_fail;
		}
		if (vlist == vlist_stop)
			return r;
	}
	label_fail:
	return NULL;
}

const miko_std_syntaxor_scope_s* miko_std_syntaxor_scope_replace(const miko_std_syntaxor_scope_s *restrict r, miko_std_syntax_s *restrict scope, vattr_vlist_t *restrict vlist_start, vattr_vlist_t *restrict vlist_stop)
{
	miko_std_syntax_s *restrict syntax;
	miko_std_syntax_source_t source;
	if (miko_std_syntax_fetch_source_by_vlist(&source, vlist_start, vlist_stop) &&
		(syntax = miko_std_syntax_create_scope_by_vlist(&r->id, &source,
			vlist_start->vattr_next, vlist_stop)))
	{
		syntax->data.syntax = (refer_nstring_t) refer_save(r->syntax);
		syntax->data.value = (refer_nstring_t) refer_save(r->syntax);
		if (miko_std_syntax_scope_replace_vlist2syntax(scope,
			vlist_start, vlist_stop->vattr_next, syntax))
		{
			refer_free(syntax);
			return r;
		}
		refer_free(syntax);
	}
	return NULL;
}
