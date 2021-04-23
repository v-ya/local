#include "box.include.h"
#include "inner.type.h"

typedef struct mpeg4_atom$container_t {
	mpeg4_atom_t atom;
	const mpeg4_t *inst;
} mpeg4_atom$container_t;

mpeg4$define$dump(container)
{
	mpeg4_atom_dump_t ud;
	const mpeg4_atom_t *restrict layer;
	const mpeg4_atom_t *restrict unknow;
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

mpeg4$define$alloc(container)
{
	mpeg4_atom$container_t *restrict r;
	r = (mpeg4_atom$container_t *) refer_alloz(sizeof(mpeg4_atom$container_t));
	if (r)
	{
		refer_set_free(r, (refer_free_f) mpeg4_atom_free_default_func);
		r->atom.interface.dump = mpeg4$define(atom, container, dump);
		r->inst = inst;
		if (
			mpeg4$inner$type$alloc$layer2(&r->atom, inst, free_skip, "free") &&
			mpeg4$inner$type$alloc$layer2(&r->atom, inst, free_skip, "skip")
		) return &r->atom;
		refer_free(r);
	}
	return NULL;
}
