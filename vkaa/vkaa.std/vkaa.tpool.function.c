#include "std.tpool.h"
#include <string.h>

// input

static void vkaa_std_var_function_input_free_func(vkaa_std_var_function_input_s *restrict r)
{
	refer_nstring_t *restrict name;
	uintptr_t i, n;
	name = r->name;
	for (i = 0, n = r->number; i < n; ++i)
	{
		if (name[i]) refer_free(name[i]);
	}
}

static vkaa_std_var_function_input_s* vkaa_std_var_function_input_alloc_empty(uintptr_t number)
{
	vkaa_std_var_function_input_s *restrict r;
	if ((r = (vkaa_std_var_function_input_s *) refer_alloz(
		sizeof(vkaa_std_var_function_input_s) + (sizeof(uintptr_t) + sizeof(refer_nstring_t)) * number)))
	{
		refer_set_free(r, (refer_free_f) vkaa_std_var_function_input_free_func);
		r->number = number;
		r->typeid = (uintptr_t *) (r + 1);
		r->name = (refer_nstring_t *) (r->typeid + number);
	}
	return r;
}

static const vkaa_syntax_s* vkaa_std_var_function_input_get_number(const vkaa_syntax_s *restrict syntax_brackets, uintptr_t *restrict rn)
{
	const vkaa_syntax_t *restrict s;
	uintptr_t i, n, number;
	s = syntax_brackets->syntax_array;
	n = syntax_brackets->syntax_number;
	number = i = 0;
	while (i < n)
	{
		if (s[i].type != vkaa_syntax_type_keyword)
			goto label_fail;
		if (++i >= n)
			goto label_fail;
		if (s[i].type != vkaa_syntax_type_keyword)
			goto label_fail;
		number += 1;
		if (++i >= n)
			break;
		if (s[i].type != vkaa_syntax_type_comma)
			goto label_fail;
		if (++i >= n)
			goto label_fail;
	}
	if (rn) *rn = number;
	return syntax_brackets;
	label_fail:
	if (rn) *rn = 0;
	return NULL;
}

static vkaa_std_var_function_input_s* vkaa_std_var_function_input_initial(vkaa_std_var_function_input_s *restrict r, const vkaa_tpool_s *restrict tpool, const vkaa_syntax_s *restrict syntax_brackets)
{
	refer_nstring_t keyword;
	const vkaa_type_s *restrict type;
	uintptr_t *restrict typeid;
	refer_nstring_t *restrict name;
	const vkaa_syntax_t *restrict s;
	uintptr_t i, n, p;
	typeid = r->typeid;
	name = r->name;
	s = syntax_brackets->syntax_array;
	n = syntax_brackets->syntax_number;
	i = p = 0;
	while (i < n)
	{
		keyword = s[i++].data.keyword;
		if (!(type = vkaa_tpool_find_name(tpool, keyword->string)))
			goto label_fail;
		typeid[p] = type->id;
		keyword = s[i++].data.keyword;
		name[p] = (refer_nstring_t) refer_save(keyword);
		++p;
		++i;
	}
	return r;
	label_fail:
	return NULL;
}

static vkaa_std_var_function_input_s* vkaa_std_var_function_input_alloc(const vkaa_tpool_s *restrict tpool, const vkaa_syntax_s *restrict syntax_brackets, uintptr_t output_typeid)
{
	vkaa_std_var_function_input_s *restrict r;
	uintptr_t n;
	if (vkaa_std_var_function_input_get_number(syntax_brackets, &n) &&
		(r = vkaa_std_var_function_input_alloc_empty(n)))
	{
		r->output_typeid = output_typeid;
		if (vkaa_std_var_function_input_initial(r, tpool, syntax_brackets))
			return r;
		refer_free(r);
	}
	return NULL;
}

int vkaa_std_var_function_input_compare(const vkaa_std_var_function_input_s *restrict input1, const vkaa_std_var_function_input_s *restrict input2)
{
	const uintptr_t *restrict t1, *restrict t2;
	const refer_nstring_t *restrict n1, *restrict n2;
	uintptr_t i, n;
	int rr;
	if (input1->number == input2->number && input1->output_typeid == input2->output_typeid)
	{
		t1 = input1->typeid;
		t2 = input2->typeid;
		n1 = input1->name;
		n2 = input2->name;
		rr = 0;
		for (i = 0, n = input1->number; i < n; ++i)
		{
			if (t1[i] != t2[i])
				goto label_fail;
			if (n1[i]->length != n2[i]->length)
				goto label_fail;
			if (strcmp(n1[i]->string, n2[i]->string))
				rr = 1;
		}
		return rr;
	}
	label_fail:
	return -1;
}

// inst

static void vkaa_std_var_function_inst_free_func(vkaa_std_var_function_inst_s *restrict r)
{
	uintptr_t i, n;
	for (i = 0, n = r->number; i < n; ++i)
	{
		if (r->input_mov[i])
			refer_free(r->input_mov[i]);
	}
	if (r->output_mov) refer_free(r->output_mov);
	if (r->output) refer_free(r->output);
	if (r->exec) refer_free(r->exec);
	if (r->scope) refer_free(r->scope);
	if (r->this) refer_free(r->this);
}

static vkaa_var_s* vkaa_std_var_function_inst_put_var(const vkaa_tpool_s *restrict tpool, vkaa_scope_s *restrict scope, const char *restrict name, uintptr_t id)
{
	vkaa_var_s *restrict v;
	if ((v = vkaa_tpool_var_create_by_id(tpool, id, NULL)))
	{
		if (vkaa_scope_put(scope, name, v))
			return v;
		refer_free(v);
	}
	return NULL;
}

static vkaa_function_s* vkaa_std_var_function_inst_put_var_mov(vkaa_execute_s *restrict exec, const vkaa_tpool_s *restrict tpool, vkaa_scope_s *restrict scope, const char *restrict name, uintptr_t id)
{
	vkaa_function_s *restrict func;
	vkaa_var_s *restrict v;
	if ((v = vkaa_std_var_function_inst_put_var(tpool, scope, name, id)))
	{
		func = (vkaa_function_s *) refer_save(vkaa_std_convert_mov_var(exec, tpool, v, v));
		refer_free(v);
		return func;
	}
	return NULL;
}

static vkaa_std_var_function_inst_s* vkaa_std_var_function_inst_alloc(const vkaa_std_var_function_input_s *restrict input, const vkaa_tpool_s *restrict tpool, vkaa_scope_s *restrict scope, uintptr_t id_scope)
{
	vkaa_std_var_function_inst_s *restrict r;
	const vkaa_type_s *restrict scope_type;
	uintptr_t i, n;
	n = input->number;
	if ((r = (vkaa_std_var_function_inst_s *) refer_alloz(sizeof(vkaa_std_var_function_inst_s) + n * sizeof(vkaa_function_s *))))
	{
		refer_set_free(r, (refer_free_f) vkaa_std_var_function_inst_free_func);
		r->number = n;
		if ((scope_type = vkaa_tpool_find_id(tpool, id_scope)) &&
			(r->exec = vkaa_execute_alloc()) &&
			(r->this = vkaa_std_type_scope_create_by_parent(scope_type, scope)) && 
			(r->scope = (vkaa_scope_s *) refer_save(r->this->scope)))
		{
			for (i = 0; i < n; ++i)
			{
				if (!(r->input_mov[i] = vkaa_std_var_function_inst_put_var_mov(
					r->exec, tpool, r->scope, input->name[i]->string, input->typeid[i])))
					goto label_fail;
			}
			if (!(r->output = vkaa_std_var_function_inst_put_var(tpool, r->scope, vkaa_std_label_return, input->output_typeid)))
				goto label_fail;
			if (vkaa_execute_push_label(r->exec, vkaa_std_label_return))
				return r;
		}
		label_fail:
		refer_free(r);
	}
	return NULL;
}

static vkaa_std_var_function_inst_s* vkaa_std_var_function_inst_okay(vkaa_std_var_function_inst_s *restrict r, const vkaa_tpool_s *restrict tpool)
{
	if (!r->output_mov &&
		vkaa_execute_pop_label(r->exec, vkaa_std_label_return, vkaa_execute_next_pos(r->exec)) &&
		(r->output_mov = (vkaa_function_s *) refer_save(vkaa_std_convert_mov_var(r->exec, tpool, r->output, r->output))) &&
		vkaa_execute_okay(r->exec))
		return r;
	return NULL;
}

void vkaa_std_var_function_inst_finally(vkaa_std_var_function_inst_s *restrict r)
{
	vkaa_function_s *restrict func;
	uintptr_t i, n;
	n = r->number;
	for (i = 0; i < n; ++i)
	{
		func = r->input_mov[i];
		vkaa_function_set_input(func, 1, func->input_list[0]);
	}
	func = r->output_mov;
	vkaa_function_set_input(func, 0, func->input_list[1]);
	vkaa_function_set_output(func, func->input_list[1]);
}

vkaa_std_var_function_inst_s* vkaa_std_var_function_inst_initial(vkaa_std_var_function_inst_s *restrict r, const vkaa_function_s *restrict func)
{
	uintptr_t i, n;
	if ((n = r->number) == func->input_number)
	{
		for (i = 0; i < n; ++i)
		{
			if (!vkaa_function_set_input(r->input_mov[i], 1, func->input_list[i]))
				goto label_fail;
		}
		if (!vkaa_function_set_input(r->output_mov, 0, func->output))
			goto label_fail;
		if (!vkaa_function_set_output(r->output_mov, func->output))
			goto label_fail;
		return r;
	}
	label_fail:
	vkaa_std_var_function_inst_finally(r);
	return NULL;
}

// stack

static void vkaa_std_var_function_stack_free_func(vkaa_std_var_function_stack_s *restrict r)
{
	uintptr_t i, n;
	for (i = 0, n = r->stack_size; i < n; ++i)
	{
		if (r->stack_inst[i])
			refer_free(r->stack_inst[i]);
	}
}

static vkaa_std_var_function_stack_s* vkaa_std_var_function_stack_alloc(const vkaa_std_var_function_input_s *restrict input, const vkaa_parse_context_t *restrict context, const vkaa_syntax_s *restrict syntax_scope, uintptr_t id_scope, uintptr_t stack_size)
{
	vkaa_std_var_function_stack_s *restrict r;
	vkaa_std_var_function_inst_s *restrict inst;
	vkaa_parse_context_t c;
	uintptr_t i;
	if (stack_size && (r = (vkaa_std_var_function_stack_s *) refer_alloz(
		sizeof(vkaa_std_var_function_stack_s) + stack_size * sizeof(vkaa_std_var_function_inst_s *))))
	{
		refer_set_free(r, (refer_free_f) vkaa_std_var_function_stack_free_func);
		r->stack_size = stack_size;
		c = *context;
		for (i = 0; i < stack_size; ++i)
		{
			if (!(r->stack_inst[i] = inst = vkaa_std_var_function_inst_alloc(input, c.tpool, c.scope, id_scope)))
				goto label_fail;
			if (!vkaa_execute_elog_enable_by_exec(inst->exec, context->execute))
				goto label_fail;
			c.execute = inst->exec;
			c.scope = inst->scope;
			c.this = &inst->this->var;
			if (!vkaa_parse_parse(&c, syntax_scope->syntax_array, syntax_scope->syntax_number, NULL))
				goto label_fail;
			if (!vkaa_std_var_function_inst_okay(inst, c.tpool))
				goto label_fail;
		}
		return r;
		label_fail:
		refer_free(r);
	}
	return NULL;
}

// function

vkaa_std_var_function_s* vkaa_std_var_function_same_input(vkaa_std_var_function_s *restrict var, const vkaa_tpool_s *restrict tpool, const vkaa_syntax_s *restrict syntax_brackets, uintptr_t output_typeid)
{
	vkaa_std_var_function_s *same;
	vkaa_std_var_function_input_s *restrict input;
	same = NULL;
	if (var->input && var->desc &&
		(input = vkaa_std_var_function_input_alloc(tpool, syntax_brackets, output_typeid)))
	{
		if (!vkaa_std_var_function_input_compare(var->input, input))
			same = var;
		refer_free(input);
	}
	return same;
}

vkaa_std_var_function_s* vkaa_std_var_function_set_input(vkaa_std_var_function_s *restrict var, const vkaa_tpool_s *restrict tpool, const vkaa_syntax_s *restrict syntax_brackets, uintptr_t output_typeid)
{
	vkaa_std_var_function_input_s *restrict input;
	vkaa_std_selector_desc_t *restrict desc;
	if ((input = vkaa_std_var_function_input_alloc(tpool, syntax_brackets, output_typeid)))
	{
		if ((desc = vkaa_std_selector_desc_alloc(input->number, input->typeid)))
		{
			desc->function = vkaa_std_function_label(function, do_call);
			desc->output = vkaa_std_selector_output_any;
			desc->convert = vkaa_std_selector_convert_none;
			desc->this_typeid = var->var.type_id;
			desc->output_typeid = input->output_typeid;
			if (var->input) refer_free(var->input);
			if (var->desc) refer_free(var->desc);
			var->input = input;
			var->desc = desc;
			return var;
		}
		refer_free(input);
	}
	return NULL;
}

vkaa_std_var_function_s* vkaa_std_var_function_set_scope(vkaa_std_var_function_s *restrict var, const vkaa_parse_context_t *restrict context, const vkaa_syntax_s *restrict syntax_scope, uintptr_t id_scope, uintptr_t stack_size)
{
	vkaa_std_var_function_stack_s *restrict s;
	if (var->input && (s = vkaa_std_var_function_stack_alloc(var->input, context, syntax_scope, id_scope, stack_size)))
	{
		if (var->stack) refer_free(var->stack);
		var->stack = s;
		return var;
	}
	return NULL;
}

static void vkaa_std_var_function_free_func(vkaa_std_var_function_s *restrict r)
{
	if (r->stack) refer_free(r->stack);
	if (r->desc) refer_free(r->desc);
	if (r->input) refer_free(r->input);
	vkaa_var_finally(&r->var);
}

static vkaa_std_type_create_define(function)
{
	vkaa_std_var_function_s *restrict r;
	if (!syntax && (r = (vkaa_std_var_function_s *) refer_alloz(sizeof(vkaa_std_var_function_s))))
	{
		refer_set_free(r, (refer_free_f) vkaa_std_var_function_free_func);
		if (vkaa_var_initial(&r->var, type))
			return &r->var;
		refer_free(r);
	}
	return NULL;
}

static vkaa_std_type_clear_define(function)
{
	if (((vkaa_std_var_function_s *) var)->stack)
	{
		refer_free(((vkaa_std_var_function_s *) var)->stack);
		((vkaa_std_var_function_s *) var)->stack = NULL;
	}
}

static vkaa_function_s* vkaa_std_type_function_selector_call(const vkaa_selector_s *restrict selector, const vkaa_selector_param_t *restrict param)
{
	vkaa_std_var_function_s *restrict this;
	if ((this = (vkaa_std_var_function_s *) param->this) && this->desc)
	{
		if (vkaa_std_selector_test(this->desc, param, NULL))
			return vkaa_std_selector_create(selector, param, this->desc);
	}
	return NULL;
}

static vkaa_std_type_init_define(function)
{
	if (vkaa_std_type_set_selector(type, "()", vkaa_std_type_function_selector_call))
		return type;
	return NULL;
}

vkaa_type_s* vkaa_std_tpool_set_function(vkaa_tpool_s *restrict tpool, vkaa_std_typeid_s *restrict typeid)
{
	vkaa_type_s *restrict type;
	if ((type = vkaa_std_tpool_set(tpool, "function", typeid->id_function, typeid,
		vkaa_std_type_create_label(function), vkaa_std_type_init_label(function))))
		type->clear = vkaa_std_type_clear_label(function);
	return type;
}
