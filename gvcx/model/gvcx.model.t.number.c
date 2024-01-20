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
	gvcx_model_value_number_t value;
};

static gvcx_model_item_s* gvcx_model_type_create(const gvcx_model_type_s *restrict t, const gvcx_model_s *restrict m, gvcx_model_type_t type, gvcx_model_flag_t flag, const char *restrict tname)
{
	return gvcx_model_item_alloc(sizeof(gvcx_model_item_number_s), type, flag, t->tname);
}
static gvcx_model_item_s* gvcx_model_type_copyto(const gvcx_model_type_s *restrict t, gvcx_model_item_number_s *restrict dst, const gvcx_model_item_number_s *restrict src)
{
	dst->value.u = src->value.u;
	return &dst->item;
}
static gvcx_model_type_s* gvcx_model_type_initial(gvcx_model_type_s *restrict t, const void *restrict pri)
{
	t->create = (gvcx_model_type_create_f) gvcx_model_type_create;
	t->copyto = (gvcx_model_type_copyto_f) gvcx_model_type_copyto;
	return t;
}

const gvcx_model_type_s* gvcx_model_type_create__u(void)
{
	return gvcx_model_type_alloc(gvcx_model_stype__uint, sizeof(gvcx_model_type_s), gvcx_model_type_initial, NULL);
}
const gvcx_model_type_s* gvcx_model_type_create__i(void)
{
	return gvcx_model_type_alloc(gvcx_model_stype__int, sizeof(gvcx_model_type_s), gvcx_model_type_initial, NULL);
}
const gvcx_model_type_s* gvcx_model_type_create__f(void)
{
	return gvcx_model_type_alloc(gvcx_model_stype__float, sizeof(gvcx_model_type_s), gvcx_model_type_initial, NULL);
}
const gvcx_model_type_s* gvcx_model_type_create__b(void)
{
	return gvcx_model_type_alloc(gvcx_model_stype__boolean, sizeof(gvcx_model_type_s), gvcx_model_type_initial, NULL);
}

// api

#define d_get(_t)  { if (i->type == gvcx_model_type__##_t) return ((const gvcx_model_item_number_s *) i)->value._t; return 0; }
#define d_set(_t, ...)  { if (i->type == gvcx_model_type__##_t && (i->flag & gvcx_model_flag__write)) { ((gvcx_model_item_number_s *) i)->value._t = __VA_ARGS__(v); return i; } return NULL; }

uint64_t gvcx_model_get_u(const gvcx_model_item_s *restrict i) d_get(u)
gvcx_model_item_s* gvcx_model_set_u(gvcx_model_item_s *restrict i, uint64_t v) d_set(u)

int64_t gvcx_model_get_i(const gvcx_model_item_s *restrict i) d_get(i)
gvcx_model_item_s* gvcx_model_set_i(gvcx_model_item_s *restrict i, int64_t v) d_set(i)

double gvcx_model_get_f(const gvcx_model_item_s *restrict i) d_get(f)
gvcx_model_item_s* gvcx_model_set_f(gvcx_model_item_s *restrict i, double v) d_set(f)

uint64_t gvcx_model_get_b(const gvcx_model_item_s *restrict i) d_get(b)
gvcx_model_item_s* gvcx_model_set_b(gvcx_model_item_s *restrict i, uint64_t v) d_set(b, !!)

#undef d_get
#undef d_set
