#ifndef _miko_type_minor_h_
#define _miko_type_minor_h_

#include <refer.h>
#include "miko.type.base.h"

typedef struct miko_minor_s miko_minor_s;

struct miko_minor_s {
	refer_string_t name;  // miko.major.minor.name
	miko_index_t minor;   // miko.major[major].minor[minor]
};

#endif
