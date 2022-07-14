#include "mtask.inner.h"

struct mtask_core_stack_t {
	struct mtask_core_s *core;
	struct mtask_pipe_s *pipe;
	struct mtask_context_t context;
};

static inline void mtask_core_task_do_task(struct mtask_input_task_s *input, const struct mtask_core_stack_t *restrict stack)
{
	mtask_deal_f deal;
	if ((deal = input->process.deal))
		deal(input->process.data, &stack->context);
}

static inline void mtask_core_task_do_semaphore(struct mtask_input_semaphore_s *input, const struct mtask_core_stack_t *restrict stack)
{
	if (!input->broadcast_okay)
	{
		const volatile uintptr_t *running;
		struct mtask_pipe_s *restrict pipe;
		yaw_signal_s *restrict signal_pipe;
		struct mtask_core_s *core;
		uintptr_t i, n;
		input->broadcast_okay = 1;
		running = &stack->context.mtask->running;
		pipe = stack->pipe;
		signal_pipe = pipe->signal_pipe;
		n = pipe->core_number;
		for (i = 0; i < n; ++i)
		{
			core = pipe->core[i];
			if (core != stack->core && !mtask_inner_queue_must_push(core->interrupt, input, signal_pipe, running))
				return ;
		}
		yaw_signal_inc_wake(signal_pipe, ~0);
	}
	if (!__sync_sub_and_fetch(&input->remaining_core, 1))
	{
		mtask_deal_f deal;
		uintptr_t index;
		index = stack->context.pipe_index;
		if (index < input->transfer.process_number &&
			(deal = input->transfer.process[index].deal))
			deal(input->transfer.process[index].data, &stack->context);
		index += 1;
		if (index < input->transfer.transfer_finish &&
			index < stack->context.mtask->pipe_number)
		{
			// transfer next pipe
			;
		}
	}
}

static inline void mtask_core_task_do_fence(struct mtask_input_fence_s *input, const struct mtask_core_stack_t *restrict stack)
{
	const volatile uintptr_t *running;
	yaw_signal_s *restrict signal_fence;
	running = &stack->context.mtask->running;
	signal_fence = stack->pipe->signal_fence;
	if (__sync_sub_and_fetch(&input->remaining_core, 1))
	{
		uint32_t status;
		for (;;)
		{
			status = yaw_signal_get(signal_fence);
			if (!*running || input->notify_okay)
				break;
			yaw_signal_wait(signal_fence, status);
		}
	}
	else
	{
		mtask_deal_f deal;
		uintptr_t index;
		index = stack->context.pipe_index;
		if (index < input->transfer.process_number &&
			(deal = input->transfer.process[index].deal))
			deal(input->transfer.process[index].data, &stack->context);
		yaw_signal_inc_wake(signal_fence, ~0);
		index += 1;
		if (index < input->transfer.transfer_finish &&
			index < stack->context.mtask->pipe_number)
		{
			// transfer next pipe
			;
		}
	}
}

static void mtask_core_task_do(struct mtask_input_s *input, const struct mtask_core_stack_t *restrict stack)
{
	switch (input->type)
	{
		case mtask_type__task:
			mtask_core_task_do_task((struct mtask_input_task_s *) input, stack);
			break;
		case mtask_type__semaphore:
			mtask_core_task_do_semaphore((struct mtask_input_semaphore_s *) input, stack);
			break;
		case mtask_type__fence:
			mtask_core_task_do_fence((struct mtask_input_fence_s *) input, stack);
			break;
		default:
			break;
	}
	refer_free(input);
}

static void mtask_core_task(yaw_s *restrict yaw)
{
	struct mtask_core_s *restrict core;
	struct mtask_pipe_s *restrict pipe;
	struct mtask_s *restrict mtask;
	yaw_signal_s *restrict signal_pipe;
	queue_s *restrict queue_input;
	queue_s *restrict queue_interrupt;
	struct mtask_core_stack_t stack;
	struct mtask_input_s *input;
	uint32_t status;
	core = (struct mtask_core_s *) yaw->data;
	if (core && (mtask = core->context.mtask) &&
		core->context.pipe_index < mtask->pipe_number &&
		(pipe = mtask->pipe[core->context.pipe_index]))
	{
		signal_pipe = pipe->signal_pipe;
		queue_input = pipe->input;
		queue_interrupt = core->interrupt;
		stack.core = core;
		stack.pipe = pipe;
		stack.context = core->context;
		while (mtask->running)
		{
			status = yaw_signal_get(signal_pipe);
			do {
				while ((input = (struct mtask_input_s *) queue_interrupt->pull(queue_interrupt)))
					mtask_core_task_do(input, &stack);
				if ((input = (struct mtask_input_s *) queue_input->pull(queue_input)))
					mtask_core_task_do(input, &stack);
			} while (input);
			yaw_signal_wait(signal_pipe, status);
		}
	}
}

static void mtask_core_free_func(struct mtask_core_s *restrict r)
{
	if (r->context.mtask && r->context.mtask->running)
		r->context.mtask->running = 0;
	if (r->task)
	{
		yaw_wait(r->task);
		refer_free(r->task);
	}
	if (r->interrupt)
		refer_free(r->interrupt);
}

struct mtask_core_s* mtask_core_alloc(const struct mtask_context_t *restrict context, uintptr_t queue_interrupt_size)
{
	struct mtask_core_s *restrict r;
	if ((r = (struct mtask_core_s *) refer_alloz(sizeof(struct mtask_core_s))))
	{
		refer_set_free(r, (refer_free_f) mtask_core_free_func);
		r->context = *context;
		r->interrupt = queue_alloc_ring(queue_interrupt_size);
		r->task = yaw_alloc(mtask_core_task, NULL);
		if (r->interrupt && r->task && yaw_start(r->task, r))
			return r;
		refer_free(r);
	}
	return NULL;
}
