#include "transport.h"
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
		pos = data + n - r->used;
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
}

void transport_final(transport_s *restrict r)
{
	if (r->layer)
		refer_free(r->layer);
	if (r->cache.data)
		free(r->cache.data);
}

transport_s* transport_send(transport_s *restrict r, const void *data, uintptr_t n, uintptr_t *restrict rn)
{
	uintptr_t _rn;
	if (!rn) rn = &_rn;
	*rn = 0;
	if (r->send)
		return r->send(r, data, n, rn);
	return NULL;
}

transport_s* transport_recv(transport_s *restrict r, void *data, uintptr_t n, uintptr_t *restrict rn)
{
	uintptr_t _rn;
	if (!rn) rn = &_rn;
	*rn = 0;
	if (r->recv)
	{
		uintptr_t n1;
		uintptr_t n2;
		if (!r->cache.used)
			return r->recv(r, data, n, rn);
		if ((n1 = transport_cache_pull(&r->cache, data, n)) < n)
			(r = r->recv(r, data + n1, n - n1, &n2)), n1 += n2;
		*rn = n1;
		return r;
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

// other

#include "inner/time.h"

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
