#include "mpeg4.inst.h"
#include "mpeg4.atom.h"
#include "mpeg4.box.h"
#include "box/inner.type.h"
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

typedef struct mpeg4_dump_atom_verbose_tree_data_t {
	mlog_s *mlog;
	uint32_t level;
} mpeg4_dump_atom_verbose_tree_data_t;

static void mpeg4_dump_atom_verbose_tree__dump_info_method_func(rbtree_t *restrict rbt, mpeg4_dump_atom_verbose_tree_data_t *restrict data)
{
	static const char *s_method_name[mpeg4_stuff_method_max] = {
		#define d_mn(_op, _name)  [mpeg4_stuff_method$##_op##$##_name] = #_op "_" #_name
		d_mn(set, major_brand),
		d_mn(set, minor_version),
		d_mn(add, compatible_brands),
		d_mn(set, data),
		d_mn(set, version_and_flags),
		d_mn(set, creation_time),
		d_mn(set, modification_time),
		d_mn(set, timescale),
		d_mn(set, duration),
		d_mn(set, rate),
		d_mn(set, volume),
		d_mn(set, matrix),
		d_mn(set, next_track_id),
		d_mn(set, track_id),
		d_mn(set, track_layer),
		d_mn(set, alternate_group),
		d_mn(set, resolution),
		d_mn(set, language),
		d_mn(set, name),
		d_mn(set, location),
		d_mn(set, graphicsmode),
		d_mn(set, opcolor),
		d_mn(set, balance),
		d_mn(add, edit_list_item),
		#undef d_mn
	};
	register mlog_s *mlog;
	register uint32_t level;
	mlog = data->mlog;
	level = data->level;
	if (rbt->key_index < mpeg4_stuff_method_max)
		mlog_level_dump("@%s\n", s_method_name[rbt->key_index]);
	else mlog_level_dump("@[unknow %016lx]", rbt->key_index);
}

static void mpeg4_dump_atom_verbose_tree__dump_info(const mpeg4_atom_s *restrict atom, uint32_t type_id, mpeg4_dump_atom_verbose_tree_data_t *restrict data);
static void mpeg4_dump_atom_verbose_tree__dump_info_layer_func(rbtree_t *restrict rbt, mpeg4_dump_atom_verbose_tree_data_t *restrict data)
{
	mpeg4_dump_atom_verbose_tree__dump_info((const mpeg4_atom_s *) rbt->value, (uint32_t) rbt->key_index, data);
}

static void mpeg4_dump_atom_verbose_tree__dump_info(const mpeg4_atom_s *restrict atom, uint32_t type_id, mpeg4_dump_atom_verbose_tree_data_t *restrict data)
{
	register mlog_s *mlog;
	register uint32_t level;
	char type_string[16];
	mlog = data->mlog;
	level = data->level;
	mlog_level_dump("[%s] %c%c%c%c%c\n",
		mpeg4$define(inner, type, string)(type_string, (mpeg4_box_type_t) {.v = type_id}),
		atom->interface.dump?'d':'-',
		atom->interface.create?'a':'-',
		atom->interface.calc?'c':'-',
		atom->interface.build?'b':'-',
		atom->interface.parse?'p':'-');
	data->level += mlog_level_width;
	if (atom->method)
		rbtree_call(&atom->method, (rbtree_func_call_f) mpeg4_dump_atom_verbose_tree__dump_info_method_func, data);
	if (atom->layer)
		rbtree_call(&atom->layer, (rbtree_func_call_f) mpeg4_dump_atom_verbose_tree__dump_info_layer_func, data);
	data->level -= mlog_level_width;
}

void mpeg4_dump_atom_verbose_tree(const struct mpeg4_atom_s *restrict atom, mlog_s *restrict mlog, uint32_t level)
{
	mpeg4_dump_atom_verbose_tree_data_t data;
	data.mlog = mlog;
	data.level = level;
	mpeg4_dump_atom_verbose_tree__dump_info(atom, 0, &data);
}
