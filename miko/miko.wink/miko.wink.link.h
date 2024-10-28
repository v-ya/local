#ifndef _miko_wink_miko_wink_link_h_
#define _miko_wink_miko_wink_link_h_

#include "miko.wink.h"
#include <rbtree.h>
#include <yaw.h>

struct miko_wink_link_s {
	yaw_lock_s *read;                // miko.wink.link.wink's read lock
	yaw_lock_s *write;               // miko.wink.link.wink's write lock
	rbtree_t *wink;                  // (uint64_t) (miko_wink_s *) wink => ((uintptr_t) rbv->value) refcount
	const miko_wink_batch_t *batch;  // &miko.wink.gomi.batch
	miko_wink_batch_t modify;        // if try link, parent and child set it
};

miko_wink_link_s* miko_wink_link_alloc(const miko_wink_batch_t *restrict batch);
rbtree_t* miko_wink_link_insert(miko_wink_link_s *restrict r, miko_wink_s *restrict item);
void miko_wink_link_delete(miko_wink_link_s *restrict r, miko_wink_s *restrict item);
void miko_wink_link_call(miko_wink_link_s *restrict r, rbtree_func_call_f call_func, void *call_data);

#endif
