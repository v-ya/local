#ifndef _miko_base_miko_strpool_h_
#define _miko_base_miko_strpool_h_

#include "miko.h"
#include <hashmap.h>

typedef struct miko_strpool_item_t miko_strpool_item_t;
typedef struct miko_strpool_s miko_strpool_s;

struct miko_strpool_item_t {
	refer_string_t string;
	uintptr_t length;
	miko_count_t count;
};

struct miko_strpool_s {
	hashmap_t strpool;
};

#endif
