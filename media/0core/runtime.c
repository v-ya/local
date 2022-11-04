#include "runtime.h"
#include <memory.h>
#include <queue/queue.h>
#include <hashmap.h>
#include <mtask.h>
#include <yaw.h>

// runtime

struct media_runtime_s {
	hashmap_t task_save;  // (struct media_runtime_task_s *)
	mtask_inst_s *mtask;
	yaw_signal_s *signal;
	queue_s *task_wait;   // (struct media_runtime_task_s *)
	queue_s *task_step;   // (struct media_runtime_task_s *)
	uintptr_t unit_core_number;
	yaw_s *daemon;
	media_atomic_ptr_t task_next_index;
};

// task

struct media_runtime_task_s {
	uintptr_t task_index;
	yaw_signal_s *signal;
	media_atomic_u32_t status;  // (enum media_runtime_status_t)
	media_atomic_u32_t result;  // (enum media_runtime_result_t)
	uintptr_t step_max;
	uintptr_t step_cur;
	struct media_runtime_task_done_t done;
	struct media_runtime_task_step_t step[];
};

#define m_task_status(_task, _from, _to)  media_atomic_u32_compare_swap(&(_task)->status, media_runtime_status__##_from, media_runtime_status__##_to)
#define m_task_result(_task, _from, _to)  media_atomic_u32_compare_swap(&(_task)->result, media_runtime_result__##_from, media_runtime_result__##_to)

// unit

struct media_runtime_unit_context_s {
	media_atomic_ptr_t unit_running_number;
	media_atomic_ptr_t unit_commit_number;
	struct media_runtime_task_s *task;
	yaw_signal_s *signal;
	queue_s *task_step;
};

struct media_runtime_unit_s {
	struct media_runtime_unit_context_s *context;
	media_runtime_deal_f deal;
	refer_t inst;
	refer_t data;
	uint8_t param[];
};

// queue

static queue_s* media_runtime_queue_push(queue_s *restrict q, yaw_signal_s *restrict s, refer_t v, const volatile uintptr_t *running)
{
	uint32_t status;
	if (!q->push(q, v))
	{
		for (;;)
		{
			status = yaw_signal_get(s);
			if (running && !*running)
				return NULL;
			if (q->push(q, v))
				break;
			yaw_signal_wait(s, status);
		}
	}
	yaw_signal_inc_wake(s, ~0);
	return q;
}

// task

static void media_runtime_task_free_func(struct media_runtime_task_s *restrict r)
{
	if (r->signal) refer_free(r->signal);
	if (r->done.pri) refer_free(r->done.pri);
	for (uintptr_t i = 0, n = r->step_max; i < n; ++i)
	{
		if (r->step[i].pri)
			refer_free(r->step[i].pri);
	}
}

static struct media_runtime_task_s* media_runtime_task_alloc(uintptr_t step_number, const struct media_runtime_task_step_t *restrict steps, const struct media_runtime_task_done_t *restrict done)
{
	struct media_runtime_task_s *restrict r;
	uintptr_t i;
	if (step_number && steps && (r = (struct media_runtime_task_s *) refer_alloz(
		sizeof(struct media_runtime_task_s) + sizeof(struct media_runtime_task_step_t) * step_number)))
	{
		refer_set_free(r, (refer_free_f) media_runtime_task_free_func);
		if ((r->signal = yaw_signal_alloc()))
		{
			yaw_signal_set(r->signal, 0);
			r->step_max = step_number;
			if (done)
			{
				r->done.done = done->done;
				r->done.pri = refer_save(done->pri);
			}
			for (i = 0; i < step_number; ++i)
			{
				if (!(r->step[i].emit = steps[i].emit))
					goto label_fail;
				r->step[i].pri = refer_save(steps[i].pri);
			}
			return r;
		}
		label_fail:
		refer_free(r);
	}
	return NULL;
}

static void media_runtime_task_notify(struct media_runtime_task_s *restrict task)
{
	media_runtime_done_f done;
	yaw_signal_s *restrict s;
	if ((done = task->done.done))
	{
		done(task, task->done.pri);
		task->done.done = NULL;
	}
	s = task->signal;
	yaw_signal_set(s, 1);
	yaw_signal_wake(s, ~0);
}

// unit

static void media_runtime_unit_context_free_func(struct media_runtime_unit_context_s *restrict r)
{
	if (r->task) refer_free(r->task);
	if (r->signal) refer_free(r->signal);
	if (r->task_step) refer_free(r->task_step);
}

static struct media_runtime_unit_context_s* media_runtime_unit_context_alloc(struct media_runtime_task_s *restrict task, yaw_signal_s *restrict signal, queue_s *restrict task_step)
{
	struct media_runtime_unit_context_s *restrict r;
	if ((r = (struct media_runtime_unit_context_s *) refer_alloz(sizeof(struct media_runtime_unit_context_s))))
	{
		refer_set_free(r, (refer_free_f) media_runtime_unit_context_free_func);
		r->task = (struct media_runtime_task_s *) refer_save(task);
		r->signal = (yaw_signal_s *) refer_save(signal);
		r->task_step = (queue_s *) refer_save(task_step);
	}
	return r;
}

static void media_runtime_unit_free_func(struct media_runtime_unit_s *restrict r)
{
	if (r->context) refer_free(r->context);
	if (r->inst) refer_free(r->inst);
	if (r->data) refer_free(r->data);
}

static struct media_runtime_unit_s* media_runtime_unit_alloc(const struct media_runtime_unit_param_t *restrict up, refer_t data, const void *restrict param)
{
	struct media_runtime_unit_s *restrict r;
	if (up->context && up->deal && (r = (struct media_runtime_unit_s *) refer_alloc(
		sizeof(struct media_runtime_unit_s) + up->param_size)))
	{
		r->context = (struct media_runtime_unit_context_s *) refer_save(up->context);
		r->deal = up->deal;
		r->inst = refer_save(up->inst);
		r->data = refer_save(data);
		memcpy(r->param, param, up->param_size);
		refer_set_free(r, (refer_free_f) media_runtime_unit_free_func);
		return r;
	}
	return NULL;
}

static void media_runtime_unit_deal(struct media_runtime_unit_s *restrict unit, mtask_context_t *restrict c)
{
	struct media_runtime_unit_context_s *restrict uc;
	uc = unit->context;
	if (!unit->deal(unit->inst, unit->data, unit->param))
	{
		m_task_result(uc->task, none, fail);
		m_task_status(uc->task, running, cancel);
	}
	if (!media_atomic_ptr_sub_fetch(&uc->unit_running_number, 1))
		media_runtime_queue_push(uc->task_step, uc->signal, uc->task, NULL);
}

// daemon

static void media_runtime_daemon_emit(struct media_runtime_s *restrict rt, struct media_runtime_task_s *restrict task)
{
	struct media_runtime_task_step_t *restrict step;
	struct media_runtime_unit_context_s *restrict uc;
	uintptr_t unit_commit_number;
	uint32_t status;
	label_next_step:
	if ((status = task->status) == media_runtime_status__running &&
		task->step_cur < task->step_max)
	{
		step = task->step + task->step_cur;
		if ((uc = media_runtime_unit_context_alloc(task, rt->signal, rt->task_step)))
		{
			if (!step->emit(task, step->pri, rt, uc))
			{
				m_task_result(task, none, fail);
				m_task_status(task, running, cancel);
			}
			unit_commit_number = uc->unit_commit_number;
			refer_free(uc);
			if (!unit_commit_number)
			{
				task->step_cur += 1;
				goto label_next_step;
			}
		}
	}
	else
	{
		if (status == media_runtime_status__running)
		{
			m_task_result(task, none, okay);
			m_task_status(task, running, done);
		}
		media_runtime_task_notify(task);
		hashmap_delete_head(&rt->task_save, (uint64_t) task->task_index);
	}
}

static void media_runtime_hashmap_free_func(hashmap_vlist_t *restrict vl)
{
	if (vl->value) refer_free(vl->value);
}

static void media_runtime_daemon(yaw_s *restrict yaw)
{
	struct media_runtime_s *restrict r;
	yaw_signal_s *restrict s;
	queue_s *restrict qw, *restrict qs;
	struct media_runtime_task_s *restrict task;
	uint32_t ss;
	r = (struct media_runtime_s *) yaw->data;
	s = r->signal;
	qw = r->task_wait;
	qs = r->task_step;
	// running
	while (yaw->running)
	{
		ss = yaw_signal_get(s);
		while ((task = (struct media_runtime_task_s *) qw->pull(qw)))
		{
			do {
				task->task_index = media_atomic_ptr_fetch_add(&r->task_next_index, 1);
			} while (hashmap_find_head(&r->task_save, (uint64_t) task->task_index));
			if (hashmap_set_head(&r->task_save, (uint64_t) task->task_index, task, media_runtime_hashmap_free_func))
			{
				m_task_status(task, wait, running);
				media_runtime_daemon_emit(r, task);
			}
			else
			{
				m_task_result(task, none, fail);
				m_task_status(task, wait, cancel);
				media_runtime_task_notify(task);
				refer_free(task);
			}
		}
		while ((task = (struct media_runtime_task_s *) qs->pull(qs)))
		{
			task->step_cur += 1;
			media_runtime_daemon_emit(r, task);
			refer_free(task);
		}
		yaw_signal_wait(s, ss);
	}
	// (stop && wait) mtask
	refer_free(r->mtask);
	r->mtask = NULL;
}

// runtime

static int media_runtime_clear_task_save_func(hashmap_vlist_t *restrict vl, void *p)
{
	struct media_runtime_task_s *restrict task;
	if ((task = (struct media_runtime_task_s *) vl->value))
	{
		m_task_status(task, running, cancel);
		media_runtime_task_notify(task);
	}
	return 1;
}

static void media_runtime_clear_task_wait(queue_s *restrict task_wait)
{
	struct media_runtime_task_s *restrict task;
	while ((task = (struct media_runtime_task_s *) task_wait->pull(task_wait)))
	{
		m_task_status(task, wait, cancel);
		media_runtime_task_notify(task);
		refer_free(task);
	}
}

static void media_runtime_free_func(struct media_runtime_s *restrict r)
{
	if (r->daemon)
	{
		yaw_stop(r->daemon);
		yaw_signal_inc_wake(r->signal, ~0);
		yaw_wait(r->daemon);
		refer_free(r->daemon);
		hashmap_isfree(&r->task_save, media_runtime_clear_task_save_func, NULL);
	}
	if (r->mtask) refer_free(r->mtask);
	media_runtime_clear_task_wait(r->task_wait);
	if (r->signal) refer_free(r->signal);
	if (r->task_wait) refer_free(r->task_wait);
	if (r->task_step) refer_free(r->task_step);
	hashmap_uini(&r->task_save);
}

struct media_runtime_s* media_runtime_alloc(uintptr_t unit_core_number, uintptr_t task_queue_limit, uintptr_t friendly)
{
	struct media_runtime_s *restrict r;
	mtask_param_inst_t mp;
	mtask_param_pipe_t mpp;
	if (!unit_core_number) unit_core_number = 1;
	if (!task_queue_limit) task_queue_limit = 1024;
	if (task_queue_limit < unit_core_number * 2)
		task_queue_limit = unit_core_number * 2;
	mp.task_cache_number = task_queue_limit * 2;
	mp.pipe_number = 1;
	mp.pipe_param = &mpp;
	mpp.core_number = unit_core_number;
	mpp.queue_input_size = task_queue_limit;
	mpp.queue_interrupt_size = unit_core_number * 16;
	mpp.friendly = !!friendly;
	if ((r = (struct media_runtime_s *) refer_alloz(sizeof(struct media_runtime_s))))
	{
		refer_set_free(r, (refer_free_f) media_runtime_free_func);
		r->unit_core_number = unit_core_number;
		if (hashmap_init(&r->task_save) &&
			(r->mtask = mtask_inst_alloc(&mp)) &&
			(r->signal = yaw_signal_alloc()) &&
			(r->task_wait = queue_alloc_ring(task_queue_limit)) &&
			(r->task_step = queue_alloc_ring(task_queue_limit)) &&
			(r->daemon = yaw_alloc_and_start(media_runtime_daemon, NULL, r))
		) return r;
		refer_free(r);
	}
	return NULL;
}

void media_runtime_stop(struct media_runtime_s *restrict runtime)
{
	if (runtime->daemon)
	{
		yaw_stop(runtime->daemon);
		yaw_signal_inc_wake(runtime->signal, ~0);
	}
}

uintptr_t media_runtime_unit_core_number(struct media_runtime_s *restrict runtime)
{
	return runtime->unit_core_number;
}

struct media_runtime_s* media_runtime_post_unit(const struct media_runtime_unit_param_t *restrict up, refer_t data, const void *restrict param)
{
	struct media_runtime_s *restrict r;
	struct media_runtime_unit_context_s *restrict c;
	struct media_runtime_unit_s *restrict unit;
	if ((r = up->runtime) && (c = up->context) &&
		(unit = media_runtime_unit_alloc(up, data, param)))
	{
		media_atomic_ptr_add(&c->unit_running_number, 1);
		if (mtask_push_task_block(r->mtask, 0, (mtask_deal_f) media_runtime_unit_deal, unit))
			media_atomic_ptr_add(&c->unit_commit_number, 1);
		else
		{
			media_atomic_ptr_sub(&c->unit_running_number, 1);
			r = NULL;
		}
		refer_free(unit);
		return r;
	}
	return NULL;
}

struct media_runtime_task_s* media_runtime_alloc_task(struct media_runtime_s *restrict runtime, uintptr_t step_number, const struct media_runtime_task_step_t *restrict steps, const struct media_runtime_task_done_t *restrict done)
{
	struct media_runtime_task_s *restrict task;
	if (runtime->daemon && runtime->daemon->running &&
		(task = media_runtime_task_alloc(step_number, steps, done)))
	{
		if (media_runtime_queue_push(runtime->task_wait, runtime->signal, task, &runtime->daemon->running))
			return task;
		refer_free(task);
	}
	return NULL;
}

struct media_runtime_task_s* media_runtime_task_cancel(struct media_runtime_task_s *restrict task)
{
	if (m_task_status(task, wait, cancel) || m_task_status(task, running, cancel))
		return task;
	return NULL;
}

struct media_runtime_task_s* media_runtime_task_is_finish(struct media_runtime_task_s *restrict task)
{
	yaw_signal_s *restrict s;
	s = task->signal;
	return yaw_signal_get(s)?task:NULL;
}

struct media_runtime_task_s* media_runtime_task_wait_time(struct media_runtime_task_s *restrict task, uintptr_t usec)
{
	yaw_signal_s *restrict s;
	s = task->signal;
	if (yaw_signal_get(s) || (yaw_signal_wait_time(s, 0, usec), yaw_signal_get(s)))
		return task;
	return NULL;
}

void media_runtime_task_wait(struct media_runtime_task_s *restrict task)
{
	yaw_signal_s *restrict s;
	s = task->signal;
	while (!yaw_signal_get(s))
		yaw_signal_wait(s, 0);
}

enum media_runtime_status_t media_runtime_get_status(struct media_runtime_task_s *restrict task)
{
	return (enum media_runtime_status_t) task->status;
}

enum media_runtime_result_t media_runtime_get_result(struct media_runtime_task_s *restrict task)
{
	return (enum media_runtime_result_t) task->result;
}
