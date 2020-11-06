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
	at_node_t **slots;
	uintptr_t slots_number;
	at_free_f free;
	void *data;
};

typedef struct at_layer_param_t {
	uint32_t op_size;
	uint32_t op_cache_count;
	uint32_t node_cache_count;
	uint32_t node_slot_cache_length;
	uint32_t node_slot_cache_count;
} at_layer_param_t;

#define at_node_run_f$def(_name, _env_t, _node_data_t)  at_node_t* _name(at_node_t *restrict node, at_layer_t *restrict layer, _env_t env, _node_data_t node_data)

at_layer_t* at_layer_alloc(const at_layer_param_t *restrict param);
void at_layer_free(at_layer_t *restrict layer);
uintptr_t at_layer_block_leak(const at_layer_t *restrict layer, uintptr_t *restrict op_b_leak, uintptr_t *restrict node_b_leak, uintptr_t *restrict slots_b_leak);

at_layer_t* at_layer_op_set(at_layer_t *restrict layer, uintptr_t type, uintptr_t slots_number, at_node_run_f run_func, at_free_f free_func, void *data);
void at_layer_op_unset(at_layer_t *restrict layer, uintptr_t type);
void at_layer_op_clear(at_layer_t *restrict layer);

at_node_t* at_layer_node_alloc(at_layer_t *restrict layer, uintptr_t type, at_free_f free_func, void *data);
void at_layer_node_free(at_layer_t *restrict layer, at_node_t *restrict node);
void at_layer_node_slots_clear(at_layer_t *restrict layer, at_node_t *restrict node);
at_node_t* at_layer_node_slots_rebuild(at_layer_t *restrict layer, at_node_t *restrict node, uintptr_t slots_number);
at_node_t* at_layer_node_slots_set(at_layer_t *restrict layer, at_node_t *restrict node, uintptr_t index, at_node_t *restrict v);
void at_layer_node_slots_unset(at_layer_t *restrict layer, at_node_t *restrict node, uintptr_t index);

at_node_t* at_layer_node_run(at_layer_t *restrict layer, at_node_t *restrict node, void *env);

at_node_t* at_node_first(const at_node_t *restrict node);
at_node_t* at_node_next(const at_node_t *restrict node);
at_node_t** at_node_address(const at_node_t *restrict node);

#endif
