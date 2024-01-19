#include "queue_ring.h"
#include "queue.h"

#include "../refer.inner.h"

struct queue_ring_s {
	queue_s interface;
	uintptr_t size;
	uintptr_t mask;
	volatile uintptr_t number;
	volatile uintptr_t reserve;
	volatile uintptr_t head;
	volatile uintptr_t tail;
	volatile uintptr_t seq_head;
	volatile uintptr_t seq_tail;
	refer_t ring[];
};

static void queue_ring_free_func(queue_ring_s *restrict r)
{
	register uintptr_t i, n;
	for (i = 0, n = r->mask + 1; i < n; ++i)
	{
		if (r->ring[i])
			refer_free(r->ring[i]);
	}
}

queue_ring_s* queue_ring_alloc(size_t size)
{
	queue_ring_s *restrict r;
	uintptr_t n;
	if (size)
	{
		n = size - 1;
		n |= n >> 32;
		n |= n >> 16;
		n |= n >> 8;
		n |= n >> 4;
		n |= n >> 2;
		n |= n >> 1;
		r = (queue_ring_s *) refer_alloz(sizeof(queue_ring_s) + sizeof(refer_t) * (n + 1));
		if (r)
		{
			refer_set_free(r, (refer_free_f) queue_ring_free_func);
			r->interface.push = (queue_push_f) queue_ring_push;
			r->interface.pull = (queue_pull_f) queue_ring_pull;
			r->size = size;
			r->mask = n;
			r->number = 0;
			r->reserve = size;
			return r;
		}
	}
	return NULL;
}

static inline uintptr_t atomic_dec_uintptr(volatile uintptr_t *p)
{
	uintptr_t n, nn;
	if ((n = *p))
	{
		do {
			if ((nn = __sync_val_compare_and_swap(p, n, n - 1)) == n)
				return n;
		} while ((n = nn));
	}
	return n;
}

queue_ring_s* queue_ring_push(register queue_ring_s *q, refer_t v)
{
	if (v && atomic_dec_uintptr(&q->reserve))
	{
		register uintptr_t n, nn;
		q->ring[(n = __sync_fetch_and_add(&q->tail, 1)) & q->mask] = refer_save(v);
		nn = n + 1;
		while (!__sync_bool_compare_and_swap(&q->seq_tail, n, nn)) ;
		__sync_add_and_fetch(&q->number, 1);
		return q;
	}
	return NULL;
}

refer_t queue_ring_pull(register queue_ring_s *q)
{
	refer_t v = NULL;
	if (atomic_dec_uintptr(&q->number))
	{
		register uintptr_t n, nn;
		v = (refer_t) __sync_lock_test_and_set(q->ring + ((n = __sync_fetch_and_add(&q->head, 1)) & q->mask), NULL);
		nn = n + 1;
		while (!__sync_bool_compare_and_swap(&q->seq_head, n, nn)) ;
		__sync_add_and_fetch(&q->reserve, 1);
	}
	return v;
}

size_t queue_ring_size(const queue_ring_s *q)
{
	return q->size;
}

size_t queue_ring_number(const queue_ring_s *q)
{
	return q->number;
}

size_t queue_ring_reserve(const queue_ring_s *q)
{
	return q->reserve;
}
