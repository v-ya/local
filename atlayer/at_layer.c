#include "at_layer.h"
#include "at_cache.h"
#include <stdlib.h>
#include <memory.h>

typedef at_node_t *at_node_ptr_t;

struct at_layer_t {
	at_op_t **op_list;
	uintptr_t op_size;
	uintptr_t op_number;
	atca_def(at_node_ptr_t);
	atc_def(at_op_t);
	atc_def(at_node_t);
};

at_layer_t* at_layer_alloc(const at_layer_param_t *restrict param)
{
	static const at_layer_param_t sparam = {
		.op_size = 1024,
		.op_cache_count = 64,
		.node_cache_count = 256,
		.node_slot_cache_length = 15,
		.node_slot_cache_count = 32
	};
	at_layer_param_t p;
	register at_layer_t *restrict r;
	if (!param) param = &sparam;
	p = *param;
	if (!p.op_size)                p.op_size                = sparam.op_size;
	if (!p.op_cache_count)         p.op_cache_count         = sparam.op_cache_count;
	if (!p.node_cache_count)       p.node_cache_count       = sparam.node_cache_count;
	if (!p.node_slot_cache_length) p.node_slot_cache_length = sparam.node_slot_cache_length;
	if (!p.node_slot_cache_count)  p.node_slot_cache_count  = sparam.node_slot_cache_count;
	r = (at_layer_t *) calloc(1, sizeof(at_layer_t));
	if (r)
	{
		r->op_list = (at_op_t **) calloc(p.op_size, sizeof(at_op_t *));
		if (r->op_list)
		{
			r->op_size = p.op_size;
			if (atca_init(r, at_node_ptr_t, 1, p.node_slot_cache_length, p.node_slot_cache_count))
			{
				atc_init(r, at_op_t, p.op_cache_count);
				atc_init(r, at_node_t, p.node_cache_count);
				return r;
			}
		}
		at_layer_free(r);
	}
	return NULL;
}

static void at_layer_op_free(register at_layer_t *restrict layer, register at_op_t *restrict op)
{
	if (op->free)
		op->free(op->data);
	atc_push(layer, at_op_t, op);
}

void at_layer_free(register at_layer_t *restrict layer)
{
	if (layer->op_list)
	{
		at_layer_op_clear(layer);
		free(layer->op_list);
	}
	atca_uini(layer, at_node_ptr_t);
	atc_uini(layer, at_op_t);
	atc_uini(layer, at_node_t);
	free(layer);
}

uintptr_t at_layer_block_leak(register const at_layer_t *restrict layer, uintptr_t *restrict op_b_leak, uintptr_t *restrict node_b_leak, uintptr_t *restrict slots_b_leak)
{
	uintptr_t n_op, n_node, n_slots;
	n_op = atc_leak(layer, at_op_t);
	n_node = atc_leak(layer, at_node_t);
	n_slots = atca_leak(layer, at_node_ptr_t);
	if (op_b_leak) *op_b_leak = n_op;
	if (node_b_leak) *node_b_leak = n_node;
	if (slots_b_leak) *slots_b_leak = n_slots;
	return n_op + n_node + n_slots;
}

at_layer_t* at_layer_op_set(register at_layer_t *restrict layer, uintptr_t type, uintptr_t slots_number, at_node_run_f run_func, at_free_f free_func, void *data)
{
	register at_op_t *restrict op, *restrict *restrict slot;
	if (type < layer->op_size && (op = atc_pull(layer, at_op_t)))
	{
		op->type = type;
		op->slots_number = slots_number;
		op->run = run_func;
		op->free = free_func;
		op->data = data;
		if (*(slot = layer->op_list + type))
			at_layer_op_free(layer, *slot);
		*slot = op;
		if (layer->op_number <= type)
			layer->op_number = type + 1;
		return layer;
	}
	return NULL;
}

void at_layer_op_unset(register at_layer_t *restrict layer, uintptr_t type)
{
	register at_op_t *restrict *restrict slot;
	if (type < layer->op_number && *(slot = layer->op_list + type))
	{
		at_layer_op_free(layer, *slot);
		*slot = NULL;
	}
}

void at_layer_op_clear(register at_layer_t *restrict layer)
{
	register uintptr_t i;
	register at_op_t *restrict *restrict op;
	op = layer->op_list;
	i = layer->op_number;
	while (i)
	{
		--i;
		if (op[i])
		{
			at_layer_op_free(layer, op[i]);
			op[i] = NULL;
		}
	}
	layer->op_number = 0;
}

at_node_t* at_layer_node_alloc(register at_layer_t *restrict layer, uintptr_t type, at_free_f free_func, void *data)
{
	register at_node_t *restrict r;
	at_op_t *restrict op;
	if (type < layer->op_number && (op = layer->op_list[type]))
	{
		if ((r = atc_pull(layer, at_node_t)))
		{
			r->op = op;
			r->parent = NULL;
			r->slots = NULL;
			r->slots_number = 0;
			r->free = free_func;
			r->data = data;
			if (!op->slots_number || at_layer_node_slots_rebuild(layer, r, op->slots_number))
				return r;
			atc_push(layer, at_node_t, r);
		}
	}
	return NULL;
}

void at_layer_node_free(register at_layer_t *restrict layer, register at_node_t *restrict node)
{
	if (node->slots)
		at_layer_node_slots_clear(layer, node);
	if (node->free)
		node->free(node->data);
	atc_push(layer, at_node_t, node);
}

void at_layer_node_slots_clear(register at_layer_t *restrict layer, register at_node_t *restrict node)
{
	register uintptr_t i, n;
	register at_node_ptr_t *p;
	if ((p = node->slots))
	{
		n = node->slots_number;
		node->slots = NULL;
		node->slots_number = 0;
		for (i = 0; i < n; ++i)
		{
			if (p[i])
				at_layer_node_free(layer, p[i]);
		}
		atca_push(layer, at_node_ptr_t, p, n);
	}
}

at_node_t* at_layer_node_slots_rebuild(register at_layer_t *restrict layer, register at_node_t *restrict node, uintptr_t slots_number)
{
	at_node_ptr_t *p;
	if (slots_number)
	{
		if (!(p = atca_pull(layer, at_node_ptr_t, slots_number)))
			return NULL;
		memset(p, 0, sizeof(at_node_ptr_t) * slots_number);
	}
	else p = NULL;
	if (node->slots)
		at_layer_node_slots_clear(layer, node);
	node->slots = p;
	node->slots_number = slots_number;
	return node;
}

at_node_t* at_layer_node_slots_set(at_layer_t *restrict layer, register at_node_t *restrict node, uintptr_t index, register at_node_t *restrict v)
{
	at_node_ptr_t *p;
	if (index < node->slots_number)
	{
		p = node->slots + index;
		if (*p) at_layer_node_free(layer, *p);
		*p = v;
		v->parent = node;
		return node;
	}
	return NULL;
}

void at_layer_node_slots_unset(at_layer_t *restrict layer, register at_node_t *restrict node, uintptr_t index)
{
	at_node_ptr_t *p;
	if (index < node->slots_number && *(p = node->slots + index))
	{
		at_layer_node_free(layer, *p);
		*p = NULL;
	}
}

at_node_t* at_layer_node_run(at_layer_t *restrict layer, register at_node_t *restrict node, void *env)
{
	register at_node_run_f func;
	if ((func = node->op->run))
		return func(node, layer, env, node->data);
	return NULL;
}

static inline const at_node_t* _at_node_first(register const at_node_t *restrict node)
{
	register const at_node_t *restrict n;
	while (node->slots_number && (n = *node->slots))
		node = n;
	return node;
}

static inline const at_node_t* _at_node_slots_next(register const at_node_t *restrict parent, register const at_node_t *restrict node)
{
	register at_node_t *restrict *restrict slots;
	register uintptr_t n;
	if ((n = parent->slots_number))
	{
		slots = parent->slots;
		while (--n)
		{
			if (*slots == node)
				return slots[1];
			++slots;
		}
	}
	return NULL;
}

at_node_t* at_node_first(register const at_node_t *restrict node)
{
	if (node) node = _at_node_first(node);
	return (at_node_t *) node;
}

at_node_t* at_node_next(register const at_node_t *restrict node)
{
	if (node)
	{
		register const at_node_t *restrict parent;
		parent = node->parent;
		if (parent)
		{
			if ((node = _at_node_slots_next(parent, node)))
				parent = _at_node_first(node);
			return (at_node_t *) parent;
		}
	}
	return NULL;
}

at_node_t** at_node_address(register const at_node_t *restrict node)
{
	register at_node_t *restrict parent;
	register at_node_t *restrict *restrict slots;
	register uintptr_t n;
	if ((parent = node->parent) && (n = parent->slots_number))
	{
		slots = parent->slots;
		do {
			if (*slots == node)
				return (at_node_t **) slots;
			++slots;
		} while (--n);
	}
	return NULL;
}
