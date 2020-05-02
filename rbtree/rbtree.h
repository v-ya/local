#ifndef _rbtree_h_
#define _rbtree_h_

#include <stdint.h>

typedef struct rbtree_t rbtree_t;
typedef void (*rbtree_func_free_t)(rbtree_t *);
typedef void (*rbtree_func_call_t)(rbtree_t *, void *);
typedef int (*rbtree_func_isfree_t)(rbtree_t *, void *);

struct rbtree_t {
	rbtree_t *u;
	rbtree_t *l;
	rbtree_t *r;
	const char *key_name;
	uint64_t key_index;
	uint32_t color;
	uint32_t mode;
	void *value;
	rbtree_func_free_t freeFunc;
};

#define rbtree_color_red        0
#define rbtree_color_black      1

rbtree_t* rbtree_insert(rbtree_t **pr, const char *name, uint64_t index, const void *value, rbtree_func_free_t freeFunc);
void rbtree_delete_by_pointer(rbtree_t **pr, rbtree_t *p);
void rbtree_delete(rbtree_t **pr, const char *name, uint64_t index);
rbtree_t* rbtree_find(rbtree_t **pr, const char *name, uint64_t index);
void rbtree_clear(rbtree_t **pr);
void rbtree_call(rbtree_t **pr, rbtree_func_call_t callFunc, void *data);
void rbtree_isfree(rbtree_t **pr, rbtree_func_isfree_t callFunc, void *data);

#endif
