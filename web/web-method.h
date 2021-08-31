#ifndef _web_method_h_
#define _web_method_h_

#include <refer.h>

typedef enum web_method_id_t {
	web_method_id_get,
	web_method_id_head,
	web_method_id_post,
	web_method_id_put,
	web_method_id_delete,
	web_method_id_trace,
	web_method_id_options,
	web_method_id_connect,
	web_method_id_custom
} web_method_id_t;

typedef struct web_method_s web_method_s;

web_method_s* web_method_alloc(void);

refer_nstring_t web_method_get(const web_method_s *restrict r, web_method_id_t mid);

refer_nstring_t web_method_set(web_method_s *restrict r, const char *restrict method, web_method_id_t *restrict mid);

#endif
