#include "mtask.inner.h"

static inline void mtask_inner_process_set(struct mtask_process_t *restrict r, mtask_deal_f deal_func, refer_t deal_data)
{
	if (r->data)
		refer_free(r->data);
	r->deal = deal_func;
	r->data = refer_save(deal_data);
}

static void mtask_input_task_free_func(struct mtask_input_task_s *restrict r)
{
	mtask_inner_process_set(&r->process, NULL, NULL);
}

struct mtask_input_task_s* mtask_inner_input_need_task(struct mtask_inst_s *restrict mtask, mtask_deal_f deal_func, refer_t deal_data)
{
	struct mtask_input_task_s *restrict r;
	queue_s *restrict q;
	if ((q = mtask->cache_task) && (r = (struct mtask_input_task_s *) q->pull(q)))
		goto label_okay;
	else if ((r = (struct mtask_input_task_s *) refer_alloz(sizeof(struct mtask_input_task_s))))
	{
		refer_set_free(r, (refer_free_f) mtask_input_task_free_func);
		r->input.type = mtask_type__task;
		r->process.deal = deal_func;
		r->process.data = refer_save(deal_data);
		label_okay:
		return r;
	}
	return NULL;
}

void mtask_inner_input_unlink_task(struct mtask_inst_s *restrict mtask, struct mtask_input_task_s *restrict task)
{
	queue_s *restrict q;
	mtask_inner_process_set(&task->process, NULL, NULL);
	if (!(q = mtask->cache_task) || !q->push(q, task))
		refer_free(task);
}

queue_s* mtask_inner_queue_must_push(queue_s *restrict q, refer_t v, yaw_signal_s *restrict s, const volatile uintptr_t *running)
{
	uint32_t status;
	if (q->push(q, v))
		return q;
	for (;;)
	{
		status = yaw_signal_get(s);
		if (!*running)
			break;
		if (q->push(q, v))
			return q;
		yaw_signal_wait(s, status);
	}
	return NULL;
}

void mtask_inner_transfer_process(const struct mtask_transfer_t *restrict transfer, const struct mtask_context_t *restrict context)
{
	mtask_deal_f deal;
	uintptr_t pipe_index;
	pipe_index = context->pipe_index;
	if (pipe_index < transfer->process_number &&
		(deal = transfer->process[pipe_index].deal))
		deal(transfer->process[pipe_index].data, context);
}

struct mtask_inst_s* mtask_inner_transfer_semaphore(struct mtask_inst_s *restrict mtask, struct mtask_input_semaphore_s *restrict semaphore, uintptr_t pipe_index)
{
	if (pipe_index < semaphore->transfer.transfer_finish &&
		pipe_index < mtask->pipe_number)
	{
		struct mtask_pipe_s *restrict pipe;
		yaw_signal_s *restrict signal_pipe;
		pipe = mtask->pipe[pipe_index];
		signal_pipe = pipe->signal_pipe;
		semaphore->broadcast_okay = 0;
		semaphore->remaining_core = pipe->core_number;
		if (mtask_inner_queue_must_push(pipe->input, semaphore, signal_pipe, &mtask->running))
		{
			yaw_signal_inc_wake(signal_pipe, ~0);
			return mtask;
		}
	}
	return NULL;
}

struct mtask_inst_s* mtask_inner_transfer_fence(struct mtask_inst_s *restrict mtask, struct mtask_input_fence_s *restrict fence, uintptr_t pipe_index)
{
	if (pipe_index < fence->transfer.transfer_finish &&
		pipe_index < mtask->pipe_number)
	{
		struct mtask_pipe_s *restrict pipe;
		yaw_signal_s *restrict signal_pipe;
		yaw_lock_s *restrict mutex_fence;
		uintptr_t number;
		pipe = mtask->pipe[pipe_index];
		signal_pipe = pipe->signal_pipe;
		mutex_fence = pipe->mutex_fence;
		number = pipe->core_number;
		fence->remaining_core = number;
		fence->notify_okay = 0;
		yaw_lock_lock(mutex_fence);
		while (number)
		{
			if (!mtask_inner_queue_must_push(pipe->input, fence, signal_pipe, &mtask->running))
				break;
			--number;
		}
		yaw_lock_unlock(mutex_fence);
		if (!number)
		{
			yaw_signal_inc_wake(signal_pipe, ~0);
			return mtask;
		}
	}
	return NULL;
}
