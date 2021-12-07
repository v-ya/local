#ifndef _elfin_inner_arch_arch_h_
#define _elfin_inner_arch_arch_h_

#include "../inner.inst.h"

struct elfin_item_s* elfin_inner_load_2c_le_64(const struct elfin_inst_s *restrict inst, const uint8_t *restrict data, uintptr_t size);

#endif
