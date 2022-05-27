#include "std.tpool.h"

static void vkaa_std_var_refer_free_func(vkaa_std_var_refer_s *restrict r)
{
	if (r->refer_type) refer_free(r->refer_type);
	if (r->refer_var) refer_free(r->refer_var);
	vkaa_var_finally(&r->var);
}

vkaa_std_var_refer_s* vkaa_std_type_refer_create_by_type(const vkaa_type_s *restrict type, const vkaa_type_s *restrict refer_type)
{
	vkaa_std_var_refer_s *restrict r;
	if ((r = (vkaa_std_var_refer_s *) refer_alloz(sizeof(vkaa_std_var_refer_s))))
	{
		refer_set_free(r, (refer_free_f) vkaa_std_var_refer_free_func);
		if (vkaa_var_initial(&r->var, type))
		{
			r->refer_type = (const vkaa_type_s *) refer_save(refer_type);
			return r;
		}
		refer_free(r);
	}
	return NULL;
}

vkaa_std_var_refer_s* vkaa_std_var_refer_must_type(vkaa_std_var_refer_s *restrict var, const vkaa_type_s *restrict refer_type)
{
	if (var->refer_type == refer_type || (!var->refer_type &&
		(var->refer_type = (const vkaa_type_s *) refer_save(refer_type))))
		return var;
	return NULL;
}

vkaa_var_s* vkaa_std_var_refer_must_value(vkaa_std_var_refer_s *restrict var)
{
	const vkaa_type_s *restrict refer_type;
	vkaa_var_s *restrict rv;
	if ((rv = var->refer_var) || ((refer_type = var->refer_type) &&
		(var->refer_var = rv = refer_type->create(refer_type, NULL))))
		return rv;
	return NULL;
}

vkaa_std_var_refer_s* vkaa_std_var_refer_link(vkaa_std_var_refer_s *restrict var, vkaa_var_s *restrict refer_var)
{
	if (!refer_var || vkaa_std_var_refer_must_type(var, refer_var->type))
	{
		refer_save(refer_var);
		if (var->refer_var) refer_free(var->refer_var);
		var->refer_var = refer_var;
		return var;
	}
	return NULL;
}

vkaa_std_var_refer_s* vkaa_std_var_refer_mov(vkaa_std_var_refer_s *restrict dst, const vkaa_std_var_refer_s *restrict src)
{
	vkaa_var_s *restrict value;
	if (vkaa_std_var_refer_must_type(dst, src->refer_type))
	{
		refer_save(value = src->refer_var);
		if (dst->refer_var) refer_free(dst->refer_var);
		dst->refer_var = value;
		return dst;
	}
	return NULL;
}

static vkaa_std_type_create_define(refer)
{
	if (!syntax)
		return &vkaa_std_type_refer_create_by_type(type, NULL)->var;
	return NULL;
}

// selector

static vkaa_std_selector_define(refer, op_store)
{
	vkaa_std_var_refer_s *restrict var;
	vkaa_var_s *restrict dst, *restrict src;
	vkaa_function_s *restrict refer_mov, *restrict rf;
	if (param->input_number == 2)
	{
		var = vkaa_std_var(refer, param->input_list[0]);
		src = param->input_list[1];
		if (var->refer_type && (dst = vkaa_std_var_refer_must_value(var)) &&
			(refer_mov = vkaa_std_convert_create_function_set_var(param->exec, param->tpool, src, dst)))
		{
			rf = vkaa_function_alloc(refer_mov, vkaa_std_function_label(refer, op_store), var->var.type, param->input_number, param->input_list);
			if (rf && !vkaa_function_set_output(rf, &var->var))
			{
				refer_free(rf);
				rf = NULL;
			}
			refer_free(refer_mov);
			return rf;
		}
	}
	return NULL;
}

static vkaa_std_selector_define(refer, op_load)
{
	vkaa_std_var_refer_s *restrict var;
	vkaa_var_s *restrict src;
	vkaa_function_s *restrict refer_mov, *restrict rf;
	if (param->input_number == 1)
	{
		var = vkaa_std_var(refer, param->input_list[0]);
		if (var->refer_type && (src = vkaa_std_var_refer_must_value(var)) &&
			(refer_mov = vkaa_std_convert_create_function_mov_var(param->exec, param->tpool, src, src)))
		{
			rf = vkaa_function_alloc(refer_mov, vkaa_std_function_label(refer, op_load), src->type, param->input_number, param->input_list);
			refer_free(refer_mov);
			return rf;
		}
	}
	return NULL;
}

static vkaa_std_type_init_define(refer)
{
	if (
		vkaa_std_type_set_function_si(type, "=", sfsi_need2m(refer, op_mov)) &&
		vkaa_std_type_set_selector(type, ":=", vkaa_std_selector_label(refer, op_store)) &&
		vkaa_std_type_set_selector(type, "*", vkaa_std_selector_label(refer, op_load)) &&
	1) return type;
	return NULL;
}

vkaa_type_s* vkaa_std_tpool_set_refer(vkaa_tpool_s *restrict tpool, vkaa_std_typeid_s *restrict typeid)
{
	return vkaa_std_tpool_set(tpool, "refer", typeid->id_refer, typeid,
		vkaa_std_type_create_label(refer), vkaa_std_type_init_label(refer));
}
