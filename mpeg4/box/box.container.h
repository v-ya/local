#ifndef _mpeg4_box_container_h_
#define _mpeg4_box_container_h_

#include "box.include.h"

typedef struct mpeg4_atom$container_s {
	mpeg4_atom_s atom;
	const mpeg4_t *inst;
} mpeg4_atom$container_s;

mpeg4$define$dump(container);
mpeg4$define$parse(container);
mpeg4$define$calc(container);
mpeg4$define$build(container);

#endif
