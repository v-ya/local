#ifndef _miko_dev_h_
#define _miko_dev_h_

#include "miko.h"
#include "miko.type.h"

// miko.vector

#define miko_vector_auto  (~(uintptr_t) 0)

miko_vector_s_t miko_vector_alloc(uintptr_t item_size, miko_vector_initial_f initial, miko_vector_finally_f finally);
uintptr_t miko_vector_push(miko_vector_s_t r, const void *item_array, uintptr_t item_count);
uintptr_t miko_vector_pop(miko_vector_s_t r, uintptr_t item_count);
uintptr_t miko_vector_count(const miko_vector_s_t r);
void* miko_vector_data(const miko_vector_s_t r);
void* miko_vector_index(const miko_vector_s_t r, uintptr_t item_index);

// (item_offset == miko_vector_auto) => (item_offset = vector.count - item_count)
// (item_count  == miko_vector_auto) => (item_count = vector.count - item_offset)
// if (return != NULL) *count must > 0
void* miko_vector_map(const miko_vector_s_t r, uintptr_t item_offset, uintptr_t item_count, uintptr_t *restrict count);

// miko.strpool

miko_strpool_s_t miko_strpool_alloc(void);
refer_string_t miko_strpool_save(miko_strpool_s_t r, const char *restrict name);
void miko_strpool_undo(miko_strpool_s_t r, const char *restrict name);

// miko.log

miko_log_s* miko_log_alloc(void);
void miko_log_set_limit(miko_log_s *restrict r, miko_log_level_t limit);
miko_log_s* miko_log_add(miko_log_s *restrict r, miko_log_level_t level, refer_string_t key, refer_string_t value);
miko_log_s* miko_log_add_key(miko_log_s *restrict r, miko_log_level_t level, const char *restrict key, refer_string_t value);
miko_log_s* miko_log_add_value(miko_log_s *restrict r, miko_log_level_t level, refer_string_t key, const char *restrict value);
miko_log_s* miko_log_add_key_value(miko_log_s *restrict r, miko_log_level_t level, const char *restrict key, const char *restrict value);
uintptr_t miko_log_into(miko_log_s *restrict r, refer_string_t name);
uintptr_t miko_log_into_name(miko_log_s *restrict r, const char *restrict name);
void miko_log_back(miko_log_s *restrict r, uintptr_t into_result);
void miko_log_print(miko_log_s *restrict r, mlog_s *restrict mlog);

// miko.iset

miko_iset_s* miko_iset_alloc(const char *restrict name);
miko_iset_s* miko_iset_add_depend(miko_iset_s *restrict r, const char *restrict name);
miko_iset_s* miko_iset_add_score(miko_iset_s *restrict r, const char *restrict name);
miko_iset_s* miko_iset_add_style(miko_iset_s *restrict r, const char *restrict name);
miko_iset_s* miko_iset_add_major(miko_iset_s *restrict r, const char *restrict major_name, miko_major_vtype_t vtype);
miko_iset_s* miko_iset_add_minor(miko_iset_s *restrict r, const char *restrict major_name, miko_minor_s *restrict minor);
miko_iset_s* miko_iset_add_default_minor(miko_iset_s *restrict r, const char *restrict major_name, const char *restrict minor_name);
miko_iset_s* miko_iset_add_instruction(miko_iset_s *restrict r, const miko_iset_pool_s *restrict pool, const char *restrict name, const miko_xpos_prefix_t xpos[], const miko_instruction_attr_t attr[], refer_t pri);

// miko.minor

void miko_minor_free_func(miko_minor_s *restrict r);
miko_minor_s* miko_minor_alloc(const char *restrict name, uintptr_t size);

// miko.env

const miko_iset_pool_s* miko_env_save_depend(miko_env_s *restrict r, miko_iset_s *restrict iset);

#endif
