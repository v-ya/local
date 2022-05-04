#include "../vkaa.function.h"
#include "../vkaa.selector.h"
#include "../vkaa.tpool.h"

static void vkaa_function_free_func(vkaa_function_s *restrict r)
{
	uintptr_t i;
	if (r->selector) refer_free(r->selector);
	if (r->this) refer_free(r->this);
	if (r->output) refer_free(r->output);
	for (i = 0; i < r->input_number; ++i)
	{
		if (r->input_list[i])
			refer_free(r->input_list[i]);
	}
}

vkaa_function_s* vkaa_function_alloc(const vkaa_selector_s *restrict selector, const vkaa_selector_param_t *restrict param, const vkaa_selector_rdata_t *restrict rdata)
{
	vkaa_function_s *restrict r;
	uintptr_t i;
	if ((r = (vkaa_function_s *) refer_alloz(sizeof(vkaa_function_s) + sizeof(vkaa_var_s *) * param->input_number)))
	{
		refer_set_free(r, (refer_free_f) vkaa_function_free_func);
		if (!(r->function = rdata->function)) goto label_fail;
		r->selector = (const vkaa_selector_s *) refer_save(selector);
		r->this = (vkaa_var_s *) refer_save(param->this);
		r->output = (vkaa_var_s *) refer_save(rdata->output_must);
		r->output_typeid = rdata->output_typeid;
		r->input_number = param->input_number;
		for (i = 0; i < r->input_number; ++i)
		{
			if (!(r->input_list[i] = (vkaa_var_s *) refer_save(param->input_list[i])))
				goto label_fail;
		}
		return r;
		label_fail:
		refer_free(r);
	}
	return NULL;
}

vkaa_var_s* vkaa_function_okay(vkaa_function_s *restrict func, const vkaa_tpool_s *restrict tpool, vkaa_scope_s *restrict scope)
{
	vkaa_var_s *restrict var;
	if ((var = func->output) || (func->output = var = vkaa_tpool_var_create_by_id(tpool, func->output_typeid, scope)))
		return var;
	return NULL;
}
