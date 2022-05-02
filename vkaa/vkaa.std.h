#ifndef _vkaa_std_h_
#define _vkaa_std_h_

#include "vkaa.h"

typedef struct vkaa_std_typeid_t {
	uintptr_t id_void;
} vkaa_std_typeid_t;

typedef struct vkaa_std_s {
	vkaa_syntaxor_s *syntaxor;
	vkaa_oplevel_s *oplevel;
	vkaa_tpool_s *tpool;
	vkaa_parse_s *parse;
	vkaa_std_typeid_t typeid;
} vkaa_std_s;

vkaa_std_s* vkaa_std_alloc(void);

#endif
