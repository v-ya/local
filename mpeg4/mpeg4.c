#include "mpeg4.h"
#include "mpeg4.inst.h"
#include "mpeg4.box.h"
#include "box/box.include.h"
#include "box/inner.type.h"

static void mpeg4_free_func(mpeg4_s *restrict r)
{
	if (r->pool)
		rbtree_clear(&r->pool);
}

const mpeg4_s* mpeg4_alloc(void)
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

const mpeg4_s* mpeg4_dump(const mpeg4_s *restrict inst, mlog_s *restrict mlog, const uint8_t *restrict data, uint64_t size, uint32_t dump_samples)
{
	register const mpeg4_atom_s *restrict atom;
	register mpeg4_dump_f dump;
	if ((dump = (atom = inst->root)->interface.dump))
	{
		mpeg4_atom_dump_t unidata = {
			.dump_level = mlog_level_inital,
			.dump_samples = !!dump_samples,
			.timescale = 1
		};
		if (dump(inst->root, mlog, data, size, &unidata))
			return inst;
	}
	return NULL;
}

mpeg4_stuff_s* mpeg4_create_root_stuff(const mpeg4_s *restrict inst)
{
	register const mpeg4_atom_s *restrict atom;
	register mpeg4_create_f create;
	if ((create = (atom = inst->root)->interface.create))
		return create(atom, inst, (mpeg4_box_type_t) { .v = 0 });
	return NULL;
}

mpeg4_stuff_s* mpeg4_create_stuff(const mpeg4_stuff_s *restrict container, const char *restrict type)
{
	register const mpeg4_atom_s *restrict atom;
	register mpeg4_create_f create;
	mpeg4_box_type_t t;
	if (type && (atom = container->atom) &&
		(t = mpeg4$define(inner, type, check)(type)).v &&
		(atom = mpeg4_atom_layer_find(atom, t.v)) &&
		(create = atom->interface.create))
		return create(atom, container->inst, t);
	return NULL;
}

mpeg4_stuff_s* mpeg4_link_stuff(mpeg4_stuff_s *restrict container, mpeg4_stuff_s *restrict stuff)
{
	if (mpeg4_atom_layer_find(container->atom, stuff->info.type.v))
		return mpeg4_stuff_container_link(container, stuff);
	return NULL;
}

mpeg4_stuff_s* mpeg4_append_stuff(mpeg4_stuff_s *restrict container, const char *restrict type)
{
	register const mpeg4_atom_s *restrict atom;
	mpeg4_box_type_t t;
	if (type && (atom = container->atom) && (t = mpeg4$define(inner, type, check)(type)).v)
		return mpeg4_stuff_container_append(container, t);
	return NULL;
}

mpeg4_stuff_s* mpeg4_parse(const mpeg4_s *restrict inst, const uint8_t *restrict data, uint64_t size)
{
	register mpeg4_stuff_s *restrict r;
	register mpeg4_parse_f parse;
	r = mpeg4_create_root_stuff(inst);
	if (r)
	{
		if ((parse = r->atom->interface.parse) && parse(r, data, size))
			return r;
		refer_free(r);
	}
	return NULL;
}

mpeg4_stuff_s* mpeg4_calc(mpeg4_stuff_s *restrict stuff)
{
	register mpeg4_calc_f calc;
	if (stuff->info.calc_okay)
		goto label_okay;
	if ((calc = stuff->atom->interface.calc) && calc(stuff))
	{
		label_okay:
		return stuff;
	}
	return NULL;
}

uint64_t mpeg4_size(const mpeg4_stuff_s *restrict stuff)
{
	if (stuff->info.calc_okay)
		return stuff->info.all_size;
	return 0;
}

const mpeg4_stuff_s* mpeg4_build(const mpeg4_stuff_s *restrict stuff, uint8_t *restrict data)
{
	register mpeg4_build_f build;
	if (stuff->info.calc_okay && (build = stuff->atom->interface.build) && build(stuff, data))
		return stuff;
	return NULL;
}
