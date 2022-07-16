#ifndef _mtask_h_
#define _mtask_h_

#include <refer.h>

typedef struct mtask_inst_s mtask_inst_s;
typedef struct mtask_timer_s mtask_timer_s;
typedef const struct mtask_context_t mtask_context_t;
typedef const struct mtask_param_inst_t mtask_param_inst_t;
typedef const struct mtask_param_pipe_t mtask_param_pipe_t;

typedef void (*mtask_deal_f)(refer_t data, mtask_context_t *restrict c);

struct mtask_context_t {
	mtask_inst_s *mtask;
	uintptr_t pipe_index;
	uintptr_t core_index;
};

struct mtask_param_inst_t {
	uintptr_t pipe_number;
	const mtask_param_pipe_t *pipe_param;
};

struct mtask_param_pipe_t {
	uintptr_t core_number;
	uintptr_t queue_input_size;
	uintptr_t queue_interrupt_size;
	uintptr_t friendly;
};

mtask_inst_s* mtask_inst_alloc(struct mtask_param_inst_t *restrict param);

#endif
