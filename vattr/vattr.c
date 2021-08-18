#include "vattr.h"
#include <stdlib.h>

// vlist

static vattr_vlist_t* vattr_vlist_alloc(register vattr_vslot_t *restrict vslot, vattr_vlist_t **restrict p_vattr, vattr_vlist_t **restrict p_vslot, refer_t value)
{
	register vattr_vlist_t *restrict r;
	if ((r = (vattr_vlist_t *) malloc(sizeof(vattr_vlist_t))))
	{
		// vattr list
		if (!(r->vattr_next = *p_vattr))
			*vslot->pp_vattr_tail = &r->vattr_next;
		else (*p_vattr)->p_vattr_next = &r->vattr_next;
		r->p_vattr_next = p_vattr;
		*p_vattr = r;
		// vslot list
		if (!(r->vslot_next = *p_vslot))
			vslot->p_vslot_tail = &r->vslot_next;
		else (*p_vslot)->p_vslot_next = &r->vslot_next;
		r->p_vslot_next = p_vslot;
		*p_vslot = r;
		// slot
		r->slot = vslot;
		// value
		r->value = refer_save(value);
		// vslot
		vslot->number += 1;
	}
	return r;
}

static void vattr_vlist_free(register vattr_vlist_t *restrict vlist)
{
	register vattr_vslot_t *restrict vslot = vlist->slot;
	vslot->number -= 1;
	if ((*vlist->p_vattr_next = vlist->vattr_next))
		vlist->vattr_next->p_vattr_next = vlist->p_vattr_next;
	else *vslot->pp_vattr_tail = vlist->p_vattr_next;
	if ((*vlist->p_vslot_next = vlist->vslot_next))
		vlist->vslot_next->p_vslot_next = vlist->p_vslot_next;
	else vslot->p_vslot_tail = vlist->p_vslot_next;
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
		r->pp_vattr_tail = &vattr->p_vattr_tail;
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

vattr_vslot_t* vattr_get_vslot(vattr_s *restrict vattr, const char *restrict key)
{
	return (vattr_vslot_t *) hashmap_get_name(&vattr->vslot, key);
}

uintptr_t vattr_get_vslot_number(vattr_s *restrict vattr, const char *restrict key)
{
	vattr_vslot_t *restrict vslot;
	if ((vslot = (vattr_vslot_t *) hashmap_get_name(&vattr->vslot, key)))
		return vslot->number;
	return 0;
}

vattr_vlist_t* vattr_vslot_first(vattr_vslot_t *restrict vslot)
{
	return vslot->vslot;
}

vattr_vlist_t* vattr_vslot_tail(vattr_vslot_t *restrict vslot)
{
	return (vattr_vlist_t *) ((uintptr_t) vslot->p_vslot_tail - offsetof(vattr_vlist_t, vslot_next));
}

vattr_vlist_t* vattr_vslot_index(vattr_vslot_t *restrict vslot, uintptr_t index)
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

vattr_vlist_t* vattr_get_vlist_first(vattr_s *restrict vattr, const char *restrict key)
{
	vattr_vslot_t *restrict vslot;
	if ((vslot = (vattr_vslot_t *) hashmap_get_name(&vattr->vslot, key)))
		return vslot->vslot;
	return NULL;
}

vattr_vlist_t* vattr_get_vlist_tail(vattr_s *restrict vattr, const char *restrict key)
{
	vattr_vslot_t *restrict vslot;
	if ((vslot = (vattr_vslot_t *) hashmap_get_name(&vattr->vslot, key)))
		return vattr_vslot_tail(vslot);
	return NULL;
}

vattr_vlist_t* vattr_get_vlist_index(vattr_s *restrict vattr, const char *restrict key, uintptr_t index)
{
	vattr_vslot_t *restrict vslot;
	if ((vslot = (vattr_vslot_t *) hashmap_get_name(&vattr->vslot, key)))
		return vattr_vslot_index(vslot, index);
	return NULL;
}

refer_t vattr_get_first(vattr_s *restrict vattr, const char *restrict key)
{
	vattr_vlist_t *restrict vlist;
	if ((vlist = vattr_get_vlist_first(vattr, key)))
		return vlist->value;
	return NULL;
}

refer_t vattr_get_tail(vattr_s *restrict vattr, const char *restrict key)
{
	vattr_vlist_t *restrict vlist;
	if ((vlist = vattr_get_vlist_tail(vattr, key)))
		return vlist->value;
	return NULL;
}

refer_t vattr_get_index(vattr_s *restrict vattr, const char *restrict key, uintptr_t index)
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
	return vattr_vlist_alloc(vlist->slot, vlist->p_vattr_next, vlist->p_vslot_next, value);
}

vattr_vlist_t* vattr_insert_vlist_next(vattr_vlist_t *restrict vlist, refer_t value)
{
	return vattr_vlist_alloc(vlist->slot, &vlist->vattr_next, &vlist->vslot_next, value);
}

vattr_vlist_t* vattr_insert_vslot_first(vattr_vslot_t *restrict vslot, refer_t value)
{
	return vattr_vlist_alloc(vslot, vslot->vslot->p_vattr_next, &vslot->vslot, value);
}

vattr_vlist_t* vattr_insert_vslot_tail(vattr_vslot_t *restrict vslot, refer_t value)
{
	return vattr_vlist_alloc(vslot, *vslot->pp_vattr_tail, vslot->p_vslot_tail, value);
}

vattr_vlist_t* vattr_insert_vslot_index(vattr_vslot_t *restrict vslot, uintptr_t index, refer_t value)
{
	vattr_vlist_t *restrict vlist;
	if ((vlist = vattr_vslot_index(vslot, index)))
		return vattr_vlist_alloc(vlist->slot, vlist->p_vattr_next, vlist->p_vslot_next, value);
	return vattr_vlist_alloc(vslot, *vslot->pp_vattr_tail, vslot->p_vslot_tail, value);
}

vattr_vlist_t* vattr_insert_first(vattr_s *restrict vattr, const char *restrict key, refer_t value)
{
	vattr_vslot_t *restrict vslot;
	if ((vslot = vattr_get_vslot(vattr, key)))
		return vattr_insert_vslot_first(vslot, value);
	return vattr_set(vattr, key, value);
}

vattr_vlist_t* vattr_insert_tail(vattr_s *restrict vattr, const char *restrict key, refer_t value)
{
	vattr_vslot_t *restrict vslot;
	if ((vslot = vattr_get_vslot(vattr, key)))
		return vattr_insert_vslot_tail(vslot, value);
	return vattr_set(vattr, key, value);
}

vattr_vlist_t* vattr_insert_index(vattr_s *restrict vattr, const char *restrict key, uintptr_t index, refer_t value)
{
	vattr_vslot_t *restrict vslot;
	if ((vslot = vattr_get_vslot(vattr, key)))
		return vattr_insert_vslot_index(vslot, index, value);
	return vattr_set(vattr, key, value);
}

vattr_vslot_t* vattr_delete_vlist(vattr_s *restrict vattr, vattr_vlist_t *restrict vlist)
{
	vattr_vslot_t *restrict vslot;
	vslot = vlist->slot;
	vattr_vlist_free(vlist);
	if (vslot->vslot)
		return vslot;
	hashmap_delete_name(&vattr->vslot, vslot->key);
	return NULL;
}

void vattr_delete_vslot(vattr_s *restrict vattr, vattr_vslot_t *restrict vslot)
{
	hashmap_delete_name(&vattr->vslot, vslot->key);
}

vattr_vslot_t* vattr_delete_vslot_first(vattr_s *restrict vattr, vattr_vslot_t *restrict vslot)
{
	return vattr_delete_vlist(vattr, vslot->vslot);
}

vattr_vslot_t* vattr_delete_vslot_tail(vattr_s *restrict vattr, vattr_vslot_t *restrict vslot)
{
	return vattr_delete_vlist(vattr, vattr_vslot_tail(vslot));
}

vattr_vslot_t* vattr_delete_vslot_index(vattr_s *restrict vattr, vattr_vslot_t *restrict vslot, uintptr_t index)
{
	vattr_vlist_t *restrict vlist;
	if ((vlist = vattr_vslot_index(vslot, index)))
		return vattr_delete_vlist(vattr, vlist);
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
		return vattr_delete_vlist(vattr, vslot->vslot);
	return NULL;
}

vattr_vslot_t* vattr_delete_tail(vattr_s *restrict vattr, const char *restrict key)
{
	vattr_vslot_t *restrict vslot;
	if ((vslot = vattr_get_vslot(vattr, key)))
		return vattr_delete_vlist(vattr, vattr_vslot_tail(vslot));
	return NULL;
}

vattr_vslot_t* vattr_delete_index(vattr_s *restrict vattr, const char *restrict key, uintptr_t index)
{
	vattr_vslot_t *restrict vslot;
	vattr_vlist_t *restrict vlist;
	if ((vslot = vattr_get_vslot(vattr, key)))
	{
		if ((vlist = vattr_vslot_index(vslot, index)))
			return vattr_delete_vlist(vattr, vlist);
	}
	return vslot;
}
