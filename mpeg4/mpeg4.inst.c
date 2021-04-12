#include "mpeg4.inst.h"
#include "mpeg4.box.h"
#include <stdlib.h>
#include <memory.h>

static void mpeg4_inst_rbtree_free_func(rbtree_t *restrict r)
{
	if (r->value)
		free(r->value);
}

mpeg4_t* mpeg4_set_type(mpeg4_t *restrict inst, uint32_t type, const mpeg4_func_box_t *restrict func)
{
	rbtree_t *restrict r;
	if ((r = rbtree_insert(&inst->type, NULL, (uint64_t) type, NULL, mpeg4_inst_rbtree_free_func)))
	{
		if ((r->value = malloc(sizeof(mpeg4_func_box_t))))
		{
			memcpy(r->value, func, sizeof(mpeg4_func_box_t));
			return inst;
		}
		rbtree_delete_by_pointer(&inst->type, r);
	}
	return NULL;
}

static inline const mpeg4_func_box_t* mpeg4_type2func(const mpeg4_t *restrict inst, mpeg4_box_type_t type)
{
	register const rbtree_t *restrict v;
	if ((v = rbtree_find(&inst->type, NULL, (uint64_t) type.v)))
		return (mpeg4_func_box_t *) v->value;
	return NULL;
}

const mpeg4_t* mpeg4_dump_level(const mpeg4_t *restrict inst, mlog_s *restrict mlog, const uint8_t *restrict data, uint64_t size, mpeg4_dump_data_t *restrict unidata, uint32_t level)
{
	uint64_t box_header_size;
	const mpeg4_func_box_t *restrict func;
	const mpeg4_func_box_t *restrict unknow;
	mpeg4_box_extend_size_t box_size;
	mpeg4_box_type_t box_type;
	box_type.v = 0;
	unknow = mpeg4_type2func(inst, box_type);
	while (size && (box_header_size = mpeg4_box_border_parse(data, size, &box_size, &box_type)))
	{
		mlog_level_dump("[%c%c%c%c] %lu\n", box_type.c[0], box_type.c[1], box_type.c[2], box_type.c[3], box_size);
		if (((func = mpeg4_type2func(inst, box_type)) || (func = unknow)) && func->dump)
		{
			if (!func->dump(inst, mlog, data + box_header_size, box_size - box_header_size, unidata, level + 1))
				break;
		}
		data += box_size;
		size -= box_size;
	}
	if (!size) return inst;
	return NULL;
}
