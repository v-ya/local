#ifndef _hashmap_h_
#define _hashmap_h_

#include <stdint.h>

#define hashmap_init_size 	(1<<4)

typedef struct hashmap_vlist_t hashmap_vlist_t;
typedef struct hashmap_t hashmap_t;
typedef void (*hashmap_func_free_t)(hashmap_vlist_t *restrict);
typedef void (*hashmap_func_call_t)(hashmap_vlist_t *restrict, void *);
typedef int (*hashmap_func_isfree_t)(hashmap_vlist_t *restrict, void *);

struct hashmap_vlist_t {
	uint64_t head;
	const char *name;
	void *value;
	hashmap_func_free_t freeFunc;
	hashmap_vlist_t *next;
};

struct hashmap_t {
	uint32_t mask;
	uint32_t number;
	hashmap_vlist_t **map;
};

// 构造析构 hashmap
hashmap_t* hashmap_init(hashmap_t *restrict hm);
void hashmap_uini(hashmap_t *restrict hm);
void hashmap_clear(hashmap_t *restrict hm);
// 申请释放 hashmap
hashmap_t* hashmap_alloc(void);
void hashmap_free(hashmap_t *restrict hm);
// 查找子节点
hashmap_vlist_t* hashmap_find_name(hashmap_t *restrict hm, const char *restrict name);
hashmap_vlist_t* hashmap_find_head(hashmap_t *restrict hm, uint64_t head);
// 删除子节点
void hashmap_delete_name(hashmap_t *restrict hm, const char *restrict name);
void hashmap_delete_head(hashmap_t *restrict hm, uint64_t head);
// 如果没有则添加，有则返回已存在的
hashmap_vlist_t* hashmap_put_name(hashmap_t *restrict hm, const char *restrict name, const void *value, hashmap_func_free_t freeFunc);
hashmap_vlist_t* hashmap_put_head(hashmap_t *restrict hm, uint64_t head, const void *value, hashmap_func_free_t freeFunc);
// 如果没有则添加，有则替换(释放旧的，创建新的)
hashmap_vlist_t* hashmap_set_name(hashmap_t *restrict hm, const char *restrict name, const void *value, hashmap_func_free_t freeFunc);
hashmap_vlist_t* hashmap_set_head(hashmap_t *restrict hm, uint64_t head, const void *value, hashmap_func_free_t freeFunc);
// 返回 value
void* hashmap_get_name(hashmap_t *restrict hm, const char *restrict name);
void* hashmap_get_head(hashmap_t *restrict hm, uint64_t head);
// 遍历每一个子节点调用 callFunc
void hashmap_call(hashmap_t *restrict hm, hashmap_func_call_t callFunc, void *data);
// 遍历每一个子节点调用 isfreeFunc，如果函数返回非零则释放改子节点
void hashmap_isfree(hashmap_t *restrict hm, hashmap_func_isfree_t isfreeFunc, void *data);

#endif
