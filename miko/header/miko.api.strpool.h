#ifndef _miko_api_strpool_h_
#define _miko_api_strpool_h_

#include "miko.type.strpool.h"

miko_strpool_s_t miko_strpool_alloc(void);
refer_string_t miko_strpool_save(miko_strpool_s_t r, const char *restrict name);
void miko_strpool_undo(miko_strpool_s_t r, const char *restrict name);

#endif
