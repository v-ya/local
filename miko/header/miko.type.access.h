#ifndef _miko_type_access_h_
#define _miko_type_access_h_

#include "miko.type.base.h"

typedef struct miko_access_t miko_access_t;
typedef struct miko_access_prefix_t miko_access_prefix_t;

struct miko_access_t {
	miko_index_t index;  // miko.form = miko.form.stack[index]
	miko_xpos_t xpos;    // var = miko.form[xpos].var
};

// this modifier prefix of a (miko_access_t)
struct miko_access_prefix_t {
	const char *score;  // (valid) xpos-score  (const, form, ...)
	const char *style;  // (valid) xpos-style  (read, write, exec)
	const char *major;  // (valid) major-type  (in miko.iset.type)
	const char *minor;  // (maybe) minor-type  (in major-type)
};

#endif
