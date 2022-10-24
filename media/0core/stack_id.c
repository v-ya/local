#include "stack_id.h"
#include <memory.h>

static void media_stack_id_free_func(struct media_stack_id_s *restrict r)
{
	if (r->name) refer_free(r->name);
}

struct media_stack_id_s* media_stack_id_alloc(media_stack_layout_initial_f initial)
{
	struct media_stack_id_s *restrict r;
	if (initial && (r = (struct media_stack_id_s *) refer_alloz(sizeof(struct media_stack_id_s))))
	{
		refer_set_free(r, (refer_free_f) media_stack_id_free_func);
		if ((r->name = refer_dump_string(initial(&r->param))))
			return r;
		refer_free(r);
	}
	return NULL;
}

// layout ...

#define d_layout(_name, _align, _copy, _clear)  \
	const char* media_stack_layout_initial__##_name(struct media_stack_param_t *restrict param)\
	{\
		param->stack_size = sizeof(struct media_stack__##_name##_t);\
		param->stack_align = _align;\
		param->copy = _copy;\
		param->clear = _clear;\
		param->layout_magic = (uintptr_t) media_stack_layout__##_name;\
		return media_sl_##_name;\
	}
#define d_copy(_name)   static struct media_stack__##_name##_t* media_stack_layout_copy__##_name(struct media_stack__##_name##_t *restrict p)
#define d_clear(_name)  static void media_stack_layout_clear__##_name(struct media_stack__##_name##_t *restrict p)
#define q_copy(_name)   ((media_stack_copy_f) media_stack_layout_copy__##_name)
#define q_clear(_name)  ((media_stack_clear_f) media_stack_layout_clear__##_name)

d_layout(oz, 16, NULL, NULL)
