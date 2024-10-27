#ifndef _miko_base_miko_icpz_h_
#define _miko_base_miko_icpz_h_

#include "miko.refer.h"

typedef struct miko_icpz_r_t miko_icpz_r_t;
typedef struct miko_icpz_w_s miko_icpz_w_s;

struct miko_icpz_r_t {
	const uint8_t *icpz;
	uintptr_t size;
	uintptr_t pos;
};

miko_icpz_r_t* miko_icpz_r_initial(miko_icpz_r_t *restrict r, const uint8_t *restrict icpz, uintptr_t size);
miko_icpz_r_t* miko_icpz_r_get(miko_icpz_r_t *restrict r, uintptr_t *restrict index);

miko_icpz_w_s* miko_icpz_w_alloc(void);
miko_icpz_w_s* miko_icpz_w_put(miko_icpz_w_s *restrict r, uintptr_t index);
const uint8_t* miko_icpz_w_data(const miko_icpz_w_s *restrict r, uintptr_t *restrict size);

#endif
