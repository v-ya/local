#include "posky_adorable.h"

static void posky_adorable_free_func(posky_adorable_s *restrict r)
{
	if (r->we) refer_free(r->we);
	if (r->me) refer_free(r->me);
	if (r->lady) refer_free(r->lady);
	rbtree_clear(&r->way);
}

posky_adorable_s* posky_adorable_alloc(uint64_t address, queue_s *we, queue_s *me, refer_t lady)
{
	posky_adorable_s *restrict r;
	r = (posky_adorable_s *) refer_alloz(sizeof(posky_adorable_s));
	if (r)
	{
		refer_set_free(r, (refer_free_f) posky_adorable_free_func);
		r->address = address;
		r->we = (queue_s *) refer_save(we);
		r->me = (queue_s *) refer_save(me);
		r->lady = refer_save(lady);
	}
	return r;
}

uint64_t posky_adorable_address(posky_adorable_s *restrict adorable)
{
	return adorable->address;
}

posky_adorable_s* posky_adorable_set_type_way(posky_adorable_s *restrict adorable, uint32_t type, posky_feeding_f feeding)
{
	rbtree_t *restrict v;
	v = rbtree_find(&adorable->way, NULL, (uint64_t) type);
	if (v)
	{
		if (feeding) v->value = feeding;
		else rbtree_delete_by_pointer(&adorable->way, v);
	}
	else if (feeding)
	{
		if (!rbtree_insert(&adorable->way, NULL, (uint64_t) type, feeding, NULL))
			return NULL;
	}
	return adorable;
}

void posky_adorable_set_type_finally(posky_adorable_s *restrict adorable, posky_feeding_f feeding)
{
	adorable->finally = feeding;
}

void posky_adorable_set_monster_filter(posky_adorable_s *restrict adorable, posky_feeding_f feeding, refer_t monster)
{
	posky_monster_set(&adorable->filter, feeding, monster);
}

void posky_adorable_set_monster_fail(posky_adorable_s *restrict adorable, posky_feeding_f feeding, refer_t monster)
{
	posky_monster_set(&adorable->fail, feeding, monster);
}

const posky_candy_s* posky_adorable_feeding(posky_adorable_s *restrict adorable, const posky_candy_s *restrict candy)
{
	register queue_s *restrict we;
	if (!posky_monster_do(&adorable->filter, candy, candy))
		return candy;
	we = adorable->we;
	if (we->push(we, candy)) return candy;
	return posky_monster_do(&adorable->fail, candy, NULL);
}

const posky_candy_s* posky_adorable_catch_gift(posky_adorable_s *adorable)
{
	register queue_s *restrict me;
	me = adorable->me;
	return (const posky_candy_s *) me->pull(me);
}

static inline posky_feeding_f posky_adorable_find_way(posky_adorable_s *restrict adorable, uint32_t type)
{
	rbtree_t *restrict v;
	if ((v = rbtree_find(&adorable->way, NULL, (uint64_t) type)))
		return (posky_feeding_f) v->value;
	return adorable->finally;
}

const posky_candy_s* posky_adorable_open_gift(posky_adorable_s *restrict adorable, const posky_candy_s *restrict candy)
{
	posky_feeding_f feeding;
	if ((feeding = posky_adorable_find_way(adorable, candy->type)))
		return feeding(adorable->lady, candy);
	return NULL;
}

void posky_adorable_clear_gift(posky_adorable_s *restrict adorable)
{
	register queue_s *restrict me;
	register const posky_candy_s* restrict candy;
	me = adorable->me;
	while ((candy = (const posky_candy_s *) me->pull(me)))
	{
		register posky_feeding_f feeding;
		if ((feeding = posky_adorable_find_way(adorable, candy->type)))
			feeding(adorable->lady, candy);
		refer_free(candy);
	}
}
