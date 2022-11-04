#ifndef _media_core_runtime_h_
#define _media_core_runtime_h_

#include "../0bits/atomic.h"
#include "media.h"

enum media_runtime_status_t {
	media_runtime_status__wait,
	media_runtime_status__running,
	media_runtime_status__cancel,
	media_runtime_status__done,
};

enum media_runtime_result_t {
	media_runtime_result__none,
	media_runtime_result__fail,
	media_runtime_result__okay,
};

struct media_runtime_s;
struct media_runtime_task_s;
struct media_runtime_unit_context_s;

typedef void (*media_runtime_done_f)(struct media_runtime_task_s *restrict task, refer_t pri);
typedef struct media_runtime_task_s* (*media_runtime_emit_f)(struct media_runtime_task_s *restrict task, refer_t pri, struct media_runtime_s *restrict rt, struct media_runtime_unit_context_s *restrict uc);
typedef void* (*media_runtime_deal_f)(refer_t inst, refer_t data, const void *restrict param);

struct media_runtime_unit_param_t {
	struct media_runtime_s *runtime;
	struct media_runtime_unit_context_s *context;
	media_runtime_deal_f deal;
	uintptr_t param_size;
	refer_t inst;
};

struct media_runtime_task_done_t {
	media_runtime_done_f done;
	refer_t pri;
};

struct media_runtime_task_step_t {
	media_runtime_emit_f emit;
	refer_t pri;
};

struct media_runtime_s* media_runtime_alloc(uintptr_t unit_core_number, uintptr_t task_queue_limit, uintptr_t friendly);
void media_runtime_stop(struct media_runtime_s *restrict runtime);
uintptr_t media_runtime_unit_core_number(struct media_runtime_s *restrict runtime);

struct media_runtime_s* media_runtime_post_unit(const struct media_runtime_unit_param_t *restrict up, refer_t data, const void *restrict param);

struct media_runtime_task_s* media_runtime_alloc_task(struct media_runtime_s *restrict runtime, uintptr_t step_number, const struct media_runtime_task_step_t *restrict steps, const struct media_runtime_task_done_t *restrict done);
struct media_runtime_task_s* media_runtime_task_cancel(struct media_runtime_task_s *restrict task);
const struct media_runtime_task_s* media_runtime_task_is_finish(const struct media_runtime_task_s *restrict task);
const struct media_runtime_task_s* media_runtime_task_is_okay(const struct media_runtime_task_s *restrict task);
struct media_runtime_task_s* media_runtime_task_wait_time(struct media_runtime_task_s *restrict task, uintptr_t usec);
void media_runtime_task_wait(struct media_runtime_task_s *restrict task);
enum media_runtime_status_t media_runtime_get_status(const struct media_runtime_task_s *restrict task);
enum media_runtime_result_t media_runtime_get_result(const struct media_runtime_task_s *restrict task);

#endif
