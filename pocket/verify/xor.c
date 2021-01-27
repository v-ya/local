#include "layer.h"
#include <memory.h>

#define def$calc(version, type, shift) \
	static type pocket_verify$xor$##version##$calc(const type *data, size_t size)\
	{\
		size_t n;\
		type xor, res;\
		n = size >> shift;\
		size &= (version - 1);\
		xor = res = 0;\
		while (n)\
		{\
			xor ^= *data++;\
			--n;\
		}\
		if (size)\
		{\
			memcpy(&res, data, size);\
			xor ^= res;\
		}\
		return xor;\
	}

#define def$build(version, type) \
	static const pocket_verify_entry_s* pocket_verify$xor$##version##$build(const pocket_verify_entry_s *restrict entry, type *verify, const type *data, uint64_t size)\
	{\
		*verify ^= pocket_verify$xor$##version##$calc(data, (size_t) size);\
		return entry;\
	}

#define def$check(version, type) \
	static const pocket_verify_entry_s* pocket_verify$xor$##version##$check(const pocket_verify_entry_s *restrict entry, void *verify, const type *data, uint64_t size)\
	{\
		if (!pocket_verify$xor$##version##$calc(data, (size_t) size))\
			return entry;\
		return NULL;\
	}

#define def(version, tag) \
	const pocket_verify_entry_s* pocket_verify$xor$##version(void)\
	{\
		return pocket_verify_entry_alloc(\
			(pocket_verify_build_f) pocket_verify$xor$##version##$build,\
			(pocket_verify_check_f) pocket_verify$xor$##version##$check,\
			version,\
			pocket_tag$##tag,\
			0\
		);\
	}\

def$calc(1, uint8_t, 0)
def$calc(2, uint16_t, 1)
def$calc(4, uint32_t, 2)
def$calc(8, uint64_t, 3)

def$build(1, uint8_t)
def$build(2, uint16_t)
def$build(4, uint32_t)
def$build(8, uint64_t)

def$check(1, uint8_t)
def$check(2, uint16_t)
def$check(4, uint32_t)
def$check(8, uint64_t)

def(1, u8)
def(2, u16)
def(4, u32)
def(8, u64)
