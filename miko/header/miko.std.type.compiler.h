#ifndef _miko_std_type_compiler_h_
#define _miko_std_type_compiler_h_

#include "miko.type.base.h"

typedef struct miko_std_compiler_s miko_std_compiler_s;
typedef struct miko_std_compiler_spec_index_t miko_std_compiler_spec_index_t;

struct miko_std_compiler_spec_index_t {
	miko_index_t name;
	miko_index_t keyword;
	miko_index_t string;
	miko_index_t multichar;
	miko_index_t number;
	miko_index_t operator;
	miko_index_t comma;
	miko_index_t semicolon;
	miko_index_t scope;
	miko_index_t param;
	miko_index_t index;
};

#endif
