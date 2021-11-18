#include "web-dns.h"
#include <hashmap.h>
#include <exbuffer.h>
#include <udns.h>
#include <transport/udp.h>
#include <queue/queue.h>
#include <yaw.h>
#include <stdlib.h>
#include <arpa/inet.h>

#define web_dns_time_gap_usec  50000

typedef struct web_dns_item_t {
	refer_nstring_t hit;
	uint64_t tkill;
} web_dns_item_t;

typedef struct web_dns_core_s {
	yaw_s *task;
	udns_inst_s *inst;
	transport_s *tp;
	yaw_lock_s *read;
	yaw_lock_s *write;
	yaw_signal_s *s_task;
	yaw_signal_s *s_okay;
	queue_s *q_task;
	queue_s *q_data;
	hashmap_t ipv4;
	hashmap_t ipv6;
} web_dns_core_s;

struct web_dns_s {
	refer_string_t target;
	udns_inst_s *inst;
	transport_s *tp;
	web_dns_core_s *core;
	const volatile uintptr_t *runing;
	uintptr_t gap;
	uintptr_t ntry;
	uint64_t timeout;
};

static inline void web_dns_signal_wake_all(yaw_signal_s *s)
{
	yaw_signal_inc(s);
	yaw_signal_wake(s, ~0);
}

static void web_dns_hashmap_free_func(hashmap_vlist_t *restrict vl)
{
	web_dns_item_t *restrict r;
	if ((r = (web_dns_item_t *) vl->value))
	{
		if (r->hit)
			refer_free(r->hit);
		free(r);
	}
}

static inline udns_s* web_dns_core_get_data(web_dns_core_s *restrict core)
{
	udns_s *restrict r;
	return (void) ((r = (udns_s *) core->q_data->pull(core->q_data)) || (r = udns_alloc(core->inst))), r;
}

static inline void web_dns_core_put_data(web_dns_core_s *restrict core, udns_s *restrict udns)
{
	udns_clear(udns);
	core->q_data->push(core->q_data, udns);
	refer_free(udns);
}

static udns_s* web_dns_core_get_question(web_dns_core_s *restrict core, const char *restrict domain, udns_type_t type)
{
	udns_s *restrict r;
	if ((r = web_dns_core_get_data(core)))
	{
		udns_set_rd(r, 1);
		if (udns_add_question_info(r, domain, type, udns_class_in))
			return r;
		web_dns_core_put_data(core, r);
	}
	return NULL;
}

static hashmap_t* web_dns_core_update_hashmap(hashmap_t *restrict h, const char *restrict domain, const char *restrict hit, uint64_t tkill)
{
	web_dns_item_t *restrict item;
	if ((item = (web_dns_item_t *) hashmap_get_name(h, domain)))
	{
		label_find:
		if (item->hit)
		{
			refer_free(item->hit);
			item->hit = NULL;
		}
		item->tkill = tkill;
		if ((item->hit = refer_dump_nstring(hit)))
			return h;
	}
	else if ((item = (web_dns_item_t *) calloc(1, sizeof(web_dns_item_t))))
	{
		if (hashmap_set_name(h, domain, item, web_dns_hashmap_free_func))
			goto label_find;
		free(item);
	}
	return NULL;
}

static web_dns_core_s* web_dns_core_update(web_dns_core_s *restrict core, udns_s *restrict udns)
{
	udns_question_s *restrict q;
	udns_resource_s *restrict r;
	hashmap_t *restrict h;
	if ((q = udns_get_question(udns, udns_type_a)))
	{
		r = udns_get_answer(udns, udns_type_a);
		h = &core->ipv4;
	}
	else if ((q = udns_get_question(udns, udns_type_aaaa)))
	{
		r = udns_get_answer(udns, udns_type_aaaa);
		h = &core->ipv6;
	}
	else goto label_fail;
	if (r)
	{
		yaw_lock_lock(core->write);
		h = web_dns_core_update_hashmap(h, q->name_string, r->data_parse, yaw_timestamp_sec() + (uint64_t) r->ttl);
		yaw_lock_unlock(core->write);
		if (h) return core;
	}
	label_fail:
	return NULL;
}

static web_dns_core_s* web_dns_core_push_task(web_dns_core_s *restrict core, const char *restrict domain, udns_type_t type)
{
	udns_s *restrict q;
	if ((q = web_dns_core_get_question(core, domain, type)))
	{
		if (core->q_task->push(core->q_task, q))
		{
			refer_free(q);
			web_dns_signal_wake_all(core->s_task);
			return core;
		}
		web_dns_core_put_data(core, q);
	}
	return NULL;
}

static void web_dns_core_task(yaw_s *task)
{
	web_dns_core_s *restrict core;
	udns_s *restrict d;
	uintptr_t length;
	uint32_t status, updated;
	uint8_t data[4096];
	core = (web_dns_core_s *) task->data;
	if (core->inst && core->tp)
	{
		while (task->running)
		{
			status = yaw_signal_get(core->s_task);
			while ((d = (udns_s *) core->q_task->pull(core->q_task)))
			{
				if ((length = udns_build_length(d)) <= sizeof(data) && udns_build_write(d, data))
					transport_send(core->tp, data, length, NULL, NULL);
				web_dns_core_put_data(core, d);
			}
			updated = 0;
			while (transport_recv(core->tp, data, sizeof(data), &length, NULL) && length)
			{
				if ((d = web_dns_core_get_data(core)))
				{
					if (udns_parse(d, data, length, NULL, udns_parse_flags_ignore_unknow) &&
						web_dns_core_update(core, d))
						updated = 1;
					web_dns_core_put_data(core, d);
				}
			}
			if (updated)
				web_dns_signal_wake_all(core->s_okay);
			yaw_signal_wait_time(core->s_task, status, web_dns_time_gap_usec);
		}
	}
}

static void web_dns_core_free_func(web_dns_core_s *restrict r)
{
	if (r->task)
	{
		yaw_stop(r->task);
		if (r->s_task)
			web_dns_signal_wake_all(r->s_task);
		yaw_wait(r->task);
		refer_free(r->task);
	}
	if (r->inst)
		refer_free(r->inst);
	if (r->tp)
		refer_free(r->tp);
	if (r->read)
		refer_free(r->read);
	if (r->write)
		refer_free(r->write);
	if (r->s_task)
		refer_free(r->s_task);
	if (r->s_okay)
		refer_free(r->s_okay);
	if (r->q_task)
		refer_free(r->q_task);
	if (r->q_data)
		refer_free(r->q_data);
	hashmap_uini(&r->ipv4);
	hashmap_uini(&r->ipv6);
}

web_dns_core_s* web_dns_core_alloc(udns_inst_s *inst, transport_s *tp, uintptr_t queue_size)
{
	web_dns_core_s *restrict r;
	r = (web_dns_core_s *) refer_alloz(sizeof(web_dns_core_s));
	if (r)
	{
		r->inst = (udns_inst_s *) refer_save(inst);
		r->tp = (transport_s *) refer_save(tp);
		refer_set_free(r, (refer_free_f) web_dns_core_free_func);
		if (yaw_lock_alloc_rwlock(&r->read, &r->write))
			goto label_fail;
		if (!(r->s_task = yaw_signal_alloc()))
			goto label_fail;
		if (!(r->s_okay = yaw_signal_alloc()))
			goto label_fail;
		if (!(r->q_task = queue_alloc_ring(queue_size)))
			goto label_fail;
		if (!(r->q_data = queue_alloc_ring(queue_size)))
			goto label_fail;
		if (!hashmap_init(&r->ipv4))
			goto label_fail;
		if (!hashmap_init(&r->ipv6))
			goto label_fail;
		if ((r->task = yaw_alloc_and_start(web_dns_core_task, NULL, r)))
			return r;
		label_fail:
		refer_free(r);
	}
	return NULL;
}

static void web_dns_free_func(web_dns_s *restrict r)
{
	if (r->core)
		refer_free(r->core);
	if (r->tp)
		refer_free(r->tp);
	if (r->inst)
		refer_free(r->inst);
	if (r->target)
		refer_free(r->target);
}

web_dns_s* web_dns_alloc(const char *restrict target)
{
	web_dns_s *restrict r;
	if ((r = (web_dns_s *) refer_alloz(sizeof(web_dns_s))))
	{
		refer_set_free(r, (refer_free_f) web_dns_free_func);
		if ((!target || (r->target = refer_dump_string(target))))
		{
			if ((!r->target || ((r->inst = udns_inst_alloc_mini()) &&
				(r->tp = transport_udp_alloc_ipv4()) &&
				transport_udp_set_remote(r->tp, r->target, 53))) &&
				(r->core = web_dns_core_alloc(r->inst, r->tp, 1024)))
			{
				r->runing = NULL;
				r->gap = web_dns_time_gap_usec;
				web_dns_attr_set_timeout(r, 0, 0);
				return r;
			}
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
		ntry = 3;
	if (!timeout_ms)
		timeout_ms = 500;
	dns->ntry = ntry;
	dns->timeout = timeout_ms * 1000;
}

web_dns_s* web_dns_set_ipv4(web_dns_s *dns, const char *restrict domain, const char *restrict ipv4)
{
	web_dns_core_s *restrict core;
	yaw_lock_s *restrict lock;
	hashmap_t *h;
	lock = (core = dns->core)->write;
	yaw_lock_lock(lock);
	h = web_dns_core_update_hashmap(&core->ipv4, domain, ipv4, ~(uint64_t) 0);
	yaw_lock_unlock(lock);
	return h?dns:NULL;
}

web_dns_s* web_dns_set_ipv6(web_dns_s *dns, const char *restrict domain, const char *restrict ipv6)
{
	web_dns_core_s *restrict core;
	yaw_lock_s *restrict lock;
	hashmap_t *h;
	lock = (core = dns->core)->write;
	yaw_lock_lock(lock);
	h = web_dns_core_update_hashmap(&core->ipv6, domain, ipv6, ~(uint64_t) 0);
	yaw_lock_unlock(lock);
	return h?dns:NULL;
}

static inline refer_nstring_t web_dns_get_save_ip_by_cache(hashmap_t *restrict h, yaw_lock_s *restrict read, const char *restrict domain, uint64_t curr)
{
	web_dns_item_t *restrict item;
	refer_nstring_t r;
	yaw_lock_lock(read);
	if ((item = (web_dns_item_t *) hashmap_get_name(h, domain)) && item->tkill >= curr && item->hit)
		r = (refer_nstring_t) refer_save(item->hit);
	else r = NULL;
	yaw_lock_unlock(read);
	return r;
}

static refer_nstring_t web_dns_get_save_ip(web_dns_s *dns, hashmap_t *restrict h, const char *restrict domain, udns_type_t type)
{
	web_dns_core_s *restrict core;
	refer_nstring_t rip;
	uintptr_t i, t;
	const volatile uintptr_t *runing;
	uint64_t curr;
	uint64_t ts_curr;
	uint64_t ts_edge;
	uint32_t status;
	if (domain)
	{
		curr = yaw_timestamp_sec();
		core = dns->core;
		if ((rip = web_dns_get_save_ip_by_cache(h, core->read, domain, curr)))
		{
			label_okay:
			return rip;
		}
		if (dns->tp)
		{
			runing = dns->runing;
			status = 0;
			for (i = 0; i < dns->ntry; ++i)
			{
				if (web_dns_core_push_task(core, domain, type))
				{
					ts_edge = (ts_curr = yaw_timestamp_usec()) + dns->timeout;
					goto label_entry;
					do {
						ts_curr = yaw_timestamp_usec();
						if ((!runing || *runing) && ts_curr < ts_edge)
						{
							t = (uintptr_t) ts_edge - ts_curr;
							if (t > dns->gap)
								t = dns->gap;
							if (t) yaw_signal_wait_time(core->s_okay, status, t);
						}
						label_entry:
						status = yaw_signal_get(core->s_okay);
						if ((rip = web_dns_get_save_ip_by_cache(h, core->read, domain, curr)))
							goto label_okay;
					} while ((!runing || *runing) && ts_curr < ts_edge);
				}
			}
		}
	}
	return NULL;
}

refer_nstring_t web_dns_get_save_ipv4(web_dns_s *dns, const char *restrict domain)
{
	return web_dns_get_save_ip(dns, &dns->core->ipv4, domain, udns_type_a);
}

refer_nstring_t web_dns_get_save_ipv6(web_dns_s *dns, const char *restrict domain)
{
	return web_dns_get_save_ip(dns, &dns->core->ipv6, domain, udns_type_aaaa);
}
