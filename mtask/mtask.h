#ifndef _mtask_h_
#define _mtask_h_

#include <refer.h>

typedef struct mtask_s mtask_s;
typedef struct mtask_timer_s mtask_timer_s;
typedef const struct mtask_context_t mtask_context_t;

typedef void (*mtask_deal_f)(refer_t data, mtask_context_t *restrict c);

struct mtask_context_t {
	mtask_s *mtask;
	uintptr_t pipe_index;
	uintptr_t core_index;
};

#endif
