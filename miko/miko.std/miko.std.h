#ifndef _miko_std_miko_std_h_
#define _miko_std_miko_std_h_

#include "../header/miko.h"
#include "../header/miko.api.h"
#include "../header/miko.type.h"
#include "../header/miko.marco.h"
#include "../header/miko.std.h"
#include "../header/miko.std.api.h"
#include "../header/miko.std.type.h"
#include "../header/miko.std.marco.h"

#define refer_ck_free(_refer_)            if (_refer_) refer_free(_refer_)
#define refer_hook_free(_refer_, _type_)  refer_set_free(_refer_, (refer_free_f) miko_std_##_type_##_free_func)

#endif
