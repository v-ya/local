#include "box.include.h"
#include "inner.fullbox.h"
#include "inner.data.h"

typedef struct sound_media_header_uni_t {
	int16_t balance;    // fix8.8
	uint16_t reserved;
} __attribute__ ((packed)) sound_media_header_uni_t;

typedef struct mpeg4_stuff__sound_media_header_t {
	inner_fullbox_t fullbox;
	double balance;
} mpeg4_stuff__sound_media_header_t;

typedef struct mpeg4_stuff__sound_media_header_s {
	mpeg4_stuff_t stuff;
	mpeg4_stuff__sound_media_header_t pri;
} mpeg4_stuff__sound_media_header_s;

static mpeg4$define$dump(smhd)
{
	inner_fullbox_t fullbox;
	sound_media_header_uni_t header;
	uint32_t level = unidata->dump_level;
	if (!mpeg4$define(inner, fullbox, get)(&fullbox, &data, &size))
		goto label_fail;
	mpeg4$define(inner, fullbox, dump)(mlog, &fullbox, NULL, level);
	if (fullbox.version)
		goto label_fail;
	if (!mpeg4$define(inner, data, get)(&header, sizeof(header), &data, &size))
		goto label_fail;
	mlog_level_dump("balance: %g\n", mpeg4_fix_point(mpeg4_n16(header.balance), 8, 8));
	if (!size) return atom;
	label_fail:
	return NULL;
}

static mpeg4$define$create(smhd)
{
	return mpeg4_stuff_alloc(atom, inst, type,
		sizeof(mpeg4_stuff__sound_media_header_s),
		NULL,
		NULL);
}

static mpeg4$define$parse(smhd)
{
	inner_fullbox_t fullbox;
	sound_media_header_uni_t header;
	if (!mpeg4$define(inner, fullbox, get)(&fullbox, &data, &size))
		goto label_fail;
	if (!mpeg4$stuff$method$call(stuff, set$version_and_flags, fullbox.version, fullbox.flags))
		goto label_fail;
	if (!mpeg4$define(inner, data, get)(&header, sizeof(header), &data, &size))
		goto label_fail;
	if (!mpeg4$stuff$method$call(stuff, set$balance, mpeg4_fix_point(mpeg4_n16(header.balance), 8, 8)))
		goto label_fail;
	if (!size) return stuff;
	label_fail:
	return NULL;
}

static mpeg4$define$calc(smhd)
{
	return mpeg4_stuff_calc_okay(stuff, sizeof(mpeg4_full_box_suffix_t) + sizeof(sound_media_header_uni_t));
}

static mpeg4$define$build(smhd)
{
	mpeg4_stuff__sound_media_header_t *restrict r = &((mpeg4_stuff__sound_media_header_s *) stuff)->pri;
	sound_media_header_uni_t header = {
		.reserved = 0
	};
	header.balance = mpeg4_fix_point_gen(r->balance, int16_t, 8, 8);
	data = mpeg4$define(inner, fullbox, set)(data, &r->fullbox);
	mpeg4$define(inner, data, set)(data, &header, sizeof(header));
	return stuff;
}

static const mpeg4_stuff_t* mpeg4$define(stuff, smhd, set$version_and_flags)(mpeg4_stuff__sound_media_header_s *restrict r, uint32_t version, uint32_t flags)
{
	if (version) goto label_fail;
	r->pri.fullbox.version = version;
	r->pri.fullbox.flags = flags;
	return &r->stuff;
	label_fail:
	return NULL;
}

static const mpeg4_stuff_t* mpeg4$define(stuff, smhd, set$balance)(mpeg4_stuff__sound_media_header_s *restrict r, double balance)
{
	r->pri.balance = balance;
	return &r->stuff;
}

static const mpeg4$define$alloc(smhd)
{
	mpeg4_atom_s *restrict r;
	r = mpeg4_atom_alloc_empty();
	if (r)
	{
		r->interface.dump = mpeg4$define(atom, smhd, dump);
		r->interface.create = mpeg4$define(atom, smhd, create);
		r->interface.parse = mpeg4$define(atom, smhd, parse);
		r->interface.calc = mpeg4$define(atom, smhd, calc);
		r->interface.build = mpeg4$define(atom, smhd, build);
		if (
			mpeg4$stuff$method$set(r, smhd, set$version_and_flags) &&
			mpeg4$stuff$method$set(r, smhd, set$balance)
		) return r;
		refer_free(r);
	}
	return NULL;
}

mpeg4$define$find(smhd)
{
	static const mpeg4_box_type_t type = { .c = "smhd" };
	return mpeg4_find_atom(inst, mpeg4$define(atom, smhd, alloc), type.v, 0);
}
