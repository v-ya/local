#include "mtask.inner.h"

static void mtask_pipe_free_func(struct mtask_pipe_s *restrict r)
{
	uintptr_t i, n;
	if (r->running)
		*r->running = 0;
	if (r->signal_pipe)
		yaw_signal_inc_wake(r->signal_pipe, ~0);
	if (r->signal_fence)
		yaw_signal_inc_wake(r->signal_fence, ~0);
	for (i = 0, n = r->core_number; i < n; ++i)
	{
		if (r->core[i])
			refer_free(r->core[i]);
	}
	if (r->input) refer_free(r->input);
	if (r->signal_pipe) refer_free(r->signal_pipe);
	if (r->signal_fence) refer_free(r->signal_fence);
	if (r->mutex_fence) refer_free(r->mutex_fence);
}

struct mtask_pipe_s* mtask_pipe_alloc(struct mtask_inst_s *restrict mtask, uintptr_t pipe_index, const struct mtask_param_pipe_t *restrict param)
{
	struct mtask_pipe_s *restrict r;
	struct mtask_context_t context;
	if ((r = (struct mtask_pipe_s *) refer_alloz(sizeof(struct mtask_pipe_s) +
		sizeof(struct mtask_core_s *) * param->core_number)))
	{
		refer_set_free(r, (refer_free_f) mtask_pipe_free_func);
		r->running = &mtask->running;
		r->pipe_index = pipe_index;
		r->input = queue_alloc_ring(param->queue_input_size);
		r->signal_pipe = yaw_signal_alloc();
		r->signal_fence = yaw_signal_alloc();
		r->mutex_fence = yaw_lock_alloc_mutex();
		r->friendly = param->friendly;
		r->core_number = param->core_number;
		if (r->input && r->signal_pipe && r->signal_fence && r->mutex_fence)
		{
			context.mtask = mtask;
			context.pipe_index = pipe_index;
			for (context.core_index = 0; context.core_index < param->core_number; ++context.core_index)
			{
				if (!(r->core[context.core_index] = mtask_core_alloc(&context, param->queue_interrupt_size)))
					goto label_fail;
			}
			return r;
		}
		label_fail:
		refer_free(r);
	}
	return NULL;
}

struct mtask_pipe_s* mtask_pipe_start(struct mtask_pipe_s *restrict pipe)
{
	uintptr_t i, n;
	for (i = 0, n = pipe->core_number; i < n; ++i)
	{
		if (!mtask_core_start(pipe->core[i]))
			goto label_fail;
	}
	return pipe;
	label_fail:
	return NULL;
}
