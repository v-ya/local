#ifndef _elfin_inner_item_func_h_
#define _elfin_inner_item_func_h_

#include <refer.h>
#include "elfin.def.h"

struct elfin_item_s;

#define d_item(_id, ...) typedef struct elfin_item_s* (*elfin_item_func__##_id##_f)(struct elfin_item_s *restrict item, ##__VA_ARGS__);
#include "inner.item.func.inc"
#undef d_item

#define d_item(_id, ...)  elfin_item_func_id__##_id,
enum elfin_item_func_id_t {
	#include "inner.item.func.inc"
	elfin_item_func_id_number
};
#undef d_item

#define d_item(_id, ...)  elfin_item_func__##_id##_f _id;
struct elfin_item_func_t {
	#include "inner.item.func.inc"
};
#undef d_item

#endif
