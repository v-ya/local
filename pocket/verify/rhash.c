#include "layer.h"
#include <memory.h>
#include <alloca.h>

#define rsize         4
#define rmask         3
#define rinit_loop_n  16

// *>^~32
static inline uint32_t randseq32_set(register uint32_t r[rsize], register uintptr_t n, uint32_t v)
{
	register uint32_t rs;
	register uint32_t ror;
	rs = r[n];
	rs *= r[n = (n + rmask) & rmask];
	ror = r[n = (n + rmask) & rmask] & 31;
	rs = (rs >> ror) | (rs << (32 - ror));
	rs ^= r[n = (n + rmask) & rmask];
	return (r[n] = ~rs ^ v);
}

// *>^~64
static inline uint64_t randseq64_set(register uint64_t r[rsize], register uintptr_t n, uint64_t v)
{
	register uint64_t rs;
	register uint32_t ror;
	rs = r[n];
	rs *= r[n = (n + rmask) & rmask];
	ror = r[n = (n + rmask) & rmask] & 63;
	rs = (rs >> ror) | (rs << (64 - ror));
	rs ^= r[n = (n + rmask) & rmask];
	return (r[n] = ~rs ^ v);
}

static void rhash32_init(uint32_t *restrict rhash, uintptr_t mask, uint32_t r[rsize], uintptr_t *restrict pi)
{
	uintptr_t i;
	memset(rhash, 0, sizeof(*rhash) * (mask + 1));
	for (i = 0; i < rinit_loop_n; ++i)
		randseq32_set(r, i & rmask, 0);
	*pi = i & rmask;
}

static void rhash64_init(uint64_t *restrict rhash, uintptr_t mask, uint64_t r[rsize], uintptr_t *restrict pi)
{
	uintptr_t i;
	memset(rhash, 0, sizeof(*rhash) * (mask + 1));
	for (i = 0; i < rinit_loop_n; ++i)
		randseq64_set(r, i & rmask, 0);
	*pi = i & rmask;
}

static void rhash32_final(uint32_t *restrict rhash, uintptr_t mask, uintptr_t j, uint32_t r[rsize], uintptr_t i)
{
	uint32_t xor = 0;
	uintptr_t k;
	k = mask + 1;
	do {
		xor ^= rhash[--k];
	} while (k);
	for (k = 0; k <= mask; ++k, i = (i + 1) & rmask)
		xor ^= (rhash[(j + k) & mask] ^= randseq32_set(r, i, xor));
}

static void rhash64_final(uint64_t *restrict rhash, uintptr_t mask, uintptr_t j, uint64_t r[rsize], uintptr_t i)
{
	uint64_t xor = 0;
	uintptr_t k;
	k = mask + 1;
	do {
		xor ^= rhash[--k];
	} while (k);
	for (k = 0; k <= mask; ++k, i = (i + 1) & rmask)
		xor ^= (rhash[(j + k) & mask] ^= randseq64_set(r, i, xor));
}

static void rhash32_calc(register uint32_t *restrict rhash, uintptr_t mask, uintptr_t *restrict p, uint32_t r[rsize], uintptr_t *restrict pi, const uint32_t *restrict data, register uintptr_t size)
{
	register uintptr_t i, j;
	j = *p;
	switch (i = *pi & rmask)
	{
		do {
			case 0:
				rhash[j] ^= randseq32_set(r, i++, *data++);
				j = (j + 1) & mask;
				if (!--size) goto label_break;
				// fall through
			case 1:
				rhash[j] ^= randseq32_set(r, i++, *data++);
				j = (j + 1) & mask;
				if (!--size) goto label_break;
				// fall through
			case 2:
				rhash[j] ^= randseq32_set(r, i++, *data++);
				j = (j + 1) & mask;
				if (!--size) goto label_break;
				// fall through
			case 3:
				rhash[j] ^= randseq32_set(r, i, *data++);
				i = 0;
				j = (j + 1) & mask;
		} while (--size);
	}
	label_break:
	*p = j;
	*pi = i;
}

static void rhash64_calc(register uint64_t *restrict rhash, uintptr_t mask, uintptr_t *restrict p, uint64_t r[rsize], uintptr_t *restrict pi, const uint64_t *restrict data, register uintptr_t size)
{
	register uintptr_t i, j;
	j = *p;
	switch (i = *pi & rmask)
	{
		do {
			case 0:
				rhash[j] ^= randseq64_set(r, i++, *data++);
				j = (j + 1) & mask;
				if (!--size) goto label_break;
				// fall through
			case 1:
				rhash[j] ^= randseq64_set(r, i++, *data++);
				j = (j + 1) & mask;
				if (!--size) goto label_break;
				// fall through
			case 2:
				rhash[j] ^= randseq64_set(r, i++, *data++);
				j = (j + 1) & mask;
				if (!--size) goto label_break;
				// fall through
			case 3:
				rhash[j] ^= randseq64_set(r, i, *data++);
				i = 0;
				j = (j + 1) & mask;
		} while (--size);
	}
	label_break:
	*p = j;
	*pi = i;
}

static void rhash32(uint32_t *restrict rhash, uintptr_t n, const uint8_t *restrict data, uintptr_t size)
{
	uint32_t r[rsize] = {0};
	uint32_t res;
	uintptr_t i, j, l;
	--n;
	rhash32_init(rhash, n, r, &i);
	j = 0;
	l = size >> 2;
	size &= 3;
	if (l) rhash32_calc(rhash, n, &j, r, &i, (const uint32_t *) data, l);
	if (size)
	{
		res = 0;
		memcpy(&res, (const uint32_t *) data + l, size);
		rhash32_calc(rhash, n, &j, r, &i, &res, 1);
	}
	rhash32_final(rhash, n, j, r, i);
}

static void rhash64(uint64_t *restrict rhash, uintptr_t n, const uint8_t *restrict data, uintptr_t size)
{
	uint64_t r[rsize] = {0};
	uint64_t res;
	uintptr_t i, j, l;
	--n;
	rhash64_init(rhash, n, r, &i);
	j = 0;
	l = size >> 3;
	size &= 7;
	if (l) rhash64_calc(rhash, n, &j, r, &i, (const uint64_t *) data, l);
	if (size)
	{
		res = 0;
		memcpy(&res, (const uint64_t *) data + l, size);
		rhash64_calc(rhash, n, &j, r, &i, &res, 1);
	}
	rhash64_final(rhash, n, j, r, i);
}

typedef struct pocket_verify_entry$rhash_s {
	pocket_verify_entry_s entry;
	uint32_t bits;
	uint32_t n;
} pocket_verify_entry$rhash_s;

static const pocket_verify_entry_s* pocket_verify$rhash$build(const pocket_verify_entry$rhash_s *restrict r, void *verify, const uint8_t *data, uint64_t size)
{
	uintptr_t n, vsize;
	if ((n = r->n) && (((n >> 2) & 0x0f) << 2) == n)
	{
		if (r->bits == 64)
		{
			uint64_t *rhash = alloca(vsize = sizeof(uint64_t) * n);
			if (rhash)
			{
				memset(verify, 0, vsize);
				rhash64(rhash, n, data, size);
				memcpy(verify, rhash, vsize);
				return &r->entry;
			}
		}
		else if (r->bits == 32)
		{
			uint32_t *rhash = alloca(vsize = sizeof(uint32_t) * n);
			if (rhash)
			{
				memset(verify, 0, vsize);
				rhash32(rhash, n, data, size);
				memcpy(verify, rhash, vsize);
				return &r->entry;
			}
		}
	}
	return NULL;
}

static const pocket_verify_entry_s* pocket_verify$rhash$check(const pocket_verify_entry$rhash_s *restrict r, void *verify, const uint8_t *data, uint64_t size)
{
	uintptr_t n, vsize;
	if ((n = r->n) && (((n >> 2) & 0x0f) << 2) == n)
	{
		if (r->bits == 64)
		{
			uint64_t *rhash = alloca(vsize = sizeof(uint64_t) * n);
			uint64_t *backup = alloca(vsize);
			if (rhash)
			{
				memcpy(backup, verify, vsize);
				memset(verify, 0, vsize);
				rhash64(rhash, n, data, size);
				memcpy(verify, backup, vsize);
				if (!memcmp(rhash, backup, vsize))
					return &r->entry;
			}
		}
		else if (r->bits == 32)
		{
			uint32_t *rhash = alloca(vsize = sizeof(uint32_t) * n);
			uint32_t *backup = alloca(vsize);
			if (rhash)
			{
				memcpy(backup, verify, vsize);
				memset(verify, 0, vsize);
				rhash32(rhash, n, data, size);
				memcpy(verify, backup, vsize);
				if (!memcmp(rhash, backup, vsize))
					return &r->entry;
			}
		}
	}
	return NULL;
}

static const pocket_verify_entry_s* pocket_verify$rhash$create(uint32_t bits, uint32_t n)
{
	pocket_verify_entry$rhash_s *restrict r;
	uint64_t size;
	pocket_tag_t tag;
	if (bits == 32)
	{
		tag = pocket_tag$u32;
		size = sizeof(uint32_t) * n;
	}
	else if (bits == 64)
	{
		tag = pocket_tag$u64;
		size = sizeof(uint64_t) * n;
	}
	else return NULL;
	if ((r = (pocket_verify_entry$rhash_s *) refer_alloc(sizeof(pocket_verify_entry$rhash_s))))
	{
		pocket_verify_entry_init(
			&r->entry,
			(pocket_verify_build_f) pocket_verify$rhash$build,
			(pocket_verify_check_f) pocket_verify$rhash$check,
			size, tag, 0);
		r->bits = bits;
		r->n = n;
	}
	return &r->entry;
}

const pocket_verify_entry_s* pocket_verify$rhash32$4(void) {return pocket_verify$rhash$create(32, 4);}
const pocket_verify_entry_s* pocket_verify$rhash32$8(void) {return pocket_verify$rhash$create(32, 8);}
const pocket_verify_entry_s* pocket_verify$rhash32$16(void) {return pocket_verify$rhash$create(32, 16);}
const pocket_verify_entry_s* pocket_verify$rhash32$32(void) {return pocket_verify$rhash$create(32, 32);}

const pocket_verify_entry_s* pocket_verify$rhash64$4(void) {return pocket_verify$rhash$create(64, 4);}
const pocket_verify_entry_s* pocket_verify$rhash64$8(void) {return pocket_verify$rhash$create(64, 8);}
const pocket_verify_entry_s* pocket_verify$rhash64$16(void) {return pocket_verify$rhash$create(64, 16);}
const pocket_verify_entry_s* pocket_verify$rhash64$32(void) {return pocket_verify$rhash$create(64, 32);}
