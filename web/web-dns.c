#include "web-dns.h"
#include <hashmap.h>
#include <exbuffer.h>
#include <udns.h>
#include <transport/tcp.h>
#include <stdlib.h>
#include <arpa/inet.h>

typedef struct web_dns_item_t {
	refer_nstring_t hit;
	udns_s *udns;
	uint64_t tkill;
} web_dns_item_t;

struct web_dns_s {
	refer_string_t target;
	udns_inst_s *inst;
	hashmap_t ipv4;
	hashmap_t ipv6;
	exbuffer_t buffer;
	const volatile uintptr_t *runing;
	uintptr_t ntry;
	uint64_t timeout;
};

static inline transport_s* web_dns_udns_request_tcp_connect_and_call(transport_s *restrict tcp, udns_s *restrict udns, exbuffer_t *restrict eb, const volatile uintptr_t *runing, uint64_t timeout)
{
	uint8_t *restrict data;
	uintptr_t size;
	uintptr_t rn;
	uint64_t tk, tn;
	transport_recv_attr_t attr = {
		.running = runing,
		.timeout_ms = 0,
		.recv_some = 1,
		.recv_full = 1
	};
	size = udns_build_length(udns);
	if (size && size < 0x10000 && (data = (uint8_t *) exbuffer_need(eb, size + 2)))
	{
		*(uint16_t *) data = htons((uint16_t) size);
		if (udns_build_write(udns, data + 2))
		{
			size += 2;
			tk = transport_timestamp_ms() + timeout;
			if (transport_tcp_wait_connect(tcp, timeout, runing) &&
				transport_send(tcp, data, size, &rn) && rn == size &&
				(tn = transport_timestamp_ms()) <= tk)
			{
				attr.timeout_ms = tk - tn;
				if ((data = (uint8_t *) exbuffer_need(eb, 2)) &&
					transport_recv(tcp, data, 2, NULL, &attr) &&
					(tn = transport_timestamp_ms()) <= tk)
				{
					attr.timeout_ms = tk - tn;
					size = (uintptr_t) ntohs(*(uint16_t *) data);
					if (size && (data = (uint8_t *) exbuffer_need(eb, size)) &&
						transport_recv(tcp, data, size, NULL, &attr) &&
						udns_parse(udns, data, size, NULL, udns_parse_flags_ignore_unknow))
						return tcp;
				}
			}
		}
	}
	return NULL;
}

static udns_s* web_dns_udns_request_tcp(web_dns_s *restrict inst, udns_s *restrict udns, const char *restrict domain, udns_type_t type)
{
	transport_s *restrict tcp;
	uintptr_t ntry;
	ntry = inst->ntry;
	while (ntry)
	{
		udns_clear(udns);
		udns_set_rd(udns, 1);
		if (!udns_add_question_info(udns, domain, type, udns_class_in))
			break;
		if (!(tcp = transport_tcp_alloc_ipv4_connect(inst->target, 53, NULL)))
			break;
		if (web_dns_udns_request_tcp_connect_and_call(tcp, udns, &inst->buffer, inst->runing, inst->timeout))
			goto label_okay;
		refer_free(tcp);
		--ntry;
	}
	return NULL;
	label_okay:
	refer_free(tcp);
	return udns;
}

static void web_dns_hashmap_free_func(hashmap_vlist_t *restrict vl)
{
	web_dns_item_t *restrict r;
	if ((r = (web_dns_item_t *) vl->value))
	{
		if (r->hit)
			refer_free(r->hit);
		if (r->udns)
			refer_free(r->udns);
		free(r);
	}
}

static void web_dns_free_func(web_dns_s *restrict r)
{
	if (r->target)
		refer_free(r->target);
	if (r->inst)
		refer_free(r->inst);
	hashmap_uini(&r->ipv4);
	hashmap_uini(&r->ipv6);
	exbuffer_uini(&r->buffer);
}

static refer_nstring_t web_dns_item_replace(web_dns_item_t *restrict item, const char *restrict hit, udns_s *restrict udns, uint64_t tkill)
{
	if (item->hit)
		refer_free(item->hit);
	if (item->udns)
		refer_free(item->udns);
	if (hit) item->hit = refer_dump_nstring(hit);
	else item->hit = NULL;
	item->udns = (udns_s *) refer_save(udns);
	item->tkill = tkill;
	return item->hit;
}

static web_dns_item_t* web_dns_hashmap_set(hashmap_t *restrict hm, const char *restrict domain, const char *restrict hit, udns_s *restrict udns, uint64_t tkill)
{
	web_dns_item_t *restrict item;
	if ((item = (web_dns_item_t *) hashmap_get_name(hm, domain)))
	{
		label_find:
		if (web_dns_item_replace(item, hit, udns, tkill))
			return item;
	}
	else if ((item = (web_dns_item_t *) calloc(1, sizeof(web_dns_item_t))))
	{
		if (hashmap_set_name(hm, domain, item, web_dns_hashmap_free_func))
			goto label_find;
		free(item);
	}
	return NULL;
}

static refer_nstring_t web_dns_hashmap_get(hashmap_t *restrict hm, web_dns_s *restrict inst, const char *restrict domain, udns_type_t type)
{
	web_dns_item_t *restrict item;
	udns_s *restrict udns;
	udns_resource_s *restrict resource;
	uint64_t tkill;
	tkill = transport_timestamp_sec();
	if ((item = (web_dns_item_t *) hashmap_get_name(hm, domain)) && tkill <= item->tkill)
	{
		label_okay:
		return item->hit;
	}
	if (inst->target)
	{
		if (item && item->udns)
			udns = (udns_s *) refer_save(item->udns);
		else udns = udns_alloc(inst->inst);
		if (udns)
		{
			if (web_dns_udns_request_tcp(inst, udns, domain, type) &&
				(resource = udns_get_answer(udns, type)) &&
				resource->data_parse)
			{
				tkill = transport_timestamp_sec() + resource->ttl;
				if (item && web_dns_item_replace(item, resource->data_parse, udns, tkill))
					goto label_okay_free_udns;
				else if (!item && (item = web_dns_hashmap_set(hm, domain, resource->data_parse, udns, tkill)))
				{
					label_okay_free_udns:
					refer_free(udns);
					goto label_okay;
				}
			}
			refer_free(udns);
		}
	}
	return NULL;
}

web_dns_s* web_dns_alloc(const char *restrict target)
{
	web_dns_s *restrict r;
	if ((r = (web_dns_s *) refer_alloz(sizeof(web_dns_s))))
	{
		refer_set_free(r, (refer_free_f) web_dns_free_func);
		if ((!target || (r->target = refer_dump_string(target))) &&
			(r->inst = udns_inst_alloc_mini()) &&
			hashmap_init(&r->ipv4) &&
			hashmap_init(&r->ipv6) &&
			exbuffer_init(&r->buffer, 0))
		{
			r->runing = NULL;
			r->ntry = 1;
			r->timeout = 500;
			return r;
		}
		refer_free(r);
	}
	return NULL;
}

void web_dns_attr_set_running(web_dns_s *restrict dns, const volatile uintptr_t *runing)
{
	dns->runing = runing;
}

void web_dns_attr_set_timeout(web_dns_s *restrict dns, uintptr_t ntry, uint64_t timeout_ms)
{
	if (!ntry)
		ntry = 1;
	if (!timeout_ms)
		timeout_ms = 500;
	dns->ntry = ntry;
	dns->timeout = timeout_ms;
}

web_dns_s* web_dns_set_ipv4(web_dns_s *restrict dns, const char *restrict domain, const char *restrict ipv4)
{
	if (web_dns_hashmap_set(&dns->ipv4, domain, ipv4, NULL, ~(uint64_t) 0))
		return dns;
	return NULL;
}

web_dns_s* web_dns_set_ipv6(web_dns_s *restrict dns, const char *restrict domain, const char *restrict ipv6)
{
	if (web_dns_hashmap_set(&dns->ipv6, domain, ipv6, NULL, ~(uint64_t) 0))
		return dns;
	return NULL;
}

refer_nstring_t web_dns_get_ipv4(web_dns_s *restrict dns, const char *restrict domain)
{
	return web_dns_hashmap_get(&dns->ipv4, dns, domain, udns_type_a);
}

refer_nstring_t web_dns_get_ipv6(web_dns_s *restrict dns, const char *restrict domain)
{
	return web_dns_hashmap_get(&dns->ipv6, dns, domain, udns_type_aaaa);
}
