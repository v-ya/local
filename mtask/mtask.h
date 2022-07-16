#ifndef _mtask_h_
#define _mtask_h_

#include <refer.h>

typedef struct mtask_inst_s mtask_inst_s;
typedef struct mtask_timer_s mtask_timer_s;
typedef const struct mtask_context_t mtask_context_t;
typedef struct mtask_param_inst_t mtask_param_inst_t;
typedef struct mtask_param_pipe_t mtask_param_pipe_t;
typedef struct mtask_param_transfer_t mtask_param_transfer_t;
typedef struct mtask_param_process_t mtask_param_process_t;

typedef void (*mtask_deal_f)(refer_t data, mtask_context_t *restrict c);

struct mtask_context_t {
	mtask_inst_s *mtask;
	uintptr_t pipe_index;
	uintptr_t core_index;
};

struct mtask_param_inst_t {
	uintptr_t task_cache_number;
	uintptr_t pipe_number;
	const mtask_param_pipe_t *pipe_param;
};

struct mtask_param_pipe_t {
	uintptr_t core_number;
	uintptr_t queue_input_size;
	uintptr_t queue_interrupt_size;
	uintptr_t friendly;
};

struct mtask_param_transfer_t {
	uintptr_t pipe_index;
	uintptr_t transfer_number;
	const mtask_param_process_t *transfer_process;
};

struct mtask_param_process_t {
	mtask_deal_f deal;
	refer_t data;
};

mtask_inst_s* mtask_inst_alloc(const mtask_param_inst_t *restrict param);
void mtask_inst_stop(mtask_inst_s *restrict mtask);

mtask_inst_s* mtask_active_pipe(mtask_inst_s *mtask, uintptr_t pipe_index);
mtask_inst_s* mtask_push_task_nonblock(mtask_inst_s *mtask, uintptr_t pipe_index, mtask_deal_f deal_func, refer_t deal_data);
mtask_inst_s* mtask_push_task_block(mtask_inst_s *mtask, uintptr_t pipe_index, mtask_deal_f deal_func, refer_t deal_data);
mtask_inst_s* mtask_push_semaphore_block(mtask_inst_s *mtask, const mtask_param_transfer_t *restrict param);
mtask_inst_s* mtask_push_fence_block(mtask_inst_s *mtask, const mtask_param_transfer_t *restrict param);
mtask_inst_s* mtask_push_semaphore_single(mtask_inst_s *mtask, uintptr_t pipe_index, uintptr_t transfer_number, mtask_deal_f deal_func, refer_t deal_data);
mtask_inst_s* mtask_push_fence_single(mtask_inst_s *mtask, uintptr_t pipe_index, uintptr_t transfer_number, mtask_deal_f deal_func, refer_t deal_data);

#endif
