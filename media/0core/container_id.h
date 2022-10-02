#ifndef _media_core_container_id_h_
#define _media_core_container_id_h_

#include <refer.h>
#include "attr.h"

struct media_container_id_s;
struct media_container_s;

typedef struct media_attr_judge_s* (*media_container_initial_judge_f)(struct media_attr_judge_s *restrict judge);
typedef refer_t (*media_container_create_pri_f)(const struct media_container_id_s *restrict id);
typedef struct media_container_s* (*media_container_parse_head_f)(struct media_container_s *restrict c);
typedef struct media_container_s* (*media_container_parse_tail_f)(struct media_container_s *restrict c);
typedef struct media_container_s* (*media_container_build_head_f)(struct media_container_s *restrict c);
typedef struct media_container_s* (*media_container_build_tail_f)(struct media_container_s *restrict c);

struct media_container_id_func_t {
	media_container_initial_judge_f initial_judge;
	media_container_create_pri_f create_pri;
	media_container_parse_head_f parse_head;
	media_container_parse_tail_f parse_tail;
	media_container_build_head_f build_head;
	media_container_build_tail_f build_tail;
};

struct media_container_id_s {
	refer_string_t name;
	const struct media_attr_judge_s *judge;
	struct media_container_id_func_t func;
};

void media_container_id_free_func(struct media_container_id_s *restrict r);
struct media_container_id_s* media_container_id_alloc(uintptr_t size, const char *restrict name, const struct media_container_id_func_t *restrict func);

#define media_container_symbol(_tf, _type, _name)            media_container__##_tf##__##_type##__##_name

#define d_media_container__initial_judge(_type, _name)       struct media_attr_judge_s* media_container_symbol(initial_judge, _type, _name)(struct media_attr_judge_s *restrict judge)
#define d_media_container__create_pri(_type, _name)          refer_t media_container_symbol(create_pri, _type, _name)(const struct media_container_id_s *restrict id)
#define d_media_container__parse_head(_type, _name)          struct media_container_s* media_container_symbol(parse_head, _type, _name)(struct media_container_s *restrict c)
#define d_media_container__parse_tail(_type, _name)          struct media_container_s* media_container_symbol(parse_tail, _type, _name)(struct media_container_s *restrict c)
#define d_media_container__build_head(_type, _name)          struct media_container_s* media_container_symbol(build_head, _type, _name)(struct media_container_s *restrict c)
#define d_media_container__build_tail(_type, _name)          struct media_container_s* media_container_symbol(build_tail, _type, _name)(struct media_container_s *restrict c)

#define d_media_container_func_initial(_func, _type, _name)  \
	_func.initial_judge = media_container_symbol(initial_judge, _type, _name);\
	_func.create_pri = media_container_symbol(create_pri, _type, _name);\
	_func.parse_head = media_container_symbol(parse_head, _type, _name);\
	_func.parse_tail = media_container_symbol(parse_tail, _type, _name);\
	_func.build_head = media_container_symbol(build_head, _type, _name);\
	_func.build_tail = media_container_symbol(build_tail, _type, _name)

#endif
