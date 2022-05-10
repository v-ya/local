#include "std.h"
#include <memory.h>

vkaa_std_selector_s* vkaa_std_selector_initial(vkaa_std_selector_s *restrict selector)
{
	selector->selector.selector = (vkaa_selector_f) vkaa_std_selector_selector;
	if ((selector->std_desc = vattr_alloc()))
		return selector;
	return NULL;
}

void vkaa_std_selector_finally(vkaa_std_selector_s *restrict selector)
{
	if (selector->std_desc) refer_free(selector->std_desc);
}

vkaa_std_selector_s* vkaa_std_selector_alloc(void)
{
	vkaa_std_selector_s *restrict r;
	if ((r = (vkaa_std_selector_s *) refer_alloz(sizeof(vkaa_std_selector_s))))
	{
		refer_set_free(r, (refer_free_f) vkaa_std_selector_finally);
		if (vkaa_std_selector_initial(r))
			return r;
		refer_free(r);
	}
	return NULL;
}

static vkaa_std_selector_desc_t* vkaa_std_selector_desc_alloc(uintptr_t input_number, const uintptr_t *restrict input_typeid)
{
	vkaa_std_selector_desc_t *restrict r;
	if ((r = (vkaa_std_selector_desc_t *) refer_alloz(sizeof(vkaa_std_selector_desc_t) + sizeof(uintptr_t) * input_number)))
	{
		r->input_number = input_number;
		memcpy(r + 1, input_typeid, sizeof(uintptr_t) * input_number);
	}
	return r;
}

vkaa_std_selector_s* vkaa_std_selector_append(vkaa_std_selector_s *restrict selector, const char *restrict name, vkaa_function_f function, vkaa_std_selector_output_t output, vkaa_std_selector_convert_t convert, uintptr_t this_typeid, uintptr_t output_typeid, uintptr_t input_number, const uintptr_t *restrict input_typeid)
{
	vkaa_std_selector_desc_t *restrict d;
	if (name && (d = vkaa_std_selector_desc_alloc(input_number, input_typeid)))
	{
		d->function = function;
		d->output = output;
		d->convert = convert;
		d->this_typeid = this_typeid;
		d->output_typeid = output_typeid;
		if (!vattr_insert_tail(selector->std_desc, name, d))
			selector = NULL;
		refer_free(d);
		return selector;
	}
	return NULL;
}

// selector do

static const vkaa_selector_s* vkaa_std_selector_test_convert_is_promotion(const vkaa_selector_s *restrict s)
{
	vattr_vlist_t *restrict vl;
	const vkaa_std_selector_desc_t *restrict desc;
	if (s->selector == (vkaa_selector_f) vkaa_std_selector_selector)
	{
		for (vl = ((const vkaa_std_selector_s *) s)->std_desc->vattr; vl; vl = vl->vattr_next)
		{
			if ((desc = (const vkaa_std_selector_desc_t *) vl->value) && !desc->input_number)
				return (desc->convert == vkaa_std_selector_convert_promotion)?s:NULL;
		}
	}
	return NULL;
}

const vkaa_std_selector_desc_t* vkaa_std_selector_test(const vkaa_std_selector_desc_t *restrict desc, const vkaa_selector_param_t *restrict param, uintptr_t *restrict score)
{
	vkaa_var_s *const *restrict input_list;
	const uintptr_t *restrict input_typeid;
	const vkaa_selector_s *restrict s;
	uintptr_t i, n, sr, sp;
	if ((n = desc->input_number) == param->input_number)
	{
		input_list = param->input_list;
		input_typeid = desc->input_typeid;
		if (desc->this_typeid && (!param->this || param->this->type_id != desc->this_typeid))
			goto label_fail;
		switch (desc->output)
		{
			case vkaa_std_selector_output_any: break;
			case vkaa_std_selector_output_this:
				if (!param->this || param->this->type_id != desc->output_typeid)
					goto label_fail;
				break;
			case vkaa_std_selector_output_input_first:
				if (!n || input_typeid[0] != desc->output_typeid)
					goto label_fail;
				break;
			case vkaa_std_selector_output_must_first:
				if (!n || input_list[0]->type_id != desc->output_typeid)
					goto label_fail;
				break;
			case vkaa_std_selector_output_new: break;
			default: goto label_fail;
		}
		sp = (uintptr_t) 0x80000000;
		for (i = sr = 0; i < n; ++i)
		{
			if (input_list[i]->type_id == input_typeid[i])
				sr |= sp;
			else if (!param->exec || !(s = vkaa_std_convert_test_by_typeid(input_list[i]->type, input_typeid[i], param->tpool)))
				goto label_fail;
			else if (vkaa_std_selector_test_convert_is_promotion(s))
				sr |= (sp >> 1);
			sp >>= 2;
		}
		if (score) *score = sr;
		return desc;
	}
	label_fail:
	if (score) *score = 0;
	return NULL;
}

vkaa_function_s* vkaa_std_selector_create(const vkaa_std_selector_s *restrict selector, const vkaa_selector_param_t *restrict param, const vkaa_std_selector_desc_t *restrict desc)
{
	vkaa_function_s *restrict rf;
	vkaa_function_s *restrict cf;
	vkaa_var_s *const *restrict input_list;
	const uintptr_t *restrict input_typeid;
	uintptr_t i, n;
	rf = NULL;
	if ((n = desc->input_number) == param->input_number)
	{
		input_list = param->input_list;
		input_typeid = desc->input_typeid;
		if ((rf = vkaa_function_alloc(
			&selector->selector, desc->function,
			vkaa_tpool_find_id(param->tpool, desc->output_typeid),
			n, input_list)))
		{
			if (desc->this_typeid)
			{
				if (!param->this || param->this->type_id != desc->this_typeid)
					goto label_fail;
				vkaa_function_set_this(rf, param->this);
			}
			for (i = 0; i < n; ++i)
			{
				if (input_list[i]->type_id == input_typeid[i])
					continue;
				if (!param->exec || !(cf = vkaa_std_convert_by_typeid(param->exec, param->tpool, input_list[i], input_typeid[i])))
					goto label_fail;
				if (!vkaa_function_set_input(rf, i, vkaa_function_okay(cf)))
					goto label_fail;
			}
			switch (desc->output)
			{
				case vkaa_std_selector_output_any:
					break;
				case vkaa_std_selector_output_this:
					if (!rf->this)
						goto label_fail;
					if (!vkaa_function_set_output(rf, rf->this))
						goto label_fail;
					break;
				case vkaa_std_selector_output_input_first:
					if (!n) goto label_fail;
					if (!vkaa_function_set_output(rf, rf->input_list[0]))
						goto label_fail;
					break;
				case vkaa_std_selector_output_must_first:
					if (!n) goto label_fail;
					if (input_list[0] != rf->input_list[0])
						goto label_fail;
					if (!vkaa_function_set_output(rf, rf->input_list[0]))
						goto label_fail;
					break;
				case vkaa_std_selector_output_new:
					if (!vkaa_function_okay(rf))
						goto label_fail;
					break;
				default: goto label_fail;
			}
			return rf;
			label_fail:
			refer_free(rf);
		}
	}
	return NULL;
}

vkaa_function_s* vkaa_std_selector_selector(const vkaa_std_selector_s *restrict selector, const vkaa_selector_param_t *restrict param)
{
	vattr_vlist_t *restrict vl;
	const vkaa_std_selector_desc_t *restrict desc, *restrict hit_desc;
	uintptr_t score, max_score;
	hit_desc = NULL;
	max_score = 0;
	for (vl = selector->std_desc->vattr; vl; vl = vl->vattr_next)
	{
		if ((desc = (const vkaa_std_selector_desc_t *) vl->value) &&
			vkaa_std_selector_test(desc, param, &score))
		{
			if (score > max_score || (score == max_score && !hit_desc))
			{
				max_score = score;
				hit_desc = desc;
			}
		}
	}
	if (hit_desc)
		return vkaa_std_selector_create(selector, param, hit_desc);
	return NULL;
}
