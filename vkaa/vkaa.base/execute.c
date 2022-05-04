#include "../vkaa.execute.h"
#include "../vkaa.function.h"

static void vkaa_execute_free_func(vkaa_execute_s *restrict r)
{
	vkaa_function_s *const *restrict array;
	uintptr_t i, n;
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

vkaa_function_s* vkaa_execute_get_last_function(const vkaa_execute_s *restrict exec)
{
	if (exec->execute_number)
		return exec->execute_array[exec->execute_number - 1];
	return NULL;
}

vkaa_var_s* vkaa_execute_get_last_var(const vkaa_execute_s *restrict exec)
{
	vkaa_function_s *restrict func;
	if ((func = vkaa_execute_get_last_function(exec)))
		return vkaa_function_okay(func);
	return NULL;
}

vkaa_execute_s* vkaa_execute_push(vkaa_execute_s *restrict exec, vkaa_function_s *restrict func)
{
	vkaa_function_s *const *restrict array;
	if (vkaa_execute_okay(exec) &&
		(array = (vkaa_function_s *const *) exbuffer_append(&exec->buffer, (const void *) &func, sizeof(func))))
	{
		exec->execute_array = array;
		exec->execute_number += 1;
		refer_save(func);
		return exec;
	}
	return NULL;
}

vkaa_execute_s* vkaa_execute_okay(vkaa_execute_s *restrict exec)
{
	vkaa_function_s *restrict last;
	if (!(last = vkaa_execute_get_last_function(exec)) || vkaa_function_okay(last))
		return exec;
	return NULL;
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
			break;
	}
	return var;
}

void vkaa_execute_clear(vkaa_execute_s *restrict exec)
{
	vkaa_function_s **restrict array;
	uintptr_t i, n;
	array = (vkaa_function_s **) exec->buffer.data;
	n = exec->execute_number;
	exec->execute_number = 0;
	exec->execute_array = NULL;
	for (i = 0; i < n; ++i)
	{
		refer_free(array[i]);
		array[i] = NULL;
	}
}
