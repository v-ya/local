#ifndef _miko_type_access_h_
#define _miko_type_access_h_

#include "miko.type.base.h"

typedef struct miko_access_t miko_access_t;
typedef struct miko_access_prefix_t miko_access_prefix_t;

struct miko_access_t {
	miko_index_t index;  // miko_form_t *form = ((miko_form_table_t *) table)[index].locate;
	miko_xpos_t xpos;    // miko_var_t  *var  = &form[xpos].var;
};

// this modifier prefix of a (miko_access_t)
struct miko_access_prefix_t {
	const char *score;  // (valid) xpos-score  (const, form, ...)
	const char *style;  // (valid) xpos-style  (read, write, exec)
	const char *major;  // (valid) major-type  (in miko.iset.type)
	const char *minor;  // (maybe) minor-type  (in major-type)
};

#endif
