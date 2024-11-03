#ifndef _miko_api_source_h_
#define _miko_api_source_h_

#include "miko.type.source.h"

miko_source_s* miko_source_alloc(const char *restrict name, refer_nstring_t source);
miko_source_pos_t* miko_source_find_by_pos(const miko_source_s *restrict r, miko_source_pos_t *restrict pos, uintptr_t pos_at_source);

#endif
