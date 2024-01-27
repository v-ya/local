#include "gvcx.model.h"

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
static d_type_initial(number, void)
{
	t->create = d_type_function(number, create);
	t->copyto = d_type_function(number, copyto);
	return t;
}

#define d_type(_t)  { return gvcx_model_type_alloc(name, gvcx_model_type__##_t, type_minor, sizeof(gvcx_model_type_s), d_type_function(number, initial), NULL); }
const gvcx_model_type_s* gvcx_model_type_create__uint(const gvcx_model_s *restrict m, const char *restrict name, uint32_t type_minor) d_type(uint)
const gvcx_model_type_s* gvcx_model_type_create__int(const gvcx_model_s *restrict m, const char *restrict name, uint32_t type_minor) d_type(int)
const gvcx_model_type_s* gvcx_model_type_create__float(const gvcx_model_s *restrict m, const char *restrict name, uint32_t type_minor) d_type(float)
const gvcx_model_type_s* gvcx_model_type_create__boolean(const gvcx_model_s *restrict m, const char *restrict name, uint32_t type_minor) d_type(boolean)
#undef d_type

// api

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
