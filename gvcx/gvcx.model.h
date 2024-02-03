#ifndef _gvcx_model_h_
#define _gvcx_model_h_

#include <refer.h>

struct gvcx_file_s;

typedef struct gvcx_model_s gvcx_model_s;
typedef struct gvcx_model_item_s gvcx_model_item_s;

gvcx_model_s* gvcx_model_alloc(void);
gvcx_model_item_s* gvcx_model_create_item(const gvcx_model_s *restrict m, const char *restrict type);
gvcx_model_item_s* gvcx_model_copyto_item(gvcx_model_item_s *restrict dst, const gvcx_model_item_s *restrict src);
gvcx_model_item_s* gvcx_model_dumpit_item(const gvcx_model_s *restrict m, const gvcx_model_item_s *restrict item);

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
const gvcx_model_item_s* gvcx_model_get_d(const gvcx_model_item_s *restrict i, struct gvcx_file_s *restrict *restrict file, uint64_t *restrict offset, uint64_t *restrict size);
gvcx_model_item_s* gvcx_model_set_d(gvcx_model_item_s *restrict i, struct gvcx_file_s *restrict file, uint64_t offset, uint64_t size);
const gvcx_model_item_s* gvcx_model_get_e(const gvcx_model_item_s *restrict i, uint64_t *restrict value, const char *restrict *restrict name);
uint64_t gvcx_model_get_e_value(const gvcx_model_item_s *restrict i);
const char* gvcx_model_get_e_name(const gvcx_model_item_s *restrict i);
gvcx_model_item_s* gvcx_model_set_e_value(gvcx_model_item_s *restrict i, uint64_t value);
gvcx_model_item_s* gvcx_model_set_e_name(gvcx_model_item_s *restrict i, const char *restrict name);
uintptr_t gvcx_model_get_a_count(const gvcx_model_item_s *restrict i);
gvcx_model_item_s* gvcx_model_get_a_index(const gvcx_model_item_s *restrict i, uintptr_t index);
gvcx_model_item_s* gvcx_model_set_a_clear(gvcx_model_item_s *restrict i);
gvcx_model_item_s* gvcx_model_set_a_append(gvcx_model_item_s *restrict i, gvcx_model_item_s *restrict item);
gvcx_model_item_s* gvcx_model_set_a_create(gvcx_model_item_s *restrict i, const char *restrict type);
gvcx_model_item_s* gvcx_model_get_o_child(const gvcx_model_item_s *restrict i, const char *restrict key);
gvcx_model_item_s* gvcx_model_get_o_value(const gvcx_model_item_s *restrict i, const char *restrict key);
gvcx_model_item_s* gvcx_model_set_o_clear(gvcx_model_item_s *restrict i);
gvcx_model_item_s* gvcx_model_set_o_delete(gvcx_model_item_s *restrict i, const char *restrict key);
gvcx_model_item_s* gvcx_model_set_o_insert(gvcx_model_item_s *restrict i, const char *restrict key, gvcx_model_item_s *restrict item);
gvcx_model_item_s* gvcx_model_set_o_create(gvcx_model_item_s *restrict i, const char *restrict key, const char *restrict type);

#endif
