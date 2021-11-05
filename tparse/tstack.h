#ifndef _tparse_tstack_h_
#define _tparse_tstack_h_

#include <refer.h>

typedef struct tparse_tstack_s tparse_tstack_s;
typedef void (*tparse_tstack_free_f)(void *restrict stack);

tparse_tstack_s* tparse_tstack_alloc(void);
uintptr_t tparse_tstack_layer_number(tparse_tstack_s *restrict stack);
void* tparse_tstack_layer(tparse_tstack_s *restrict stack, uintptr_t layer_rpos, uintptr_t *restrict rsize);
void* tparse_tstack_push(tparse_tstack_s *restrict stack, uintptr_t size, tparse_tstack_free_f free_func);
void tparse_tstack_pop(tparse_tstack_s *restrict stack);
void tparse_tstack_clear(tparse_tstack_s *restrict stack);

#endif
