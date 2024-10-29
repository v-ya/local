#ifndef _miko_base_miko_vector_h_
#define _miko_base_miko_vector_h_

#include "miko.h"
#include <exbuffer.h>

typedef struct miko_vector_s miko_vector_s;

struct miko_vector_s {
	exbuffer_t vector;
	miko_vector_initial_f initial;
	miko_vector_finally_f finally;
	uintptr_t item_size;
	uintptr_t item_count;
};

#endif
