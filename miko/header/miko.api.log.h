#ifndef _miko_api_log_h_
#define _miko_api_log_h_

#include "miko.type.log.h"
#include <mlog.h>

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

#endif
