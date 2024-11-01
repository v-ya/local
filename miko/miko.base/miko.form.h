#ifndef _miko_base_miko_form_h_
#define _miko_base_miko_form_h_

#include "miko.h"
#include <exbuffer.h>

typedef struct miko_form_impl_w miko_form_impl_w;

struct miko_form_impl_w {
	miko_form_w form;
	exbuffer_t buffer;
	miko_count_t limit;
};

#endif
