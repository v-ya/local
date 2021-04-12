#ifndef _mpeg4_inner_bindata_h_
#define _mpeg4_inner_bindata_h_

#include "box.include.h"

void mpeg4$define(inner, bindata, dump)(mlog_s *restrict mlog, const uint8_t *restrict data, uint64_t size, uintptr_t n, uint32_t level);

#endif
