#ifndef _refer_h_
#define _refer_h_

#include <stdint.h>
#include <sys/types.h>

typedef void* refer_t;
typedef void (*refer_free_f)(void *restrict v);

refer_t refer_alloc(size_t size);
refer_t refer_alloz(size_t size);
refer_t refer_free(refer_t v);
void refer_set_free(refer_t v, refer_free_f free_func);
refer_t refer_save(refer_t v);
uint64_t refer_save_number(refer_t v);

#endif
