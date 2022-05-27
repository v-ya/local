#include "../vkaa.function.h"
#include "../vkaa.type.h"
#include "../vkaa.var.h"

static void vkaa_function_free_func(vkaa_function_s *restrict r)
{
	uintptr_t i;
	if (r->pri_data) refer_free(r->pri_data);
	if (r->this) refer_free(r->this);
	if (r->output_type) refer_free(r->output_type);
	if (r->output) refer_free(r->output);
	for (i = 0; i < r->input_number; ++i)
	{
		if (r->input_list[i])
			refer_free(r->input_list[i]);
	}
}

vkaa_function_s* vkaa_function_alloc(refer_t pri_data, vkaa_function_f function, const vkaa_type_s *restrict output_type, uintptr_t input_number, vkaa_var_s *const *restrict input_list)
{
	vkaa_function_s *restrict r;
	uintptr_t i;
	if ((r = (vkaa_function_s *) refer_alloz(sizeof(vkaa_function_s) + sizeof(vkaa_var_s *) * input_number)))
	{
		refer_set_free(r, (refer_free_f) vkaa_function_free_func);
		if (!(r->function = function))
			goto label_fail;
		r->pri_data = refer_save(pri_data);
		if (!(r->output_type = (const vkaa_type_s *) refer_save(output_type)))
			goto label_fail;
		r->input_number = input_number;
		for (i = 0; i < r->input_number; ++i)
		{
			if (!(r->input_list[i] = (vkaa_var_s *) refer_save(input_list[i])))
				goto label_fail;
		}
		return r;
		label_fail:
		refer_free(r);
	}
	return NULL;
}

void vkaa_function_set_this(vkaa_function_s *restrict func, vkaa_var_s *restrict this)
{
	if (func->this) refer_free(func->this);
	func->this = (vkaa_var_s *) refer_save(this);
}

vkaa_function_s* vkaa_function_set_output(vkaa_function_s *restrict func, vkaa_var_s *restrict output)
{
	if (!output || (output->type == func->output_type))
	{
		if (func->output) refer_free(func->output);
		func->output = (vkaa_var_s *) refer_save(output);
		return func;
	}
	return NULL;
}

vkaa_function_s* vkaa_function_set_input(vkaa_function_s *restrict func, uintptr_t index, vkaa_var_s *restrict input)
{
	if (input && index < func->input_number)
	{
		refer_free(func->input_list[index]);
		func->input_list[index] = (vkaa_var_s *) refer_save(input);
		return func;
	}
	return NULL;
}

vkaa_var_s* vkaa_function_okay(vkaa_function_s *restrict func)
{
	vkaa_var_s *restrict var;
	if ((var = func->output) || (func->output = var = func->output_type->create(func->output_type, NULL)))
		return var;
	return NULL;
}
