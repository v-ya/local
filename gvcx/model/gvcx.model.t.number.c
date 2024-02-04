#include "gvcx.model.h"
#include <inttypes.h>

typedef union gvcx_model_value_number_t gvcx_model_value_number_t;
typedef struct gvcx_model_item_number_s gvcx_model_item_number_s;

union gvcx_model_value_number_t {
	uint64_t u;
	int64_t i;
	double f;
	uint64_t b;
};

struct gvcx_model_item_number_s {
	gvcx_model_item_s item;
	uintptr_t nbits;
	gvcx_model_value_number_t value;
};

static d_type_create(number, gvcx_model_type_s)
{
	return gvcx_model_item_alloc(sizeof(gvcx_model_item_number_s), t);
}
static d_type_copyto(number, gvcx_model_type_s, gvcx_model_item_number_s)
{
	dst->nbits = src->nbits;
	dst->value.u = src->value.u;
	return &dst->item;
}
static d_type_iprint(uint, gvcx_model_type_s, gvcx_model_item_number_s)
{
	if (gvcx_model_item_iprint(&item->item, log))
		mlog_printf(log->input, " = %" PRIu64 " (%zu bits)\n", item->value.u, item->nbits);
}
static d_type_iprint(int, gvcx_model_type_s, gvcx_model_item_number_s)
{
	if (gvcx_model_item_iprint(&item->item, log))
		mlog_printf(log->input, " = %" PRIi64 " (%zu bits)\n", item->value.i, item->nbits);
}
static d_type_iprint(float, gvcx_model_type_s, gvcx_model_item_number_s)
{
	if (gvcx_model_item_iprint(&item->item, log))
		mlog_printf(log->input, " = %g (%zu bits)\n", item->value.f, item->nbits);
}
static d_type_iprint(boolean, gvcx_model_type_s, gvcx_model_item_number_s)
{
	if (gvcx_model_item_iprint(&item->item, log))
		mlog_printf(log->input, " = %s (%zu bits)\n", item->value.b?"true":"false", item->nbits);
}
static d_type_initial(number, void)
{
	t->create = d_type_function(number, create);
	t->copyto = d_type_function(number, copyto);
	switch (t->type_major)
	{
		case gvcx_model_type__uint:    t->iprint = d_type_function(uint,    iprint); break;
		case gvcx_model_type__int:     t->iprint = d_type_function(int,     iprint); break;
		case gvcx_model_type__float:   t->iprint = d_type_function(float,   iprint); break;
		case gvcx_model_type__boolean: t->iprint = d_type_function(boolean, iprint); break;
		default: t->iprint = NULL; break;
	}
	return t;
}

#define d_type(_t)  { return gvcx_model_type_alloc(name, gvcx_model_type__##_t, type_minor, sizeof(gvcx_model_type_s), d_type_function(number, initial), NULL); }
const gvcx_model_type_s* gvcx_model_type_create__uint(const gvcx_model_s *restrict m, const char *restrict name, uint32_t type_minor) d_type(uint)
const gvcx_model_type_s* gvcx_model_type_create__int(const gvcx_model_s *restrict m, const char *restrict name, uint32_t type_minor) d_type(int)
const gvcx_model_type_s* gvcx_model_type_create__float(const gvcx_model_s *restrict m, const char *restrict name, uint32_t type_minor) d_type(float)
const gvcx_model_type_s* gvcx_model_type_create__boolean(const gvcx_model_s *restrict m, const char *restrict name, uint32_t type_minor) d_type(boolean)
#undef d_type

// api

uintptr_t gvcx_model_get_n_bits(const gvcx_model_item_s *restrict i)
{
	switch (i->type_major)
	{
		case gvcx_model_type__uint:
		case gvcx_model_type__int:
		case gvcx_model_type__float:
		case gvcx_model_type__boolean:
			return ((const gvcx_model_item_number_s *) i)->nbits;
		default:
			return 0;
	}
}
gvcx_model_item_s* gvcx_model_set_n_bits(gvcx_model_item_s *restrict i, uintptr_t nbist)
{
	if (i->item_flag & gvcx_model_flag__write)
	{
		switch (i->type_major)
		{
			case gvcx_model_type__uint:
			case gvcx_model_type__int:
			case gvcx_model_type__float:
			case gvcx_model_type__boolean:
				((gvcx_model_item_number_s *) i)->nbits = nbist;
				return i;
			default:
				break;
		}
	}
	return NULL;
}

#define d_get(_f, _t)  { if (i->type_major == gvcx_model_type__##_t) return ((const gvcx_model_item_number_s *) i)->value._f; return 0; }
#define d_set(_f, _t, ...)  { if (i->type_major == gvcx_model_type__##_t && (i->item_flag & gvcx_model_flag__write)) { ((gvcx_model_item_number_s *) i)->value._f = __VA_ARGS__(v); return i; } return NULL; }

uint64_t gvcx_model_get_u(const gvcx_model_item_s *restrict i) d_get(u, uint)
gvcx_model_item_s* gvcx_model_set_u(gvcx_model_item_s *restrict i, uint64_t v) d_set(u, uint)

int64_t gvcx_model_get_i(const gvcx_model_item_s *restrict i) d_get(i, int)
gvcx_model_item_s* gvcx_model_set_i(gvcx_model_item_s *restrict i, int64_t v) d_set(i, int)

double gvcx_model_get_f(const gvcx_model_item_s *restrict i) d_get(f, float)
gvcx_model_item_s* gvcx_model_set_f(gvcx_model_item_s *restrict i, double v) d_set(f, float)

uint64_t gvcx_model_get_b(const gvcx_model_item_s *restrict i) d_get(b, boolean)
gvcx_model_item_s* gvcx_model_set_b(gvcx_model_item_s *restrict i, uint64_t v) d_set(b, boolean, !!)

#undef d_get
#undef d_set
