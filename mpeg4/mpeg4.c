#include "mpeg4.h"
#include "mpeg4.inst.h"
#include "mpeg4.box.h"
#include "box/box.include.h"

static void mpeg4_free_func(mpeg4_s *restrict r)
{
	if (r->pool)
		rbtree_clear(&r->pool);
}

mpeg4_s* mpeg4_alloc(void)
{
	mpeg4_s *restrict r;
	if ((r = (mpeg4_s *) refer_alloz(sizeof(mpeg4_s))))
	{
		refer_set_free(r, (refer_free_f) mpeg4_free_func);
		if ((r->unknow = mpeg4$define(atom, unknow, find)(r)) &&
			(r->root = mpeg4$define(atom, file, find)(r)))
			return r;
		refer_free(r);
	}
	return NULL;
}

const mpeg4_s* mpeg4_dump(const mpeg4_s *restrict inst, mlog_s *restrict mlog, const uint8_t *restrict data, uint64_t size)
{
	if (inst->root->interface.dump)
	{
		mpeg4_atom_dump_t unidata = {
			.dump_level = mlog_level_inital,
			.dump_samples = 0,
			.timescale = 1
		};
		if (inst->root->interface.dump(inst->root, mlog, data, size, &unidata))
			return inst;
	}
	return NULL;
}
