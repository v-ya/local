#include "../vkaa.scope.h"

typedef struct vkaa_scope_notify_s vkaa_scope_notify_s;

typedef struct vkaa_scope_impl_s {
	vkaa_scope_s scope;
	vkaa_scope_notify_s *child_list;
	vkaa_scope_notify_s *parent_notify;
} vkaa_scope_impl_s;

struct vkaa_scope_notify_s {
	vkaa_scope_notify_s *next;
	vkaa_scope_notify_s **p_next;
	vkaa_scope_impl_s *child;
};

static void vkaa_scope_notify_insert(vkaa_scope_notify_s **restrict p, vkaa_scope_notify_s *restrict notify)
{
	notify->p_next = p;
	if ((notify->next = *p))
		(*p)->p_next = &notify->next;
	*p = notify;
}

static void vkaa_scope_notify_drop(vkaa_scope_notify_s *restrict notify)
{
	if ((*notify->p_next = notify->next))
		notify->next->p_next = notify->p_next;
	notify->next = NULL;
	notify->p_next = NULL;
}

static void vkaa_scope_notify_free_func(vkaa_scope_notify_s *restrict r)
{
	vkaa_scope_notify_drop(r);
	r->child->scope.parent = NULL;
	r->child->parent_notify = NULL;
}

static vkaa_scope_notify_s* vkaa_scope_impl_link(vkaa_scope_impl_s *restrict c, vkaa_scope_impl_s *restrict p)
{
	vkaa_scope_notify_s *restrict r;
	if ((r = (vkaa_scope_notify_s *) refer_alloz(sizeof(vkaa_scope_notify_s))))
	{
		r->child = c;
		vkaa_scope_notify_insert(&p->child_list, r);
		refer_set_free(r, (refer_free_f) vkaa_scope_notify_free_func);
		c->scope.parent = &p->scope;
		c->parent_notify = r;
		return r;
	}
	return NULL;
}

static void vkaa_scope_impl_clear_child_list(vkaa_scope_notify_s *volatile *restrict p_child_list)
{
	vkaa_scope_notify_s *restrict notify;
	while ((notify = *p_child_list))
		refer_free(notify);
}

static void vkaa_scope_free_func(vkaa_scope_impl_s *restrict r)
{
	vkaa_scope_impl_clear_child_list((vkaa_scope_notify_s *volatile *) &r->child_list);
	if (r->parent_notify) refer_free(r->parent_notify);
	hashmap_uini(&r->scope.var);
}

vkaa_scope_s* vkaa_scope_alloc(vkaa_scope_s *restrict parent)
{
	vkaa_scope_impl_s *restrict r;
	if ((r = (vkaa_scope_impl_s *) refer_alloz(sizeof(vkaa_scope_impl_s))))
	{
		refer_set_free(r, (refer_free_f) vkaa_scope_free_func);
		if (hashmap_init(&r->scope.var) &&
			(!parent || vkaa_scope_impl_link(r, (vkaa_scope_impl_s *) parent)))
			return &r->scope;
		refer_free(r);
	}
	return NULL;
}

static vkaa_scope_s* vkaa_scope_rpath_exist(vkaa_scope_s *restrict r, vkaa_scope_s *restrict e)
{
	while (r)
	{
		if (r == e)
			return e;
		r = r->parent;
	}
	return NULL;
}

vkaa_scope_s* vkaa_scope_set_parent(vkaa_scope_s *restrict scope, vkaa_scope_s *restrict parent)
{
	vkaa_scope_notify_s *restrict notify;
	if ((notify = ((vkaa_scope_impl_s *) scope)->parent_notify))
		refer_free(notify);
	if (!vkaa_scope_rpath_exist(parent, scope))
	{
		if (!parent || vkaa_scope_impl_link((vkaa_scope_impl_s *) scope, (vkaa_scope_impl_s *) parent))
			return scope;
	}
	return NULL;
}

vkaa_var_s* vkaa_scope_find(const vkaa_scope_s *restrict scope, const char *restrict name)
{
	vkaa_var_s *restrict var;
	var = NULL;
	while (scope && !(var = (vkaa_var_s *) hashmap_get_name(&scope->var, name)))
		scope = scope->parent;
	return var;
}

static void vkaa_scope_hashmap_free_func(hashmap_vlist_t *restrict vl)
{
	if (vl->value) refer_free(vl->value);
}

vkaa_var_s* vkaa_scope_put(vkaa_scope_s *restrict scope, const char *restrict name, vkaa_var_s *var)
{
	if (!hashmap_find_name(&scope->var, name) && hashmap_set_name(&scope->var, name, var, vkaa_scope_hashmap_free_func))
	{
		refer_save(var);
		return var;
	}
	return NULL;
}

vkaa_var_s* vkaa_scope_set(vkaa_scope_s *restrict scope, const char *restrict name, vkaa_var_s *var)
{
	if (hashmap_set_name(&scope->var, name, var, vkaa_scope_hashmap_free_func))
	{
		refer_save(var);
		return var;
	}
	return NULL;
}

void vkaa_scope_unset(vkaa_scope_s *restrict scope, const char *restrict name)
{
	hashmap_delete_name(&scope->var, name);
}
