#ifndef _layer_model_h_
#define _layer_model_h_

#include <refer.h>

struct layer_log_s;
struct layer_file_s;

typedef struct layer_model_s layer_model_s;
typedef struct layer_model_item_s layer_model_item_s;

layer_model_s* layer_model_alloc(void);
layer_model_item_s* layer_model_create_item(const layer_model_s *restrict m, const char *restrict type);
layer_model_item_s* layer_model_copyto_item(layer_model_item_s *restrict dst, const layer_model_item_s *restrict src);
layer_model_item_s* layer_model_dumpit_item(const layer_model_s *restrict m, const layer_model_item_s *restrict item);
void layer_model_iprint_item(struct layer_log_s *restrict log, const layer_model_item_s *restrict item);

uintptr_t layer_model_get_n_bits(const layer_model_item_s *restrict i);
layer_model_item_s* layer_model_set_n_bits(layer_model_item_s *restrict i, uintptr_t nbist);
uint64_t layer_model_get_u(const layer_model_item_s *restrict i);
layer_model_item_s* layer_model_set_u(layer_model_item_s *restrict i, uint64_t v);
int64_t layer_model_get_i(const layer_model_item_s *restrict i);
layer_model_item_s* layer_model_set_i(layer_model_item_s *restrict i, int64_t v);
double layer_model_get_f(const layer_model_item_s *restrict i);
layer_model_item_s* layer_model_set_f(layer_model_item_s *restrict i, double v);
uint64_t layer_model_get_b(const layer_model_item_s *restrict i);
layer_model_item_s* layer_model_set_b(layer_model_item_s *restrict i, uint64_t v);
refer_nstring_t layer_model_get_s(const layer_model_item_s *restrict i);
layer_model_item_s* layer_model_set_s(layer_model_item_s *restrict i, refer_nstring_t v);
layer_model_item_s* layer_model_set_s_str(layer_model_item_s *restrict i, const char *restrict v);
layer_model_item_s* layer_model_set_s_str_len(layer_model_item_s *restrict i, const char *restrict v, uintptr_t n);
const layer_model_item_s* layer_model_get_d(const layer_model_item_s *restrict i, struct layer_file_s *restrict *restrict file, uint64_t *restrict offset, uint64_t *restrict size);
layer_model_item_s* layer_model_set_d(layer_model_item_s *restrict i, struct layer_file_s *restrict file, uint64_t offset, uint64_t size);
const layer_model_item_s* layer_model_get_e(const layer_model_item_s *restrict i, uint64_t *restrict value, const char *restrict *restrict name);
uint64_t layer_model_get_e_value(const layer_model_item_s *restrict i);
const char* layer_model_get_e_name(const layer_model_item_s *restrict i);
layer_model_item_s* layer_model_set_e_value(layer_model_item_s *restrict i, uint64_t value);
layer_model_item_s* layer_model_set_e_name(layer_model_item_s *restrict i, const char *restrict name);
uintptr_t layer_model_get_a_count(const layer_model_item_s *restrict i);
layer_model_item_s* layer_model_get_a_index(const layer_model_item_s *restrict i, uintptr_t index);
layer_model_item_s* layer_model_set_a_clear(layer_model_item_s *restrict i);
layer_model_item_s* layer_model_set_a_append(layer_model_item_s *restrict i, layer_model_item_s *restrict item);
layer_model_item_s* layer_model_set_a_create(layer_model_item_s *restrict i, const char *restrict type);
layer_model_item_s* layer_model_get_o_child(const layer_model_item_s *restrict i, const char *restrict key);
layer_model_item_s* layer_model_get_o_value(const layer_model_item_s *restrict i, const char *restrict key);
layer_model_item_s* layer_model_set_o_clear(layer_model_item_s *restrict i);
layer_model_item_s* layer_model_set_o_delete(layer_model_item_s *restrict i, const char *restrict key);
layer_model_item_s* layer_model_set_o_insert(layer_model_item_s *restrict i, const char *restrict key, layer_model_item_s *restrict item);
layer_model_item_s* layer_model_set_o_create(layer_model_item_s *restrict i, const char *restrict key, const char *restrict type);

#endif
