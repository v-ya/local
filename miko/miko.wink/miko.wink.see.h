#ifndef _miko_wink_miko_wink_see_h_
#define _miko_wink_miko_wink_see_h_

#include "miko.wink.h"
#include <stdlib.h>

typedef struct miko_wink_see_t miko_wink_see_t;

struct miko_wink_see_t {
	miko_list_t inode;  // mount at miko.wink.gomi.search
	miko_wink_w *wink;  // miko.wink.link.wink's write lock
};

void miko_wink_see_wink_to_gomi(miko_wink_gomi_s *restrict gomi, miko_wink_w *restrict wink);

#define miko_wink_see_free(_see_)  free(_see_)

#endif
