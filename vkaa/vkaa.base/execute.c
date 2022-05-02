#include "../vkaa.execute.h"
#include "../vkaa.function.h"

static void vkaa_execute_free_func(vkaa_execute_s *restrict r)
{
	vkaa_function_s *const *restrict array;
	uintptr_t i, n;
	if (r->last_var) refer_free(r->last_var);
	array = r->execute_array;
	n = r->execute_number;
	for (i = 0; i < n; ++i)
		refer_free(array[i]);
	exbuffer_uini(&r->buffer);
}

vkaa_execute_s* vkaa_execute_alloc(void)
{
	vkaa_execute_s *restrict r;
	if ((r = (vkaa_execute_s *) refer_alloz(sizeof(vkaa_execute_s))))
	{
		refer_set_free(r, (refer_free_f) vkaa_execute_free_func);
		if (exbuffer_init(&r->buffer, 0))
			return r;
		refer_free(r);
	}
	return NULL;
}

vkaa_execute_s* vkaa_execute_push(vkaa_execute_s *restrict exec, const vkaa_function_s *restrict func)
{
	vkaa_function_s *const *restrict array;
	if ((array = exbuffer_append(&exec->buffer, (const void *) &func, sizeof(func))))
	{
		exec->execute_array = array;
		exec->execute_number += 1;
		return exec;
	}
	return NULL;
}

vkaa_var_s* vkaa_execute_set_last(vkaa_execute_s *restrict exec, vkaa_var_s *restrict var)
{
	if (exec->last_var) refer_free(exec->last_var);
	return (exec->last_var = (vkaa_var_s *) refer_save(var));
}

vkaa_var_s* vkaa_execute_do(const vkaa_execute_s *restrict exec)
{
	vkaa_function_s *const *restrict array;
	uintptr_t i, n;
	vkaa_var_s *restrict var;
	array = exec->execute_array;
	n = exec->execute_number;
	var = NULL;
	for (i = 0; i < n; ++i)
	{
		if (!(var = array[i]->function(array[i])))
			goto label_fail;
	}
	return exec->last_var;
	label_fail:
	return NULL;
}
