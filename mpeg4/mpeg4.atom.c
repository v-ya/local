#include "mpeg4.atom.h"

void mpeg4_atom_free_default_func(mpeg4_atom_s *restrict r)
{
	if (r->layer)
		rbtree_clear(&r->layer);
	if (r->method)
		rbtree_clear(&r->method);
}

mpeg4_atom_s* mpeg4_atom_alloc_empty(void)
{
	mpeg4_atom_s *restrict r;
	if ((r = (mpeg4_atom_s *) refer_alloz(sizeof(mpeg4_atom_s))))
		refer_set_free(r, (refer_free_f) mpeg4_atom_free_default_func);
	return r;
}

mpeg4_atom_s* mpeg4_atom_layer_link(mpeg4_atom_s *restrict atom, uint32_t type, const mpeg4_atom_s *restrict layer)
{
	if (layer && rbtree_insert(&atom->layer, NULL, (uint64_t) type, layer, NULL))
		return atom;
	return NULL;
}

const mpeg4_atom_s* mpeg4_atom_layer_find(const mpeg4_atom_s *restrict atom, uint32_t type)
{
	register const rbtree_t *restrict v;
	if ((v = rbtree_find(&atom->layer, NULL, (uint64_t) type)))
		return (const mpeg4_atom_s *) v->value;
	return NULL;
}

