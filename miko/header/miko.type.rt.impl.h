#ifndef _miko_type_rt_impl_h_
#define _miko_type_rt_impl_h_

#include <refer.h>
#include <mlog.h>
#include "miko.wink.h"
#include "miko.type.rt.h"
#include "miko.type.base.h"
#include "miko.type.instr.h"
#include "miko.type.form.h"
#include "miko.type.table.h"
#include "miko.type.env.runtime.h"
#include "miko.type.program.h"

typedef struct miko_rt_context_t miko_rt_context_t;
typedef struct miko_rt_dynamic_t miko_rt_dynamic_t;

struct miko_rt_context_t {
	const miko_instr_t *const *instr_array;  // null | symbol initial
	miko_count_t instr_count;                // 0    | symbol initial
	miko_offset_t instr_index;               // 0    | symbol initial
};

struct miko_rt_dynamic_t {
	miko_form_w *stack;          // empty | symbol initial
	miko_form_table_s *segment;  // empty | symbol initial
	miko_rt_context_t context;   // empty | symbol initial
};

struct miko_rt_s {
	miko_program_s *program;            // by alloc param
	mlog_s *mlog;                       // by alloc param
	miko_wink_gomi_s *gomi;             // by program save
	const miko_env_runtime_s *runtime;  // by program save
	const miko_table_s *process;        // create by runtime.process
	miko_rt_dynamic_t dynamic;          // initial by program.symbol, fix by exec
};

#endif
