#ifndef _miko_api_env_h_
#define _miko_api_env_h_

#include "miko.type.env.h"
#include "miko.type.iset.h"
#include "miko.type.env.runtime.h"
#include "miko.type.major.h"
#include "miko.type.instruction.h"

const miko_iset_pool_s* miko_env_save_depend(miko_env_s *restrict r, miko_iset_s *restrict iset);

refer_string_t miko_env_runtime_segment_by_index(const miko_env_runtime_s *restrict r, miko_index_t index);
refer_string_t miko_env_runtime_segment_by_name(const miko_env_runtime_s *restrict r, const char *restrict name);
refer_string_t miko_env_runtime_action_by_index(const miko_env_runtime_s *restrict r, miko_index_t index);
refer_string_t miko_env_runtime_action_by_name(const miko_env_runtime_s *restrict r, const char *restrict name);
refer_string_t miko_env_runtime_interrupt_by_index(const miko_env_runtime_s *restrict r, miko_index_t index);
refer_string_t miko_env_runtime_interrupt_by_name(const miko_env_runtime_s *restrict r, const char *restrict name);
miko_major_s* miko_env_runtime_major_by_index(const miko_env_runtime_s *restrict r, miko_index_t index);
miko_major_s* miko_env_runtime_major_by_name(const miko_env_runtime_s *restrict r, const char *restrict name);
const miko_instruction_s* miko_env_runtime_instruction_by_index(const miko_env_runtime_s *restrict r, miko_index_t index);
const miko_instruction_s* const* miko_env_runtime_instruction_by_name(const miko_env_runtime_s *restrict r, const char *restrict name, miko_count_t *restrict count);
miko_index_t miko_env_runtime_instruction_get_index(const miko_env_runtime_s *restrict r, const char *restrict name, const miko_access_prefix_t prefix[]);
const miko_instruction_s* miko_env_runtime_instruction_get_ptr(const miko_env_runtime_s *restrict r, const char *restrict name, const miko_access_prefix_t prefix[]);

#endif
