#include "mpeg4.inst.h"
#include "mpeg4.atom.h"
#include "mpeg4.box.h"
#include <stdlib.h>
#include <memory.h>

static void mpeg4_rbtree_free_refer_func(rbtree_t *restrict r)
{
	if (r->value)
		refer_free(r->value);
}

const struct mpeg4_atom_s* mpeg4_find_atom(mpeg4_t *restrict inst, mpeg4_atom_alloc_f func, uint32_t type, uint32_t extra)
{
	rbtree_t *restrict r;
	const mpeg4_atom_s *restrict atom;
	uint64_t index;
	index = ((uint64_t) extra << 32) | (uint64_t) type;
	if ((r = rbtree_find(&inst->pool, NULL, index)))
		goto label_ok;
	if ((atom = func(inst)))
	{
		if ((r = rbtree_insert(&inst->pool, NULL, index, atom, mpeg4_rbtree_free_refer_func)))
		{
			label_ok:
			return (const mpeg4_atom_s *) r->value;
		}
		refer_free(atom);
	}
	return NULL;
}
