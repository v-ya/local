#include "posky_group.h"
#include "posky_slot.h"
#include "posky_name.h"
#include <stdlib.h>

static inline posky_group_t* posky_group_alloc(posky_group_t **restrict pg, posky_group_t **restrict ps, posky_slot_t *restrict slot, uint64_t address)
{
	register posky_group_t *restrict r, *restrict n;
	r = (posky_group_t *) malloc(sizeof(posky_group_t));
	if (r)
	{
		r->address = address;
		r->slot = slot;
		// group list
		if ((r->group_next = n = *pg))
			n->p_group_next = &r->group_next;
		r->p_group_next = pg;
		*pg = r;
		// slot list
		if ((r->slot_next = n = *ps))
			n->p_slot_next = &r->slot_next;
		r->p_slot_next = ps;
		*ps = r;
	}
	return r;
}

static inline void posky_group_free(register posky_group_t *restrict p)
{
	register posky_group_t *restrict n;
	// group list
	if ((n = p->group_next))
		n->p_group_next = p->p_group_next;
	*p->p_group_next = n;
	// slot list
	if ((n = p->slot_next))
		n->p_slot_next = p->p_slot_next;
	*p->p_slot_next = n;
	free(p);
}

static void posky_group_free_func(rbtree_t *restrict v)
{
	register posky_group_t *restrict p, *q;
	if ((p = (posky_group_t *) v->value))
	{
		do {
			q = p->group_next;
			posky_group_free(p);
		} while ((p = q));
	}
}

rbtree_t* posky_group_create(rbtree_t *restrict *restrict group, hashmap_t *restrict group_name, uint32_t *restrict group_index, const char *restrict name)
{
	rbtree_t *restrict v;
	uint32_t group_id;
	group_id = *group_index;
	do {
		if (!++group_id) ++group_id;
	} while (rbtree_find(group, NULL, (uint64_t) group_id));
	if ((v = rbtree_insert(group, NULL, (uint64_t) group_id, NULL, (rbtree_func_free_f) posky_group_free_func)))
	{
		*group_index = group_id;
		if (!name || posky_create_name(group_name, name, v->key_index, v))
			return v;
		--*group_index;
		rbtree_delete_by_pointer(group, v);
	}
	return NULL;
}

rbtree_t* posky_group_find(rbtree_t *const restrict *restrict group, const hashmap_t *restrict group_name, const char *restrict name, uint64_t address)
{
	if (name) return (rbtree_t *) hashmap_get_name(group_name, name);
	else return rbtree_find(group, NULL, address);
}

posky_group_t* posky_group_get(rbtree_t *const restrict *restrict group, const hashmap_t *restrict group_name, const char *restrict name, uint64_t address)
{
	rbtree_t *restrict v;
	if ((v = posky_group_find(group, group_name, name, address)))
		return (posky_group_t *) v->value;
	return NULL;
}

uint32_t posky_group_delete(rbtree_t *restrict *restrict group, hashmap_t *restrict group_name, const char *restrict name, uint64_t address)
{
	rbtree_t *restrict v;
	if ((v = posky_group_find(group, group_name, name, address)))
	{
		posky_delete_name(group_name, v->key_index);
		rbtree_delete_by_pointer(group, v);
	}
	return !!v;
}

void posky_group_list_delete_by_slot(posky_group_t *restrict slot_group_list)
{
	posky_group_t *restrict p;
	while ((p = slot_group_list))
	{
		slot_group_list = p->slot_next;
		posky_group_free(p);
	}
}

posky_group_t* posky_group_join(rbtree_t *restrict g, struct posky_slot_t *restrict slot)
{
	posky_group_t *restrict r, **restrict p;
	uint64_t address;
	p = &slot->group;
	address = g->key_index;
	while ((r = *p))
	{
		if (r->address == address)
			goto label_ok;
		p = &r->slot_next;
	}
	r = posky_group_alloc((posky_group_t **) &g->value, p, slot, address);
	label_ok:
	return r;
}

posky_group_t* posky_group_exist(rbtree_t *restrict g, struct posky_slot_t *restrict slot)
{
	posky_group_t *restrict r;
	uint64_t address;
	r = slot->group;
	address = g->key_index;
	while (r)
	{
		if (r->address == address)
			break;
		r = r->slot_next;
	}
	return r;
}

void posky_group_leave(rbtree_t *restrict g, struct posky_slot_t *restrict slot)
{
	posky_group_t *restrict r;
	if ((r = posky_group_exist(g, slot)))
		posky_group_free(r);
}
