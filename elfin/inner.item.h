#ifndef _elfin_inner_item_h_
#define _elfin_inner_item_h_

#include "inner.item.func.h"

struct elfin_item_inst_s;
struct elfin_item_s;

typedef struct elfin_item_s* (*elfin_item_alloc_f)(const struct elfin_item_inst_s *restrict inst);

struct elfin_item_inst_s {
	const char *name;
	const char *type;
	elfin_item_alloc_f alloc;
	uintptr_t reserved;
	struct elfin_item_func_t func;
};

struct elfin_item_s {
	const char *name;
	const struct elfin_item_inst_s *inst;
};

struct elfin_item_inst_s* elfin_inner_alloc_item_inst(void);

struct elfin_item_s* elfin_inner_link_item(struct elfin_item_s *restrict item, const struct elfin_item_inst_s *restrict inst);
void elfin_inner_clear_item(struct elfin_item_s *restrict item);

#endif
