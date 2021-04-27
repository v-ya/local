#include "box.include.h"
#include "inner.fullbox.h"
#include "inner.data.h"

static mpeg4$define$dump(stco)
{
	inner_fullbox_t fullbox;
	const uint32_t *chunk_offset;
	uint32_t entry_count;
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
	while (entry_count && size >= sizeof(uint32_t))
	{
		if (unidata->dump_samples)
			mlog_level_dump("chunk_offset = %u\n",
				mpeg4_n32(*chunk_offset));
		++chunk_offset;
		size -= sizeof(uint32_t);
		--entry_count;
	}
	if (size || entry_count)
		goto label_fail;
	return atom;
	label_fail:
	return NULL;
}

static mpeg4$define$dump(co64)
{
	inner_fullbox_t fullbox;
	const uint64_t *chunk_offset;
	uint32_t entry_count;
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
	while (entry_count && size >= sizeof(uint64_t))
	{
		if (unidata->dump_samples)
			mlog_level_dump("chunk_offset = %lu\n",
				mpeg4_n64(*chunk_offset));
		++chunk_offset;
		size -= sizeof(uint64_t);
		--entry_count;
	}
	if (size || entry_count)
		goto label_fail;
	return atom;
	label_fail:
	return NULL;
}

static const mpeg4$define$alloc(stco)
{
	mpeg4_atom_s *restrict r;
	r = mpeg4_atom_alloc_empty();
	if (r)
	{
		r->interface.dump = mpeg4$define(atom, stco, dump);
	}
	return r;
}

static const mpeg4$define$alloc(co64)
{
	mpeg4_atom_s *restrict r;
	r = mpeg4_atom_alloc_empty();
	if (r)
	{
		r->interface.dump = mpeg4$define(atom, co64, dump);
	}
	return r;
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
