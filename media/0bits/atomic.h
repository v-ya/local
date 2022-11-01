#ifndef _media_bits_atomic_h_
#define _media_bits_atomic_h_

#include <stdint.h>

typedef volatile uint32_t media_atomic_u32_t;
typedef volatile uint64_t media_atomic_u64_t;
typedef volatile uintptr_t media_atomic_ptr_t;

#define media_atomic_u32_add(_p, _v)  __sync_add_and_fetch(_p, _v)
#define media_atomic_u64_add(_p, _v)  __sync_add_and_fetch(_p, _v)
#define media_atomic_ptr_add(_p, _v)  __sync_add_and_fetch(_p, _v)

#define media_atomic_u32_sub(_p, _v)  __sync_sub_and_fetch(_p, _v)
#define media_atomic_u64_sub(_p, _v)  __sync_sub_and_fetch(_p, _v)
#define media_atomic_ptr_sub(_p, _v)  __sync_sub_and_fetch(_p, _v)

#define media_atomic_u32_add_fetch(_p, _v)  __sync_add_and_fetch(_p, _v)
#define media_atomic_u64_add_fetch(_p, _v)  __sync_add_and_fetch(_p, _v)
#define media_atomic_ptr_add_fetch(_p, _v)  __sync_add_and_fetch(_p, _v)

#define media_atomic_u32_sub_fetch(_p, _v)  __sync_sub_and_fetch(_p, _v)
#define media_atomic_u64_sub_fetch(_p, _v)  __sync_sub_and_fetch(_p, _v)
#define media_atomic_ptr_sub_fetch(_p, _v)  __sync_sub_and_fetch(_p, _v)

#define media_atomic_u32_fetch_add(_p, _v)  __sync_fetch_and_add(_p, _v)
#define media_atomic_u64_fetch_add(_p, _v)  __sync_fetch_and_add(_p, _v)
#define media_atomic_ptr_fetch_add(_p, _v)  __sync_fetch_and_add(_p, _v)

#define media_atomic_u32_fetch_sub(_p, _v)  __sync_fetch_and_sub(_p, _v)
#define media_atomic_u64_fetch_sub(_p, _v)  __sync_fetch_and_sub(_p, _v)
#define media_atomic_ptr_fetch_sub(_p, _v)  __sync_fetch_and_sub(_p, _v)

#define media_atomic_u32_compare_swap(_p, _c, _v)  __sync_bool_compare_and_swap(_p, _c, _v)
#define media_atomic_u64_compare_swap(_p, _c, _v)  __sync_bool_compare_and_swap(_p, _c, _v)
#define media_atomic_ptr_compare_swap(_p, _c, _v)  __sync_bool_compare_and_swap(_p, _c, _v)

#endif
