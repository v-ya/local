#include "../vkaa.tpool.h"
#include "../vkaa.error.h"
#include "../vkaa.type.h"
#include "../vkaa.var.h"

static void vkaa_tpool_free_func(vkaa_tpool_s *restrict r)
{
	if (r->n2t) refer_free(r->n2t);
	if (r->i2v) rbtree_clear(&r->i2v);
	if (r->i2t) rbtree_clear(&r->i2t);
	if (r->e) refer_free(r->e);
}

static void vkaa_tpool_rbtree_free_func(rbtree_t *restrict rbv)
{
	if (rbv->value) refer_free(rbv->value);
}

vkaa_tpool_s* vkaa_tpool_alloc(void)
{
	vkaa_tpool_s *restrict r;
	if ((r = (vkaa_tpool_s *) refer_alloz(sizeof(vkaa_tpool_s))))
	{
		refer_set_free(r, (refer_free_f) vkaa_tpool_free_func);
		if ((r->n2t = vattr_alloc()) && (r->e = vkaa_error_alloc()))
			return r;
		refer_free(r);
	}
	return NULL;
}

uintptr_t vkaa_tpool_genid(vkaa_tpool_s *restrict tpool)
{
	return ++tpool->id_last;
}

const vkaa_type_s* vkaa_tpool_find_name(const vkaa_tpool_s *restrict tpool, const char *restrict name)
{
	return (const vkaa_type_s *) vattr_get_first(tpool->n2t, name);
}

const vkaa_type_s* vkaa_tpool_find_id(const vkaa_tpool_s *restrict tpool, uintptr_t id)
{
	rbtree_t *restrict v;
	if ((v = rbtree_find(&tpool->i2t, NULL, (uint64_t) id)))
		return (const vkaa_type_s *) v->value;
	return NULL;
}

vkaa_tpool_s* vkaa_tpool_insert(vkaa_tpool_s *restrict tpool, vkaa_type_s *restrict type)
{
	rbtree_t *restrict vid;
	if (type->id && !rbtree_find(&tpool->i2t, NULL, (uint64_t) type->id) &&
		(vid = rbtree_insert(&tpool->i2t, NULL, (uint64_t) type->id, type, vkaa_tpool_rbtree_free_func)))
	{
		refer_save(type);
		if (!type->name || vattr_insert_first(tpool->n2t, type->name, type))
		{
			if (!type->error)
				type->error = (vkaa_error_s *) refer_save(tpool->e);
			return tpool;
		}
		rbtree_delete_by_pointer(&tpool->i2t, vid);
	}
	return NULL;
}

void vkaa_tpool_remove_by_name(vkaa_tpool_s *restrict tpool, const char *restrict name)
{
	vattr_vlist_t *restrict vl;
	const vkaa_type_s *restrict type;
	if ((vl = vattr_get_vlist_first(tpool->n2t, name)))
	{
		if ((type = (const vkaa_type_s *) vl->value))
		{
			rbtree_delete(&tpool->i2t, NULL, (uint64_t) type->id);
			rbtree_delete(&tpool->i2v, NULL, (uint64_t) type->id);
		}
		vattr_delete_vlist(vl);
	}
}

void vkaa_tpool_remove_by_id(vkaa_tpool_s *restrict tpool, uintptr_t id)
{
	rbtree_t *restrict vid;
	const vkaa_type_s *restrict type;
	if ((vid = rbtree_find(&tpool->i2t, NULL, (uint64_t) id)))
	{
		if ((type = (const vkaa_type_s *) vid->value) && type->name)
			vattr_delete_first(tpool->n2t, type->name);
		rbtree_delete_by_pointer(&tpool->i2t, vid);
		rbtree_delete(&tpool->i2v, NULL, (uint64_t) id);
	}
}

void vkaa_tpool_clear(vkaa_tpool_s *restrict tpool)
{
	vattr_clear(tpool->n2t);
	rbtree_clear(&tpool->i2t);
	rbtree_clear(&tpool->i2v);
	tpool->id_last = 0;
}

vkaa_var_s* vkaa_tpool_var_const_enable(vkaa_tpool_s *restrict tpool, const vkaa_type_s *restrict type)
{
	vkaa_var_s *restrict var;
	rbtree_delete(&tpool->i2v, NULL, type->id);
	if ((var = type->create(type)))
	{
		if (rbtree_insert(&tpool->i2v, NULL, type->id, var, vkaa_tpool_rbtree_free_func))
			return var;
		refer_free(var);
	}
	return NULL;
}

vkaa_var_s* vkaa_tpool_var_const_enable_by_name(vkaa_tpool_s *restrict tpool, const char *restrict name)
{
	const vkaa_type_s *restrict type;
	if ((type = vkaa_tpool_find_name(tpool, name)))
		return vkaa_tpool_var_const_enable(tpool, type);
	return NULL;
}

vkaa_var_s* vkaa_tpool_var_const_enable_by_id(vkaa_tpool_s *restrict tpool, uintptr_t id)
{
	const vkaa_type_s *restrict type;
	if ((type = vkaa_tpool_find_id(tpool, id)))
		return vkaa_tpool_var_const_enable(tpool, type);
	return NULL;
}

void vkaa_tpool_var_const_disable_by_name(vkaa_tpool_s *restrict tpool, const char *restrict name)
{
	const vkaa_type_s *restrict type;
	if ((type = vkaa_tpool_find_name(tpool, name)))
		rbtree_delete(&tpool->i2v, NULL, type->id);
}

void vkaa_tpool_var_const_disable_by_id(vkaa_tpool_s *restrict tpool, uintptr_t id)
{
	rbtree_delete(&tpool->i2v, NULL, id);
}

vkaa_var_s* vkaa_tpool_var_create(const vkaa_tpool_s *restrict tpool, const vkaa_type_s *restrict type)
{
	rbtree_t *restrict rbv;
	if ((rbv = rbtree_find(&tpool->i2v, NULL, type->id)))
		return (vkaa_var_s *) refer_save(rbv->value);
	return type->create(type);
}

vkaa_var_s* vkaa_tpool_var_create_by_name(const vkaa_tpool_s *restrict tpool, const char *restrict name)
{
	const vkaa_type_s *restrict type;
	if ((type = vkaa_tpool_find_name(tpool, name)))
		return vkaa_tpool_var_create(tpool, type);
	return NULL;
}

vkaa_var_s* vkaa_tpool_var_create_by_id(const vkaa_tpool_s *restrict tpool, uintptr_t id)
{
	const vkaa_type_s *restrict type;
	if ((type = vkaa_tpool_find_id(tpool, id)))
		return vkaa_tpool_var_create(tpool, type);
	return NULL;
}
