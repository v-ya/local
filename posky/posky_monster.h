#ifndef _posky_monster_h_
#define _posky_monster_h_

#include "posky.h"

typedef struct posky_monster_t {
	posky_feeding_f feeding;
	refer_t monster;
} posky_monster_t;

void posky_monster_set(posky_monster_t *restrict m, posky_feeding_f feeding, refer_t monster);

static inline const posky_candy_s* posky_monster_do(posky_monster_t *restrict m, const posky_candy_s *candy, const posky_candy_s *rdef)
{
	if (m->feeding) return m->feeding(m->monster, candy);
	return rdef;
}

#endif
