#ifndef _miko_api_icpz_h_
#define _miko_api_icpz_h_

#include "miko.type.icpz.h"

miko_icpz_r_t* miko_icpz_r_initial(miko_icpz_r_t *restrict r, const uint8_t *restrict icpz, uintptr_t size);
miko_icpz_r_t* miko_icpz_r_get(miko_icpz_r_t *restrict r, uintptr_t *restrict index);

miko_icpz_w_s* miko_icpz_w_alloc(void);
miko_icpz_w_s* miko_icpz_w_put(miko_icpz_w_s *restrict r, uintptr_t index);
const uint8_t* miko_icpz_w_data(const miko_icpz_w_s *restrict r, uintptr_t *restrict size);

#endif
