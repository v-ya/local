#ifndef _posky_adorable_h_
#define _posky_adorable_h_

#include "posky.h"
#include "posky_monster.h"
#include <rbtree.h>

struct posky_adorable_s {
	uint64_t address;
	queue_s *we;
	queue_s *me;
	refer_t lady;
	// me => candy => way || finally
	rbtree_t *way;            // posky_candy_s.type => (posky_feeding_f)
	posky_feeding_f finally;
	posky_monster_t filter;   // candy => (filter) ?ok => we
	posky_monster_t fail;     // candy => we ?fail => (fail)
};

posky_adorable_s* posky_adorable_alloc(uint64_t address, queue_s *we, queue_s *me, refer_t lady);

#endif
