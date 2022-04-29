#ifndef _vkaa_std_h_
#define _vkaa_std_h_

#include "vkaa.h"

#define vkaa_std_oplevel_comma     "comma"
#define vkaa_std_oplevel_assign    "assign"
#define vkaa_std_oplevel_condition "condition"
#define vkaa_std_oplevel_relation  "relation"
#define vkaa_std_oplevel_logic     "logic"
#define vkaa_std_oplevel_bitwise   "bitwise"
#define vkaa_std_oplevel_arith_1   "arith.1"
#define vkaa_std_oplevel_arith_2   "arith.2"
#define vkaa_std_oplevel_arith_3   "arith.3"
#define vkaa_std_oplevel_unary     "unary"
#define vkaa_std_oplevel_inquiry   "inquiry"
#define vkaa_std_oplevel_brackets  "brackets"

typedef struct vkaa_std_typeid_t {
	uintptr_t id_void;
} vkaa_std_typeid_t;

typedef struct vkaa_std_s {
	vkaa_oplevel_s *oplevel;
	vkaa_tpool_s *tpool;
	vkaa_parse_s *parse;
	vkaa_std_typeid_t typeid;
} vkaa_std_s;

vkaa_std_s* vkaa_std_alloc(void);

#endif
