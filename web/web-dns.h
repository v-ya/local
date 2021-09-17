#ifndef _web_dns_h_
#define _web_dns_h_

#include <refer.h>

typedef struct web_dns_s web_dns_s;

web_dns_s* web_dns_alloc(const char *restrict target);
void web_dns_attr_set_running(web_dns_s *restrict dns, const volatile uintptr_t *runing);
void web_dns_attr_set_timeout(web_dns_s *restrict dns, uintptr_t ntry, uint64_t timeout_ms);
web_dns_s* web_dns_set_ipv4(web_dns_s *restrict dns, const char *restrict domain, const char *restrict ipv4);
web_dns_s* web_dns_set_ipv6(web_dns_s *restrict dns, const char *restrict domain, const char *restrict ipv6);
refer_nstring_t web_dns_get_ipv4(web_dns_s *restrict dns, const char *restrict domain);
refer_nstring_t web_dns_get_ipv6(web_dns_s *restrict dns, const char *restrict domain);

#endif
