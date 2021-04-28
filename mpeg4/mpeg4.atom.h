#ifndef _mpeg4_atom_h_
#define _mpeg4_atom_h_

#include "mpeg4.inst.h"
#include "mpeg4.stuff.h"

typedef struct mpeg4_atom_s mpeg4_atom_s;

typedef struct mpeg4_atom_dump_t mpeg4_atom_dump_t;

typedef const mpeg4_atom_s* (*mpeg4_dump_f)(const mpeg4_atom_s *restrict atom, mlog_s *restrict mlog, const uint8_t *restrict data, uint64_t size, mpeg4_atom_dump_t *restrict unidata);
typedef mpeg4_stuff_t* (*mpeg4_create_f)(const mpeg4_atom_s *restrict atom, const mpeg4_t *restrict inst, mpeg4_box_type_t type);
typedef mpeg4_stuff_t* (*mpeg4_parse_f)(mpeg4_stuff_t *restrict stuff, const uint8_t *restrict data, uint64_t size);
typedef mpeg4_stuff_t* (*mpeg4_calc_f)(mpeg4_stuff_t *restrict stuff);
typedef const mpeg4_stuff_t* (*mpeg4_build_f)(const mpeg4_stuff_t *restrict stuff, uint8_t *restrict data);

#define mpeg4$define$alloc(_name)              mpeg4_atom_s* mpeg4$define(atom, _name, alloc)(struct mpeg4_s *restrict inst)
#define mpeg4$define$find(_name)               const mpeg4_atom_s* mpeg4$define(atom, _name, find)(struct mpeg4_s *restrict inst)
#define mpeg4$define$dump(_name)               const mpeg4_atom_s* mpeg4$define(atom, _name, dump)(const mpeg4_atom_s *restrict atom, mlog_s *restrict mlog, const uint8_t *restrict data, uint64_t size, mpeg4_atom_dump_t *restrict unidata)
#define mpeg4$define$create(_name)             mpeg4_stuff_t* mpeg4$define(atom, _name, create)(const mpeg4_atom_s *restrict atom, const mpeg4_t *restrict inst, mpeg4_box_type_t type)
#define mpeg4$define$parse(_name)              mpeg4_stuff_t* mpeg4$define(atom, _name, parse)(mpeg4_stuff_t *restrict stuff, const uint8_t *restrict data, uint64_t size)
#define mpeg4$define$calc(_name)               mpeg4_stuff_t* mpeg4$define(atom, _name, calc)(mpeg4_stuff_t *restrict stuff)
#define mpeg4$define$build(_name)              const mpeg4_stuff_t* mpeg4$define(atom, _name, build)(const mpeg4_stuff_t *restrict stuff, uint8_t *restrict data)
#define mpeg4$define$stuff$init(_name, _type)  mpeg4_stuff_t* mpeg4$define(stuff, _name, init)(_type *restrict r)
#define mpeg4$define$stuff$free(_name, _type)  void mpeg4$define(stuff, _name, free)(_type *restrict r)

typedef struct mpeg4_atom_func_t {
	mpeg4_dump_f dump;
	mpeg4_create_f create;
	mpeg4_parse_f parse;
	mpeg4_calc_f calc;
	mpeg4_build_f build;
} mpeg4_atom_func_t;

struct mpeg4_atom_s {
	mpeg4_atom_func_t interface;  // func
	rbtree_t *layer;              // (uint32_t) type => (mpeg4_atom_s *)
	rbtree_t *method;             // (mpeg4_stuff_method_t) => (mpeg4_stuff_func$*)
};

void mpeg4_atom_free_default_func(mpeg4_atom_s *restrict r);
mpeg4_atom_s* mpeg4_atom_alloc_empty(void);
mpeg4_atom_s* mpeg4_atom_layer_link(mpeg4_atom_s *restrict atom, uint32_t type, const mpeg4_atom_s *restrict layer);
const mpeg4_atom_s* mpeg4_atom_layer_find(const mpeg4_atom_s *restrict atom, uint32_t type);

#endif
