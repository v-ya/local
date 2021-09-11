#ifndef _udns_inner_type_func_
#define _udns_inner_type_func_

#include <stdint.h>
#include <stddef.h>

struct udns_type_arg_stack_t;

#define inner_type_initial(_name)       void udns_func_type_##_name##_initial(struct udns_type_arg_stack_t *restrict arg)
#define inner_type_finaly(_name)        void udns_func_type_##_name##_finaly(struct udns_type_arg_stack_t *restrict arg)
#define inner_type_parse_length(_name)  uintptr_t udns_func_type_##_name##_pl(struct udns_type_arg_stack_t *restrict arg, const uint8_t *restrict data, uintptr_t size, uintptr_t pos)
#define inner_type_build_length(_name)  uintptr_t udns_func_type_##_name##_bl(struct udns_type_arg_stack_t *restrict arg, const char *parse)
#define inner_type_parse_write(_name)   struct udns_type_arg_stack_t* udns_func_type_##_name##_pw(struct udns_type_arg_stack_t *restrict arg, char *restrict parse)
#define inner_type_build_write(_name)   struct udns_type_arg_stack_t* udns_func_type_##_name##_bw(struct udns_type_arg_stack_t *restrict arg, uint8_t *restrict data)

// domain
inner_type_initial(domain);
#define udns_func_type_domain_finaly NULL
inner_type_parse_length(domain);
inner_type_build_length(domain);
inner_type_parse_write(domain);
inner_type_build_write(domain);

// a
inner_type_initial(a);
#define udns_func_type_a_finaly NULL
inner_type_parse_length(a);
inner_type_build_length(a);
inner_type_parse_write(a);
inner_type_build_write(a);

// ns
#define udns_func_type_ns_initial udns_func_type_domain_initial
#define udns_func_type_ns_finaly udns_func_type_domain_finaly
#define udns_func_type_ns_pl udns_func_type_domain_pl
#define udns_func_type_ns_bl udns_func_type_domain_bl
#define udns_func_type_ns_pw udns_func_type_domain_pw
#define udns_func_type_ns_bw udns_func_type_domain_bw

// cname
#define udns_func_type_cname_initial udns_func_type_domain_initial
#define udns_func_type_cname_finaly udns_func_type_domain_finaly
#define udns_func_type_cname_pl udns_func_type_domain_pl
#define udns_func_type_cname_bl udns_func_type_domain_bl
#define udns_func_type_cname_pw udns_func_type_domain_pw
#define udns_func_type_cname_bw udns_func_type_domain_bw

// aaaa
inner_type_initial(aaaa);
#define udns_func_type_aaaa_finaly NULL
inner_type_parse_length(aaaa);
inner_type_build_length(aaaa);
inner_type_parse_write(aaaa);
inner_type_build_write(aaaa);

#endif
