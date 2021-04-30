#ifndef _mpeg4_stuff_h_
#define _mpeg4_stuff_h_

#include "mpeg4.inst.h"
#include "mpeg4.box.h"

typedef struct mpeg4_stuff_s mpeg4_stuff_t;

typedef mpeg4_stuff_t* (*mpeg4_stuff_init_f)(mpeg4_stuff_t *restrict stuff);

typedef struct mpeg4_stuff_container_type_finder_t {
	mpeg4_stuff_t *same_list;
	mpeg4_stuff_t **p_same_tail;
} mpeg4_stuff_container_type_finder_t;

typedef struct mpeg4_stuff_container_t {
	rbtree_t *finder;
	mpeg4_stuff_t *list;
	mpeg4_stuff_t **p_tail;
} mpeg4_stuff_container_t;

typedef struct mpeg4_stuff_link_t {
	mpeg4_stuff_t *next;          // = *this->p_next
	mpeg4_stuff_t *same_next;     // = *this->p_same_next
	mpeg4_stuff_t **p_next;       // = &last->next
	mpeg4_stuff_t **p_same_next;  // = &same_last->same_next
	mpeg4_stuff_t *container;     // = parent
} mpeg4_stuff_link_t;

typedef struct mpeg4_stuff_box_t {
	mpeg4_box_type_t type;  // box type
	uint32_t calc_okay;     // (box size && all size) is vaild
	uint64_t inner_size;    // box inner size
	uint64_t all_size;      // box inner size + box header size
	uint64_t link_number;   // number of child box
} mpeg4_stuff_box_t;

struct mpeg4_stuff_s {
	const mpeg4_t *inst;                // refer save
	const struct mpeg4_atom_s *atom;    // only link
	mpeg4_stuff_container_t container;  // list => refer save
	mpeg4_stuff_link_t link;            // only link
	mpeg4_stuff_box_t info;
};

void mpeg4_stuff_free_default_func(mpeg4_stuff_t *restrict r);
mpeg4_stuff_t* mpeg4_stuff_alloc(const struct mpeg4_atom_s *restrict atom, const mpeg4_t *restrict inst, mpeg4_box_type_t type, size_t size, mpeg4_stuff_init_f init_func, refer_free_f free_func);
void mpeg4_stuff_calc_okay(mpeg4_stuff_t *restrict stuff, uint64_t box_inner_size);
void mpeg4_stuff_calc_invalid(mpeg4_stuff_t *restrict stuff);
mpeg4_stuff_t* mpeg4_stuff_container_link(mpeg4_stuff_t *restrict container, mpeg4_stuff_t *restrict stuff);
mpeg4_stuff_t* mpeg4_stuff_container_append(mpeg4_stuff_t *restrict container, mpeg4_box_type_t type);
mpeg4_stuff_t* mpeg4_stuff_container_find(mpeg4_stuff_t *restrict container, const mpeg4_box_type_t *restrict type);
void mpeg4_stuff_unlink(mpeg4_stuff_t *restrict stuff);

#endif
