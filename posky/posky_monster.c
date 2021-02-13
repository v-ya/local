#include "posky_monster.h"

void posky_monster_set(posky_monster_t *restrict m, posky_feeding_f feeding, refer_t monster)
{
	if (m->monster) refer_free(m->monster);
	m->feeding = feeding;
	m->monster = refer_save(monster);
}
