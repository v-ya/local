#ifndef _multicalc_h_
#define _multicalc_h_

#include <stdint.h>

typedef struct multicalc_t multicalc_t;

typedef void (*multicalc_do_f)(void *data);

multicalc_t* multicalc_alloc(uint32_t n, uintptr_t stack_size);
void multicalc_free(multicalc_t *restrict r);
void multicalc_set_func(multicalc_t *restrict r, uint32_t index, multicalc_do_f func);
void multicalc_set_data(multicalc_t *restrict r, uint32_t index, void *data);
void multicalc_set_status(multicalc_t *restrict r, uint32_t index, uintptr_t enable);
void multicalc_set_all_func(multicalc_t *restrict r, multicalc_do_f func);
void multicalc_set_all_data(multicalc_t *restrict r, void *data);
void multicalc_set_all_status(multicalc_t *restrict r, uintptr_t enable);
void multicalc_wake(multicalc_t *restrict r);
void multicalc_wait(multicalc_t *restrict r);

#endif
