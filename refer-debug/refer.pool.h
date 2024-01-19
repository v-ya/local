#ifndef _refer_pool_h_
#define _refer_pool_h_

#include "refer.debug.h"

typedef enum refer_pool_id_t refer_pool_id_t;
typedef enum refer_pool_fn_t refer_pool_fn_t;
typedef struct refer_pool_item_t refer_pool_item_t;
typedef struct refer_pool_item_s refer_pool_item_s;
typedef struct refer_pool_s refer_pool_s;
typedef void (*refer_pool_memless_f)(void *ptr, const refer_pool_item_t *restrict item, uint64_t ref_count);
typedef void (*refer_pool_memmiss_f)(void *ptr, const refer_pool_item_t *restrict item);

enum refer_pool_id_t {
	refer_pool_id__alloc,
	refer_pool_id__refer,
	refer_pool_id_max,
};

enum refer_pool_fn_t {
	// insert
	refer_pool_fn__malloc,
	refer_pool_fn__calloc,
	refer_pool_fn__posix_memalign,
	refer_pool_fn__aligned_alloc,
	refer_pool_fn__memalign,
	refer_pool_fn__valloc,
	refer_pool_fn__pvalloc,
	refer_pool_fn__refer_alloc,
	refer_pool_fn__refer_alloz,
	// delete
	refer_pool_fn__free,
	refer_pool_fn__refer_free,
	// update
	refer_pool_fn__refer_save,
	refer_pool_fn__refer_set_free,
	// touch
	refer_pool_fn__refer_get_free,
	refer_pool_fn__refer_save_number,
	// realloc
	refer_pool_fn__realloc,
	// ...
	refer_pool_fn_max
};

struct refer_pool_item_t {
	refer_pool_id_t id;
	refer_pool_fn_t fn;
	void *raddr;
	void *ptr;
	void *func;  // realloc => orginal ptr
	uintptr_t block;
	uintptr_t size;
	uintptr_t alignment;
	uint64_t timestamp;
};

struct refer_pool_item_s {
	refer_pool_item_t item;
};

refer_pool_s* refer_pool_alloc(uintptr_t queue_size);
void refer_pool_stop(refer_pool_s *restrict r);

void refer_pool_push__malloc(refer_pool_s *restrict pool, void *raddr, void *ptr, size_t size);
void refer_pool_push__calloc(refer_pool_s *restrict pool, void *raddr, void *ptr, size_t nmemb, size_t size);
void refer_pool_push__posix_memalign(refer_pool_s *restrict pool, void *raddr, void *ptr, size_t alignment, size_t size);
void refer_pool_push__aligned_alloc(refer_pool_s *restrict pool, void *raddr, void *ptr, size_t alignment, size_t size);
void refer_pool_push__memalign(refer_pool_s *restrict pool, void *raddr, void *ptr, size_t alignment, size_t size);
void refer_pool_push__valloc(refer_pool_s *restrict pool, void *raddr, void *ptr, size_t size);
void refer_pool_push__pvalloc(refer_pool_s *restrict pool, void *raddr, void *ptr, size_t size);
void refer_pool_push__realloc(refer_pool_s *restrict pool, void *raddr, void *ptr, void *ptr0, size_t size);
void refer_pool_push__free(refer_pool_s *restrict pool, void *raddr, void *ptr);

void refer_pool_push__refer_alloc(refer_pool_s *restrict pool, void *raddr, refer_t ptr, size_t size);
void refer_pool_push__refer_alloz(refer_pool_s *restrict pool, void *raddr, refer_t ptr, size_t size);
void refer_pool_push__refer_free(refer_pool_s *restrict pool, void *raddr, refer_t ptr);
void refer_pool_push__refer_set_free(refer_pool_s *restrict pool, void *raddr, refer_t ptr, refer_free_f free_func);
void refer_pool_push__refer_get_free(refer_pool_s *restrict pool, void *raddr, refer_t ptr);
void refer_pool_push__refer_save(refer_pool_s *restrict pool, void *raddr, refer_t ptr);
void refer_pool_push__refer_save_number(refer_pool_s *restrict pool, void *raddr, refer_t ptr);

void refer_pool_memless(refer_pool_s *restrict pool, refer_pool_id_t id, refer_pool_memless_f memless, void *ptr);
void refer_pool_memmiss(refer_pool_s *restrict pool, refer_pool_id_t id, refer_pool_memmiss_f memmiss, void *ptr);

#endif
