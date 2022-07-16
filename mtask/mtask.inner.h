#ifndef _mtask_inner_h_
#define _mtask_inner_h_

#include "mtask.h"
#include <queue/queue.h>
#include <yaw.h>

// input

enum mtask_type_t {
	mtask_type__task,
	mtask_type__semaphore,
	mtask_type__fence,
};

struct mtask_process_t {
	mtask_deal_f deal;
	refer_t data;
};

struct mtask_transfer_t {
	uintptr_t transfer_finish;
	uintptr_t process_number;
	struct mtask_process_t process[];
};

struct mtask_input_s {
	enum mtask_type_t type;
};

struct mtask_input_task_s {
	struct mtask_input_s input;
	struct mtask_process_t process;
};

struct mtask_input_semaphore_s {
	struct mtask_input_s input;
	volatile uintptr_t broadcast_okay;
	volatile uintptr_t remaining_core;
	struct mtask_transfer_t transfer;
};

struct mtask_input_fence_s {
	struct mtask_input_s input;
	volatile uintptr_t remaining_initial;
	volatile uintptr_t notify_okay;
	volatile uintptr_t remaining_finally;
	struct mtask_transfer_t transfer;
};

// core

struct mtask_core_s {
	struct mtask_context_t context;
	queue_s *interrupt;
	yaw_s *task;
};

// pipe

struct mtask_pipe_s {
	volatile uintptr_t *running;
	uintptr_t pipe_index;
	queue_s *input;
	yaw_signal_s *signal_pipe;
	yaw_signal_s *signal_fence;
	yaw_lock_s *mutex_fence;
	uintptr_t friendly;
	uintptr_t core_number;
	struct mtask_core_s *core[];
};

// mtask

struct mtask_inst_s {
	volatile uintptr_t running;
	queue_s *cache_task;
	uintptr_t pipe_number;
	struct mtask_pipe_s *pipe[];
};

// inner

struct mtask_input_task_s* mtask_inner_input_need_task(struct mtask_inst_s *restrict mtask, mtask_deal_f deal_func, refer_t deal_data);
void mtask_inner_input_unlink_task(struct mtask_inst_s *restrict mtask, struct mtask_input_task_s *restrict task);
struct mtask_input_semaphore_s* mtask_inner_input_create_semaphore(uintptr_t pipe_number);
struct mtask_input_fence_s* mtask_inner_input_create_fence(uintptr_t pipe_number);
void mtask_inner_transfer_set(struct mtask_transfer_t *restrict transfer, const struct mtask_param_transfer_t *restrict param);

queue_s* mtask_inner_queue_must_push(queue_s *restrict q, refer_t v, yaw_signal_s *restrict s, const volatile uintptr_t *running);
void mtask_inner_transfer_process(const struct mtask_transfer_t *restrict transfer, const struct mtask_context_t *restrict context);
struct mtask_inst_s* mtask_inner_transfer_semaphore(struct mtask_inst_s *restrict mtask, struct mtask_input_semaphore_s *restrict semaphore, uintptr_t pipe_index);
struct mtask_inst_s* mtask_inner_transfer_fence(struct mtask_inst_s *restrict mtask, struct mtask_input_fence_s *restrict fence, uintptr_t pipe_index);

// core

struct mtask_core_s* mtask_core_alloc(const struct mtask_context_t *restrict context, uintptr_t queue_interrupt_size);
struct mtask_core_s* mtask_core_start(struct mtask_core_s *restrict core);

// pipe

struct mtask_pipe_s* mtask_pipe_alloc(struct mtask_inst_s *restrict mtask, uintptr_t pipe_index, const struct mtask_param_pipe_t *restrict param);
struct mtask_pipe_s* mtask_pipe_start(struct mtask_pipe_s *restrict pipe);

// timer

#endif
