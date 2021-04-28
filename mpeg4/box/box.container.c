#include "box.include.h"
#include "inner.type.h"

typedef struct mpeg4_atom$container_t {
	mpeg4_atom_s atom;
	const mpeg4_t *inst;
} mpeg4_atom$container_t;

mpeg4$define$dump(container)
{
	mpeg4_atom_dump_t ud;
	const mpeg4_atom_s *restrict layer;
	const mpeg4_atom_s *restrict unknow;
	uint64_t box_header_size;
	mpeg4_box_extend_size_t box_size;
	mpeg4_box_type_t box_type;
	uint32_t level;
	char type_string[16];
	ud = *unidata;
	level = ud.dump_level;
	unknow = ((const mpeg4_atom$container_t *) atom)->inst->unknow;
	while (size && (box_header_size = mpeg4_box_border_parse(data, size, &box_size, &box_type)))
	{
		mlog_level_dump("[%s] %lu\n", mpeg4$define(inner, type, string)(type_string, box_type), box_size);
		if (((layer = mpeg4_atom_layer_find(atom, box_type.v)) || (layer = unknow)) && layer->interface.dump)
		{
			ud.dump_level = level + mlog_level_width;
			if (!layer->interface.dump(layer, mlog, data + box_header_size, box_size - box_header_size, &ud))
				break;
		}
		data += box_size;
		size -= box_size;
	}
	if (!size) return atom;
	return NULL;
}

static mpeg4$define$create(container)
{
	return mpeg4_stuff_alloc(atom, inst, type, sizeof(mpeg4_stuff_t), NULL, NULL);
}

mpeg4$define$parse(container)
{
	const mpeg4_atom_s *restrict atom;
	const mpeg4_atom_s *restrict layer;
	mpeg4_stuff_t *restrict value;
	uint64_t box_header_size;
	mpeg4_box_extend_size_t box_size;
	mpeg4_box_type_t box_type;
	atom = stuff->atom;
	value = NULL;
	while (size && (box_header_size = mpeg4_box_border_parse(data, size, &box_size, &box_type)))
	{
		if ((layer = mpeg4_atom_layer_find(atom, box_type.v)) && layer->interface.parse)
		{
			if (!(value = mpeg4_stuff_container_append(stuff, box_type)))
				goto label_fail;
			if (!layer->interface.parse(value, data + box_header_size, box_size - box_header_size))
				goto label_fail;
			value = NULL;
		}
		data += box_size;
		size -= box_size;
	}
	if (!size) return stuff;
	label_fail:
	if (value) mpeg4_stuff_unlink(value);
	return NULL;
}

mpeg4$define$calc(container)
{
	mpeg4_stuff_t *restrict value;
	mpeg4_calc_f calc;
	uint64_t size, n;
	size = n = 0;
	for (value = stuff->container.list; value; value = value->link.next)
	{
		if (value->info.calc_okay)
		{
			label_calc_okay:
			size += value->info.all_size;
			++n;
			continue;
		}
		if ((calc = value->atom->interface.calc) && calc(value) && value->info.calc_okay)
			goto label_calc_okay;
		return NULL;
	}
	stuff->info.link_number = n;
	mpeg4_stuff_calc_okay(stuff, size);
	return stuff;
}

mpeg4$define$build(container)
{
	mpeg4_stuff_t *restrict value;
	mpeg4_build_f build;
	uint64_t size;
	if (!stuff->info.calc_okay)
		goto label_fail;
	size = 0;
	for (value = stuff->container.list; value; value = value->link.next)
	{
		if (!value->info.calc_okay)
			goto label_fail;
		size += mpeg4_box_border_build(data + size, value->info.all_size, value->info.type);
		if (!(build = value->atom->interface.build))
			goto label_fail;
		if (!build(value, data + size))
			goto label_fail;
		size += value->info.inner_size;
	}
	return stuff;
	label_fail:
	return NULL;
}

mpeg4$define$alloc(container)
{
	mpeg4_atom$container_t *restrict r;
	r = (mpeg4_atom$container_t *) refer_alloz(sizeof(mpeg4_atom$container_t));
	if (r)
	{
		refer_set_free(r, (refer_free_f) mpeg4_atom_free_default_func);
		r->atom.interface.dump = mpeg4$define(atom, container, dump);
		r->atom.interface.create = mpeg4$define(atom, container, create);
		r->atom.interface.parse = mpeg4$define(atom, container, parse);
		r->atom.interface.calc = mpeg4$define(atom, container, calc);
		r->atom.interface.build = mpeg4$define(atom, container, build);
		r->inst = inst;
		if (
			mpeg4$inner$type$alloc$layer2(&r->atom, inst, free_skip, "free") &&
			mpeg4$inner$type$alloc$layer2(&r->atom, inst, free_skip, "skip")
		) return &r->atom;
		refer_free(r);
	}
	return NULL;
}
