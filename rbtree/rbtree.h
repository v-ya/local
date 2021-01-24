#ifndef _rbtree_h_
#define _rbtree_h_

#include <stdint.h>

typedef struct rbtree_t rbtree_t;
typedef void (*rbtree_func_free_f)(rbtree_t *restrict);
typedef void (*rbtree_func_call_f)(rbtree_t *restrict, void *);
typedef int (*rbtree_func_isfree_f)(rbtree_t *restrict, void *);

struct rbtree_t {
	rbtree_t *u;
	rbtree_t *l;
	rbtree_t *r;
	const char *key_name;
	uint64_t key_index;
	uint32_t color;
	uint32_t mode;
	void *value;
	rbtree_func_free_f freeFunc;
};

#define rbtree_color_red        0
#define rbtree_color_black      1

rbtree_t* rbtree_insert(rbtree_t *restrict *restrict pr, const char *restrict name, uint64_t index, const void *value, rbtree_func_free_f freeFunc);
void rbtree_delete_by_pointer(rbtree_t *restrict *restrict pr, rbtree_t *restrict p);
void rbtree_delete(rbtree_t *restrict *restrict pr, const char *restrict name, uint64_t index);
rbtree_t* rbtree_find(rbtree_t *restrict *restrict pr, const char *restrict name, uint64_t index);
void rbtree_clear(rbtree_t *restrict *restrict pr);
void rbtree_call(rbtree_t *restrict *restrict pr, rbtree_func_call_f callFunc, void *data);
void rbtree_isfree(rbtree_t *restrict *restrict pr, rbtree_func_isfree_f callFunc, void *data);

#endif
