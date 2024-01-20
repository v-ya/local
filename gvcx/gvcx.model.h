#ifndef _gvcx_model_h_
#define _gvcx_model_h_

#include <refer.h>

typedef struct gvcx_model_s gvcx_model_s;
typedef struct gvcx_model_item_s gvcx_model_item_s;

const gvcx_model_s* gvcx_model_alloc(void);

uint64_t gvcx_model_get_u(const gvcx_model_item_s *restrict i);
gvcx_model_item_s* gvcx_model_set_u(gvcx_model_item_s *restrict i, uint64_t v);
int64_t gvcx_model_get_i(const gvcx_model_item_s *restrict i);
gvcx_model_item_s* gvcx_model_set_i(gvcx_model_item_s *restrict i, int64_t v);
double gvcx_model_get_f(const gvcx_model_item_s *restrict i);
gvcx_model_item_s* gvcx_model_set_f(gvcx_model_item_s *restrict i, double v);
uint64_t gvcx_model_get_b(const gvcx_model_item_s *restrict i);
gvcx_model_item_s* gvcx_model_set_b(gvcx_model_item_s *restrict i, uint64_t v);


#endif
