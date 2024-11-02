#ifndef _miko_type_base_h_
#define _miko_type_base_h_

#include <stdint.h>

typedef uintptr_t miko_bool_t;          // 0: false, other true
typedef uintptr_t miko_index_t;         // vaild >  0
typedef uintptr_t miko_count_t;         // vaild >  0
typedef uintptr_t miko_offset_t;        // vaild >= 0
typedef uintptr_t miko_xseg_t;          // vaild >  0
typedef uintptr_t miko_xpos_t;          // vaild >= 0
typedef miko_index_t miko_interrupt_t;  // 0 is okay, other is interrupt type, (ep. div 0)

typedef void (*miko_func_f)(void);

#define miko_interrupt_none  0

#endif
