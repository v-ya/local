#include "dylink.h"
#include <stdlib.h>
#include <memory.h>
#include <hashmap.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

size_t dylink_check(const uint8_t *restrict r, size_t size, const char *restrict machine)
{
	const dylink_header_t *restrict h;
	const dylink_isym_t *restrict ih;
	const dylink_esym_t *restrict eh;
	size_t n, a, b, c;
	if (size < sizeof(dylink_header_t)) goto Err;
	h = (const dylink_header_t *) r;
	if (h->img_offset > size || (n = size - h->img_offset) < h->img_size) goto Err;
	if (h->strpool_offset > size || (n = size - h->strpool_offset) < h->strpool_size) goto Err;
	if (h->isym_offset > size || (n = size - h->isym_offset) / sizeof(dylink_isym_t) < h->isym_number) goto Err;
	if (h->esym_offset > size || (n = size - h->esym_offset) / sizeof(dylink_esym_t) < h->esym_number) goto Err;
	if (h->img_takeup < h->img_size) goto Err;
	a = h->strpool_offset;
	b = h->strpool_offset + h->strpool_size;
	c = h->img_takeup;
	ih = (const dylink_isym_t *) (r + h->isym_offset);
	n = h->isym_number;
	while (n)
	{
		if (ih->name_offset < a || ih->name_offset >= b) goto Err;
		if (ih->offset >= c) goto Err;
		--n;
		++ih;
	}
	eh = (const dylink_esym_t *) (r + h->esym_offset);
	n = h->esym_number;
	while (n)
	{
		if (eh->name_offset < a || eh->name_offset >= b) goto Err;
		if (eh->offset >= c) goto Err;
		--n;
		++eh;
	}
	return (c + h->esym_number * sizeof(void *) + 0x0f) & ~0x0ful;
	Err:
	return 0;
}

int dylink_link(uint8_t *restrict d, const uint8_t *restrict r, dylink_set_f dylink_set, dylink_import_f import_func, dylink_export_f export_func, void *pri)
{
	const dylink_header_t *restrict h;
	const dylink_isym_t *restrict ih;
	const dylink_esym_t *restrict eh;
	void *ptr, **plt;
	uint32_t n, b;
	h = (const dylink_header_t *) r;
	memcpy(d, r + h->img_offset, h->img_size);
	// export
	if ((n = h->esym_number))
	{
		if (!export_func)
			goto label_fail;
		eh = (const dylink_esym_t *) (r + h->esym_offset);
		plt = (void **) (d + h->img_takeup);
		while (n)
		{
			if (export_func(pri, (char *) r + eh->name_offset, *(void **) plt = d + eh->offset, plt))
				goto label_fail;
			++plt;
			--n;
			++eh;
		}
	}
	// import
	if ((n = h->isym_number))
	{
		if (!dylink_set || !import_func)
			goto label_fail;
		ih = (const dylink_isym_t *) (r + h->isym_offset);
		b = 0;
		ptr = plt = NULL;
		while (n)
		{
			if (ih->name_offset != b)
			{
				b = ih->name_offset;
				if (!(ptr = import_func(pri, (char *) r + ih->name_offset, &plt)))
					goto label_fail;
			}
			if (dylink_set(ih->type, d + ih->offset, ih->addend, ptr, plt))
				goto label_fail;
			--n;
			++ih;
		}
	}
	return 0;
	label_fail:
	return -1;
}

void* m_x86_64_dylink_plt_set(void *restrict dst, void *func, void ***restrict plt)
{
	// ff 25 02 00 00 00 xx xx
	// -- -- -- -- -- -- -- --
	// jmpq *0x2(%rip)
	*(uint64_t *) dst = 0x0225ff;
	*((void **) dst + 1) = func;
	if (plt) *plt = (void **) dst + 1;
	return dst;
}

int m_x86_64_dylink_set(uint32_t type, void *restrict dst, int64_t addend, void *ptr, void **plt)
{
	uint64_t p;
	switch (type)
	{
		case 1:
			*(uint64_t *) dst = (uint64_t) ((uintptr_t) ptr + addend);
			break;
		case 2:
		case 4:
			p = (uint64_t) ((uintptr_t) ptr + addend - (uintptr_t) dst);
			type = (uint32_t) p;
			p = (p >> 31) | 0xffffffff80000000ul;
			if (!p && !~p) goto Err;
			*(uint32_t *) dst = type;
			break;
		case 42:
			if (!plt) goto Err;
			p = (uint64_t) ((uintptr_t) plt + addend - (uintptr_t) dst);
			type = (uint32_t) p;
			p = (p >> 31) | 0xffffffff80000000ul;
			if (!p && !~p) goto Err;
			*(uint32_t *) dst = type;
			break;
		default:
			Err:
			return -1;
	}
	return 0;
}

struct dylink_pool_t {
	dylink_pool_t *restrict entry;
	dylink_pool_t *restrict upper;
	hashmap_t symbol;
	hashmap_t symplt;
	uint8_t *xmemory;
	size_t xmem_size;
	size_t xmem_offset;
	const char *machine;
	dylink_set_f dylink_set;
	dylink_plt_set_f dylink_plt_set;
	size_t plt_size;
	dylink_pool_report_f report_func;
	void *report_pri;
	uint32_t export_number;
};

dylink_pool_t* dylink_pool_alloc(const char *restrict mechine, dylink_set_f dylink_set, dylink_plt_set_f dylink_plt_set, size_t xmem_size)
{
	dylink_pool_t *dp;
	// 4 KiB align
	xmem_size = (xmem_size >> 12) << 12;
	if (mechine && dylink_set && dylink_plt_set && xmem_size)
	{
		dp = (dylink_pool_t *) calloc(1, sizeof(dylink_pool_t));
		if (dp)
		{
			if (hashmap_init(&dp->symbol) && hashmap_init(&dp->symplt))
			{
				dp->xmemory = xmem_alloc(xmem_size);
				if (dp->xmemory)
				{
					dp->entry = dp;
					dp->xmem_size = xmem_size;
					dp->machine = mechine;
					dp->dylink_set = dylink_set;
					dp->dylink_plt_set = dylink_plt_set;
					dp->plt_size = 16;
					return dp;
				}
			}
			hashmap_uini(&dp->symbol);
			hashmap_uini(&dp->symplt);
			free(dp);
		}
	}
	return NULL;
}

void dylink_pool_free(dylink_pool_t *restrict dp)
{
	if (dp)
	{
		hashmap_uini(&dp->symbol);
		hashmap_uini(&dp->symplt);
		if (dp->xmemory) xmem_free(dp->xmemory, dp->xmem_size);
		free(dp);
	}
}

void dylink_pool_set_report(dylink_pool_t *restrict dp, dylink_pool_report_f func, void *pri)
{
	dp->report_func = func;
	dp->report_pri = pri;
}

int dylink_pool_set_func(dylink_pool_t *restrict dp, const char *restrict symbol, void *func)
{
	void **plt;
	if (!hashmap_find_name(&dp->symbol, symbol))
	{
		dylink_pool_t *restrict entry;
		entry = dp->entry;
		if (entry->xmem_offset + entry->plt_size <= entry->xmem_size)
		{
			func = entry->dylink_plt_set(entry->xmemory + entry->xmem_offset, func, &plt);
			if (hashmap_set_name(&dp->symbol, symbol, func, NULL))
			{
				if (hashmap_set_name(&dp->symplt, symbol, plt, NULL))
				{
					if (!dp->report_func || !dp->report_func(
						dp->report_pri,
						dylink_pool_report_type_set_symbol,
						symbol,
						func,
						plt
					))
					{
						entry->xmem_offset += entry->plt_size;
						return 0;
					}
					hashmap_delete_name(&dp->symplt, symbol);
				}
				hashmap_delete_name(&dp->symbol, symbol);
			}
		}
	}
	return -1;
}

void* dylink_pool_get_symbol(const dylink_pool_t *restrict dp, const char *restrict symbol, void ***restrict plt)
{
	void *ptr;
	do {
		if ((ptr = hashmap_get_name(&dp->symbol, symbol)))
		{
			if (plt) *plt = (void **) hashmap_get_name(&dp->symplt, symbol);
			return ptr;
		}
	} while ((dp = dp->upper));
	return NULL;
}

void dylink_pool_delete_symbol(dylink_pool_t *restrict dp, const char *restrict symbol)
{
	if (dp->report_func)
	{
		dp->report_func(
			dp->report_pri,
			dylink_pool_report_type_delete_symbol,
			symbol,
			hashmap_get_name(&dp->symbol, symbol),
			(void **) hashmap_get_name(&dp->symplt, symbol)
		);
	}
	hashmap_delete_name(&dp->symplt, symbol);
	hashmap_delete_name(&dp->symbol, symbol);
}

static void* dylink_pool_load_import_func(dylink_pool_t *restrict dp, const char *restrict symbol, void ***restrict plt)
{
	dylink_pool_t *u;
	void *ptr;
	u = dp;
	do {
		if ((ptr = hashmap_get_name(&u->symbol, symbol)))
		{
			*plt = (void **) hashmap_get_name(&u->symplt, symbol);
			break;
		}
	} while ((u = u->upper));
	if (dp->report_func && dp->report_func(
		dp->report_pri,
		ptr?
			dylink_pool_report_type_import_ok:
			dylink_pool_report_type_import_fail,
		symbol,
		ptr,
		*plt
	))
	{
		*plt = ptr = NULL;
	}
	return ptr;
}

static int dylink_pool_load_export_func(dylink_pool_t *restrict dp, const char *restrict symbol, void *ptr, void **plt)
{
	if (!hashmap_find_name(&dp->symbol, symbol))
	{
		if (hashmap_set_name(&dp->symbol, symbol, ptr, NULL))
		{
			if (hashmap_set_name(&dp->symplt, symbol, plt, NULL))
			{
				if (!dp->report_func || !dp->report_func(
					dp->report_pri,
					dylink_pool_report_type_export_ok,
					symbol,
					ptr,
					plt
				))
				{
					++dp->export_number;
					return 0;
				}
				hashmap_delete_name(&dp->symplt, symbol);
			}
			hashmap_delete_name(&dp->symbol, symbol);
		}
	}
	if (dp->report_func)
	{
		dp->report_func(
			dp->report_pri,
			dylink_pool_report_type_export_fail,
			symbol,
			ptr,
			plt
		);
	}
	return -1;
}

static void dylink_pool_load_clear_export(dylink_pool_t *restrict dp, const uint8_t *restrict r, uint32_t n)
{
	const dylink_esym_t *restrict eh;
	eh = (const dylink_esym_t *) (r + ((const dylink_header_t *) r)->esym_offset);
	while (n)
	{
		dylink_pool_delete_symbol(dp, (char *) (r + eh->name_offset));
		++eh;
		--n;
	}
}

int dylink_pool_load(dylink_pool_t *restrict dp, const uint8_t *restrict dylink_data, size_t dylink_size)
{
	dylink_pool_t *save;
	size_t dylink_takeup, esym_number;
	if (dylink_data)
	{
		save = dp;
		dp = dp->entry;
		dylink_takeup = dylink_check(dylink_data, dylink_size, dp->machine);
		esym_number = ((const dylink_header_t *) dylink_data)->esym_number;
		if (dylink_takeup && dylink_takeup + esym_number * dp->plt_size <= dp->xmem_size)
		{
			dp->export_number = 0;
			if (!dylink_link(
				dp->xmemory + dp->xmem_offset,
				dylink_data,
				dp->dylink_set,
				(dylink_import_f) dylink_pool_load_import_func,
				(dylink_export_f) dylink_pool_load_export_func,
				save
			))
			{
				dp->export_number = 0;
				dp->xmem_offset += dylink_takeup;
				return 0;
			}
			if (dp->export_number)
			{
				dylink_pool_load_clear_export(dp, dylink_data, dp->export_number);
				dp->export_number = 0;
			}
		}
	}
	return -1;
}

int dylink_pool_load_file(dylink_pool_t *restrict dp, const char *restrict path)
{
	struct stat st;
	int fd, r;
	uint8_t *data;
	size_t size;
	r = -1;
	if (path && ~(fd = open(path, O_RDONLY)))
	{
		if (!fstat(fd, &st) && (size = (size_t) st.st_size) &&
			(data = (uint8_t *) malloc(size)))
		{
			if ((size_t) read(fd, data, size) == size)
				r = dylink_pool_load(dp, data, size);
			free(data);
		}
		close(fd);
	}
	return r;
}

dylink_pool_t* dylink_pool_alloc_local(dylink_pool_t *restrict dp)
{
	dylink_pool_t *restrict upper;
	if ((upper = dp))
	{
		dp = (dylink_pool_t *) calloc(1, sizeof(dylink_pool_t));
		if (dp)
		{
			if (hashmap_init(&dp->symbol) && hashmap_init(&dp->symplt))
			{
				dp->entry = upper->entry;
				dp->upper = upper;
				dp->report_func = upper->report_func;
				dp->report_pri = upper->report_pri;
				return dp;
			}
			hashmap_uini(&dp->symbol);
			hashmap_uini(&dp->symplt);
			free(dp);
		}
	}
	return NULL;
}

dylink_pool_t* dylink_pool_upper(dylink_pool_t *restrict dp)
{
	return dp->upper;
}

int dylink_pool_sync_symbol(const dylink_pool_t *restrict dp, const char *restrict symbol, const char *restrict upper_symbol)
{
	dylink_pool_t *restrict upper;
	if ((upper = dp->upper) && symbol)
	{
		void *ptr;
		hashmap_vlist_t *restrict vl;
		if (!upper_symbol) upper_symbol = symbol;
		if ((ptr = hashmap_get_name(&dp->symbol, symbol)) &&
			(vl = hashmap_put_name(&upper->symbol, upper_symbol, ptr, NULL)) &&
			vl->value == ptr)
		{
			if (hashmap_set_name(&upper->symplt, upper_symbol, ptr = hashmap_get_name(&dp->symplt, symbol), NULL))
			{
				if (!dp->report_func || !dp->report_func(
					dp->report_pri,
					dylink_pool_report_type_set_symbol,
					symbol,
					vl->value,
					(void **) ptr
				)) return 0;
				hashmap_delete_name(&upper->symplt, upper_symbol);
			}
			hashmap_delete_name(&upper->symbol, upper_symbol);
		}
	}
	return -1;
}

static void dylink_pool_sync_symbol_all_func(hashmap_vlist_t *restrict vl, const dylink_pool_t *restrict dp)
{
	dylink_pool_sync_symbol(dp, vl->name, NULL);
}

int dylink_pool_sync_symbol_all(const dylink_pool_t *restrict dp)
{
	dylink_pool_t *upper;
	if ((upper = dp->upper))
	{
		uint32_t want;
		want = upper->symbol.number + dp->symbol.number;
		hashmap_call(&dp->symbol, (hashmap_func_call_f) dylink_pool_sync_symbol_all_func, dp);
		if (upper->symbol.number == want)
			return 0;
	}
	return -1;
}
