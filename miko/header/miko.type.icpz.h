#ifndef _miko_type_icpz_h_
#define _miko_type_icpz_h_

#include <stdint.h>

typedef struct miko_icpz_r_t miko_icpz_r_t;
typedef struct miko_icpz_w_s miko_icpz_w_s;

struct miko_icpz_r_t {
	const uint8_t *icpz;
	uintptr_t size;
	uintptr_t pos;
};

#endif
