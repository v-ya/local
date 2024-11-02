#include "miko.env.h"
#include "miko.iset.h"

static void miko_env_reset(miko_env_s *restrict r)
{
	if (r->runtime)
	{
		refer_free(r->runtime);
		r->runtime = NULL;
	}
	vattr_clear(r->segment);
	vattr_clear(r->action);
	vattr_clear(r->interrupt);
	vattr_clear(r->major);
	vattr_clear(r->process);
	vattr_clear(r->instruction);
}

static vattr_s* miko_env_vattr_append_vslot(vattr_s *restrict target, const vattr_vslot_t *restrict source)
{
	const vattr_vlist_t *restrict vlist;
	vattr_vlist_t *restrict pos;
	if (!(pos = vattr_get_vlist_first(target, source->key)))
	{
		for (vlist = source->vslot; vlist; vlist = vlist->vslot_next)
		{
			if (!vattr_insert_tail(target, source->key, vlist->value))
				goto label_fail;
		}
	}
	else
	{
		for (vlist = source->vslot; vlist; vlist = vlist->vslot_next)
		{
			if (!vattr_insert_vlist_last(pos, vlist->value))
				goto label_fail;
		}
	}
	return target;
	label_fail:
	return NULL;
}

// target = {"A" => a1, "B" => {b1, b2}, "C" => {c1, c2}}
// source = {"X" => {x1, x2}, "B" => {b3, b4, b5}, "Y" => y1, "A" => a2, "Z" => z1}
// => target = {"A" => {a2, a1}, "B" => {b3, b4, b5, b1, b2}, "C" => {c1, c2}, "X" => {x1, x2}, "Y" => y1, "Z" => z1}
static vattr_s* miko_env_vattr_append(vattr_s *restrict target, const vattr_s *restrict source)
{
	const vattr_vlist_t *restrict vlist;
	for (vlist = source->vattr; vlist; vlist = vlist->vattr_next)
	{
		// vlist is first of vslot
		if (vlist->vslot->vslot == vlist)
		{
			if (!miko_env_vattr_append_vslot(target, vlist->vslot))
				goto label_fail;
		}
	}
	return target;
	label_fail:
	return NULL;
}

miko_env_s* miko_env_okay(miko_env_s *restrict r)
{
	const vattr_vlist_t *restrict vlist;
	const miko_iset_s *restrict iset;
	miko_env_reset(r);
	for (vlist = r->pool->vattr; vlist; vlist = vlist->vattr_next)
	{
		iset = (const miko_iset_s *) vlist->value;
		if (!miko_env_vattr_append(r->segment, iset->segment))
			goto label_fail;
		if (!miko_env_vattr_append(r->action, iset->action))
			goto label_fail;
		if (!miko_env_vattr_append(r->interrupt, iset->interrupt))
			goto label_fail;
		if (!miko_env_vattr_append(r->major, iset->major))
			goto label_fail;
		if (!miko_env_vattr_append(r->process, iset->process))
			goto label_fail;
		if (!miko_env_vattr_append(r->instruction, iset->instruction))
			goto label_fail;
	}
	if ((r->runtime = miko_env_runtime_alloc(r)))
		return r;
	label_fail:
	return NULL;
}
