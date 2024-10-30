#ifndef _miko_type_form_h_
#define _miko_type_form_h_

#include "miko.type.var.h"
#include "miko.wink.h"

typedef struct miko_form_t miko_form_t;
typedef struct miko_form_w miko_form_w;
typedef struct miko_form_table_t miko_form_table_t;
typedef struct miko_form_table_w miko_form_table_w;

typedef void (*miko_form_free_f)(miko_form_w *restrict r, void *restrict v);

struct miko_form_t {
	miko_var_t var;
	miko_form_free_f vfree;
};

struct miko_form_w {
	miko_form_t *form;
	uintptr_t count;
};

struct miko_form_table_t {
	miko_form_t *locate;
	miko_form_w *inode;
};

struct miko_form_table_w {
	miko_form_table_t *table;
	uintptr_t count;
};

#endif
