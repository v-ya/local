#ifndef _mpeg4_inner_type_h_
#define _mpeg4_inner_type_h_

#include "box.include.h"

#define mpeg4$inner$type$alloc$layer2(_atom, _inst, _name, _ts)  mpeg4_atom_layer_link(_atom, mpeg4$define(inner, type, parse)(_ts).v, mpeg4$define(atom, _name, find)(_inst))
#define mpeg4$inner$type$alloc$layer(_atom, _inst, _name)        mpeg4$inner$type$alloc$layer2(_atom, _inst, _name, #_name)

mpeg4_box_type_t mpeg4$define(inner, type, parse)(const char *restrict s);

// buffer[16]
const char* mpeg4$define(inner, type, string)(char *restrict buffer, mpeg4_box_type_t type);

#endif
