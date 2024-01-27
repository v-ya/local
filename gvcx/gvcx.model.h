#ifndef _gvcx_model_h_
#define _gvcx_model_h_

#include <refer.h>

typedef struct gvcx_model_s gvcx_model_s;
typedef struct gvcx_model_item_s gvcx_model_item_s;

gvcx_model_s* gvcx_model_alloc(void);
gvcx_model_item_s* gvcx_model_create_item(const gvcx_model_s *restrict m, const char *restrict type);
gvcx_model_item_s* gvcx_model_copyto_item(const gvcx_model_s *restrict m, gvcx_model_item_s *restrict dst, const gvcx_model_item_s *restrict src);

uint64_t gvcx_model_get_u(const gvcx_model_item_s *restrict i);
gvcx_model_item_s* gvcx_model_set_u(gvcx_model_item_s *restrict i, uint64_t v);
int64_t gvcx_model_get_i(const gvcx_model_item_s *restrict i);
gvcx_model_item_s* gvcx_model_set_i(gvcx_model_item_s *restrict i, int64_t v);
double gvcx_model_get_f(const gvcx_model_item_s *restrict i);
gvcx_model_item_s* gvcx_model_set_f(gvcx_model_item_s *restrict i, double v);
uint64_t gvcx_model_get_b(const gvcx_model_item_s *restrict i);
gvcx_model_item_s* gvcx_model_set_b(gvcx_model_item_s *restrict i, uint64_t v);
refer_nstring_t gvcx_model_get_s(const gvcx_model_item_s *restrict i);
gvcx_model_item_s* gvcx_model_set_s(gvcx_model_item_s *restrict i, refer_nstring_t v);
gvcx_model_item_s* gvcx_model_set_s_str(gvcx_model_item_s *restrict i, const char *restrict v);
gvcx_model_item_s* gvcx_model_set_s_str_len(gvcx_model_item_s *restrict i, const char *restrict v, uintptr_t n);

#endif
