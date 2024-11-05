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

const miko_std_syntaxor_scope_s* miko_std_syntaxor_scope_replace(const miko_std_syntaxor_scope_s *restrict r, vattr_vlist_t *restrict vlist_start, vattr_vlist_t *restrict vlist_stop)
{
	const miko_std_syntax_s *restrict syntax_start;
	const miko_std_syntax_s *restrict syntax_stop;
	miko_std_syntax_s *restrict scope;
	vattr_vlist_t *restrict vlist;
	vattr_s *restrict vattr;
	if ((vattr = vlist_start->vslot->vattr) == vlist_stop->vslot->vattr)
	{
		syntax_start = (const miko_std_syntax_s *) vlist_start->value;
		syntax_stop = (const miko_std_syntax_s *) vlist_stop->value;
		if ((scope = miko_std_syntax_create_scope(&r->id, &syntax_start->source)))
		{
			// create and set scope
			scope->source.tail = syntax_stop->source.tail;
			scope->data.syntax = (refer_nstring_t) refer_save(r->syntax);
			scope->data.value = (refer_nstring_t) refer_save(r->syntax);
			// copy scope.data.scope by (vlist_start, vlist_stop)
			for (vlist = vlist_start->vattr_next; vlist && vlist != vlist_stop; vlist = vlist->vattr_next)
			{
				if (!vattr_insert_tail(scope->data.scope, vlist->vslot->key, vlist->value))
					goto label_fail;
			}
			if (vlist != vlist_stop)
				goto label_fail;
			// insert scope to last of vlist_start
			if (!(vlist = vattr_insert_tail(vattr, scope->id.name, scope)))
				goto label_fail;
			if (!vattr_moveto_vattr_last(vlist_start, vlist))
				goto label_fail;
			refer_free(scope);
			// delete [vlist_start, vlist_stop]
			vlist_stop = vlist_stop->vattr_next;
			for (vlist = vlist_start; vlist && vlist != vlist_stop; vlist = vlist_start)
			{
				vlist_start = vlist->vattr_next;
				vattr_delete_vlist(vlist);
			}
			return r;
			label_fail:
			refer_free(scope);
		}
	}
	return NULL;
}
