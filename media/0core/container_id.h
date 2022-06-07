#ifndef _media_core_container_id_h_
#define _media_core_container_id_h_

#include <refer.h>
#include "attr.h"

typedef refer_t (*media_container_create_pri_f)(void);

struct media_container_id_s {
	refer_string_t name;
	const struct media_attr_judge_s *judge;
	media_container_create_pri_f create;
};

#endif
