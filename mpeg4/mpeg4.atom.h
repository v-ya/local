#ifndef _mpeg4_atom_h_
#define _mpeg4_atom_h_

#include "mpeg4.inst.h"

typedef struct mpeg4_atom_t mpeg4_atom_t;

typedef struct mpeg4_atom_dump_t mpeg4_atom_dump_t;

typedef const mpeg4_atom_t* (*mpeg4_dump_f)(const mpeg4_atom_t *restrict atom, mlog_s *restrict mlog, const uint8_t *restrict data, uint64_t size, mpeg4_atom_dump_t *restrict unidata);

#define mpeg4$define$alloc(_name)   mpeg4_atom_t* mpeg4$define(atom, _name, alloc)(struct mpeg4_s *restrict inst)
#define mpeg4$define$find(_name)    const mpeg4_atom_t* mpeg4$define(atom, _name, find)(struct mpeg4_s *restrict inst)
#define mpeg4$define$dump(_name)    const mpeg4_atom_t* mpeg4$define(atom, _name, dump)(const mpeg4_atom_t *restrict atom, mlog_s *restrict mlog, const uint8_t *restrict data, uint64_t size, mpeg4_atom_dump_t *restrict unidata)

typedef struct mpeg4_atom_func_t {
	mpeg4_dump_f dump;
} mpeg4_atom_func_t;

struct mpeg4_atom_t {
	mpeg4_atom_func_t interface;
	rbtree_t *layer;
};

void mpeg4_atom_free_default_func(mpeg4_atom_t *restrict r);
mpeg4_atom_t* mpeg4_atom_alloc_empty(void);
mpeg4_atom_t* mpeg4_atom_layer_link(mpeg4_atom_t *restrict atom, uint32_t type, const mpeg4_atom_t *restrict layer);
const mpeg4_atom_t* mpeg4_atom_layer_find(const mpeg4_atom_t *restrict atom, uint32_t type);

#endif
