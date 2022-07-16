#include "mtask.inner.h"

static struct mtask_param_inst_t* mtask_inst_test_param(struct mtask_param_inst_t *restrict param)
{
	const struct mtask_param_pipe_t *restrict p;
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

static void mtask_inst_free_func(struct mtask_inst_s *restrict r)
{
	uintptr_t i, n;
	r->running = 0;
	for (i = 0, n = r->pipe_number; i < n; ++i)
	{
		if (r->pipe[i])
			refer_free(r->pipe[i]);
	}
}

mtask_inst_s* mtask_inst_alloc(struct mtask_param_inst_t *restrict param)
{
	struct mtask_inst_s *restrict r;
	uintptr_t i, n;
	if (mtask_inst_test_param(param) && (r = (struct mtask_inst_s *) refer_alloz(
		sizeof(struct mtask_inst_s) + sizeof(struct mtask_pipe_s *) * param->pipe_number)))
	{
		refer_set_free(r, (refer_free_f) mtask_inst_free_func);
		r->running = 1;
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
