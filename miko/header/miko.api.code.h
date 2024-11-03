#ifndef _miko_api_code_h_
#define _miko_api_code_h_

#include "miko.type.code.h"
#include "miko.type.env.runtime.h"

miko_code_builder_s* miko_code_builder_alloc(const miko_env_runtime_s *restrict runtime);
miko_code_builder_s* miko_code_builder_new_code(miko_code_builder_s *restrict r);
void miko_code_builder_argv_reset(miko_code_builder_s *restrict r);
miko_code_builder_s* miko_code_builder_argv_push(miko_code_builder_s *restrict r, const miko_access_prefix_t *restrict prefix, miko_xpos_t xpos);
miko_code_builder_s* miko_code_builder_code_push_instruction(miko_code_builder_s *restrict r, const char *restrict name);
miko_code_builder_s* miko_code_builder_code_push_code(miko_code_builder_s *restrict r, const miko_code_s *restrict code);
miko_code_s* miko_code_builder_save_code(miko_code_builder_s *restrict r);

#endif
