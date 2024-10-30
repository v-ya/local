#ifndef _miko_type_var_h_
#define _miko_type_var_h_

#include <stdint.h>
#include <refer.h>
#include "miko.wink.h"

typedef union miko_var_t miko_var_t;

union miko_var_t {
	// int
	int8_t      s8;
	int16_t     s16;
	int32_t     s32;
	int64_t     s64;
	intptr_t    sptr;
	// uint
	uint8_t     u8;
	uint16_t    u16;
	uint32_t    u32;
	uint64_t    u64;
	uintptr_t   uptr;
	// pointer
	void       *ptr;
	refer_t     refer;
	miko_wink_t wink;
};

#endif
