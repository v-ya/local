#ifndef _media_core_container_id_h_
#define _media_core_container_id_h_

#include <refer.h>
#include "attr.h"

typedef struct media_attr_judge_s* (*media_container_initial_judge_f)(struct media_attr_judge_s *restrict judge);

typedef refer_t (*media_container_create_pri_f)(void);

struct media_container_id_func_t {
	media_container_create_pri_f create;
};

struct media_container_id_s {
	refer_string_t name;
	const struct media_attr_judge_s *judge;
	struct media_container_id_func_t func;
};

struct media_container_id_s* media_container_id_alloc(const char *restrict name, media_container_initial_judge_f initial, const struct media_container_id_func_t *restrict func);

#endif
