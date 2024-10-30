#ifndef _miko_api_minor_h_
#define _miko_api_minor_h_

#include "miko.type.minor.h"

void miko_minor_free_func(miko_minor_s *restrict r);
miko_minor_s* miko_minor_alloc(const char *restrict name, uintptr_t size);

#endif
