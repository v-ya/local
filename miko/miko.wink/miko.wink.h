#ifndef _miko_wink_miko_wink_h_
#define _miko_wink_miko_wink_h_

#include "../miko.wink.h"
#include "miko.list.h"

#define refer_ck_free(_refer_)            if (_refer_) refer_free(_refer_)
#define refer_hook_free(_refer_, _type_)  refer_set_free(_refer_, (refer_free_f) miko_wink_##_type_##_free_func)

typedef volatile uintptr_t miko_wink_batch_t;
typedef enum miko_wink_status_t miko_wink_status_t;
typedef enum miko_wink_visible_t miko_wink_visible_t;

typedef struct miko_wink_s miko_wink_s;
typedef struct miko_wink_link_s miko_wink_link_s;
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

struct miko_wink_s {
	miko_list_t inode;          // mount at miko.wink.gomi.root
	miko_wink_link_s *link;     // save wink link
	refer_free_f free;          // user free func
	volatile uint32_t status;   // this must modify by miko.wink.gomi
	volatile uint32_t visible;  // this must modify by miko.wink.gomi
};

// ep. A -link> x, B -unlink> x
// (gomi) searched A or unsearch A
// (user) A -link> x
// (user) B -unlink> x
// (gomi) search B
// (gomi) free x
// must confirm free before search x
// (gomi) searched A or unsearch A
// (user) A -link> x
// (gomi) search okay (searched B)
// (gomi) free lost (B -link> x)
// (user) push x to search
// (user) B -unlink> x
// (gomi) will next to search

#endif
