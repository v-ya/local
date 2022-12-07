#ifndef _media_core_component_h_
#define _media_core_component_h_

#include <refer.h>

#define media_cp_fdct_i32_r8p16  "cp/fdct:i32/r8p16"
#define media_cp_zigzag_8x8      "cp/zigzag/8x8"

refer_t media_component_create__fdct_i32_r8p16(void);
refer_t media_component_create__zigzag_8x8(void);

#endif
