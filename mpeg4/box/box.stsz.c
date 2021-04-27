#include "box.include.h"
#include "inner.fullbox.h"
#include "inner.data.h"

typedef struct sample_size_t {
	uint32_t sample_size;
	uint32_t sample_count;
} __attribute__ ((packed)) sample_size_t;

static mpeg4$define$dump(stsz)
{
	inner_fullbox_t fullbox;
	const uint32_t *entry_size;
	uint64_t all_sample_size;
	sample_size_t sample;
	uint32_t sample_size, i;
	uint32_t level = unidata->dump_level;
	if (!mpeg4$define(inner, fullbox, get)(&fullbox, &data, &size))
		goto label_fail;
	mpeg4$define(inner, fullbox, dump)(mlog, &fullbox, NULL, level);
	if (fullbox.version)
		goto label_fail;
	if (!mpeg4$define(inner, data, get)(&sample, sizeof(sample), &data, &size))
		goto label_fail;
	sample.sample_size = mpeg4_n32(sample.sample_size);
	sample.sample_count = mpeg4_n32(sample.sample_count);
	mlog_level_dump("sample_size:  %u\n", sample.sample_size);
	mlog_level_dump("sample_count: %u\n", sample.sample_count);
	entry_size = (const uint32_t *) data;
	level += mlog_level_width;
	all_sample_size = 0;
	if (!sample.sample_size)
	{
		for (i = 0; i < sample.sample_count && size >= sizeof(uint32_t); ++i)
		{
			all_sample_size += (sample_size = mpeg4_n32(*entry_size));
			if (unidata->dump_samples)
				mlog_level_dump("[%6u] = %u\n", i, sample_size);
			++entry_size;
			size -= sizeof(uint32_t);
		}
		if (i < sample.sample_count)
			goto label_fail;
	}
	else all_sample_size = sample.sample_size * sample.sample_count;
	if (size) goto label_fail;
	mlog_level_dump("all sample_size: %lu\n", all_sample_size);
	return atom;
	label_fail:
	return NULL;
}

static const mpeg4$define$alloc(stsz)
{
	mpeg4_atom_s *restrict r;
	r = mpeg4_atom_alloc_empty();
	if (r)
	{
		r->interface.dump = mpeg4$define(atom, stsz, dump);
	}
	return r;
}

mpeg4$define$find(stsz)
{
	static const mpeg4_box_type_t type = { .c = "stsz" };
	return mpeg4_find_atom(inst, mpeg4$define(atom, stsz, alloc), type.v, 0);
}
