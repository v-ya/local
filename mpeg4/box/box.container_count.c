#include "box.include.h"
#include "inner.fullbox.h"
#include "inner.data.h"

typedef struct mpeg4_stuff__container_count_s {
	mpeg4_stuff_t stuff;
	inner_fullbox_t fullbox;
} mpeg4_stuff__container_count_s;

mpeg4$define$dump(container);
static mpeg4$define$dump(container_count)
{
	inner_fullbox_t fullbox;
	uint32_t entry_count;
	uint32_t level = unidata->dump_level;
	if (!mpeg4$define(inner, fullbox, get)(&fullbox, &data, &size))
		goto label_fail;
	mpeg4$define(inner, fullbox, dump)(mlog, &fullbox, NULL, level);
	if (!mpeg4$define(inner, uint32_t, get)(&entry_count, &data, &size))
		goto label_fail;
	mlog_level_dump("entry count: %u\n", entry_count);
	return mpeg4$define(atom, container, dump)(atom, mlog, data, size, unidata);
	label_fail:
	return NULL;
}

static mpeg4$define$create(container_count)
{
	return mpeg4_stuff_alloc(atom, inst, type, sizeof(mpeg4_stuff__container_count_s), NULL, NULL);
}

mpeg4$define$parse(container);
static mpeg4$define$parse(container_count)
{
	inner_fullbox_t fullbox;
	uint32_t entry_count;
	if (!mpeg4$define(inner, fullbox, get)(&fullbox, &data, &size))
		goto label_fail;
	if (!mpeg4$stuff$method$call(stuff, set$version_and_flags, fullbox.version, fullbox.flags))
		goto label_fail;
	if (!mpeg4$define(inner, uint32_t, get)(&entry_count, &data, &size))
		goto label_fail;
	return mpeg4$define(atom, container, parse)(stuff, data, size);
	label_fail:
	return NULL;
}

mpeg4$define$calc(container);
static mpeg4$define$calc(container_count)
{
	if (mpeg4$define(atom, container, calc)(stuff))
		return mpeg4_stuff_calc_okay(stuff, stuff->info.inner_size + sizeof(mpeg4_full_box_suffix_t) + sizeof(uint32_t));
	return NULL;
}

mpeg4$define$build(container);
static mpeg4$define$build(container_count)
{
	data = mpeg4$define(inner, fullbox, set)(data, &((mpeg4_stuff__container_count_s *) stuff)->fullbox);
	data = mpeg4$define(inner, uint32_t, set)(data, stuff->info.link_number);
	return mpeg4$define(atom, container, build)(stuff, data);
}

static inner_method_set_fullbox(container_count, mpeg4_stuff__container_count_s, fullbox, ~0u);
static inner_method_get_fullbox(container_count, mpeg4_stuff__container_count_s, fullbox);

mpeg4$define$alloc(container_count)
{
	mpeg4_atom_s *restrict r;
	r = mpeg4$define(atom, container, alloc)(inst);
	if (r)
	{
		r->interface.dump = mpeg4$define(atom, container_count, dump);
		r->interface.create = mpeg4$define(atom, container_count, create);
		r->interface.parse = mpeg4$define(atom, container_count, parse);
		r->interface.calc = mpeg4$define(atom, container_count, calc);
		r->interface.build = mpeg4$define(atom, container_count, build);
		if (
			mpeg4$stuff$method$set(r, container_count, set$version_and_flags) &&
			mpeg4$stuff$method$set(r, container_count, get$version_and_flags)
		) return r;
	}
	return r;
}
