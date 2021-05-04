#include "box.include.h"
#include "inner.data.h"

typedef struct mpeg4_stuff__sgpd_roll_roll_s {
	mpeg4_stuff_t stuff;
	int16_t roll_distance;
} mpeg4_stuff__sgpd_roll_roll_s;

static mpeg4$define$dump(sgpd_roll)
{
	uint32_t level = unidata->dump_level;
	int16_t roll_distance;
	if (!mpeg4$define(inner, int16_t, get)(&roll_distance, &data, &size))
		goto label_fail;
	mlog_level_dump("roll distance: %d\n", roll_distance);
	unidata->temp_size = sizeof(int16_t);
	return atom;
	label_fail:
	return NULL;
}

static mpeg4$define$create(sgpd_roll)
{
	return mpeg4_stuff_alloc(atom, inst, type, sizeof(mpeg4_stuff__sgpd_roll_roll_s), NULL, NULL);
}

static mpeg4$define$parse(sgpd_roll)
{
	int16_t roll_distance;
	if (!mpeg4$define(inner, int16_t, get)(&roll_distance, &data, &size))
		goto label_fail;
	if (!mpeg4$stuff$method$call(stuff, set$roll_distance, roll_distance))
		goto label_fail;
	return stuff;
	label_fail:
	return NULL;
}

static mpeg4$define$calc(sgpd_roll)
{
	return mpeg4_stuff_calc_okay(stuff, sizeof(int16_t));
}

static mpeg4$define$build(sgpd_roll)
{
	mpeg4$define(inner, int16_t, set)(data, ((mpeg4_stuff__sgpd_roll_roll_s *) stuff)->roll_distance);
	return stuff;
}

static const mpeg4_stuff_t* mpeg4$define(stuff, sgpd_roll, set$roll_distance)(mpeg4_stuff__sgpd_roll_roll_s *restrict r, int16_t roll_distance)
{
	r->roll_distance = roll_distance;
	return &r->stuff;
}

static const mpeg4$define$alloc(sgpd_roll)
{
	mpeg4_atom_s *restrict r;
	r = mpeg4_atom_alloc_empty();
	if (r)
	{
		r->interface.dump = mpeg4$define(atom, sgpd_roll, dump);
		r->interface.create = mpeg4$define(atom, sgpd_roll, create);
		r->interface.parse = mpeg4$define(atom, sgpd_roll, parse);
		r->interface.calc = mpeg4$define(atom, sgpd_roll, calc);
		r->interface.build = mpeg4$define(atom, sgpd_roll, build);
		if (mpeg4$stuff$method$set(r, sgpd_roll, set$roll_distance))
			return r;
		refer_free(r);
	}
	return NULL;
}

mpeg4$define$find(sgpd_roll)
{
	static const mpeg4_box_type_t type = { .c = "roll" };
	static const mpeg4_box_type_t extra = { .c = "sgpd" };
	return mpeg4_find_atom(inst, mpeg4$define(atom, sgpd_roll, alloc), type.v, extra.v);
}
