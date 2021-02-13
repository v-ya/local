#ifndef _posky_slot_h_
#define _posky_slot_h_

#include "posky.h"
#include <stdlib.h>

typedef struct posky_slot_t posky_slot_t;
struct posky_group_t;

struct posky_slot_t {
	uint64_t address;
	uint32_t version;
	uint32_t index;
	posky_adorable_s *adorable;
	struct posky_group_t *group;
};

uint64_t* posky_slot_array_init(posky_slot_t *restrict array, uint32_t size);
void posky_slot_array_uini(posky_slot_t *restrict array, uint32_t size);
posky_slot_t* posky_slot_gen_empty(posky_slot_t *restrict array, uint64_t *restrict map, uint32_t size);
void posky_slot_delete(posky_slot_t *restrict array, uint64_t *restrict map, uint32_t size, uint32_t index);
void posky_slot_clear(posky_slot_t *restrict slot);

#endif
