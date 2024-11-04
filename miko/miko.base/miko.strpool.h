#ifndef _miko_base_miko_strpool_h_
#define _miko_base_miko_strpool_h_

#include "miko.h"
#include <hashmap.h>
#include <exbuffer.h>

typedef struct miko_strpool_item_t miko_strpool_item_t;
typedef struct miko_strpool_s miko_strpool_s;
typedef struct miko_strlist_s miko_strlist_s;

struct miko_strpool_item_t {
	refer_string_t string;
	uintptr_t length;
	miko_count_t count;
};

struct miko_strpool_s {
	hashmap_t strpool;  // name => (miko_strpool_item_t *) item
};

struct miko_strlist_s {
	exbuffer_t data;     // name[]
	hashmap_t strlist;   // name => (miko_index_t) index
	miko_count_t count;  // count(name[])
};

miko_strlist_s* miko_strlist_alloc(void);
miko_strlist_s* miko_strlist_push(miko_strlist_s *restrict r, const char *restrict name, uintptr_t length);

#endif
