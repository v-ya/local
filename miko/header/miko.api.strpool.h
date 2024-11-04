#ifndef _miko_api_strpool_h_
#define _miko_api_strpool_h_

#include "miko.type.base.h"
#include "miko.type.strpool.h"

miko_strpool_s_t miko_strpool_alloc(void);
// used name, return (refer_string_t) name
refer_string_t miko_strpool_fetch(miko_strpool_s_t r, const char *restrict name);
// used name, return refer_save((refer_string_t) name)
refer_string_t miko_strpool_save(miko_strpool_s_t r, const char *restrict name);
// un-used name
void miko_strpool_undo(miko_strpool_s_t r, const char *restrict name);

miko_strlist_s_t miko_strpool_create_strlist(miko_strpool_s_t r);

miko_index_t miko_strlist_index(miko_strlist_s_t r, const char *restrict name);
const uint8_t* miko_strlist_data(miko_strlist_s_t r, uintptr_t *restrict rsize);

#endif
