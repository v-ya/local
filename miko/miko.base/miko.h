#ifndef _miko_base_miko_h_
#define _miko_base_miko_h_

#include "../miko.h"
#include "../miko.dev.h"

typedef struct miko_instruction_func_t miko_instruction_func_t;
typedef struct miko_instruction_args_t miko_instruction_args_t;
typedef struct miko_instruction_args_s miko_instruction_args_s;
typedef struct miko_instruction_s miko_instruction_s;

#define refer_ck_free(_refer_)            if (_refer_) refer_free(_refer_)
#define refer_hook_free(_refer_, _type_)  refer_set_free(_refer_, (refer_free_f) miko_##_type_##_free_func)

#endif
