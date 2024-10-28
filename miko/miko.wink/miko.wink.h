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
	miko_list_t inode;          // this must modify by miko.wink.gomi
	miko_wink_link_s *link;     // save wink link
	refer_free_f free;          // user free func
	volatile uint32_t status;   // this must modify by miko.wink.gomi
	volatile uint32_t visible;  // this must modify by miko.wink.gomi
};

#define miko_wink_modify_flag            (~((~(uintptr_t) 0) >> 1))
#define miko_wink_batch_modify(_batch_)  ((_batch_) | miko_wink_modify_flag)

#endif
