#ifndef _miko_h_
#define _miko_h_

#include <refer.h>
#include <mlog.h>
#include "miko.type.env.h"
#include "miko.type.program.argv.h"
#include "miko.type.program.h"
#include "miko.type.rt.h"

miko_env_s* miko_env_create(mlog_s *restrict mlog, miko_env_register_f register_array[]);

miko_rt_s* miko_rt_alloc(miko_program_s *restrict program, mlog_s *restrict mlog, uintptr_t stack_max_limit);
void miko_rt_reset(miko_rt_s *restrict r);
miko_rt_s* miko_rt_load(miko_rt_s *restrict r, const char *restrict name, miko_count_t argc, const miko_program_argv_t *restrict argv);
miko_rt_s* miko_rt_exec(miko_rt_s *restrict r);
const miko_rt_backup_s* miko_rt_backup(miko_rt_s *restrict r);
miko_rt_s* miko_rt_goback(miko_rt_s *restrict r, const miko_rt_backup_s *restrict backup);
// user confirm exec(miko_rt_s *) correct finish
uintptr_t miko_rt_backup_only_instr_index(miko_rt_s *restrict r);
// user confirm exec(miko_rt_s *) correct finish
void miko_rt_goback_only_instr_index(miko_rt_s *restrict r, uintptr_t instr_index);

#endif
