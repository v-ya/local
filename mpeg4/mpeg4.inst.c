#include "mpeg4.inst.h"
#include "mpeg4.atom.h"
#include "mpeg4.box.h"
#include "mpeg4.stuff.method.h"
#include "box/inner.type.h"
#include <stdlib.h>
#include <memory.h>
#include <alloca.h>

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

struct mpeg4_stuff_s* mpeg4_parse_stco_and_mdat(struct mpeg4_stuff_s *restrict root, const uint8_t *restrict data_start)
{
	mpeg4_stuff_t **mdat;
	mpeg4_stuff_t **stco;
	uint64_t *offset;
	mpeg4_stuff_t *restrict v;
	const void *mdat_start;
	uintptr_t n_mdat;
	uintptr_t n_stco;
	uintptr_t i;
	if (!mpeg4$stuff$method$call(root, inner$get_mdat, &mdat, &n_mdat))
		goto label_fail;
	if (!mpeg4$stuff$method$call(root, inner$get_stco, &stco, &n_stco))
		goto label_fail;
	offset = NULL;
	if (n_mdat)
	{
		offset = (uint64_t *) alloca(sizeof(uint64_t) * n_mdat);
		if (!offset) goto label_fail;
		for (i = 0; i < n_mdat; ++i)
		{
			v = mdat[i];
			if (!mpeg4$stuff$method$call(v, inner$do_parse_mdat, &mdat_start, NULL))
				goto label_fail;
			offset[i] = (uint64_t) ((uintptr_t) mdat_start - (uintptr_t) data_start);
		}
	}
	for (i = 0; i < n_stco; ++i)
	{
		v = stco[i];
		if (!mpeg4$stuff$method$call(v, inner$do_parse_stco, mdat, offset, n_mdat))
			goto label_fail;
	}
	mpeg4$stuff$method$call(root, inner$clear);
	return root;
	label_fail:
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
		d_mn(set, minor_brand),
		d_mn(set, minor_version),
		d_mn(add, compatible_brands),
		d_mn(set, version_and_flags),
		d_mn(get, version_and_flags),
		d_mn(set, data),
		d_mn(add, data),
		d_mn(calc, offset),
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
		d_mn(set, track_resolution),
		d_mn(set, language),
		d_mn(set, name),
		d_mn(set, location),
		d_mn(set, graphicsmode),
		d_mn(set, opcolor),
		d_mn(set, balance),
		d_mn(set, data_reference_index),
		d_mn(set, pixel_resolution),
		d_mn(set, ppi_resolution),
		d_mn(set, frame_per_sample),
		d_mn(set, depth),
		d_mn(set, audio_channel_count),
		d_mn(set, audio_sample_size),
		d_mn(set, bitrate),
		d_mn(set, pixel_aspect_ratio),
		d_mn(set, es_id),
		d_mn(set, decoder_config_profile),
		d_mn(set, avcC_indication),
		d_mn(set, avcC_extra),
		d_mn(add, avcC_sps),
		d_mn(add, avcC_pps),
		d_mn(add, avcC_extra_spse),
		d_mn(set, av1C_seq),
		d_mn(set, av1C_flag),
		d_mn(set, av1C_initial_presentation_delay),
		d_mn(add, dfLa_metadata),
		d_mn(add, edit_list_item),
		d_mn(add, chunk_offset),
		d_mn(set, sample_count),
		d_mn(add, sample_size),
		d_mn(add, sample_to_chunk),
		d_mn(add, time_to_sample),
		d_mn(add, composition_offset),
		d_mn(add, sync_sample),
		d_mn(add, sample_to_group),
		d_mn(set, default_sample_description_index),
		d_mn(set, roll_distance),
		d_mn(set, ilst_data_text),
		#undef d_mn
	};
	register mlog_s *mlog;
	register uint32_t level;
	mlog = data->mlog;
	level = data->level;
	if (rbt->key_index < mpeg4_stuff_method_max)
		mlog_level_dump("@%s\n", s_method_name[rbt->key_index]);
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
