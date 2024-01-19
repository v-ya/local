#define _GNU_SOURCE
#include "refer.debug.h"
#include "refer.pool.h"
#include <dlfcn.h>
#include <execinfo.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <inttypes.h>

// #define _print_

#ifndef _print_
#define p(...)
#define print_address(_name, _addr)
#else
#define p(...)  write(1, #__VA_ARGS__ "\n", sizeof(#__VA_ARGS__)); print_address("caller", __builtin_return_address(0))
static void print_address(const char *restrict name, void *addr)
{
	char buffer[4096];
	Dl_info info;
	int n;
	n = snprintf(buffer, sizeof(buffer), "\t" "%s: %p\n", name, addr);
	if (n > 0) write(1, buffer, n);
	if (dladdr(addr, &info))
	{
		n = snprintf(buffer, sizeof(buffer),
			"\t\t" "file-name   = %s\n"
			"\t\t" "file-base   = %p\n"
			"\t\t" "symbol-name = %s\n"
			"\t\t" "symbol-addr = %p\n",
			info.dli_fname, info.dli_fbase,
			info.dli_sname, info.dli_saddr);
		if (n > 0) write(1, buffer, n);
	}
}
#endif

refer_debug_real_t _rf_;

// refer impl

static refer_t __impl_refer_alloc(size_t size)
{
	refer_s *r;
	r = (refer_s *) _rf_.__real_malloc(size + sizeof(refer_s));
	if (r)
	{
		r->inode = 1;
		r->free_func = NULL;
		return (refer_t) (r + 1);
	}
	return (refer_t) r;
}

static refer_t __impl_refer_alloz(size_t size)
{
	refer_s *r;
	r = (refer_s *) _rf_.__real_calloc(1, size + sizeof(refer_s));
	if (r)
	{
		r->inode = 1;
		return (refer_t) (r + 1);
	}
	return (refer_t) r;
}

static refer_t __impl_refer_free(refer_t v)
{
	if (!__sync_sub_and_fetch(&((refer_s *) v - 1)->inode, 1))
	{
		refer_s *s;
		s = (refer_s *) v - 1;
		if (s->free_func) s->free_func((void *) v);
		_rf_.__real_free(s);
		v = NULL;
	}
	return v;
}

static void __impl_refer_set_free(refer_t v, refer_free_f free_func)
{
	((refer_s *) v - 1)->free_func = free_func;
}

static refer_free_f __impl_refer_get_free(refer_t v)
{
	return ((refer_s *) v - 1)->free_func;
}

static refer_t __impl_refer_save(refer_t v)
{
	if (v) __sync_add_and_fetch(&((refer_s *) v - 1)->inode, 1);
	return v;
}

static uint64_t __impl_refer_save_number(refer_t v)
{
	return ((refer_s *) v - 1)->inode;
}

// init && fini

static void refer_debug_print_addr(const char *restrict name, void *addr)
{
	char buffer[4096];
	Dl_info info;
	int n;
	n = snprintf(buffer, sizeof(buffer), "\t" "%s: %p\n", name, addr);
	if (n > 0) write(1, buffer, n);
	info.dli_fname = NULL;
	info.dli_fbase = NULL;
	info.dli_sname = NULL;
	info.dli_saddr = NULL;
	dladdr(addr, &info);
	if (!info.dli_fname) info.dli_fname = "";
	if (!info.dli_sname) info.dli_sname = "";
	n = snprintf(buffer, sizeof(buffer),
		"\t\t" "file-name   = %s\n"
		"\t\t" "file-base   = %p\n"
		"\t\t" "symbol-name = %s\n"
		"\t\t" "symbol-addr = %p\n",
		info.dli_fname, info.dli_fbase,
		info.dli_sname, info.dli_saddr);
	if (n > 0) write(1, buffer, n);
}
static void refer_debug_print_item(const refer_pool_item_t *restrict item, const uint64_t *restrict ref_count)
{
	static const char *const fn_string[refer_pool_fn_max] = {
		[refer_pool_fn__malloc] = "malloc",
		[refer_pool_fn__calloc] = "calloc",
		[refer_pool_fn__posix_memalign] = "posix_memalign",
		[refer_pool_fn__aligned_alloc] = "aligned_alloc",
		[refer_pool_fn__memalign] = "memalign",
		[refer_pool_fn__valloc] = "valloc",
		[refer_pool_fn__pvalloc] = "pvalloc",
		[refer_pool_fn__refer_alloc] = "refer_alloc",
		[refer_pool_fn__refer_alloz] = "refer_alloz",
		[refer_pool_fn__free] = "free",
		[refer_pool_fn__refer_free] = "refer_free",
		[refer_pool_fn__refer_save] = "refer_save",
		[refer_pool_fn__refer_set_free] = "refer_set_free",
		[refer_pool_fn__refer_get_free] = "refer_get_free",
		[refer_pool_fn__refer_save_number] = "refer_save_number",
		[refer_pool_fn__realloc] = "realloc",
	};
	char buffer[4096];
	int n;
	n = snprintf(buffer, sizeof(buffer),
		"[%s](%s:%" PRIu64 ") ptr=%p, block=%zu, size=%zu, align=%zu, time=%.3fms\n",
		fn_string[item->fn], ref_count?"ref":"miss", ref_count?*ref_count:(uint64_t) 0,
		item->ptr, item->block, item->size, item->alignment, (double) item->timestamp * 0.001);
	if (n > 0) write(1, buffer, n);
	if (item->raddr) refer_debug_print_addr("caller", item->raddr);
	if (item->func) refer_debug_print_addr("free_func", item->func);
}
static void refer_debug_memless_func(void *ptr, const refer_pool_item_t *restrict item, uint64_t ref_count)
{
	refer_debug_print_item(item, &ref_count);
}
static void refer_debug_memmiss_func(void *ptr, const refer_pool_item_t *restrict item)
{
	refer_debug_print_item(item, NULL);
}
static void refer_debug_print(refer_pool_s *restrict p)
{
	refer_pool_memmiss(p, refer_pool_id__alloc, refer_debug_memmiss_func, NULL);
	refer_pool_memmiss(p, refer_pool_id__refer, refer_debug_memmiss_func, NULL);
	refer_pool_memless(p, refer_pool_id__alloc, refer_debug_memless_func, NULL);
	refer_pool_memless(p, refer_pool_id__refer, refer_debug_memless_func, NULL);
}

static refer_pool_s *pool = NULL;

_kira_ _init_ void __init_refer_debug(void)
{
	p(__init_refer_debug);
	refer_pool_s *restrict p;
	_rf_.__real_malloc = dlsym(RTLD_NEXT, "malloc");
	_rf_.__real_calloc = dlsym(RTLD_NEXT, "calloc");
	_rf_.__real_posix_memalign = dlsym(RTLD_NEXT, "posix_memalign");
	_rf_.__real_aligned_alloc = dlsym(RTLD_NEXT, "aligned_alloc");
	_rf_.__real_memalign = dlsym(RTLD_NEXT, "memalign");
	_rf_.__real_valloc = dlsym(RTLD_NEXT, "valloc");
	_rf_.__real_pvalloc = dlsym(RTLD_NEXT, "pvalloc");
	_rf_.__real_realloc = dlsym(RTLD_NEXT, "realloc");
	_rf_.__real_free = dlsym(RTLD_NEXT, "free");
	_rf_.__real_refer_alloc = __impl_refer_alloc;
	_rf_.__real_refer_alloz = __impl_refer_alloz;
	_rf_.__real_refer_free = __impl_refer_free;
	_rf_.__real_refer_set_free = __impl_refer_set_free;
	_rf_.__real_refer_get_free = __impl_refer_get_free;
	_rf_.__real_refer_save = __impl_refer_save;
	_rf_.__real_refer_save_number = __impl_refer_save_number;
	if ((p = refer_pool_alloc(4096)))
		pool = p;
}

_kira_ _fini_ void __fini_refer_debug(void)
{
	p(__fini_refer_debug);
	refer_pool_s *restrict p;
	p = pool;
	pool = NULL;
	if (p)
	{
		refer_pool_stop(p);
		refer_debug_print(p);
		_rf_.__real_refer_free(p);
	}
}

// malloc

_kira_ void *malloc(size_t size)
{
	p(malloc);
	void *ptr;
	if ((ptr = _rf_.__real_malloc(size)))
		refer_pool_push__malloc(pool, __builtin_return_address(0), ptr, size);
	return ptr;
}

_kira_ void *calloc(size_t nmemb, size_t size)
{
	p(calloc);
	void *ptr;
	if ((ptr = _rf_.__real_calloc(nmemb, size)))
		refer_pool_push__calloc(pool, __builtin_return_address(0), ptr, nmemb, size);
	return ptr;
}

_kira_ int posix_memalign(void **memptr, size_t alignment, size_t size)
{
	p(posix_memalign);
	void *ptr;
	int result;
	if (!(result = _rf_.__real_posix_memalign(memptr, alignment, size)) && (ptr = *memptr))
		refer_pool_push__posix_memalign(pool, __builtin_return_address(0), ptr, alignment, size);
	return result;
}

_kira_ void *aligned_alloc(size_t alignment, size_t size)
{
	p(aligned_alloc);
	void *ptr;
	if ((ptr = _rf_.__real_aligned_alloc(alignment, size)))
		refer_pool_push__aligned_alloc(pool, __builtin_return_address(0), ptr, alignment, size);
	return ptr;
}

_kira_ void *memalign(size_t alignment, size_t size)
{
	p(memalign);
	void *ptr;
	if ((ptr = _rf_.__real_memalign(alignment, size)))
		refer_pool_push__memalign(pool, __builtin_return_address(0), ptr, alignment, size);
	return ptr;
}

_kira_ void *valloc(size_t size)
{
	p(valloc);
	void *ptr;
	if ((ptr = _rf_.__real_valloc(size)))
		refer_pool_push__valloc(pool, __builtin_return_address(0), ptr, size);
	return ptr;
}

_kira_ void *pvalloc(size_t size)
{
	p(pvalloc);
	void *ptr;
	if ((ptr = _rf_.__real_pvalloc(size)))
		refer_pool_push__pvalloc(pool, __builtin_return_address(0), ptr, size);
	return ptr;
}

_kira_ void *realloc(void *ptr, size_t size)
{
	p(realloc);
	void *nptr;
	if ((nptr = _rf_.__real_realloc(ptr, size)))
		refer_pool_push__realloc(pool, __builtin_return_address(0), nptr, ptr, size);
	return nptr;
}

_kira_ void free(void *ptr)
{
	p(free);
	if (ptr) refer_pool_push__free(pool, __builtin_return_address(0), ptr);
	_rf_.__real_free(ptr);
}

// refer

_kira_ refer_t refer_alloc(size_t size)
{
	p(refer_alloc);
	refer_t ptr;
	if ((ptr = _rf_.__real_refer_alloc(size)))
		refer_pool_push__refer_alloc(pool, __builtin_return_address(0), ptr, size);
	return ptr;
}

_kira_ refer_t refer_alloz(size_t size)
{
	p(refer_alloz);
	refer_t ptr;
	if ((ptr = _rf_.__real_refer_alloz(size)))
		refer_pool_push__refer_alloz(pool, __builtin_return_address(0), ptr, size);
	return ptr;
}

_kira_ refer_t refer_free(refer_t v)
{
	p(refer_free);
	refer_pool_push__refer_free(pool, __builtin_return_address(0), v);
	return _rf_.__real_refer_free(v);
}

_kira_ void refer_set_free(refer_t v, refer_free_f free_func)
{
	p(refer_set_free);
	print_address("free_func", free_func);
	refer_pool_push__refer_set_free(pool, __builtin_return_address(0), v, free_func);
	_rf_.__real_refer_set_free(v, free_func);
}

_kira_ refer_free_f refer_get_free(refer_t v)
{
	p(refer_get_free);
	refer_pool_push__refer_get_free(pool, __builtin_return_address(0), v);
	return _rf_.__real_refer_get_free(v);
}

_kira_ refer_t refer_save(refer_t v)
{
	p(refer_save);
	refer_pool_push__refer_save(pool, __builtin_return_address(0), v);
	return _rf_.__real_refer_save(v);
}

_kira_ uint64_t refer_save_number(refer_t v)
{
	p(refer_save_number);
	refer_pool_push__refer_save_number(pool, __builtin_return_address(0), v);
	return _rf_.__real_refer_save_number(v);
}
