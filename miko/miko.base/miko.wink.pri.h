#ifndef _miko_base_miko_wink_pri_h_
#define _miko_base_miko_wink_pri_h_

#include "miko.wink.h"
#include <exbuffer.h>
#include <rbtree.h>
#include <yaw.h>

typedef enum miko_wink_status_t miko_wink_status_t;
typedef enum miko_wink_visible_t miko_wink_visible_t;
typedef struct miko_wink_search_s miko_wink_search_s;

enum miko_wink_status_t {
	miko_wink_status__lost,
	miko_wink_status__root
};

enum miko_wink_visible_t {
	miko_wink_visible__lost,
	miko_wink_visible__wink,
	miko_wink_visible__visit,
};

struct miko_wink_search_s {
	exbuffer_t wink_array;
	rbtree_t *exist;
};

struct miko_wink_gomi_s {
	miko_list_t *root;
	miko_wink_search_s *search;
	miko_wink_search_s *cache;
	uintptr_t miko_gomi_msec;
	volatile uintptr_t version;
	yaw_s *daemon;
};

struct miko_wink_to_s {
	yaw_lock_s *read;
	yaw_lock_s *write;
	rbtree_t *wink;     // (uint64_t) (miko_wink_s *) wink => ((uintptr_t) rbv->value) refcount
	const volatile uintptr_t *gomi_version;
	volatile uintptr_t modify_version;
};

#define miko_wink_modify_flag  (~((~(uintptr_t) 0) >> 1))
#define miko_wink_modify_mask  ((~(uintptr_t) 0) >> 1)

#endif
