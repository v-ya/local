#ifndef _udns_inner_type_func_
#define _udns_inner_type_func_

#include <stdint.h>
#include <stddef.h>

struct udns_type_arg_stack_t;

#define inner_type_initial(_name)       void udns_func_type_##_name##_initial(struct udns_type_arg_stack_t *restrict arg)
#define inner_type_finaly(_name)        void udns_func_type_##_name##_finaly(struct udns_type_arg_stack_t *restrict arg)
#define inner_type_parse_length(_name)  uintptr_t udns_func_type_##_name##_pl(const uint8_t *restrict data, uintptr_t length, struct udns_type_arg_stack_t *restrict arg)
#define inner_type_build_length(_name)  uintptr_t udns_func_type_##_name##_bl(const char *parse, struct udns_type_arg_stack_t *restrict arg)
#define inner_type_parse_write(_name)   struct udns_type_arg_stack_t* udns_func_type_##_name##_pw(char *restrict parse, struct udns_type_arg_stack_t *restrict arg)
#define inner_type_build_write(_name)   struct udns_type_arg_stack_t* udns_func_type_##_name##_bw(uint8_t *restrict data, struct udns_type_arg_stack_t *restrict arg)

inner_type_initial(a);
#define udns_func_type_a_finaly NULL
inner_type_parse_length(a);
inner_type_build_length(a);
inner_type_parse_write(a);
inner_type_build_write(a);

#endif
