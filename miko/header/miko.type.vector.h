#ifndef _miko_type_vector_h_
#define _miko_type_vector_h_

#include <refer.h>

typedef struct miko_vector_s *miko_vector_s_t;

typedef void* (*miko_vector_initial_f)(void *restrict v);
typedef void (*miko_vector_finally_f)(void *restrict v);

#endif
