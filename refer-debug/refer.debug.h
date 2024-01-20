#ifndef _refer_debug_h_
#define _refer_debug_h_

#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

// LD_PRELOAD=.../librefer.debug.so <command>

#define _kira_  __attribute__((visibility("default")))
#define _init_  __attribute__((constructor))
#define _fini_  __attribute__((destructor))

typedef void (*refer_free_f)(void *restrict v);
typedef struct refer_s refer_s;
typedef const void* refer_t;
struct refer_s {
	uint64_t inode;
	refer_free_f free_func;
};

typedef struct refer_debug_real_t refer_debug_real_t;
struct refer_debug_real_t {
	void (*__libc_freeres)(void);
	void *(*__real_malloc)(size_t size);
	void *(*__real_calloc)(size_t nmemb, size_t size);
	int (*__real_posix_memalign)(void **memptr, size_t alignment, size_t size);
	void *(*__real_aligned_alloc)(size_t alignment, size_t size);
	void *(*__real_memalign)(size_t alignment, size_t size);
	void *(*__real_valloc)(size_t size);
	void *(*__real_pvalloc)(size_t size);
	void *(*__real_realloc)(void *ptr, size_t size);
	void (*__real_free)(void *ptr);
	refer_t (*__real_refer_alloc)(size_t size);
	refer_t (*__real_refer_alloz)(size_t size);
	refer_t (*__real_refer_free)(refer_t v);
	void (*__real_refer_set_free)(refer_t v, refer_free_f free_func);
	refer_free_f (*__real_refer_get_free)(refer_t v);
	refer_t (*__real_refer_save)(refer_t v);
	uint64_t (*__real_refer_save_number)(refer_t v);
};

extern refer_debug_real_t _rf_;

#endif
