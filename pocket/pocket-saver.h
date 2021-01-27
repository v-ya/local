#ifndef _pocket_saver_h_
#define _pocket_saver_h_

#include "pocket.h"
#include <refer.h>

typedef struct pocket_saver_s pocket_saver_s;
typedef struct pocket_saver_attr_index_t pocket_saver_index_t;

pocket_saver_s* pocket_saver_alloc(void);

pocket_saver_s* pocket_saver_set_verify(pocket_saver_s *restrict saver, const char *restrict verify);
pocket_saver_s* pocket_saver_set_tag(pocket_saver_s *restrict saver, const char *restrict tag);
pocket_saver_s* pocket_saver_set_version(pocket_saver_s *restrict saver, const char *restrict version);
pocket_saver_s* pocket_saver_set_author(pocket_saver_s *restrict saver, const char *restrict author);
pocket_saver_s* pocket_saver_set_time(pocket_saver_s *restrict saver, const char *restrict time);
pocket_saver_s* pocket_saver_set_description(pocket_saver_s *restrict saver, const char *restrict description);
pocket_saver_s* pocket_saver_set_flag(pocket_saver_s *restrict saver, const char *restrict flag);

pocket_saver_index_t* pocket_saver_root_system(pocket_saver_s *restrict saver);
pocket_saver_index_t* pocket_saver_root_user(pocket_saver_s *restrict saver);

pocket_saver_index_t* pocket_saver_cd(pocket_saver_index_t *restrict index, const char *const restrict *restrict path);
void pocket_saver_delete(pocket_saver_index_t *restrict index, const char *const restrict *restrict path);
pocket_saver_s* pocket_saver_create_null(pocket_saver_s *restrict saver, pocket_saver_index_t *restrict index, const char *const restrict *restrict path);
pocket_saver_s* pocket_saver_create_index(pocket_saver_s *restrict saver, pocket_saver_index_t *restrict index, const char *const restrict *restrict path);
pocket_saver_s* pocket_saver_create_string(pocket_saver_s *restrict saver, pocket_saver_index_t *restrict index, const char *const restrict *restrict path, const char *restrict value);
pocket_saver_s* pocket_saver_create_text(pocket_saver_s *restrict saver, pocket_saver_index_t *restrict index, const char *const restrict *restrict path, const char *restrict data, uintptr_t align);
pocket_saver_s* pocket_saver_create_u8(pocket_saver_s *restrict saver, pocket_saver_index_t *restrict index, const char *const restrict *restrict path, const uint8_t *restrict data, uint64_t n, uintptr_t align);
pocket_saver_s* pocket_saver_create_s8(pocket_saver_s *restrict saver, pocket_saver_index_t *restrict index, const char *const restrict *restrict path, const int8_t *restrict data, uint64_t n, uintptr_t align);
pocket_saver_s* pocket_saver_create_u16(pocket_saver_s *restrict saver, pocket_saver_index_t *restrict index, const char *const restrict *restrict path, const uint16_t *restrict data, uint64_t n, uintptr_t align);
pocket_saver_s* pocket_saver_create_s16(pocket_saver_s *restrict saver, pocket_saver_index_t *restrict index, const char *const restrict *restrict path, const int16_t *restrict data, uint64_t n, uintptr_t align);
pocket_saver_s* pocket_saver_create_u32(pocket_saver_s *restrict saver, pocket_saver_index_t *restrict index, const char *const restrict *restrict path, const uint32_t *restrict data, uint64_t n, uintptr_t align);
pocket_saver_s* pocket_saver_create_s32(pocket_saver_s *restrict saver, pocket_saver_index_t *restrict index, const char *const restrict *restrict path, const int32_t *restrict data, uint64_t n, uintptr_t align);
pocket_saver_s* pocket_saver_create_u64(pocket_saver_s *restrict saver, pocket_saver_index_t *restrict index, const char *const restrict *restrict path, const uint64_t *restrict data, uint64_t n, uintptr_t align);
pocket_saver_s* pocket_saver_create_s64(pocket_saver_s *restrict saver, pocket_saver_index_t *restrict index, const char *const restrict *restrict path, const int64_t *restrict data, uint64_t n, uintptr_t align);
pocket_saver_s* pocket_saver_create_f32(pocket_saver_s *restrict saver, pocket_saver_index_t *restrict index, const char *const restrict *restrict path, const float *restrict data, uint64_t n, uintptr_t align);
pocket_saver_s* pocket_saver_create_f64(pocket_saver_s *restrict saver, pocket_saver_index_t *restrict index, const char *const restrict *restrict path, const double *restrict data, uint64_t n, uintptr_t align);
pocket_saver_s* pocket_saver_create_custom(pocket_saver_s *restrict saver, pocket_saver_index_t *restrict index, const char *const restrict *restrict path, const char *tag, const void *restrict data, uint64_t size, uintptr_t align);

uint8_t* pocket_saver_build(pocket_saver_s *restrict saver, uint64_t *restrict size, const struct pocket_verify_s *restrict verify);
void pocket_saver_build_free(uint8_t *restrict pocket);
pocket_saver_s* pocket_saver_save(pocket_saver_s *restrict saver, const char *restrict path, const struct pocket_verify_s *restrict verify);

#endif
