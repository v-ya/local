#include "dylink.h"
#include "xmem.h"
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <hashmap.h>

size_t dylink_check(uint8_t *r, size_t size, const char *machine)
{
	dylink_header_s *h;
	dylink_isym_s *ih;
	dylink_esym_s *eh;
	size_t n, a, b, c;
	if (size < sizeof(dylink_header_s)) goto Err;
	h = (dylink_header_s *) r;
	if (h->img_offset > size || (n = size - h->img_offset) < h->img_size) goto Err;
	if (h->strpool_offset > size || (n = size - h->strpool_offset) < h->strpool_size) goto Err;
	if (h->isym_offset > size || (n = size - h->isym_offset) / sizeof(dylink_isym_s) < h->isym_number) goto Err;
	if (h->esym_offset > size || (n = size - h->esym_offset) / sizeof(dylink_esym_s) < h->esym_number) goto Err;
	if (h->img_takeup < h->img_size) goto Err;
	a = h->strpool_offset;
	b = h->strpool_offset + h->strpool_size;
	c = h->img_takeup;
	ih = (dylink_isym_s *) (r + h->isym_offset);
	n = h->isym_number;
	while (n)
	{
		if (ih->name_offset < a || ih->name_offset >= b) goto Err;
		if (ih->offset >= c) goto Err;
		--n;
		++ih;
	}
	eh = (dylink_esym_s *) (r + h->esym_offset);
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

int dylink_link(uint8_t *d, uint8_t *r, dylink_set_f dylink_set, dylink_import_f import_func, dylink_export_f export_func, void *pri)
{
	dylink_header_s *h;
	dylink_isym_s *ih;
	dylink_esym_s *eh;
	void *ptr, **plt;
	uint32_t n, b;
	h = (dylink_header_s *) r;
	memcpy(d, r + h->img_offset, h->img_size);
	ih = (dylink_isym_s *) (r + h->isym_offset);
	n = h->isym_number;
	if (n)
	{
		if (!dylink_set || !import_func) goto Err;
		b = 0;
		ptr = plt = NULL;
		while (n)
		{
			if (ih->name_offset != b)
			{
				b = ih->name_offset;
				if (!(ptr = import_func(pri, (char *) r + ih->name_offset, &plt)))
					goto Err;
			}
			if (dylink_set(ih->type, d + ih->offset, ih->addend, ptr, plt))
				goto Err;
			--n;
			++ih;
		}
	}
	if (export_func)
	{
		eh = (dylink_esym_s *) (r + h->esym_offset);
		n = h->esym_number;
		plt = (void **) (d + h->img_takeup);
		while (n)
		{
			if (export_func(pri, (char *) r + eh->name_offset, *(void **) plt = d + eh->offset, plt))
				goto Err;
			++plt;
			--n;
			++eh;
		}
	}
	return 0;
	Err:
	return -1;
}

void* m_x86_64_dylink_plt_set(void *dst, void *func, void ***plt)
{
	// 48 b8 -- -- -- -- -- --
	// -- -- ff e0 cc cc cc cc
	// mov rax, func
	// jmp rax
	*(uint64_t *) dst = ((uintptr_t) func << 16) | 0xb848UL;
	*((uint64_t *) dst + 1) = ((uintptr_t) func >> 48) | 0xcccccccce0ff0000;
	if (plt) *plt = (void **)((uint8_t *) dst + 2);
	return dst;
}

int m_x86_64_dylink_set(uint32_t type, void *dst, int64_t addend, void *ptr, void **plt)
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

struct dylink_pool_s {
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

dylink_pool_s* dylink_pool_alloc(const char *mechine, dylink_set_f dylink_set, dylink_plt_set_f dylink_plt_set, size_t xmem_size)
{
	dylink_pool_s *dp;
	// 4 KiB align
	xmem_size = (xmem_size >> 12) << 12;
	if (mechine && dylink_set && dylink_plt_set && xmem_size)
	{
		dp = (dylink_pool_s *) calloc(1, sizeof(dylink_pool_s));
		if (dp)
		{
			if (hashmap_init(&dp->symbol) && hashmap_init(&dp->symplt))
			{
				dp->xmemory = xmem_alloc(xmem_size);
				if (dp->xmemory)
				{
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

void dylink_pool_free(dylink_pool_s *dp)
{
	if (dp)
	{
		hashmap_uini(&dp->symbol);
		hashmap_uini(&dp->symplt);
		if (dp->xmemory) xmem_free(dp->xmemory, dp->xmem_size);
		free(dp);
	}
}

void dylink_pool_set_report(dylink_pool_s *dp, dylink_pool_report_f func, void *pri)
{
	dp->report_func = func;
	dp->report_pri = pri;
}

int dylink_pool_set_func(dylink_pool_s *dp, const char *symbol, void *func)
{
	void **plt;
	if (!hashmap_find_name(&dp->symbol, symbol))
	{
		if (dp->xmem_offset + dp->plt_size <= dp->xmem_size)
		{
			func = dp->dylink_plt_set(dp->xmemory + dp->xmem_offset, func, &plt);
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
						dp->xmem_offset += dp->plt_size;
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

void* dylink_pool_get_symbol(dylink_pool_s *dp, const char *symbol, void ***plt)
{
	if (plt) *plt = (void **) hashmap_get_name(&dp->symplt, symbol);
	return hashmap_get_name(&dp->symbol, symbol);
}

void dylink_pool_delete_symbol(dylink_pool_s *dp, const char *symbol)
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

static void* dylink_pool_load_import_func(dylink_pool_s *dp, const char *symbol, void ***plt)
{
	void *ptr;
	*plt = (void **) hashmap_get_name(&dp->symplt, symbol);
	ptr = hashmap_get_name(&dp->symbol, symbol);
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

static int dylink_pool_load_export_func(dylink_pool_s *dp, const char *symbol, void *ptr, void **plt)
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

static void dylink_pool_load_clear_export(dylink_pool_s *dp, uint8_t *r, uint32_t n)
{
	dylink_esym_s *eh;
	eh = (dylink_esym_s *) (r + ((dylink_header_s *) r)->esym_offset);
	while (n)
	{
		dylink_pool_delete_symbol(dp, (char *) (r + eh->name_offset));
		++eh;
		--n;
	}
}

int dylink_pool_load(dylink_pool_s *dp, uint8_t *dylink_data, size_t dylink_size)
{
	size_t dylink_takeup;
	if (dylink_data)
	{
		dylink_takeup = dylink_check(dylink_data, dylink_size, dp->machine);
		if (dylink_takeup && dylink_takeup + dp->plt_size <= dp->xmem_size)
		{
			dp->export_number = 0;
			if (!dylink_link(
				dp->xmemory + dp->xmem_offset,
				dylink_data,
				dp->dylink_set,
				(dylink_import_f) dylink_pool_load_import_func,
				(dylink_export_f) dylink_pool_load_export_func,
				dp
			))
			{
				dp->export_number = 0;
				dp->xmem_offset += dylink_takeup;
				return 0;
			}
			if (dp->export_number)
			{
				dylink_pool_load_clear_export(dp, dylink_data, dp->export_number);
				dp->export_number = 0;;
			}
		}
	}
	return -1;
}

int dylink_pool_load_file(dylink_pool_s *dp, const char *path)
{
	FILE *fp;
	uint8_t *data;
	size_t size;
	int r;
	r = -1;
	if (path)
	{
		fp = fopen(path, "rb");
		if (fp)
		{
			fseek(fp, 0, SEEK_END);
			size = ftell(fp);
			if (size)
			{
				data = malloc(size);
				if (data)
				{
					fseek(fp, 0, SEEK_SET);
					if (fread(data, 1, size, fp) == size)
					{
						r = dylink_pool_load(dp, data, size);
					}
					free(data);
				}
			}
			fclose(fp);
		}
	}
	return r;
}
