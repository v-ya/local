#ifndef _miko_wink_miko_wink_search_h_
#define _miko_wink_miko_wink_search_h_

#include "miko.wink.h"
#include <exbuffer.h>
#include <rbtree.h>

struct miko_wink_search_s {
	exbuffer_t wink_array;
	rbtree_t *exist;
};

miko_wink_search_s* miko_wink_search_alloc(void);
miko_wink_search_s* miko_wink_search_append(miko_wink_search_s *restrict r, miko_wink_w *restrict wink);
miko_wink_w** miko_wink_search_data(const miko_wink_search_s *restrict r, uintptr_t *restrict count);
void miko_wink_search_clear(miko_wink_search_s *restrict r);

#endif
