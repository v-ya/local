#include "miko.std.marco.h"
#include "miko.std.syntaxor.h"

static void miko_std_marco_pool_free_func(miko_std_marco_pool_s *restrict r)
{
	refer_ck_free(r->pool);
}

miko_std_marco_pool_s* miko_std_marco_pool_alloc(const miko_std_marco_index_t *restrict index)
{
	miko_std_marco_pool_s *restrict r;
	if (index && index->name && index->comma && index->param &&
		(r = (miko_std_marco_pool_s *) refer_alloz(sizeof(miko_std_marco_pool_s))))
	{
		refer_hook_free(r, marco_pool);
		if ((r->pool = vattr_alloc()))
		{
			r->index = *index;
			return r;
		}
		refer_free(r);
	}
	return NULL;
}

miko_std_marco_pool_s* miko_std_marco_pool_add(miko_std_marco_pool_s *restrict r, const miko_std_syntax_s *restrict name, const miko_std_syntax_s *restrict param, const miko_std_marco_scope_s *restrict marco)
{
	miko_std_marco_s *restrict item;
	if ((name && name->data.syntax && name->id.index == r->index.name) &&
		(!param || (param->data.scope && param->id.index == r->index.param)) &&
		marco && !vattr_get_vslot(r->pool, name->data.syntax->string) &&
		(item = miko_std_marco_alloc(&r->index, param, marco)))
	{
		if (!vattr_insert_tail(r->pool, name->data.syntax->string, item))
			r = NULL;
		refer_free(item);
		return r;
	}
	return NULL;
}

miko_std_marco_pool_s* miko_std_marco_pool_del(miko_std_marco_pool_s *restrict r, const miko_std_syntax_s *restrict name)
{
	vattr_vslot_t *restrict vslot;
	if ((name && name->data.syntax && name->id.index == r->index.name) &&
		(vslot = vattr_get_vslot(r->pool, name->data.syntax->string)))
	{
		vattr_delete_vslot(vslot);
		return r;
	}
	return NULL;
}

const miko_std_marco_s* miko_std_marco_pool_exist(miko_std_marco_pool_s *restrict r, const miko_std_syntax_s *restrict name, miko_bool_t *restrict need_param)
{
	const miko_std_marco_s *restrict marco;
	if ((name && name->data.syntax && name->id.index == r->index.name) &&
		(marco = (const miko_std_marco_s *) vattr_get_first(r->pool, name->data.syntax->string)))
	{
		if (need_param) *need_param = !!marco->name_cache;
		return marco;
	}
	if (need_param) *need_param = 0;
	return NULL;
}

static miko_std_marco_scope_s* miko_std_marco_pool_marco_replace_args(miko_std_marco_scope_s *restrict scope, miko_std_marco_args_s *restrict args, miko_index_t name)
{
	miko_std_marco_argv_s *restrict argv;
	miko_std_syntax_s *restrict v;
	vattr_vlist_t *restrict vlist;
	vattr_vlist_t *restrict vnext;
	for (vlist = scope->vattr; vlist; vlist = vnext)
	{
		vnext = vlist->vattr_next;
		v = (miko_std_syntax_s *) vlist->value;
		if (v->id.index == name && v->data.syntax &&
			(argv = (miko_std_marco_argv_s *) vattr_get_first(args, v->data.syntax->string)))
		{
			if (!miko_std_syntax_scope_replace_vlist2vlist(vlist, vnext, argv->vattr, NULL))
				goto label_fail;
		}
		else if (v->data.scope && !miko_std_marco_pool_marco_replace_args(v->data.scope, args, name))
			goto label_fail;
	}
	return scope;
	label_fail:
	return NULL;
}

static miko_std_marco_scope_s* miko_std_marco_pool_marco_with_conflict(miko_std_marco_pool_s *restrict r, const miko_std_marco_s *restrict marco, const miko_std_syntax_s *restrict param, rbtree_t *restrict *restrict conflict);

static miko_std_marco_pool_s* miko_std_marco_pool_marco_replace_marco(miko_std_marco_pool_s *restrict r, vattr_vlist_t *restrict vlist, vattr_vlist_t *restrict vstop, rbtree_t *restrict *restrict conflict)
{
	const miko_std_marco_s *restrict marco;
	miko_std_marco_scope_s *restrict result;
	miko_std_syntax_s *restrict param;
	miko_std_syntax_s *restrict v;
	vattr_vlist_t *restrict vnext;
	while (vlist && vlist != vstop)
	{
		vnext = vlist->vattr_next;
		v = (miko_std_syntax_s *) vlist->value;
		if ((marco = miko_std_marco_pool_exist(r, v, NULL)))
		{
			param = NULL;
			if (marco->name_cache)
			{
				if (!vnext || vnext == vstop)
					goto label_fail;
				param = (miko_std_syntax_s *) vnext->value;
				vnext = vnext->vattr_next;
			}
			if (!(result = miko_std_marco_pool_marco_with_conflict(r, marco, param, conflict)))
				goto label_fail;
			if (!miko_std_syntax_scope_replace_vlist2vlist(vlist, vnext, result->vattr, NULL))
				goto label_fail_result;
			refer_free(result);
		}
		else if (v->data.scope && !miko_std_marco_pool_marco_replace_marco(r, v->data.scope->vattr, NULL, conflict))
			goto label_fail;
		vlist = vnext;
	}
	return r;
	label_fail_result:
	refer_free(result);
	label_fail:
	return NULL;
}

static miko_std_marco_scope_s* miko_std_marco_pool_marco_with_conflict(miko_std_marco_pool_s *restrict r, const miko_std_marco_s *restrict marco, const miko_std_syntax_s *restrict param, rbtree_t *restrict *restrict conflict)
{
	miko_std_marco_scope_s *restrict scope;
	miko_std_marco_args_s *restrict args;
	if (!rbtree_find(conflict, NULL, (uintptr_t) marco) &&
		rbtree_insert(conflict, NULL, (uintptr_t) marco, NULL, NULL))
	{
		if (!(!marco->name_cache ^ !param) && (scope = miko_std_marco_create_scope(marco)))
		{
			if (param)
			{
				if (!(args = miko_std_marco_create_args(marco, &r->index, param)))
					goto label_fail;
				if (!miko_std_marco_pool_marco_replace_args(scope, args, r->index.name))
					goto label_fail_args;
				refer_free(args);
			}
			if (!miko_std_marco_pool_marco_replace_marco(r, scope->vattr, NULL, conflict))
				goto label_fail;
			rbtree_delete(conflict, NULL, (uintptr_t) marco);
			return scope;
			label_fail_args:
			refer_free(args);
			label_fail:
			refer_free(scope);
		}
		rbtree_delete(conflict, NULL, (uintptr_t) marco);
	}
	return NULL;
}

miko_std_marco_scope_s* miko_std_marco_pool_save_result(miko_std_marco_pool_s *restrict r, const miko_std_marco_s *restrict marco, const miko_std_syntax_s *restrict param)
{
	miko_std_marco_scope_s *restrict scope;
	rbtree_t *conflict;
	conflict = NULL;
	scope = miko_std_marco_pool_marco_with_conflict(r, marco, param, &conflict);
	rbtree_clear(&conflict);
	return scope;
}

miko_std_marco_pool_s* miko_std_marco_pool_replace_scope(miko_std_marco_pool_s *restrict r, miko_std_marco_scope_s *restrict scope)
{
	rbtree_t *conflict;
	conflict = NULL;
	r = miko_std_marco_pool_marco_replace_marco(r, scope->vattr, NULL, &conflict);
	rbtree_clear(&conflict);
	return r;
}

miko_std_marco_pool_s* miko_std_marco_pool_replace_vlist(miko_std_marco_pool_s *restrict r, vattr_vlist_t *restrict vlist_start, vattr_vlist_t *restrict vlist_stop)
{
	rbtree_t *conflict;
	conflict = NULL;
	r = miko_std_marco_pool_marco_replace_marco(r, vlist_start, vlist_stop, &conflict);
	rbtree_clear(&conflict);
	return r;
}
