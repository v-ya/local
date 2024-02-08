#include "vattr.h"
#include <stdlib.h>

#define inv_offset_ptr(_t, _m, _p)  ((_t *) ((uintptr_t) (_p) - offsetof(_t, _m)))

// vlist

static inline void vattr_vlist_link_vattr(register vattr_vlist_t *restrict vlist, register vattr_s *restrict vattr, vattr_vlist_t **restrict p_vattr)
{
	if (!(vlist->vattr_next = *p_vattr))
		vattr->p_vattr_tail = &vlist->vattr_next;
	else (*p_vattr)->p_vattr_next = &vlist->vattr_next;
	vlist->p_vattr_next = p_vattr;
	*p_vattr = vlist;
}

static inline void vattr_vlist_unlink_vattr(register vattr_vlist_t *restrict vlist, register vattr_s *restrict vattr)
{
	if ((*vlist->p_vattr_next = vlist->vattr_next))
		vlist->vattr_next->p_vattr_next = vlist->p_vattr_next;
	else vattr->p_vattr_tail = vlist->p_vattr_next;
}

static inline void vattr_vlist_link_vslot(register vattr_vlist_t *restrict vlist, register vattr_vslot_t *restrict vslot, vattr_vlist_t **restrict p_vslot)
{
	if (!(vlist->vslot_next = *p_vslot))
		vslot->p_vslot_tail = &vlist->vslot_next;
	else (*p_vslot)->p_vslot_next = &vlist->vslot_next;
	vlist->p_vslot_next = p_vslot;
	*p_vslot = vlist;
	vslot->number += 1;
}

static inline void vattr_vlist_unlink_vslot(register vattr_vlist_t *restrict vlist, register vattr_vslot_t *restrict vslot)
{
	vslot->number -= 1;
	if ((*vlist->p_vslot_next = vlist->vslot_next))
		vlist->vslot_next->p_vslot_next = vlist->p_vslot_next;
	else vslot->p_vslot_tail = vlist->p_vslot_next;
}

static vattr_vlist_t* vattr_vlist_alloc(vattr_vslot_t *restrict vslot, vattr_vlist_t **restrict p_vattr, vattr_vlist_t **restrict p_vslot, refer_t value)
{
	vattr_vlist_t *restrict r;
	if ((r = (vattr_vlist_t *) malloc(sizeof(vattr_vlist_t))))
	{
		vattr_vlist_link_vattr(r, vslot->vattr, p_vattr);
		vattr_vlist_link_vslot(r, vslot, p_vslot);
		r->vslot = vslot;
		r->value = refer_save(value);
	}
	return r;
}

static void vattr_vlist_free(register vattr_vlist_t *restrict vlist)
{
	vattr_vslot_t *restrict vslot;
	vslot = vlist->vslot;
	vattr_vlist_unlink_vattr(vlist, vslot->vattr);
	vattr_vlist_unlink_vslot(vlist, vslot);
	if (vlist->value)
		refer_free(vlist->value);
	free(vlist);
}

// vslot

static void vattr_vslot_free(vattr_vslot_t *restrict vslot)
{
	register vattr_vlist_t *v;
	while ((v = vslot->vslot))
		vattr_vlist_free(v);
	free(vslot);
}

static vattr_vslot_t* vattr_vslot_alloc(vattr_s *restrict vattr)
{
	vattr_vslot_t *restrict r;
	if ((r = (vattr_vslot_t *) malloc(sizeof(vattr_vslot_t))))
	{
		r->key = NULL;
		r->number = 0;
		r->vslot = NULL;
		r->p_vslot_tail = &r->vslot;
		r->vattr = vattr;
	}
	return r;
}

// vattr

static void vattr_vslot_free_func(hashmap_vlist_t *restrict vl)
{
	if (vl->value)
		vattr_vslot_free((vattr_vslot_t *) vl->value);
}

static void vattr_free_func(vattr_s *restrict r)
{
	hashmap_uini(&r->vslot);
}

vattr_s* vattr_alloc(void)
{
	vattr_s *restrict r;
	if ((r = (vattr_s *) refer_alloc(sizeof(vattr_s))))
	{
		if (hashmap_init(&r->vslot))
		{
			refer_set_free(r, (refer_free_f) vattr_free_func);
			r->vattr = NULL;
			r->p_vattr_tail = &r->vattr;
			return r;
		}
		refer_free(r);
	}
	return NULL;
}

void vattr_clear(vattr_s *restrict vattr)
{
	hashmap_clear(&vattr->vslot);
}

vattr_s* vattr_append(vattr_s *restrict dst, const vattr_s *restrict src)
{
	const vattr_vlist_t *restrict v;
	for (v = src->vattr; v; v = v->vattr_next)
	{
		if (!vattr_insert_tail(dst, v->vslot->key, v->value))
			goto label_fail;
	}
	return dst;
	label_fail:
	return NULL;
}

vattr_s* vattr_copy(vattr_s *restrict dst, const vattr_s *restrict src)
{
	hashmap_clear(&dst->vslot);
	return vattr_append(dst, src);
}

vattr_vlist_t* vattr_first(const vattr_s *restrict vattr)
{
	return vattr->vattr;
}

vattr_vlist_t* vattr_tail(const vattr_s *restrict vattr)
{
	return inv_offset_ptr(vattr_vlist_t, vattr_next, vattr->p_vattr_tail);
}

vattr_vlist_t* vattr_next(const vattr_vlist_t *restrict vlist)
{
	return vlist->vattr_next;
}

vattr_vlist_t* vattr_last(const vattr_vlist_t *restrict vlist)
{
	return inv_offset_ptr(vattr_vlist_t, vattr_next, vlist->p_vattr_next);
}

vattr_vlist_t* vattr_next_end(const vattr_s *restrict vattr)
{
	return NULL;
}

vattr_vlist_t* vattr_last_end(const vattr_s *restrict vattr)
{
	return inv_offset_ptr(vattr_vlist_t, vattr_next, &vattr->vattr);
}

vattr_vlist_t* vattr_vslot_first(const vattr_vslot_t *restrict vslot)
{
	return vslot->vslot;
}

vattr_vlist_t* vattr_vslot_tail(const vattr_vslot_t *restrict vslot)
{
	return inv_offset_ptr(vattr_vlist_t, vslot_next, vslot->p_vslot_tail);
}

vattr_vlist_t* vattr_vslot_next(const vattr_vlist_t *restrict vlist)
{
	return vlist->vslot_next;
}

vattr_vlist_t* vattr_vslot_last(const vattr_vlist_t *restrict vlist)
{
	return inv_offset_ptr(vattr_vlist_t, vslot_next, vlist->p_vslot_next);
}

vattr_vlist_t* vattr_vslot_next_end(const vattr_vslot_t *restrict vslot)
{
	return NULL;
}

vattr_vlist_t* vattr_vslot_last_end(const vattr_vslot_t *restrict vslot)
{
	return inv_offset_ptr(vattr_vlist_t, vslot_next, &vslot->vslot);
}

vattr_vlist_t* vattr_vslot_index(const vattr_vslot_t *restrict vslot, uintptr_t index)
{
	if (index < vslot->number)
	{
		vattr_vlist_t *restrict v;
		v = vslot->vslot;
		while (index)
		{
			--index;
			v = v->vslot_next;
		}
		return v;
	}
	return NULL;
}

vattr_vslot_t* vattr_get_vslot(const vattr_s *restrict vattr, const char *restrict key)
{
	return (vattr_vslot_t *) hashmap_get_name(&vattr->vslot, key);
}

uintptr_t vattr_get_vslot_number(const vattr_s *restrict vattr, const char *restrict key)
{
	vattr_vslot_t *restrict vslot;
	if ((vslot = (vattr_vslot_t *) hashmap_get_name(&vattr->vslot, key)))
		return vslot->number;
	return 0;
}

vattr_vlist_t* vattr_get_vlist_first(const vattr_s *restrict vattr, const char *restrict key)
{
	vattr_vslot_t *restrict vslot;
	if ((vslot = (vattr_vslot_t *) hashmap_get_name(&vattr->vslot, key)))
		return vslot->vslot;
	return NULL;
}

vattr_vlist_t* vattr_get_vlist_tail(const vattr_s *restrict vattr, const char *restrict key)
{
	vattr_vslot_t *restrict vslot;
	if ((vslot = (vattr_vslot_t *) hashmap_get_name(&vattr->vslot, key)))
		return vattr_vslot_tail(vslot);
	return NULL;
}

vattr_vlist_t* vattr_get_vlist_index(const vattr_s *restrict vattr, const char *restrict key, uintptr_t index)
{
	vattr_vslot_t *restrict vslot;
	if ((vslot = (vattr_vslot_t *) hashmap_get_name(&vattr->vslot, key)))
		return vattr_vslot_index(vslot, index);
	return NULL;
}

refer_t vattr_get_first(const vattr_s *restrict vattr, const char *restrict key)
{
	vattr_vlist_t *restrict vlist;
	if ((vlist = vattr_get_vlist_first(vattr, key)))
		return vlist->value;
	return NULL;
}

refer_t vattr_get_tail(const vattr_s *restrict vattr, const char *restrict key)
{
	vattr_vlist_t *restrict vlist;
	if ((vlist = vattr_get_vlist_tail(vattr, key)))
		return vlist->value;
	return NULL;
}

refer_t vattr_get_index(const vattr_s *restrict vattr, const char *restrict key, uintptr_t index)
{
	vattr_vlist_t *restrict vlist;
	if ((vlist = vattr_get_vlist_index(vattr, key, index)))
		return vlist->value;
	return NULL;
}

vattr_vlist_t* vattr_set(vattr_s *restrict vattr, const char *restrict key, refer_t value)
{
	vattr_vslot_t *restrict vslot;
	vattr_vlist_t *restrict vlist;
	hashmap_vlist_t *restrict vl;
	if ((vslot = vattr_vslot_alloc(vattr)))
	{
		if ((vl = hashmap_set_name(&vattr->vslot, key, vslot, vattr_vslot_free_func)))
		{
			vslot->key = vl->name;
			if ((vlist = vattr_vlist_alloc(vslot, vattr->p_vattr_tail, vslot->p_vslot_tail, value)))
				return vlist;
			hashmap_delete_name(&vattr->vslot, key);
		}
		else vattr_vslot_free(vslot);
	}
	return NULL;
}

vattr_vlist_t* vattr_insert_vlist_last(vattr_vlist_t *restrict vlist, refer_t value)
{
	return vattr_vlist_alloc(vlist->vslot, vlist->p_vattr_next, vlist->p_vslot_next, value);
}

vattr_vlist_t* vattr_insert_vlist_next(vattr_vlist_t *restrict vlist, refer_t value)
{
	return vattr_vlist_alloc(vlist->vslot, &vlist->vattr_next, &vlist->vslot_next, value);
}

vattr_vlist_t* vattr_insert_vslot_first_vattr(vattr_vslot_t *restrict vslot, refer_t value)
{
	return vattr_vlist_alloc(vslot, &vslot->vattr->vattr, &vslot->vslot, value);
}

vattr_vlist_t* vattr_insert_vslot_first_vslot(vattr_vslot_t *restrict vslot, refer_t value)
{
	return vattr_vlist_alloc(vslot, vslot->vslot->p_vattr_next, &vslot->vslot, value);
}

vattr_vlist_t* vattr_insert_vslot_tail_vslot(vattr_vslot_t *restrict vslot, refer_t value)
{
	return vattr_vlist_alloc(vslot, &inv_offset_ptr(vattr_vlist_t, vslot_next, vslot->p_vslot_tail)->vattr_next, &vslot->vslot, value);
}

vattr_vlist_t* vattr_insert_vslot_tail_vattr(vattr_vslot_t *restrict vslot, refer_t value)
{
	return vattr_vlist_alloc(vslot, vslot->vattr->p_vattr_tail, vslot->p_vslot_tail, value);
}

vattr_vlist_t* vattr_insert_vslot_index_last(vattr_vslot_t *restrict vslot, uintptr_t index, refer_t value)
{
	vattr_vlist_t *restrict vlist;
	if ((vlist = vattr_vslot_index(vslot, index)))
		return vattr_vlist_alloc(vlist->vslot, vlist->p_vattr_next, vlist->p_vslot_next, value);
	return vattr_vlist_alloc(vslot, vslot->vattr->p_vattr_tail, vslot->p_vslot_tail, value);
}

vattr_vlist_t* vattr_insert_vslot_index_next(vattr_vslot_t *restrict vslot, uintptr_t index, refer_t value)
{
	vattr_vlist_t *restrict vlist;
	if ((vlist = vattr_vslot_index(vslot, index)))
		return vattr_vlist_alloc(vlist->vslot, &vlist->vattr_next, &vlist->vslot_next, value);
	return vattr_vlist_alloc(vslot, vslot->vattr->p_vattr_tail, vslot->p_vslot_tail, value);
}

vattr_vlist_t* vattr_insert_first(vattr_s *restrict vattr, const char *restrict key, refer_t value)
{
	vattr_vslot_t *restrict vslot;
	if ((vslot = vattr_get_vslot(vattr, key)))
		return vattr_insert_vslot_first_vattr(vslot, value);
	return vattr_set(vattr, key, value);
}

vattr_vlist_t* vattr_insert_first_vslot(vattr_s *restrict vattr, const char *restrict key, refer_t value)
{
	vattr_vslot_t *restrict vslot;
	if ((vslot = vattr_get_vslot(vattr, key)))
		return vattr_insert_vslot_first_vslot(vslot, value);
	return vattr_set(vattr, key, value);
}

vattr_vlist_t* vattr_insert_tail_vslot(vattr_s *restrict vattr, const char *restrict key, refer_t value)
{
	vattr_vslot_t *restrict vslot;
	if ((vslot = vattr_get_vslot(vattr, key)))
		return vattr_insert_vslot_tail_vslot(vslot, value);
	return vattr_set(vattr, key, value);
}

vattr_vlist_t* vattr_insert_tail(vattr_s *restrict vattr, const char *restrict key, refer_t value)
{
	vattr_vslot_t *restrict vslot;
	if ((vslot = vattr_get_vslot(vattr, key)))
		return vattr_insert_vslot_tail_vattr(vslot, value);
	return vattr_set(vattr, key, value);
}

vattr_vlist_t* vattr_insert_index_last(vattr_s *restrict vattr, const char *restrict key, uintptr_t index, refer_t value)
{
	vattr_vslot_t *restrict vslot;
	if ((vslot = vattr_get_vslot(vattr, key)))
		return vattr_insert_vslot_index_last(vslot, index, value);
	return vattr_set(vattr, key, value);
}

vattr_vlist_t* vattr_insert_index_next(vattr_s *restrict vattr, const char *restrict key, uintptr_t index, refer_t value)
{
	vattr_vslot_t *restrict vslot;
	if ((vslot = vattr_get_vslot(vattr, key)))
		return vattr_insert_vslot_index_next(vslot, index, value);
	return vattr_set(vattr, key, value);
}

vattr_vlist_t* vattr_moveto_vattr_last(vattr_vlist_t *restrict vpos, vattr_vlist_t *restrict item)
{
	vattr_s *restrict vattr;
	if ((uintptr_t) vpos != (uintptr_t) item && (vattr = item->vslot->vattr) == vpos->vslot->vattr)
	{
		vattr_vlist_unlink_vattr(item, vattr);
		vattr_vlist_link_vattr(item, vattr, vpos->p_vattr_next);
		return item;
	}
	return NULL;
}

vattr_vlist_t* vattr_moveto_vattr_next(vattr_vlist_t *restrict vpos, vattr_vlist_t *restrict item)
{
	vattr_s *restrict vattr;
	if ((uintptr_t) vpos != (uintptr_t) item && (vattr = item->vslot->vattr) == vpos->vslot->vattr)
	{
		vattr_vlist_unlink_vattr(item, vattr);
		vattr_vlist_link_vattr(item, vattr, &vpos->vattr_next);
		return item;
	}
	return NULL;
}

vattr_vlist_t* vattr_moveto_vslot_last(vattr_vlist_t *restrict vpos, vattr_vlist_t *restrict item)
{
	vattr_vslot_t *restrict vslot;
	if ((uintptr_t) vpos != (uintptr_t) item && (vslot = item->vslot) == vpos->vslot)
	{
		vattr_vlist_unlink_vslot(item, vslot);
		vattr_vlist_link_vslot(item, vslot, vpos->p_vslot_next);
		return item;
	}
	return NULL;
}

vattr_vlist_t* vattr_moveto_vslot_next(vattr_vlist_t *restrict vpos, vattr_vlist_t *restrict item)
{
	vattr_vslot_t *restrict vslot;
	if ((uintptr_t) vpos != (uintptr_t) item && (vslot = item->vslot) == vpos->vslot)
	{
		vattr_vlist_unlink_vslot(item, vslot);
		vattr_vlist_link_vslot(item, vslot, &vpos->vslot_next);
		return item;
	}
	return NULL;
}

vattr_vslot_t* vattr_delete_vlist(vattr_vlist_t *restrict vlist)
{
	vattr_vslot_t *restrict vslot;
	vslot = vlist->vslot;
	vattr_vlist_free(vlist);
	if (vslot->vslot)
		return vslot;
	hashmap_delete_name(&vslot->vattr->vslot, vslot->key);
	return NULL;
}

void vattr_delete_vslot(vattr_vslot_t *restrict vslot)
{
	hashmap_delete_name(&vslot->vattr->vslot, vslot->key);
}

vattr_vslot_t* vattr_delete_vslot_first(vattr_vslot_t *restrict vslot)
{
	return vattr_delete_vlist(vslot->vslot);
}

vattr_vslot_t* vattr_delete_vslot_tail(vattr_vslot_t *restrict vslot)
{
	return vattr_delete_vlist(vattr_vslot_tail(vslot));
}

vattr_vslot_t* vattr_delete_vslot_index(vattr_vslot_t *restrict vslot, uintptr_t index)
{
	vattr_vlist_t *restrict vlist;
	if ((vlist = vattr_vslot_index(vslot, index)))
		return vattr_delete_vlist(vlist);
	return vslot;
}

void vattr_delete(vattr_s *restrict vattr, const char *restrict key)
{
	hashmap_delete_name(&vattr->vslot, key);
}

vattr_vslot_t* vattr_delete_first(vattr_s *restrict vattr, const char *restrict key)
{
	vattr_vslot_t *restrict vslot;
	if ((vslot = vattr_get_vslot(vattr, key)))
		return vattr_delete_vlist(vslot->vslot);
	return NULL;
}

vattr_vslot_t* vattr_delete_tail(vattr_s *restrict vattr, const char *restrict key)
{
	vattr_vslot_t *restrict vslot;
	if ((vslot = vattr_get_vslot(vattr, key)))
		return vattr_delete_vlist(vattr_vslot_tail(vslot));
	return NULL;
}

vattr_vslot_t* vattr_delete_index(vattr_s *restrict vattr, const char *restrict key, uintptr_t index)
{
	vattr_vslot_t *restrict vslot;
	vattr_vlist_t *restrict vlist;
	if ((vslot = vattr_get_vslot(vattr, key)))
	{
		if ((vlist = vattr_vslot_index(vslot, index)))
			return vattr_delete_vlist(vlist);
	}
	return vslot;
}
