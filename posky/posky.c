#include "posky.h"
#include "posky_monster.h"
#include "posky_adorable.h"
#include "posky_group.h"
#include "posky_slot.h"
#include "posky_name.h"
#include <memory.h>

struct posky_s {
	uintptr_t size;
	uintptr_t number;
	uint32_t group_number;
	uint32_t group_index;
	hashmap_t group_name;      // name => (posky_group_t *), head => (const char *) name
	hashmap_t adorable_name;   // name => (posky_slot_t *), head => (const char *) name
	posky_monster_t w_filter;  // candy => (filter) ?ok => slot(address).adorable.me
	posky_monster_t w_none;    // candy => slot(address).adorable.me ?none => (none)
	posky_monster_t w_fail;    // candy => slot(address).adorable.me ?fail => (fail)
	posky_monster_t a_filter;  // set => adorable
	posky_monster_t a_fail;    // set => adorable
	queue_s *we;
	rbtree_t *group;           // index => (posky_group_t *) list
	uint64_t *map;
	posky_slot_t slot[];
};

static void posky_free_func(posky_s *restrict r)
{
	hashmap_uini(&r->group_name);
	hashmap_uini(&r->adorable_name);
	posky_monster_set(&r->w_filter, NULL, NULL);
	posky_monster_set(&r->w_none, NULL, NULL);
	posky_monster_set(&r->w_fail, NULL, NULL);
	posky_monster_set(&r->a_filter, NULL, NULL);
	posky_monster_set(&r->a_fail, NULL, NULL);
	if (r->we) refer_free(r->we);
	if (r->group)
		rbtree_clear(&r->group);
	if (r->map)
	{
		posky_slot_array_uini(r->slot, r->size);
		free(r->map);
	}
}

posky_s* posky_alloc(queue_s *we, size_t adorable_size)
{
	if (we && (uint32_t) adorable_size)
	{
		posky_s *restrict r;
		r = (posky_s *) refer_alloc(sizeof(posky_s) + sizeof(posky_slot_t) * adorable_size);
		if (r)
		{
			memset(r, 0, sizeof(posky_s));
			refer_set_free(r, (refer_free_f) posky_free_func);
			r->size = (uint32_t) adorable_size;
			if (hashmap_init(&r->group_name) && hashmap_init(&r->adorable_name) && (r->map = posky_slot_array_init(r->slot, r->size)))
			{
				r->we = (queue_s *) refer_save(we);
				return r;
			}
			refer_free(r);
		}
	}
	return NULL;
}

posky_s* posky_alloc_ring(size_t adorable_size, size_t queue_size)
{
	posky_s *restrict r;
	queue_s *restrict we;
	r = NULL;
	we = queue_alloc_ring(queue_size);
	if (we)
	{
		r = posky_alloc(we, adorable_size);
		refer_free(we);
	}
	return r;
}

void posky_set_monster_filter(posky_s *restrict posky, posky_feeding_f feeding, refer_t monster)
{
	posky_monster_set(&posky->w_filter, feeding, monster);
}

void posky_set_monster_none(posky_s *restrict posky, posky_feeding_f feeding, refer_t monster)
{
	posky_monster_set(&posky->w_none, feeding, monster);
}

void posky_set_monster_fail(posky_s *restrict posky, posky_feeding_f feeding, refer_t monster)
{
	posky_monster_set(&posky->w_fail, feeding, monster);
}

void posky_set_adorable_monster_filter(posky_s *restrict posky, posky_feeding_f feeding, refer_t monster)
{
	posky_monster_set(&posky->a_filter, feeding, monster);
}

void posky_set_adorable_monster_fail(posky_s *restrict posky, posky_feeding_f feeding, refer_t monster)
{
	posky_monster_set(&posky->a_fail, feeding, monster);
}

static posky_slot_t* posky_find_slot(posky_s *restrict posky, const char *restrict name, uint64_t address)
{
	posky_slot_t *restrict slot;
	slot = NULL;
	if (name) slot = (posky_slot_t *) hashmap_get_name(&posky->adorable_name, name);
	else
	{
		uint32_t index;
		index = (uint32_t) address;
		if (address >> 32 && index < posky->size && posky->slot[index].address == address)
			slot = posky->slot + index;
	}
	return slot;
}

const posky_candy_s* posky_feeding(posky_s *restrict posky, const posky_candy_s *restrict candy)
{
	register queue_s *restrict we;
	we = posky->we;
	if (we->push(we, candy)) return candy;
	return posky_monster_do(&posky->w_fail, candy, NULL);
}

const posky_candy_s* posky_catch_gift(posky_s *posky)
{
	register queue_s *restrict we;
	we = posky->we;
	return (const posky_candy_s *) we->pull(we);
}

const posky_candy_s* posky_feeding_adorable(posky_s *restrict posky, const posky_candy_s *restrict candy)
{
	register queue_s *restrict q;
	if (!posky_monster_do(&posky->w_filter, candy, candy))
		goto label_ok;
	if (candy->target >> 32)
	{
		// adorable
		posky_slot_t *restrict slot;
		slot = posky_find_slot(posky, NULL, candy->target);
		if (slot)
		{
			q = slot->adorable->me;
			if (q->push(q, candy)) goto label_ok;
			goto label_fail;
		}
	}
	else
	{
		// group
		rbtree_t *restrict v;
		v = posky_group_find(&posky->group, &posky->group_name, NULL, candy->target);
		if (v)
		{
			posky_group_t *restrict group;
			group = (posky_group_t *) v->value;
			while (group)
			{
				q = group->slot->adorable->me;
				if (!q->push(q, candy))
					v = NULL;
				group = group->group_next;
			}
			if (v) goto label_ok;
			goto label_fail;
		}
	}
	return posky_monster_do(&posky->w_none, candy, NULL);
	label_ok:
	return candy;
	label_fail:
	return posky_monster_do(&posky->w_fail, candy, NULL);
}

void posky_clear_gift(posky_s *restrict posky)
{
	register queue_s *restrict we;
	register const posky_candy_s* restrict candy;
	we = posky->we;
	while ((candy = (const posky_candy_s *) we->pull(we)))
	{
		posky_feeding_adorable(posky, candy);
		refer_free(candy);
	}
}

static inline void posky_delete_adorable_by_slot(posky_s *restrict posky, posky_slot_t *restrict slot)
{
	posky_delete_name(&posky->adorable_name, slot->address);
	posky_slot_delete(posky->slot, posky->map, posky->size, slot->index);
}

void posky_clear_lonely_adorable(posky_s *restrict posky)
{
	posky_slot_t *restrict slot;
	uint32_t i, n, k;
	n = posky->size;
	k = posky->number;
	for (i = 0; k && i < n; ++i)
	{
		if ((slot = posky->slot + i)->address)
		{
			--k;
			if (refer_save_number(slot->adorable) <= 1)
			{
				posky_delete_adorable_by_slot(posky, slot);
				--posky->number;
			}
		}
	}
}

uint64_t posky_create_group(posky_s *restrict posky, const char *restrict name)
{
	rbtree_t *restrict g;
	g = posky_group_create(&posky->group, &posky->group_name, &posky->group_index, name);
	if (g)
	{
		++posky->group_number;
		return g->key_index;
	}
	return posky_address_none;
}

void posky_delete_group(posky_s *restrict posky, const char *restrict name, uint64_t address)
{
	posky->group_number -= posky_group_delete(&posky->group, &posky->group_name, name, address);
}

uint64_t posky_address_group(posky_s *restrict posky, const char *restrict name)
{
	rbtree_t *g;
	g = posky_group_find(&posky->group, &posky->group_name, name, 0);
	if (g) return g->key_index;
	return posky_address_none;
}

const char* posky_name_group(posky_s *restrict posky, uint64_t address)
{
	return (const char *) hashmap_get_head(&posky->group_name, address);
}

posky_s* posky_join_group(posky_s *restrict posky, const char *restrict group_name, uint64_t group_address, const char *restrict adorable_name, uint64_t adorable_address)
{
	rbtree_t *restrict g;
	posky_slot_t *restrict slot;
	if ((g = posky_group_find(&posky->group, &posky->group_name, group_name, group_address)) &&
		(slot = posky_find_slot(posky, adorable_name, adorable_address)) && posky_group_join(g, slot))
		return posky;
	return NULL;
}

void posky_leave_group(posky_s *restrict posky, const char *restrict group_name, uint64_t group_address, const char *restrict adorable_name, uint64_t adorable_address)
{
	rbtree_t *restrict g;
	posky_slot_t *restrict slot;
	if ((g = posky_group_find(&posky->group, &posky->group_name, group_name, group_address)) &&
		(slot = posky_find_slot(posky, adorable_name, adorable_address)))
		posky_group_leave(g, slot);
}

posky_adorable_s* posky_exist_group(posky_s *restrict posky, const char *restrict group_name, uint64_t group_address, const char *restrict adorable_name, uint64_t adorable_address)
{
	rbtree_t *restrict g;
	posky_slot_t *restrict slot;
	if ((g = posky_group_find(&posky->group, &posky->group_name, group_name, group_address)) &&
		(slot = posky_find_slot(posky, adorable_name, adorable_address)) && posky_group_exist(g, slot))
		return slot->adorable;
	return NULL;
}

posky_adorable_s* posky_create_adorable(posky_s *restrict posky, const char *restrict name, queue_s *me, refer_t lady)
{
	posky_adorable_s *restrict r;
	posky_slot_t *restrict slot;
	if (posky->number < posky->size)
	{
		slot = posky_slot_gen_empty(posky->slot, posky->map, posky->size);
		if (slot)
		{
			if ((slot->adorable = r = posky_adorable_alloc(slot->address, posky->we, me, lady)) &&
				(!name || posky_create_name(&posky->adorable_name, name, r->address, slot)))
			{
				posky_monster_set(&r->filter, posky->a_filter.feeding, posky->a_filter.monster);
				posky_monster_set(&r->fail, posky->a_fail.feeding, posky->a_fail.monster);
				++posky->number;
				return (posky_adorable_s *) refer_save(r);
			}
			posky_delete_adorable_by_slot(posky, slot);
		}
	}
	return NULL;
}

posky_adorable_s* posky_create_adorable_ring(posky_s *restrict posky, const char *restrict name, refer_t lady, size_t queue_size)
{
	posky_adorable_s *restrict r;
	queue_s *restrict me;
	r = NULL;
	me = queue_alloc_ring(queue_size);
	if (me)
	{
		r = posky_create_adorable(posky, name, me, lady);
		refer_free(me);
	}
	return r;
}

void posky_delete_adorable(posky_s *restrict posky, const char *restrict name, uint64_t address)
{
	posky_slot_t *restrict slot;
	if ((slot = posky_find_slot(posky, name, address)))
	{
		posky_delete_adorable_by_slot(posky, slot);
		--posky->number;
	}
}

posky_adorable_s* posky_find_adorable(posky_s *restrict posky, const char *restrict name, uint64_t address)
{
	posky_slot_t *restrict slot;
	if ((slot = posky_find_slot(posky, name, address)))
		return slot->adorable;
	return NULL;
}

uint64_t posky_address_adorable(posky_s *restrict posky, const char *restrict name)
{
	posky_slot_t *restrict slot;
	slot = (posky_slot_t *) hashmap_get_name(&posky->adorable_name, name);
	if (slot) return slot->address;
	return posky_address_none;
}

const char* posky_name_adorable(posky_s *restrict posky, uint64_t address)
{
	return (const char *) hashmap_get_head(&posky->adorable_name, address);
}
