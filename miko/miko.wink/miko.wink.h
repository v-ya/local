#ifndef _miko_wink_miko_wink_h_
#define _miko_wink_miko_wink_h_

#include "../header/miko.wink.h"
#include "miko.list.h"
#include <yaw.h>

#define refer_ck_free(_refer_)            if (_refer_) refer_free(_refer_)
#define refer_hook_free(_refer_, _type_)  refer_set_free(_refer_, (refer_free_f) miko_wink_##_type_##_free_func)

typedef volatile uintptr_t miko_wink_batch_t;

typedef struct miko_wink_w miko_wink_w;
typedef struct miko_wink_search_s miko_wink_search_s;

struct miko_wink_w {
	miko_list_t inode;
	miko_wink_gomi_s *gomi;
	yaw_lock_s *rlock;
	yaw_lock_s *wlock;
	miko_wink_view_f view;
	miko_wink_free_f free;
	volatile uintptr_t refs;
	uintptr_t visible;
};

#define miko_wink_visible__outside  0
#define miko_wink_visible__inside   1

void miko_wink_w_free(miko_wink_w *restrict r);

#endif
