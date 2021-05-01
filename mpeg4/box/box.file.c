#include "box.include.h"
#include "inner.type.h"
#include "inner.data.h"

typedef struct mpeg4_stuff__file_s {
	mpeg4_stuff_t stuff;
	inner_array_t mdat;
	inner_array_t stco;
} mpeg4_stuff__file_s;

static mpeg4$define$stuff$init(file, mpeg4_stuff__file_s)
{
	mpeg4$define(inner, array, init)(&r->mdat, sizeof(mpeg4_stuff_t *));
	mpeg4$define(inner, array, init)(&r->stco, sizeof(mpeg4_stuff_t *));
	return &r->stuff;
}

static mpeg4$define$stuff$free(file, mpeg4_stuff__file_s)
{
	mpeg4$define(inner, array, clear)(&r->mdat);
	mpeg4$define(inner, array, clear)(&r->stco);
	mpeg4_stuff_free_default_func(&r->stuff);
}

static mpeg4$define$create(file)
{
	return mpeg4_stuff_alloc(atom, inst, type,
		sizeof(mpeg4_stuff__file_s),
		(mpeg4_stuff_init_f) mpeg4$define(stuff, file, init),
		(refer_free_f) mpeg4$define(stuff, file, free));
}

static const mpeg4_stuff_t* mpeg4$define(stuff, file, inner$push_mdat)(mpeg4_stuff__file_s *restrict r, mpeg4_stuff_t *restrict mdat)
{
	if (mpeg4$define(inner, array, append_data)(&r->mdat, 1, (const void *) &mdat))
		return &r->stuff;
	return NULL;
}

static const mpeg4_stuff_t* mpeg4$define(stuff, file, inner$push_stco)(mpeg4_stuff__file_s *restrict r, mpeg4_stuff_t *restrict stco)
{
	if (mpeg4$define(inner, array, append_data)(&r->stco, 1, (const void *) &stco))
		return &r->stuff;
	return NULL;
}

static const mpeg4_stuff_t* mpeg4$define(stuff, file, inner$get_mdat)(mpeg4_stuff__file_s *restrict r, mpeg4_stuff_t ***restrict mdat, uintptr_t *restrict n)
{
	if (mdat) *mdat = (mpeg4_stuff_t **) r->mdat.array;
	if (n) *n = r->mdat.number;
	return &r->stuff;
}

static const mpeg4_stuff_t* mpeg4$define(stuff, file, inner$get_stco)(mpeg4_stuff__file_s *restrict r, mpeg4_stuff_t ***restrict stco, uintptr_t *restrict n)
{
	if (stco) *stco = (mpeg4_stuff_t **) r->stco.array;
	if (n) *n = r->stco.number;
	return &r->stuff;
}

static const mpeg4_stuff_t* mpeg4$define(stuff, file, inner$clear)(mpeg4_stuff__file_s *restrict r)
{
	mpeg4$define(inner, array, clear)(&r->mdat);
	mpeg4$define(inner, array, clear)(&r->stco);
	return &r->stuff;
}

static const mpeg4$define$alloc(file)
{
	mpeg4_atom_s *restrict r;
	r = mpeg4$define(atom, container, alloc)(inst);
	if (r)
	{
		r->interface.create = mpeg4$define(atom, file, create);
		if (
			mpeg4$inner$type$alloc$layer(r, inst, ftyp) &&
			mpeg4$inner$type$alloc$layer(r, inst, mdat) &&
			mpeg4$inner$type$alloc$layer(r, inst, moov) &&
			mpeg4$stuff$method$set(r, file, inner$push_mdat) &&
			mpeg4$stuff$method$set(r, file, inner$push_stco) &&
			mpeg4$stuff$method$set(r, file, inner$get_mdat) &&
			mpeg4$stuff$method$set(r, file, inner$get_stco) &&
			mpeg4$stuff$method$set(r, file, inner$clear)
		) return r;
		refer_free(r);
	}
	return NULL;
}

mpeg4$define$find(file)
{
	static const mpeg4_box_type_t type = { .c = "file" };
	static const mpeg4_box_type_t extra = { .c = "root" };
	return mpeg4_find_atom(inst, mpeg4$define(atom, file, alloc), type.v, extra.v);
}
