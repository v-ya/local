#include "std.tpool.h"

// input

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

static vkaa_function_s* vkaa_std_var_function_inst_push_mov(vkaa_execute_s *restrict exec, const vkaa_tpool_s *restrict tpool, vkaa_var_s *restrict var)
{
	vkaa_function_s *restrict func;
	vkaa_selector_s *restrict s;
	vkaa_selector_param_t param;
	vkaa_var_s *input_list[2];
	if ((s = vkaa_var_find_selector(var, "=")))
	{
		input_list[0] = var;
		input_list[1] = var;
		param.tpool = tpool;
		param.exec = exec;
		param.this = NULL;
		param.input_list = input_list;
		param.input_number = 2;
		if ((func = s->selector(s, &param)))
		{
			if (func->output == var && !vkaa_execute_push(exec, func))
				return func;
			refer_free(func);
		}
	}
	return NULL;
}

static vkaa_function_s* vkaa_std_var_function_inst_put_var_mov(vkaa_execute_s *restrict exec, const vkaa_tpool_s *restrict tpool, vkaa_scope_s *restrict scope, const char *restrict name, uintptr_t id)
{
	vkaa_function_s *restrict func;
	vkaa_var_s *restrict v;
	if ((v = vkaa_std_var_function_inst_put_var(tpool, scope, name, id)))
	{
		func = vkaa_std_var_function_inst_push_mov(exec, tpool, v);
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
		(r->output_mov = vkaa_std_var_function_inst_push_mov(r->exec, tpool, r->output)) &&
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

vkaa_std_var_function_inst_s* vkaa_std_var_function_inst_initial(vkaa_std_var_function_inst_s *restrict r, vkaa_function_s *restrict func)
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

static void vkaa_std_var_function_free_func(vkaa_std_var_function_s *restrict r)
{
	if (r->inst) refer_free(r->inst);
	if (r->desc) refer_free(r->desc);
	if (r->input) refer_free(r->input);
	vkaa_var_finally(&r->var);
}

// function

vkaa_std_var_function_s* vkaa_std_type_function_set_input(vkaa_std_var_function_s *restrict var, const vkaa_parse_context_t *restrict context, const vkaa_syntax_s *restrict syntax_brackets, uintptr_t output_typeid)
{
	return NULL;
}

vkaa_std_var_function_s* vkaa_std_type_function_set_scope(vkaa_std_var_function_s *restrict var, const vkaa_parse_context_t *restrict context, const vkaa_syntax_s *restrict syntax_scope, uintptr_t id_scope)
{
	vkaa_std_var_function_inst_s *restrict inst;
	vkaa_parse_context_t c;
	c = *context;
	if (!var->exec_in_number && var->input &&
		(inst = vkaa_std_var_function_inst_alloc(var->input, c.tpool, c.scope, id_scope)))
	{
		c.execute = inst->exec;
		c.scope = inst->scope;
		c.this = &inst->this->var;
		if (vkaa_parse_parse(&c, syntax_scope->syntax_array, syntax_scope->syntax_number, NULL) &&
			vkaa_std_var_function_inst_okay(inst, c.tpool))
		{
			if (var->inst) refer_free(var->inst);
			var->inst = inst;
			return var;
		}
		refer_free(inst);
	}
	return NULL;
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

vkaa_type_s* vkaa_std_tpool_set_function(vkaa_tpool_s *restrict tpool, vkaa_std_typeid_s *restrict typeid)
{
	return vkaa_std_tpool_set(tpool, "function", typeid->id_function, vkaa_std_type_create_label(function), NULL, typeid);
}
