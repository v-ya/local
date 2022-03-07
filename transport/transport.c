#include "transport.h"
#include "inner/transport.h"
#include "inner/time.h"
#include "inner/define.h"
#include <stdlib.h>
#include <memory.h>

// transport cache

static inline transport_cache_t* transport_cache_resize(transport_cache_t *restrict r, uintptr_t n)
{
	uint8_t *data;
	uint8_t *pos;
	uintptr_t size;
	size = 4096;
	while (size && (size < n))
		size <<= 1;
	if (n <= size && (data = (uint8_t *) malloc(size)))
	{
		pos = data + size - r->used;
		if (r->used)
			memcpy(pos, r->pos, r->used);
		if (r->data)
			free(r->data);
		r->data = data;
		r->pos = pos;
		r->size = size;
		return r;
	}
	return NULL;
}

static uintptr_t transport_cache_pull(transport_cache_t *restrict r, void *data, uintptr_t n)
{
	if (r->used && n)
	{
		if (n > r->used)
			n = r->used;
		memcpy(data, r->pos, n);
		r->pos += n;
		r->used -= n;
		return n;
	}
	return 0;
}

static transport_cache_t* transport_cache_push(transport_cache_t *restrict r, const void *data, uintptr_t n)
{
	if (!n) goto label_okay;
	if ((r->used + n <= r->size) || transport_cache_resize(r, r->used + n))
	{
		r->pos -= n;
		r->used += n;
		memcpy(r->pos, data, n);
		label_okay:
		return r;
	}
	return NULL;
}

static void transport_cache_discard(transport_cache_t *restrict r)
{
	if (r->data)
		free(r->data);
	r->data = NULL;
	r->pos = NULL;
	r->size = 0;
	r->used = 0;
}

// transport

void transport_initial(transport_s *restrict r, const char *type, transport_s *layer, transport_send_f send, transport_recv_f recv)
{
	r->type = type;
	r->layer = (transport_s *) refer_save(layer);
	r->send = send;
	r->recv = recv;
	r->cache.data = NULL;
	r->cache.pos = NULL;
	r->cache.size = 0;
	r->cache.used = 0;
	if (!layer) r->p_socket = NULL;
	else r->p_socket = layer->p_socket;
}

void transport_final(transport_s *restrict r)
{
	if (r->layer)
		refer_free(r->layer);
	if (r->cache.data)
		free(r->cache.data);
}

typedef transport_s* (*transport_attr_do_f)(transport_s *restrict r, void *data, uintptr_t n, uintptr_t *restrict rn);

static inline transport_s* transport_attr_do(transport_s *restrict r, void *data, uintptr_t n, uintptr_t *restrict rn, transport_attr_t *restrict attr, transport_attr_do_f do_func)
{
	if (n)
	{
		uintptr_t n1;
		if (attr && (attr->flags & (transport_attr_flag_do_some | transport_attr_flag_do_full)))
		{
			const volatile uintptr_t *running;
			uint64_t die_timestamp;
			if (!(attr->flags & transport_attr_flag_do_full) && *rn)
				goto label_once;
			running = attr->running;
			die_timestamp = transport_timestamp_ms() + attr->timeout_ms;
			goto label_entry;
			do {
				transport_inner_sleep_maybe_ms(transport_inner_define_time_gap_ms);
				label_entry:
				r = do_func(r, data, n, &n1);
				*rn += n1;
				data += n1;
				n -= n1;
				if (!n || (!(attr->flags & transport_attr_flag_do_full) && n1))
				{
					if (attr->flags & transport_attr_flag_modify_timeout)
					{
						uint64_t now;
						if (die_timestamp >= (now = transport_timestamp_ms()))
							attr->timeout_ms = die_timestamp - now;
						else attr->timeout_ms = 0;
					}
					goto label_okay;
				}
			} while (r && (!running || *running) &&
				transport_timestamp_ms() < die_timestamp);
			r = NULL;
		}
		else
		{
			label_once:
			r = do_func(r, data, n, &n1);
			*rn += n1;
		}
	}
	label_okay:
	return r;
}

transport_s* transport_send(transport_s *restrict r, const void *data, uintptr_t n, uintptr_t *restrict rn, transport_attr_t *restrict attr)
{
	uintptr_t _rn;
	if (!rn) rn = &_rn;
	*rn = 0;
	if (r->send)
		return transport_attr_do(r, (void *) data, n, rn, attr, (transport_attr_do_f) r->send);
	return NULL;
}

transport_s* transport_recv(transport_s *restrict r, void *data, uintptr_t n, uintptr_t *restrict rn, transport_attr_t *restrict attr)
{
	uintptr_t _rn;
	uintptr_t n1;
	if (!rn) rn = &_rn;
	*rn = 0;
	if (r->recv)
	{
		if (r->cache.used)
		{
			*rn += (n1 = transport_cache_pull(&r->cache, data, n));
			data += n1;
			n -= n1;
		}
		return transport_attr_do(r, data, n, rn, attr, (transport_attr_do_f) r->recv);
	}
	return NULL;
}

transport_s* transport_push_recv(transport_s *restrict r, const void *data, uintptr_t n)
{
	if (transport_cache_push(&r->cache, data, n))
		return r;
	return NULL;
}

void transport_discard_cache(transport_s *restrict r)
{
	transport_cache_discard(&r->cache);
}

// timestamp

uint64_t transport_timestamp_sec(void)
{
	struct timespec t = {.tv_sec = 0, .tv_nsec = 0};
	transport_inner_timespec_now(&t);
	return (uint64_t) t.tv_sec;
}

uint64_t transport_timestamp_ms(void)
{
	struct timespec t = {.tv_sec = 0, .tv_nsec = 0};
	transport_inner_timespec_now(&t);
	return transport_inner_timespec_to_ms(&t);
}

uint64_t transport_timestamp_us(void)
{
	struct timespec t = {.tv_sec = 0, .tv_nsec = 0};
	transport_inner_timespec_now(&t);
	return transport_inner_timespec_to_us(&t);
}

// tpoll

#include "inner/tpoll.h"

struct transport_poll_s {
	transport_inner_poll_s *restrict tpoll;
	refer_t *cache;
	uintptr_t size;
	uintptr_t number;
};

static void transport_poll_free_func(transport_poll_s *restrict r)
{
	if (r->tpoll) refer_free(r->tpoll);
	if (r->number) transport_poll_clear_get(r);
}

transport_poll_s* transport_poll_alloc(uintptr_t size)
{
	transport_poll_s *restrict r;
	r = (transport_poll_s *) refer_alloc(sizeof(transport_poll_s) + sizeof(refer_t) * size);
	if (r)
	{
		r->tpoll = transport_inner_poll_alloc(size);
		r->cache = (refer_t *) (r + 1);
		r->size = size;
		r->number = 0;
		refer_set_free(r, (refer_free_f) transport_poll_free_func);
		if (r->tpoll)
			return r;
		refer_free(r);
	}
	return NULL;
}

void transport_poll_clear_get(transport_poll_s *restrict tpoll)
{
	refer_t *restrict p;
	uintptr_t i, n;
	p = tpoll->cache;
	n = tpoll->number;
	for (i = 0; i < n; ++i)
		if (p[i]) refer_free(p[i]);
	tpoll->number = 0;
}

transport_poll_s* transport_poll_insert(transport_poll_s *restrict tpoll, transport_s *restrict tp, transport_poll_do_f func, refer_t pri, uint64_t timestamp_kill, transport_poll_flag_t flags)
{
	if (tp && tp->p_socket && func && pri &&
		(flags & (transport_poll_flag_read | transport_poll_flag_write)))
	{
		uint32_t events;
		events = 0;
		if (flags & transport_poll_flag_read) events |= EPOLLIN;
		if (flags & transport_poll_flag_write) events |= EPOLLOUT;
		if (flags & transport_poll_flag_edge) events |= EPOLLET;
		if (transport_inner_poll_insert(tpoll->tpoll, tp, func, pri, timestamp_kill, events))
			return tpoll;
	}
	return NULL;
}

refer_t* transport_poll_get(transport_poll_s *restrict tpoll, uintptr_t *restrict number, uint64_t timeout_ms)
{
	if (tpoll->number)
		transport_poll_clear_get(tpoll);
	if (number)
	{
		uintptr_t n;
		n = transport_inner_poll_get_okay(tpoll->tpoll, timeout_ms, tpoll->cache, tpoll->size);
		if (n < tpoll->size)
			n += transport_inner_poll_get_timeout(tpoll->tpoll, transport_timestamp_ms(), tpoll->cache + n, tpoll->size - n);
		if ((*number = tpoll->number = n))
			return tpoll->cache;
	}
	return NULL;
}

uintptr_t transport_poll_number(transport_poll_s *restrict tpoll)
{
	return transport_inner_poll_number(tpoll->tpoll);
}
