#include "dylink.h"
#include <memory.h>

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
	void *ptr, *plt;
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
		plt = d + h->img_takeup;
		while (n)
		{
			if (export_func(pri, (char *) r + eh->name_offset, *(void **) plt = d + eh->offset, &plt))
				goto Err;
			plt = (uint8_t *) plt + sizeof(void *);
			--n;
			++eh;
		}
	}
	return 0;
	Err:
	return -1;
}

void* m_x86_64_plt_set(void *dst, void *func)
{
	// 48 b8 -- -- -- -- -- --
	// -- -- ff e0 cc cc cc cc
	// mov rax, func
	// jmp rax
	*(uint64_t *) dst = ((uintptr_t) func << 16) | 0xb848UL;
	*((uint64_t *) dst + 1) = ((uintptr_t) func >> 48) | 0xcccccccce0ff0000;
	return dst;
}

int m_x86_64_dylink_set(uint32_t type, void *dst, int64_t addend, void *ptr, void *plt)
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
