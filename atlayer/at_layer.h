#ifndef _at_layer_h_
#define _at_layer_h_

#include <stdint.h>

typedef struct at_layer_t at_layer_t;
typedef struct at_op_t at_op_t;
typedef struct at_node_t at_node_t;
typedef void (*at_free_f)(void *restrict data);
typedef at_node_t* (*at_node_run_f)(at_node_t *restrict node, at_layer_t *restrict layer, void *env, void *node_data);
typedef void (*at_node_transform_f)(at_node_t *restrict *restrict node, at_layer_t *restrict layer, void *env);

struct at_op_t {
	uintptr_t type;
	uintptr_t slots_number;
	at_node_run_f run;
	at_free_f free;
	void *data;
};

struct at_node_t {
	at_op_t *op;
	at_node_t *parent;
	at_node_t *next;
	at_node_t *last;
	at_node_t **slots;
	uintptr_t slots_number;
	at_free_f free;
	void *data;
};

typedef struct at_layer_param_t {
	uint32_t op_size;                 // 操作符 id 上限 (最大操作符数量)
	uint32_t op_cache_count;          // at_op_t 缓存批量申请数量
	uint32_t node_cache_count;        // at_node_t 缓存批量申请数量
	uint32_t node_slot_cache_length;  // 可被缓存的 slots_number 上限
	uint32_t node_slot_cache_count;   // slots 缓存数组批量申请数量
} at_layer_param_t;

/// @brief define at_node_run_f
/// @param _name function name
/// @param _env_t type of env | void *
/// @param _node_data_t type of node_data | void *
/// @return at_node_t* (*)(at_node_t *restrict node, at_layer_t *restrict layer, void *env, void *node_data)
#define at_node_run_f$def(_name, _env_t, _node_data_t)  at_node_t* _name(at_node_t *restrict node, at_layer_t *restrict layer, _env_t env, _node_data_t node_data)

/// @brief alloc at_layer_t
/// @param param maybe NULL
/// @return at_layer_t
at_layer_t* at_layer_alloc(const at_layer_param_t *restrict param);
/// @brief free at_layer_t
/// @param layer must not NULL
void at_layer_free(at_layer_t *restrict layer);
/// @brief 测试 at_layer_t 缓存泄漏块数量
/// @param layer must not NULL
/// @param op_b_leak maybe NULL, return block leak of at_op_t
/// @param node_b_leak maybe NULL, return block leak of at_node_t
/// @param slots_b_leak maybe NULL, return block leak of at_node_t.slots
/// @return block leak of (at_op_t + at_node_t + at_node_t.slots)
uintptr_t at_layer_block_leak(const at_layer_t *restrict layer, uintptr_t *restrict op_b_leak, uintptr_t *restrict node_b_leak, uintptr_t *restrict slots_b_leak);

/// @brief 设置(替换) type 的操作符
at_layer_t* at_layer_op_set(at_layer_t *restrict layer, uintptr_t type, uintptr_t slots_number, at_node_run_f run_func, at_free_f free_func, void *data);
/// @brief 解除 type 的操作符
void at_layer_op_unset(at_layer_t *restrict layer, uintptr_t type);
/// @brief 清空所有的操作符
void at_layer_op_clear(at_layer_t *restrict layer);

/// @brief 从 layer 中申请一个类型为 type 的操作节点
at_node_t* at_layer_node_alloc(at_layer_t *restrict layer, uintptr_t type, at_free_f free_func, void *data);
/// @brief 向 layer 释放节点 node
void at_layer_node_free(at_layer_t *restrict layer, at_node_t *restrict node);

/// @brief 释放节点 node 的所有下级节点，并设置 node->slots_number := 0
void at_layer_node_slots_clear(at_layer_t *restrict layer, at_node_t *restrict node);
/// @brief 重建节点 node 的下级节点数量，会释放原有的下级节点
at_node_t* at_layer_node_slots_rebuild(at_layer_t *restrict layer, at_node_t *restrict node, uintptr_t slots_number);
/// @brief 设置(替换) node 的第 index 下级节点，原先的下级节点会被释放
at_node_t* at_layer_node_slots_set(at_layer_t *restrict layer, at_node_t *restrict node, uintptr_t index, at_node_t *restrict v);
/// @brief 解除 node 的第 index 下级节点，原先的下级节点会被释放
void at_layer_node_slots_unset(at_layer_t *restrict layer, at_node_t *restrict node, uintptr_t index);

/// @brief 对 node 节点链中的每一节点按顺序执行该节点类型对应的操作，返回最后一个节点执行的返回值，所有中间结果将被忽略和释放
at_node_t* at_layer_node_run(at_layer_t *restrict layer, at_node_t *restrict node, void *env);

/// @brief 向 node 节点链最后追加 v 节点链
/// @param node 节点链链头
/// @param v 节点链链头，无父节点
/// @param tail_cache 利用/返回 保存的节点链链尾，可为空
void at_node_append(at_node_t *restrict node, at_node_t *restrict v, at_node_t *restrict *restrict tail_cache);
/// @brief 向 node 节点之后添加 v 节点链
/// @param node 节点链链项
/// @param v 节点链链头，无父节点
void at_node_insert_after(at_node_t *restrict node, at_node_t *restrict v);
/// @brief 向 node 节点之前添加 v 节点链，如果 node 为链头会转移相应的地位给 v
/// @param node 节点链链项
/// @param v 节点链链头，无父节点
void at_node_insert_before(at_node_t *restrict node, at_node_t *restrict v);
/// @brief 从 node 节点链中取出 node 链项，如果 node 为链头会尝试转移相应的地位下一链项
/// @param node 节点链链项
/// @param p_list 返回节点链链头，无父节点
at_node_t* at_node_takeout(at_node_t *restrict node, at_node_t *restrict *restrict p_list);

/// @brief 取出 node 的第 index 下级节点，之后 node 的第 index 下级节点会为 NULL
at_node_t* at_node_slots_takeout(at_node_t *restrict node, uintptr_t index);

/// @brief 遍历节点树，寻找节点树起始节点
/// @param node 节点树根节点
at_node_t* at_node_tree_first(const at_node_t *restrict node);
/// @brief 遍历节点树，寻找节点树下一节点
/// @param node 节点树节点 (at_node_tree_first 或 at_node_tree_next 返回值)
at_node_t* at_node_tree_next(const at_node_t *restrict node);

/// @brief 获取 node 节点在父节点(如果有的话)的下级节点位置
/// @param node must not NULL
/// @param index 返回值下级节点位置索引
/// @return 下级节点位置 (NULL => 失败)
at_node_t** at_node_address_by_slots(const at_node_t *restrict node, uintptr_t *restrict index);

#endif
