#ifndef _refer_inner_h_
#define _refer_inner_h_

#include "refer.debug.h"
#define malloc (_rf_.__real_malloc)
#define calloc (_rf_.__real_calloc)
#define posix_memalign (_rf_.__real_posix_memalign)
#define aligned_alloc (_rf_.__real_aligned_alloc)
#define memalign (_rf_.__real_memalign)
#define valloc (_rf_.__real_valloc)
#define pvalloc (_rf_.__real_pvalloc)
#define realloc (_rf_.__real_realloc)
#define free (_rf_.__real_free)
#define refer_alloc (_rf_.__real_refer_alloc)
#define refer_alloz (_rf_.__real_refer_alloz)
#define refer_free (_rf_.__real_refer_free)
#define refer_set_free (_rf_.__real_refer_set_free)
#define refer_get_free (_rf_.__real_refer_get_free)
#define refer_save (_rf_.__real_refer_save)
#define refer_save_number (_rf_.__real_refer_save_number)

#endif
