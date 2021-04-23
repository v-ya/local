#include "mpeg4.atom.h"

void mpeg4_atom_free_default_func(mpeg4_atom_t *restrict r)
{
	if (r->layer)
		rbtree_clear(&r->layer);
}

mpeg4_atom_t* mpeg4_atom_alloc_empty(void)
{
	mpeg4_atom_t *restrict r;
	if ((r = (mpeg4_atom_t *) refer_alloz(sizeof(mpeg4_atom_t))))
		refer_set_free(r, (refer_free_f) mpeg4_atom_free_default_func);
	return r;
}

mpeg4_atom_t* mpeg4_atom_layer_link(mpeg4_atom_t *restrict atom, uint32_t type, const mpeg4_atom_t *restrict layer)
{
	if (layer && rbtree_insert(&atom->layer, NULL, (uint64_t) type, layer, NULL))
		return atom;
	return NULL;
}

const mpeg4_atom_t* mpeg4_atom_layer_find(const mpeg4_atom_t *restrict atom, uint32_t type)
{
	register const rbtree_t *restrict v;
	if ((v = rbtree_find(&atom->layer, NULL, (uint64_t) type)))
		return (const mpeg4_atom_t *) v->value;
	return NULL;
}

