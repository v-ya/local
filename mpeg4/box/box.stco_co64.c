#include "box.include.h"
#include "inner.fullbox.h"
#include "inner.data.h"
#include <stdlib.h>

typedef struct mpeg4_atom$stco_s {
	mpeg4_atom_s atom;
	const mpeg4_atom_s *co64;
} mpeg4_atom$stco_s;

typedef struct mpeg4_stuff__chunk_offset_mdat_t {
	mpeg4_stuff_t *mdat;
	uintptr_t used;
	uint64_t offset;
} mpeg4_stuff__chunk_offset_mdat_t;

typedef struct mpeg4_stuff__chunk_offset_item_t {
	mpeg4_stuff__chunk_offset_mdat_t *mdat_link;
	uint64_t mdat_offset;
	uint64_t file_offset;
} mpeg4_stuff__chunk_offset_item_t;

typedef struct mpeg4_stuff__chunk_offset_t {
	inner_fullbox_t fullbox;
	inner_array_t chunk_offset;
	rbtree_t *mdat_pool;
	uint32_t need_64;
	uint32_t must_keep;
	uintptr_t calc_loop;
	const void *parse_data;
	uint64_t parse_size;
} mpeg4_stuff__chunk_offset_t;

typedef struct mpeg4_stuff__chunk_offset_s {
	mpeg4_stuff_t stuff;
	mpeg4_stuff__chunk_offset_t pri;
} mpeg4_stuff__chunk_offset_s;

static void mpeg4$define(inner, chunk_offset, link_mdat_free_func)(rbtree_t *restrict r)
{
	mpeg4_stuff__chunk_offset_mdat_t *restrict m;
	if ((m = (mpeg4_stuff__chunk_offset_mdat_t *) r->value))
	{
		if (m->mdat)
			refer_free(m->mdat);
		free(m);
	}
}

static mpeg4_stuff__chunk_offset_mdat_t* mpeg4$define(inner, chunk_offset, link_mdat)(mpeg4_stuff__chunk_offset_t *restrict r, mpeg4_stuff_t *mdat)
{
	rbtree_t *restrict v;
	mpeg4_stuff__chunk_offset_mdat_t *restrict value;
	if ((v = rbtree_find(&r->mdat_pool, NULL, (uint64_t) (uintptr_t) mdat)))
	{
		label_ok:
		return (mpeg4_stuff__chunk_offset_mdat_t *) v->value;
	}
	if ((value = (mpeg4_stuff__chunk_offset_mdat_t *) malloc(sizeof(mpeg4_stuff__chunk_offset_mdat_t))))
	{
		value->mdat = (mpeg4_stuff_t *) refer_save(mdat);
		value->used = 0;
		value->offset = 0;
		if ((v = rbtree_insert(&r->mdat_pool, NULL, (uint64_t) (uintptr_t) mdat, value, mpeg4$define(inner, chunk_offset, link_mdat_free_func))))
			goto label_ok;
		refer_free(value->mdat);
		free(value);
	}
	return NULL;
}

static void mpeg4$define(inner, chunk_offset, mdat_calc_func)(rbtree_t *restrict r, uintptr_t *restrict error_number)
{
	mpeg4_stuff__chunk_offset_mdat_t *restrict m;
	if ((m = (mpeg4_stuff__chunk_offset_mdat_t *) r->value))
	{
		if (mpeg4$stuff$method$call(m->mdat, calc$offset, &m->offset))
			return ;
	}
	*error_number += 1;
}

static mpeg4_stuff__chunk_offset_t* mpeg4$define(inner, chunk_offset, calc_mdat)(mpeg4_stuff__chunk_offset_t *restrict r)
{
	uintptr_t error_number = 0;
	rbtree_call(&r->mdat_pool, (rbtree_func_call_f) mpeg4$define(inner, chunk_offset, mdat_calc_func), &error_number);
	if (!error_number)
		return r;
	return NULL;
}

static mpeg4_stuff__chunk_offset_t* mpeg4$define(inner, chunk_offset, calc_item)(mpeg4_stuff__chunk_offset_t *restrict r)
{
	mpeg4_stuff__chunk_offset_item_t *restrict item;
	uintptr_t n;
	uint32_t need_64 = 0;
	item = (mpeg4_stuff__chunk_offset_item_t *) r->chunk_offset.array;
	n = r->chunk_offset.number;
	while (n)
	{
		if ((item->file_offset = item->mdat_offset + item->mdat_link->offset) > 0xfffffffful)
			need_64 = 1;
		++item;
		--n;
	}
	r->need_64 = need_64;
	return r;
}

static mpeg4_stuff__chunk_offset_t* mpeg4$define(inner, chunk_offset, calc)(mpeg4_stuff__chunk_offset_t *restrict r)
{
	if (mpeg4$define(inner, chunk_offset, calc_mdat)(r))
		return mpeg4$define(inner, chunk_offset, calc_item)(r);
	return NULL;
}

static mpeg4$define$dump(stco)
{
	inner_fullbox_t fullbox;
	const uint32_t *chunk_offset;
	uint32_t entry_count, i;
	uint32_t level = unidata->dump_level;
	if (!mpeg4$define(inner, fullbox, get)(&fullbox, &data, &size))
		goto label_fail;
	mpeg4$define(inner, fullbox, dump)(mlog, &fullbox, NULL, level);
	if (fullbox.version)
		goto label_fail;
	if (!mpeg4$define(inner, uint32_t, get)(&entry_count, &data, &size))
		goto label_fail;
	mlog_level_dump("entry count: %u\n", entry_count);
	chunk_offset = (const uint32_t *) data;
	level += mlog_level_width;
	for (i = 0; i < entry_count && size >= sizeof(uint32_t); ++i)
	{
		if (unidata->dump_samples)
			mlog_level_dump("(%6u) = %u\n", i + 1, mpeg4_n32(*chunk_offset));
		++chunk_offset;
		size -= sizeof(uint32_t);
	}
	if (size || i < entry_count)
		goto label_fail;
	return atom;
	label_fail:
	return NULL;
}

static mpeg4$define$dump(co64)
{
	inner_fullbox_t fullbox;
	const uint64_t *chunk_offset;
	uint32_t entry_count, i;
	uint32_t level = unidata->dump_level;
	if (!mpeg4$define(inner, fullbox, get)(&fullbox, &data, &size))
		goto label_fail;
	mpeg4$define(inner, fullbox, dump)(mlog, &fullbox, NULL, level);
	if (fullbox.version)
		goto label_fail;
	if (!mpeg4$define(inner, uint32_t, get)(&entry_count, &data, &size))
		goto label_fail;
	mlog_level_dump("entry count: %u\n", entry_count);
	chunk_offset = (const uint64_t *) data;
	level += mlog_level_width;
	for (i = 0; i < entry_count && size >= sizeof(uint64_t); ++i)
	{
		if (unidata->dump_samples)
			mlog_level_dump("(%6u) = %lu\n", i + 1, mpeg4_n64(*chunk_offset));
		++chunk_offset;
		size -= sizeof(uint64_t);
	}
	if (size || i < entry_count)
		goto label_fail;
	return atom;
	label_fail:
	return NULL;
}

static mpeg4$define$stuff$init(stco, mpeg4_stuff__chunk_offset_s)
{
	mpeg4$define(inner, array, init)(&r->pri.chunk_offset, sizeof(mpeg4_stuff__chunk_offset_item_t));
	return &r->stuff;
}

static mpeg4$define$stuff$free(stco, mpeg4_stuff__chunk_offset_s)
{
	mpeg4$define(inner, array, clear)(&r->pri.chunk_offset);
	if (r->pri.mdat_pool)
		rbtree_clear(&r->pri.mdat_pool);
	mpeg4_stuff_free_default_func(&r->stuff);
}

static mpeg4$define$create(stco)
{
	return mpeg4_stuff_alloc(atom, inst, type,
		sizeof(mpeg4_stuff__chunk_offset_s),
		(mpeg4_stuff_init_f) mpeg4$define(stuff, stco, init),
		(refer_free_f) mpeg4$define(stuff, stco, free));
}

static mpeg4$define$parse(stco)
{
	mpeg4_stuff_t *restrict root;
	inner_fullbox_t fullbox;
	uint32_t entry_count;
	if (!mpeg4$define(inner, fullbox, get)(&fullbox, &data, &size))
		goto label_fail;
	if (!mpeg4$stuff$method$call(stuff, set$version_and_flags, fullbox.version, fullbox.flags))
		goto label_fail;
	if (!mpeg4$define(inner, uint32_t, get)(&entry_count, &data, &size))
		goto label_fail;
	if (stuff->info.type.v == ((mpeg4_box_type_t) {.c = "stco"}).v)
	{
		if ((uint64_t) entry_count * sizeof(uint32_t) != size)
			goto label_fail;
	}
	else if (stuff->info.type.v == ((mpeg4_box_type_t) {.c = "co64"}).v)
	{
		if ((uint64_t) entry_count * sizeof(uint64_t) != size)
			goto label_fail;
	}
	else goto label_fail;
	if (!mpeg4$stuff$method$call(stuff, inner$set_parse, data, size))
		goto label_fail;
	if (!(root = stuff->link.container))
		goto label_fail;
	while (root->link.container)
		root = root->link.container;
	if (!mpeg4$stuff$method$call(root, inner$push_stco, stuff))
		goto label_fail;
	return stuff;
	label_fail:
	return NULL;
}

static mpeg4$define$calc(stco)
{
	mpeg4_stuff__chunk_offset_t *restrict r = &((mpeg4_stuff__chunk_offset_s *) stuff)->pri;
	if (r->calc_loop++)
		goto label_return;
	if (!mpeg4$define(inner, chunk_offset, calc)(r))
		goto label_fail;
	if (!r->need_64)
	{
		label_return:
		--r->calc_loop;
		return mpeg4_stuff_calc_okay(stuff,
			sizeof(mpeg4_full_box_suffix_t) +
			sizeof(uint32_t) +
			sizeof(uint32_t) * r->chunk_offset.number);
	}
	if (!r->must_keep)
	{
		if (!mpeg4_stuff_replace(stuff, ((mpeg4_atom$stco_s *) stuff->atom)->co64, (mpeg4_box_type_t) {.c = "co64"}))
			goto label_fail;
		// re-calc
		if (!stuff->atom->interface.calc)
			goto label_fail;
		--r->calc_loop;
		return stuff->atom->interface.calc(stuff);
	}
	label_fail:
	--r->calc_loop;
	return NULL;
}

static mpeg4$define$calc(co64)
{
	mpeg4_stuff__chunk_offset_t *restrict r = &((mpeg4_stuff__chunk_offset_s *) stuff)->pri;
	if (r->calc_loop++)
		goto label_return;
	if (!mpeg4$define(inner, chunk_offset, calc)(r))
		goto label_fail;
	label_return:
	--r->calc_loop;
	return mpeg4_stuff_calc_okay(stuff,
		sizeof(mpeg4_full_box_suffix_t) +
		sizeof(uint32_t) +
		sizeof(uint64_t) * r->chunk_offset.number);
	label_fail:
	--r->calc_loop;
	return NULL;
}

static mpeg4$define$build(stco)
{
	mpeg4_stuff__chunk_offset_t *restrict r = &((mpeg4_stuff__chunk_offset_s *) stuff)->pri;
	const mpeg4_stuff__chunk_offset_item_t *restrict item;
	uint32_t *restrict offset;
	uintptr_t n;
	data = mpeg4$define(inner, fullbox, set)(data, &r->fullbox);
	offset = (uint32_t *) mpeg4$define(inner, uint32_t, set)(data, (uint32_t) (n = r->chunk_offset.number));
	item = (const mpeg4_stuff__chunk_offset_item_t *) r->chunk_offset.array;
	while (n)
	{
		*offset++ = mpeg4_n32((uint32_t) (item++)->file_offset);
		--n;
	}
	return stuff;
}

static mpeg4$define$build(co64)
{
	mpeg4_stuff__chunk_offset_t *restrict r = &((mpeg4_stuff__chunk_offset_s *) stuff)->pri;
	const mpeg4_stuff__chunk_offset_item_t *restrict item;
	uint64_t *restrict offset;
	uintptr_t n;
	data = mpeg4$define(inner, fullbox, set)(data, &r->fullbox);
	offset = (uint64_t *) mpeg4$define(inner, uint32_t, set)(data, (uint32_t) (n = r->chunk_offset.number));
	item = (const mpeg4_stuff__chunk_offset_item_t *) r->chunk_offset.array;
	while (n)
	{
		*offset++ = mpeg4_n64((item++)->file_offset);
		--n;
	}
	return stuff;
}

static inner_method_set_fullbox(stco, mpeg4_stuff__chunk_offset_s, pri.fullbox, 0);
static inner_method_get_fullbox(stco, mpeg4_stuff__chunk_offset_s, pri.fullbox);

static const mpeg4_stuff_t* mpeg4$define(stuff, stco, add$chunk_offset)(mpeg4_stuff__chunk_offset_s *restrict r, mpeg4_stuff_t *restrict mdat, uint64_t offset, uint32_t *restrict chunk_id)
{
	if (chunk_id) *chunk_id = (uint32_t) (r->pri.chunk_offset.number + 1);
	if (mdat)
	{
		mpeg4_stuff__chunk_offset_item_t *restrict item;
		mpeg4_stuff__chunk_offset_mdat_t *restrict m;
		if (!(item = (mpeg4_stuff__chunk_offset_item_t *) mpeg4$define(inner, array, append_point)(&r->pri.chunk_offset, 1)))
			goto label_fail;
		if (!(m = mpeg4$define(inner, chunk_offset, link_mdat)(&r->pri, mdat)))
			goto label_fail_reback;
		item->mdat_link = m;
		item->mdat_offset = offset;
	}
	return &r->stuff;
	label_fail_reback:
	r->pri.chunk_offset.number -= 1;
	label_fail:
	return NULL;
}

static const mpeg4_stuff_t* mpeg4$define(stuff, stco, inner$set_parse)(mpeg4_stuff__chunk_offset_s *restrict r, const void *data, uint64_t size)
{
	r->pri.parse_data = data;
	r->pri.parse_size = size;
	return &r->stuff;
}

static inline uintptr_t mpeg4$define(inner, do_parse_stco, find_mdat)(register const uint64_t *restrict offset, uintptr_t n, uint64_t pos)
{
	uintptr_t i, r;
	r = i = 0;
	while (i < n)
	{
		if (pos < offset[i])
			break;
		r = i++;
	}
	return r;
}

static const mpeg4_stuff_t* mpeg4$define(stuff, stco, inner$do_parse_stco)(mpeg4_stuff__chunk_offset_s *restrict r, mpeg4_stuff_t *restrict const *restrict mdat, const uint64_t *restrict offset, uintptr_t n)
{
	const uint32_t *restrict data;
	uint64_t pos;
	uintptr_t i, number;
	data = (const uint32_t *) r->pri.parse_data;
	number = r->pri.parse_size / sizeof(uint32_t);
	r->pri.parse_data = NULL;
	r->pri.parse_size = 0;
	if (n)
	{
		while (number)
		{
			i = mpeg4$define(inner, do_parse_stco, find_mdat)(offset, n, pos = (uint64_t) mpeg4_n32(*data));
			if (!mpeg4$define(stuff, stco, add$chunk_offset)(r, mdat[i], pos - offset[i], NULL))
				goto label_fail;
			++data;
			--number;
		}
	}
	return &r->stuff;
	label_fail:
	return NULL;
}

static const mpeg4_stuff_t* mpeg4$define(stuff, co64, inner$do_parse_stco)(mpeg4_stuff__chunk_offset_s *restrict r, mpeg4_stuff_t *restrict const *restrict mdat, const uint64_t *restrict offset, uintptr_t n)
{
	const uint64_t *restrict data;
	uint64_t pos;
	uintptr_t i, number;
	data = (const uint64_t *) r->pri.parse_data;
	number = r->pri.parse_size / sizeof(uint64_t);
	r->pri.parse_data = NULL;
	r->pri.parse_size = 0;
	if (n)
	{
		while (number)
		{
			i = mpeg4$define(inner, do_parse_stco, find_mdat)(offset, n, pos = mpeg4_n64(*data));
			if (!mpeg4$define(stuff, stco, add$chunk_offset)(r, mdat[i], pos - offset[i], NULL))
				goto label_fail;
			++data;
			--number;
		}
	}
	return &r->stuff;
	label_fail:
	return NULL;
}

static const mpeg4$define$alloc(stco)
{
	mpeg4_atom$stco_s *restrict r;
	r = (mpeg4_atom$stco_s *) refer_alloz(sizeof(mpeg4_atom$stco_s));
	if (r)
	{
		refer_set_free(r, (refer_free_f) mpeg4_atom_free_default_func);
		if ((r->co64 = mpeg4$define(atom, co64, find)(inst)))
		{
			r->atom.interface.dump = mpeg4$define(atom, stco, dump);
			r->atom.interface.create = mpeg4$define(atom, stco, create);
			r->atom.interface.parse = mpeg4$define(atom, stco, parse);
			r->atom.interface.calc = mpeg4$define(atom, stco, calc);
			r->atom.interface.build = mpeg4$define(atom, stco, build);
			if (
				mpeg4$stuff$method$set(&r->atom, stco, set$version_and_flags) &&
				mpeg4$stuff$method$set(&r->atom, stco, get$version_and_flags) &&
				mpeg4$stuff$method$set(&r->atom, stco, add$chunk_offset) &&
				mpeg4$stuff$method$set(&r->atom, stco, inner$set_parse) &&
				mpeg4$stuff$method$set(&r->atom, stco, inner$do_parse_stco)
			) return &r->atom;
		}
		refer_free(r);
	}
	return NULL;
}

static const mpeg4$define$alloc(co64)
{
	mpeg4_atom_s *restrict r;
	r = mpeg4_atom_alloc_empty();
	if (r)
	{
		r->interface.dump = mpeg4$define(atom, co64, dump);
		r->interface.create = mpeg4$define(atom, stco, create);
		r->interface.parse = mpeg4$define(atom, stco, parse);
		r->interface.calc = mpeg4$define(atom, co64, calc);
		r->interface.build = mpeg4$define(atom, co64, build);
		if (
			mpeg4$stuff$method$set(r, stco, set$version_and_flags) &&
			mpeg4$stuff$method$set(r, stco, get$version_and_flags) &&
			mpeg4$stuff$method$set(r, stco, add$chunk_offset) &&
			mpeg4$stuff$method$set(r, stco, inner$set_parse) &&
			mpeg4$stuff$method$set(r, co64, inner$do_parse_stco)
		) return r;
		refer_free(r);
	}
	return NULL;
}

mpeg4$define$find(stco)
{
	static const mpeg4_box_type_t type = { .c = "stco" };
	return mpeg4_find_atom(inst, mpeg4$define(atom, stco, alloc), type.v, 0);
}

mpeg4$define$find(co64)
{
	static const mpeg4_box_type_t type = { .c = "co64" };
	return mpeg4_find_atom(inst, mpeg4$define(atom, co64, alloc), type.v, 0);
}
