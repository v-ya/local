#ifndef _miko_base_miko_wink_h_
#define _miko_base_miko_wink_h_

#include "miko.refer.h"
#include "miko.list.h"

typedef struct miko_wink_gomi_s miko_wink_gomi_s;
typedef struct miko_wink_to_s miko_wink_to_s;
typedef struct miko_wink_s miko_wink_s;

struct miko_wink_s {
	miko_list_t inode;          // this must modify by miko.wink.gomi
	miko_wink_to_s *wink;       // save wink link
	refer_free_f free;          // user free func
	volatile uint32_t status;   // this must modify by miko.wink.gomi
	volatile uint32_t visible;  // this must modify by miko.wink.gomi
};

miko_wink_gomi_s* miko_wink_gomi_alloc(uintptr_t miko_gomi_msec);

// create wink chunk
miko_wink_s* miko_wink_alloc(miko_wink_gomi_s *restrict gomi, uintptr_t size, refer_free_f free);

// link once parent => child
const miko_wink_s* miko_wink_link(miko_wink_s *restrict parent, const miko_wink_s *restrict child);

// unlink once parent => child
void miko_wink_unlink(miko_wink_s *restrict parent, const miko_wink_s *restrict child);

// if it is none-linked, allow miko.wink.gomi free it
void miko_wink_lost(miko_wink_s *restrict r);

#endif
