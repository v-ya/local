#define _DEFAULT_SOURCE
#include "refer.pool.h"
#include <linux/futex.h>
#include <sys/syscall.h>
#include <pthread.h>
#include <memory.h>
#include <time.h>
#include "inner/rbtree.h"
#include "inner/queue_ring.h"
#include "inner/inline_futex.h"
#include "refer.inner.h"

typedef struct refer_pool_signal_s refer_pool_signal_s;
typedef struct refer_pool_block_t refer_pool_block_t;
typedef struct refer_pool_heap_s refer_pool_heap_s;

struct refer_pool_signal_s {
	volatile uint32_t status;
	volatile uint32_t number;
};

struct refer_pool_block_t {
	refer_pool_item_t item;
	uint64_t ref_count;
};

struct refer_pool_heap_s {
	rbtree_t *pool;
	refer_pool_item_t *uhit_array;
	uintptr_t uhit_count;
	uintptr_t uhit_n_max;
};

struct refer_pool_s {
	refer_pool_heap_s *heap[refer_pool_id_max];
	queue_ring_s *queue;
	pthread_t thread;
	uint64_t timestamp;
	volatile uintptr_t running;
	volatile uint32_t signal_push;
	volatile uint32_t signal_pull;
};

// signal

static uint32_t refer_pool_signal_get(volatile uint32_t *signal)
{
	return *signal;
}

static void refer_pool_signal_inc_wake(volatile uint32_t *signal, uint32_t number)
{
	if (number > INT32_MAX)
		number = INT32_MAX;
	if (number)
	{
		__sync_add_and_fetch(signal, 1);
		yaw_inline_futex_wake(signal, number);
	}
}

static void refer_pool_signal_wait(volatile uint32_t *signal, uint32_t status, struct timespec *restrict ts)
{
	if (*signal == status)
		yaw_inline_futex_wait(signal, status, ts);
}

// heap

static void refer_pool_heap_pool_free_func(rbtree_t *restrict rbv)
{
	free(rbv->value);
}

static void refer_pool_heap_free_func(refer_pool_heap_s *restrict r)
{
	rbtree_clear(&r->pool);
	if (r->uhit_array) free(r->uhit_array);
}

static refer_pool_heap_s* refer_pool_heap_alloc(void)
{
	refer_pool_heap_s *restrict r;
	if ((r = (refer_pool_heap_s *) refer_alloz(sizeof(refer_pool_heap_s))))
	{
		refer_set_free(r, (refer_free_f) refer_pool_heap_free_func);
		return r;
	}
	return NULL;
}

static refer_pool_heap_s* refer_pool_heap_new_item(refer_pool_heap_s *restrict r, const refer_pool_item_t *restrict item)
{
	refer_pool_block_t *restrict block;
	if ((block = (refer_pool_block_t *) malloc(sizeof(refer_pool_block_t))))
	{
		memcpy(&block->item, item, sizeof(refer_pool_item_t));
		block->item.func = NULL;
		block->ref_count = 1;
		if (rbtree_insert(&r->pool, NULL, (uint64_t) (uintptr_t) item->ptr, block, refer_pool_heap_pool_free_func))
			return r;
		free(block);
	}
	return NULL;
}

static refer_pool_heap_s* refer_pool_heap_del_item(refer_pool_heap_s *restrict r, void *key)
{
	refer_pool_block_t *restrict block;
	rbtree_t *restrict rbv;
	if ((rbv = rbtree_find(&r->pool, NULL, (uint64_t) (uintptr_t) key)))
	{
		block = (refer_pool_block_t *) rbv->value;
		if (!(block->ref_count -= 1))
			rbtree_delete_by_pointer(&r->pool, rbv);
		return r;
	}
	return NULL;
}

static refer_pool_heap_s* refer_pool_heap_ref_item(refer_pool_heap_s *restrict r, void *key)
{
	refer_pool_block_t *restrict block;
	rbtree_t *restrict rbv;
	if ((rbv = rbtree_find(&r->pool, NULL, (uint64_t) (uintptr_t) key)))
	{
		block = (refer_pool_block_t *) rbv->value;
		block->ref_count += 1;
		return r;
	}
	return NULL;
}

static refer_pool_heap_s* refer_pool_heap_set_item_func(refer_pool_heap_s *restrict r, void *key, void *func)
{
	refer_pool_block_t *restrict block;
	rbtree_t *restrict rbv;
	if ((rbv = rbtree_find(&r->pool, NULL, (uint64_t) (uintptr_t) key)))
	{
		block = (refer_pool_block_t *) rbv->value;
		block->item.func = func;
		return r;
	}
	return NULL;
}

static refer_pool_heap_s* refer_pool_heap_get_item(refer_pool_heap_s *restrict r, void *key)
{
	rbtree_t *restrict rbv;
	if ((rbv = rbtree_find(&r->pool, NULL, (uint64_t) (uintptr_t) key)))
		return r;
	return NULL;
}

static refer_pool_heap_s* refer_pool_heap_realloc_item(refer_pool_heap_s *restrict r, const refer_pool_item_t *restrict item)
{
	refer_pool_block_t *restrict block;
	rbtree_t *restrict rbv;
	refer_pool_heap_s *r1, *r2;
	if (item->func == item->ptr)
	{
		if ((rbv = rbtree_find(&r->pool, NULL, (uint64_t) (uintptr_t) item->func)))
		{
			block = (refer_pool_block_t *) rbv->value;
			memcpy(&block->item, item, sizeof(refer_pool_item_t));
			block->item.func = NULL;
			block->ref_count = 1;
			return r;
		}
	}
	else if (item->func)
	{
		r1 = refer_pool_heap_del_item(r, item->func);
		r2 = refer_pool_heap_new_item(r, item);
		if (r1 && r2) return r;
	}
	else return refer_pool_heap_new_item(r, item);
	return NULL;
}

static refer_pool_heap_s* refer_pool_heap_hit(refer_pool_heap_s *restrict r, const refer_pool_item_t *restrict item)
{
	switch (item->fn)
	{
		case refer_pool_fn__malloc:
		case refer_pool_fn__calloc:
		case refer_pool_fn__posix_memalign:
		case refer_pool_fn__aligned_alloc:
		case refer_pool_fn__memalign:
		case refer_pool_fn__valloc:
		case refer_pool_fn__pvalloc:
		case refer_pool_fn__refer_alloc:
		case refer_pool_fn__refer_alloz:
			// insert
			return refer_pool_heap_new_item(r, item);
		case refer_pool_fn__free:
		case refer_pool_fn__refer_free:
			// delete (sub ref_count)
			return refer_pool_heap_del_item(r, item->ptr);
		case refer_pool_fn__refer_save:
			// add ref_count
			return refer_pool_heap_ref_item(r, item->ptr);
		case refer_pool_fn__refer_set_free:
			// set func
			return refer_pool_heap_set_item_func(r, item->ptr, item->func);
		case refer_pool_fn__refer_get_free:
		case refer_pool_fn__refer_save_number:
			// touch
			return refer_pool_heap_get_item(r, item->ptr);
		case refer_pool_fn__realloc:
			// realloc
			return refer_pool_heap_realloc_item(r, item);
		default:
			return NULL;
	}
}

static refer_pool_heap_s* refer_pool_heap_uhit(refer_pool_heap_s *restrict r, const refer_pool_item_t *restrict item)
{
	refer_pool_item_t *restrict p;
	uintptr_t max, size;
	if ((p = r->uhit_array) && r->uhit_count < r->uhit_n_max)
		goto label_entry;
	if ((max = r->uhit_n_max))
		max <<= 1;
	else max = 1024;
	size = max * sizeof(refer_pool_item_t);
	if (size / sizeof(refer_pool_item_t) > r->uhit_n_max)
	{
		if ((p = (refer_pool_item_t *) realloc(r->uhit_array, size)))
		{
			r->uhit_array = p;
			r->uhit_n_max = max;
			label_entry:
			memcpy(p + r->uhit_count, item, sizeof(refer_pool_item_t));
			r->uhit_count += 1;
			return r;
		}
	}
	return NULL;
}

static void refer_pool_heap_deal(refer_pool_heap_s *restrict r, const refer_pool_item_t *restrict item)
{
	if (!refer_pool_heap_hit(r, item))
		refer_pool_heap_uhit(r, item);
}

// pool thread

static void *refer_pool_thread_func(refer_pool_s *restrict r)
{
	refer_pool_item_s *restrict item;
	queue_ring_s *restrict queue;
	uint32_t status, pulled;
	queue = r->queue;
	for (;;)
	{
		status = refer_pool_signal_get(&r->signal_push);
		pulled = 0;
		while ((item = (refer_pool_item_s *) queue_ring_pull(queue)))
		{
			if ((uint32_t) item->item.id < refer_pool_id_max)
				refer_pool_heap_deal(r->heap[item->item.id], &item->item);
			refer_free(item);
			pulled = 1;
		}
		if (pulled) refer_pool_signal_inc_wake(&r->signal_pull, ~0);
		if (!r->running) break;
		refer_pool_signal_wait(&r->signal_push, status, NULL);
	}
	refer_pool_signal_inc_wake(&r->signal_pull, ~0);
	return NULL;
}

// pool

static uint64_t refer_pool_timestamp(void)
{
	struct timespec ts;
	if (yaw_inline_timespec_curr(&ts))
		return (uint64_t) ts.tv_sec * dt_s2ms + (uint64_t) ts.tv_nsec / dt_ms2ns;
	return 0;
}

static void refer_pool_free_func(refer_pool_s *restrict r)
{
	uintptr_t i, n;
	refer_pool_stop(r);
	for (i = 0, n = refer_pool_id_max; i < n; ++i)
	{
		if (r->heap[i])
			refer_free(r->heap[i]);
	}
	if (r->queue) refer_free(r->queue);
}

refer_pool_s* refer_pool_alloc(uintptr_t queue_size)
{
	refer_pool_s *restrict r;
	uintptr_t i, n;
	if ((r = (refer_pool_s *) refer_alloz(sizeof(refer_pool_s))))
	{
		refer_set_free(r, (refer_free_f) refer_pool_free_func);
		for (i = 0, n = refer_pool_id_max; i < n; ++i)
		{
			if (!(r->heap[i] = refer_pool_heap_alloc()))
				goto label_fail;
		}
		if (!(r->queue = queue_ring_alloc(queue_size)))
			goto label_fail;
		r->running = 1;
		r->timestamp = refer_pool_timestamp();
		if (!pthread_create(&r->thread, NULL, (void *(*)(void *)) refer_pool_thread_func, r))
			return r;
		label_fail:
		refer_free(r);
	}
	return NULL;
}

void refer_pool_stop(refer_pool_s *restrict r)
{
	r->running = 0;
	refer_pool_signal_inc_wake(&r->signal_push, 1);
	if (r->thread)
	{
		pthread_join(r->thread, NULL);
		r->thread = 0;
	}
}

static void refer_pool_push(refer_pool_s *restrict r, refer_pool_item_s *restrict item)
{
	uint32_t status;
	item->item.timestamp -= r->timestamp;
	if (!queue_ring_push(r->queue, item))
	{
		for (;;)
		{
			status = refer_pool_signal_get(&r->signal_pull);
			if (!r->running) break;
			if (queue_ring_push(r->queue, item)) break;
			refer_pool_signal_wait(&r->signal_pull, status, NULL);
		}
	}
	refer_free(item);
}

#define d_func_body(_id, _fn, _ptr, _func, _block, _size, _alignment)  \
	{\
		refer_pool_item_s *restrict item;\
		if (pool && (item = (refer_pool_item_s *) refer_alloc(sizeof(refer_pool_item_s))))\
		{\
			item->item.id = refer_pool_id_##_id;\
			item->item.fn = refer_pool_fn_##_fn;\
			item->item.raddr = raddr;\
			item->item.ptr = (void *) _ptr;\
			item->item.func = (void *) _func;\
			item->item.block = _block;\
			item->item.size = _size;\
			item->item.alignment = _alignment;\
			item->item.timestamp = refer_pool_timestamp();\
			refer_pool_push(pool, item);\
		}\
	}

void refer_pool_push__malloc(refer_pool_s *restrict pool, void *raddr, void *ptr, size_t size)
	d_func_body(_alloc, _malloc, ptr, NULL, 1, size, 0)
void refer_pool_push__calloc(refer_pool_s *restrict pool, void *raddr, void *ptr, size_t nmemb, size_t size)
	d_func_body(_alloc, _calloc, ptr, NULL, nmemb, size, 0)
void refer_pool_push__posix_memalign(refer_pool_s *restrict pool, void *raddr, void *ptr, size_t alignment, size_t size)
	d_func_body(_alloc, _posix_memalign, ptr, NULL, 1, size, alignment)
void refer_pool_push__aligned_alloc(refer_pool_s *restrict pool, void *raddr, void *ptr, size_t alignment, size_t size)
	d_func_body(_alloc, _aligned_alloc, ptr, NULL, 1, size, alignment)
void refer_pool_push__memalign(refer_pool_s *restrict pool, void *raddr, void *ptr, size_t alignment, size_t size)
	d_func_body(_alloc, _memalign, ptr, NULL, 1, size, alignment)
void refer_pool_push__valloc(refer_pool_s *restrict pool, void *raddr, void *ptr, size_t size)
	d_func_body(_alloc, _valloc, ptr, NULL, 1, size, 0)
void refer_pool_push__pvalloc(refer_pool_s *restrict pool, void *raddr, void *ptr, size_t size)
	d_func_body(_alloc, _pvalloc, ptr, NULL, 1, size, 0)
void refer_pool_push__realloc(refer_pool_s *restrict pool, void *raddr, void *ptr, void *ptr0, size_t size)
	d_func_body(_alloc, _realloc, ptr, ptr0, 1, size, 0)
void refer_pool_push__free(refer_pool_s *restrict pool, void *raddr, void *ptr)
	d_func_body(_alloc, _free, ptr, NULL, 0, 0, 0)

void refer_pool_push__refer_alloc(refer_pool_s *restrict pool, void *raddr, refer_t ptr, size_t size)
	d_func_body(_refer, _refer_alloc, ptr, NULL, 1, size, 0)
void refer_pool_push__refer_alloz(refer_pool_s *restrict pool, void *raddr, refer_t ptr, size_t size)
	d_func_body(_refer, _refer_alloz, ptr, NULL, 1, size, 0)
void refer_pool_push__refer_free(refer_pool_s *restrict pool, void *raddr, refer_t ptr)
	d_func_body(_refer, _refer_free, ptr, NULL, 0, 0, 0)
void refer_pool_push__refer_set_free(refer_pool_s *restrict pool, void *raddr, refer_t ptr, refer_free_f free_func)
	d_func_body(_refer, _refer_set_free, ptr, free_func, 0, 0, 0)
void refer_pool_push__refer_get_free(refer_pool_s *restrict pool, void *raddr, refer_t ptr)
	d_func_body(_refer, _refer_get_free, ptr, NULL, 0, 0, 0)
void refer_pool_push__refer_save(refer_pool_s *restrict pool, void *raddr, refer_t ptr)
	d_func_body(_refer, _refer_save, ptr, NULL, 0, 0, 0)
void refer_pool_push__refer_save_number(refer_pool_s *restrict pool, void *raddr, refer_t ptr)
	d_func_body(_refer, _refer_save_number, ptr, NULL, 0, 0, 0)

struct refer_pool_memless_t {
	refer_pool_memless_f func;
	void *ptr;
};

static void refer_pool_memless_func(rbtree_t *restrict rbv, const struct refer_pool_memless_t *restrict argv)
{
	const refer_pool_block_t *restrict block;
	block = (const refer_pool_block_t *) rbv->value;
	argv->func(argv->ptr, &block->item, block->ref_count);
}

void refer_pool_memless(refer_pool_s *restrict pool, refer_pool_id_t id, refer_pool_memless_f memless, void *ptr)
{
	struct refer_pool_memless_t argv;
	if (memless && (uint32_t) id < refer_pool_id_max)
	{
		argv.func = memless;
		argv.ptr = ptr;
		rbtree_call(&pool->heap[id]->pool, (rbtree_func_call_f) refer_pool_memless_func, &argv);
	}
}

void refer_pool_memmiss(refer_pool_s *restrict pool, refer_pool_id_t id, refer_pool_memmiss_f memmiss, void *ptr)
{
	refer_pool_heap_s *restrict heap;
	const refer_pool_item_t *restrict uhit_array;
	uintptr_t uhit_count, i;
	if (memmiss && (uint32_t) id < refer_pool_id_max)
	{
		heap = pool->heap[id];
		uhit_array = heap->uhit_array;
		uhit_count = heap->uhit_count;
		for (i = 0; i < uhit_count; ++i)
			memmiss(ptr, &uhit_array[i]);
	}
}

#undef d_func_body
