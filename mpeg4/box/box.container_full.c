#include "box.container.h"
#include "inner.fullbox.h"

typedef struct mpeg4_stuff__container_full_s {
	mpeg4_stuff_t stuff;
	inner_fullbox_t fullbox;
} mpeg4_stuff__container_full_s;

static mpeg4$define$dump(container_full)
{
	inner_fullbox_t fullbox;
	uint32_t level = unidata->dump_level;
	if (!mpeg4$define(inner, fullbox, get)(&fullbox, &data, &size))
		goto label_fail;
	mpeg4$define(inner, fullbox, dump)(mlog, &fullbox, NULL, level);
	return mpeg4$define(atom, container, dump)(atom, mlog, data, size, unidata);
	label_fail:
	return NULL;
}

static mpeg4$define$create(container_full)
{
	return mpeg4_stuff_alloc(atom, inst, type, sizeof(mpeg4_stuff__container_full_s), NULL, NULL);
}

static mpeg4$define$parse(container_full)
{
	inner_fullbox_t fullbox;
	if (!mpeg4$define(inner, fullbox, get)(&fullbox, &data, &size))
		goto label_fail;
	if (!mpeg4$stuff$method$call(stuff, set$version_and_flags, fullbox.version, fullbox.flags))
		goto label_fail;
	return mpeg4$define(atom, container, parse)(stuff, data, size);
	label_fail:
	return NULL;
}

static mpeg4$define$calc(container_full)
{
	if (mpeg4$define(atom, container, calc)(stuff))
		return mpeg4_stuff_calc_okay(stuff, stuff->info.inner_size + sizeof(mpeg4_full_box_suffix_t));
	return NULL;
}

static mpeg4$define$build(container_full)
{
	data = mpeg4$define(inner, fullbox, set)(data, &((mpeg4_stuff__container_full_s *) stuff)->fullbox);
	return mpeg4$define(atom, container, build)(stuff, data);
}

static inner_method_set_fullbox(container_full, mpeg4_stuff__container_full_s, fullbox, ~0u);
static inner_method_get_fullbox(container_full, mpeg4_stuff__container_full_s, fullbox);

mpeg4$define$alloc(container_full)
{
	mpeg4_atom$container_s *restrict r;
	r = (mpeg4_atom$container_s *) refer_alloz(sizeof(mpeg4_atom$container_s));
	if (r)
	{
		refer_set_free(r, (refer_free_f) mpeg4_atom_free_default_func);
		r->atom.interface.dump = mpeg4$define(atom, container_full, dump);
		r->atom.interface.create = mpeg4$define(atom, container_full, create);
		r->atom.interface.parse = mpeg4$define(atom, container_full, parse);
		r->atom.interface.calc = mpeg4$define(atom, container_full, calc);
		r->atom.interface.build = mpeg4$define(atom, container_full, build);
		r->inst = inst;
		if (
			mpeg4$stuff$method$set(&r->atom, container_full, set$version_and_flags) &&
			mpeg4$stuff$method$set(&r->atom, container_full, get$version_and_flags)
		) return &r->atom;
		refer_free(r);
	}
	return NULL;
}
