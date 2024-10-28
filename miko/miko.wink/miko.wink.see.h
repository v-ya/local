#ifndef _miko_wink_miko_wink_see_h_
#define _miko_wink_miko_wink_see_h_

#include "miko.wink.h"

typedef struct miko_wink_see_s miko_wink_see_s;

struct miko_wink_see_s {
	miko_list_t inode;  // mount at miko.wink.gomi.search
	miko_wink_s *wink;  // miko.wink.link.wink's write lock
};

void miko_wink_see_wink_to_gomi(miko_wink_gomi_s *restrict gomi, miko_wink_s *restrict wink);

#endif
