#ifndef _miko_type_log_h_
#define _miko_type_log_h_

#include <refer.h>

typedef uintptr_t miko_log_into_t;  // vaild < ~(uintptr_t) 0

typedef struct miko_log_s miko_log_s;

typedef enum miko_log_level_t miko_log_level_t;

enum miko_log_level_t {
	miko_log_level__error,
	miko_log_level__waring,
	miko_log_level__notify,
	miko_log_level__debug,
	miko_log_level_max
};

#endif
