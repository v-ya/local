#ifndef _miko_base_miko_list_h_
#define _miko_base_miko_list_h_

#include <stdint.h>

typedef struct miko_list_t miko_list_t;

struct miko_list_t {
	miko_list_t *next;
};

#endif
