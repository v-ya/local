#ifndef _posky_group_h_
#define _posky_group_h_

#include "posky.h"
#include <rbtree.h>
#include <hashmap.h>

typedef struct posky_group_t posky_group_t;
struct posky_slot_t;

struct posky_group_t {
	uint64_t address;
	struct posky_slot_t *slot;
	posky_group_t *group_next;
	posky_group_t *slot_next;
	posky_group_t **p_group_next;
	posky_group_t **p_slot_next;
};

rbtree_t* posky_group_create(rbtree_t *restrict *restrict group, hashmap_t *restrict group_name, uint32_t *restrict group_index, const char *restrict name);
rbtree_t* posky_group_find(rbtree_t *const restrict *restrict group, const hashmap_t *restrict group_name, const char *restrict name, uint64_t address);
posky_group_t* posky_group_get(rbtree_t *const restrict *restrict group, const hashmap_t *restrict group_name, const char *restrict name, uint64_t address);
uint32_t posky_group_delete(rbtree_t *restrict *restrict group, hashmap_t *restrict group_name, const char *restrict name, uint64_t address);

void posky_group_list_delete_by_slot(posky_group_t *restrict slot_group_list);

posky_group_t* posky_group_join(rbtree_t *restrict g, struct posky_slot_t *restrict slot);
posky_group_t* posky_group_exist(rbtree_t *restrict g, struct posky_slot_t *restrict slot);
void posky_group_leave(rbtree_t *restrict g, struct posky_slot_t *restrict slot);

#endif
