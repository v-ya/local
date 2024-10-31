#ifndef _miko_type_form_h_
#define _miko_type_form_h_

#include "miko.type.var.h"
#include "miko.type.major.h"
#include "miko.wink.h"

typedef struct miko_form_t miko_form_t;
typedef struct miko_form_w miko_form_w;
typedef miko_form_t **miko_form_table_t;
typedef struct miko_form_table_s miko_form_table_s;

struct miko_form_t {
	miko_var_t var;   // store var
	uintptr_t vtype;  // const miko.major.vtype
};

struct miko_form_w {
	miko_form_t *form;
	uintptr_t count;
};

// don't at this call (miko_wink_used or miko_wink_ref) by (miko_form_t *)
struct miko_form_table_s {
	miko_form_table_t table_array;
	uintptr_t table_count;
};

#endif
