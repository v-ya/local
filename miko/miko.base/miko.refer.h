#ifndef _miko_base_miko_refer_h_
#define _miko_base_miko_refer_h_

#include <refer.h>

#define refer_ck_free(_refer_)            if (_refer_) refer_free(_refer_)
#define refer_hook_free(_refer_, _type_)  refer_set_free(_refer_, (refer_free_f) miko_##_type_##_free_func)

#endif
