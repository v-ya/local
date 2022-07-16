#include "mtask.inner.h"

static mtask_param_inst_t* mtask_inst_test_param(mtask_param_inst_t *restrict param)
{
	const mtask_param_pipe_t *restrict p;
	uintptr_t i, n, cn;
	if ((n = param->pipe_number) && (p = param->pipe_param))
	{
		for (i = 0; i < n; ++i)
		{
			if (!(cn = p[i].core_number))
				goto label_fail;
			if (p[i].queue_input_size < cn)
				goto label_fail;
			if (p[i].queue_interrupt_size < cn)
				goto label_fail;
		}
		return param;
	}
	label_fail:
	return NULL;
}

static void mtask_inst_free_func(mtask_inst_s *restrict r)
{
	uintptr_t i, n;
	r->running = 0;
	for (i = 0, n = r->pipe_number; i < n; ++i)
	{
		if (r->pipe[i])
			refer_free(r->pipe[i]);
	}
}

mtask_inst_s* mtask_inst_alloc(mtask_param_inst_t *restrict param)
{
	mtask_inst_s *restrict r;
	uintptr_t i, n;
	if (mtask_inst_test_param(param) && (r = (mtask_inst_s *) refer_alloz(
		sizeof(mtask_inst_s) + sizeof(struct mtask_pipe_s *) * param->pipe_number)))
	{
		refer_set_free(r, (refer_free_f) mtask_inst_free_func);
		r->running = 1;
		if (param->task_cache_number)
		{
			if (!(r->cache_task = queue_alloc_ring(param->task_cache_number)))
				goto label_fail;
		}
		r->pipe_number = param->pipe_number;
		for (i = 0, n = param->pipe_number; i < n; ++i)
		{
			if (!(r->pipe[i] = mtask_pipe_alloc(r, i, param->pipe_param + i)))
				goto label_fail;
		}
		return r;
		label_fail:
		refer_free(r);
	}
	return NULL;
}

mtask_inst_s* mtask_push_task_active(mtask_inst_s *mtask, uintptr_t pipe_index)
{
	yaw_signal_s *restrict signal_pipe;
	if (pipe_index < mtask->pipe_number)
	{
		signal_pipe = mtask->pipe[pipe_index]->signal_pipe;
		yaw_signal_inc_wake(signal_pipe, ~0);
		return mtask;
	}
	return NULL;
}

mtask_inst_s* mtask_push_task_nonblock(mtask_inst_s *mtask, uintptr_t pipe_index, mtask_deal_f deal_func, refer_t deal_data)
{
	struct mtask_input_task_s *restrict task;
	queue_s *restrict queue;
	if (pipe_index < mtask->pipe_number &&
		(task = mtask_inner_input_need_task(mtask, deal_func, deal_data)))
	{
		queue = mtask->pipe[pipe_index]->input;
		if (queue->push(queue, task))
		{
			refer_free(task);
			return mtask;
		}
		refer_free(task);
	}
	return NULL;
}

mtask_inst_s* mtask_push_task_block(mtask_inst_s *mtask, uintptr_t pipe_index, mtask_deal_f deal_func, refer_t deal_data)
{
	struct mtask_input_task_s *restrict task;
	struct mtask_pipe_s *restrict pipe;
	if (pipe_index < mtask->pipe_number &&
		(task = mtask_inner_input_need_task(mtask, deal_func, deal_data)))
	{
		pipe = mtask->pipe[pipe_index];
		if (mtask_inner_queue_must_push(pipe->input, task, pipe->signal_pipe, &mtask->running))
		{
			refer_free(task);
			return mtask;
		}
		refer_free(task);
	}
	return NULL;
}
