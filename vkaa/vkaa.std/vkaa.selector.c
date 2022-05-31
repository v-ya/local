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

static void vkaa_std_selector_desc_free_func(vkaa_std_selector_desc_s *restrict r)
{
	if (r->pri_data) refer_free(r->pri_data);
}

vkaa_std_selector_desc_s* vkaa_std_selector_desc_alloc(uintptr_t input_number, const uintptr_t *restrict input_typeid)
{
	vkaa_std_selector_desc_s *restrict r;
	if ((r = (vkaa_std_selector_desc_s *) refer_alloz(sizeof(vkaa_std_selector_desc_s) + sizeof(uintptr_t) * input_number)))
	{
		refer_set_free(r, (refer_free_f) vkaa_std_selector_desc_free_func);
		r->input_number = input_number;
		if (input_number && input_typeid)
			memcpy(r->input_typeid, input_typeid, sizeof(uintptr_t) * input_number);
	}
	return r;
}

vkaa_std_selector_desc_s* vkaa_std_selector_desc_alloc_by_param(const vkaa_std_selector_desc_input_t *restrict must, const vkaa_std_selector_desc_input_t *restrict maybe)
{
	vkaa_std_selector_desc_s *restrict r;
	const vkaa_std_selector_desc_input_t *restrict p;
	uintptr_t must_number, maybe_number, i, n;
	must_number = maybe_number = 1;
	if ((p = must))
	{
		while ((n = p->type_enum_number))
		{
			must_number += n + 1;
			++p;
		}
	}
	if ((p = maybe))
	{
		while ((n = p->type_enum_number))
		{
			maybe_number += n + 1;
			++p;
		}
	}
	if ((r = (vkaa_std_selector_desc_s *) refer_alloz(sizeof(vkaa_std_selector_desc_s) + sizeof(uintptr_t) * (must_number + maybe_number))))
	{
		refer_set_free(r, (refer_free_f) vkaa_std_selector_desc_free_func);
		r->input_number = ~(uintptr_t) 0;
		i = 0;
		if ((p = must))
		{
			while ((n = p->type_enum_number))
			{
				r->input_typeid[i++] = n;
				memcpy(r->input_typeid + i, p->type_enum_array, sizeof(uintptr_t) * n);
				i += n;
				++p;
			}
		}
		r->input_typeid[i++] = 0;
		if ((p = maybe))
		{
			while ((n = p->type_enum_number))
			{
				r->input_typeid[i++] = n;
				memcpy(r->input_typeid + i, p->type_enum_array, sizeof(uintptr_t) * n);
				i += n;
				++p;
			}
		}
		r->input_typeid[i] = 0;
	}
	return r;
}

const vkaa_std_selector_desc_s* vkaa_std_selector_desc_check(const vkaa_std_selector_desc_s *restrict desc)
{
	if (!desc->function)
		goto label_fail;
	if (!desc->output_typeid)
		goto label_fail;
	switch (desc->output)
	{
		case vkaa_std_selector_output_any:
		case vkaa_std_selector_output_new:
			break;
		case vkaa_std_selector_output_this:
			if (!desc->this_typeid)
				goto label_fail;
			if (desc->this_typeid != desc->output_typeid)
				goto label_fail;
			break;
		case vkaa_std_selector_output_input_first:
		case vkaa_std_selector_output_must_first:
			if (~desc->input_number)
			{
				if (!desc->input_number)
					goto label_fail;
				if (desc->input_typeid[0] != desc->output_typeid)
					goto label_fail;
			}
			else
			{
				if (desc->input_typeid[0] != 1)
					goto label_fail;
				if (desc->input_typeid[1] != desc->output_typeid)
					goto label_fail;
			}
			break;
		default:
			goto label_fail;
	}
	return desc;
	label_fail:
	return NULL;
}

vkaa_std_selector_s* vkaa_std_selector_append(vkaa_std_selector_s *restrict selector, const char *restrict name, vkaa_function_f function, refer_t pri_data, vkaa_std_selector_output_t output, vkaa_std_selector_convert_t convert, uintptr_t this_typeid, uintptr_t output_typeid, uintptr_t input_number, const uintptr_t *restrict input_typeid)
{
	vkaa_std_selector_desc_s *restrict d;
	if (name && (d = vkaa_std_selector_desc_alloc(input_number, input_typeid)))
	{
		d->function = function;
		d->pri_data = refer_save(pri_data);
		d->output = output;
		d->convert = convert;
		d->this_typeid = this_typeid;
		d->output_typeid = output_typeid;
		if (!vkaa_std_selector_desc_check(d) || !vattr_insert_tail(selector->std_desc, name, d))
			selector = NULL;
		refer_free(d);
		return selector;
	}
	return NULL;
}

vkaa_std_selector_s* vkaa_std_selector_append_si(vkaa_std_selector_s *restrict selector, const char *restrict name, vkaa_function_f function, refer_t pri_data, vkaa_std_selector_output_t output, vkaa_std_selector_convert_t convert, uintptr_t this_typeid, uintptr_t output_typeid, uintptr_t input_number, uintptr_t input_typeid)
{
	vkaa_std_selector_desc_s *restrict d;
	if (name && (d = vkaa_std_selector_desc_alloc(input_number, NULL)))
	{
		d->function = function;
		d->pri_data = refer_save(pri_data);
		d->output = output;
		d->convert = convert;
		d->this_typeid = this_typeid;
		d->output_typeid = output_typeid;
		while (input_number)
			d->input_typeid[--input_number] = input_typeid;
		if (!vkaa_std_selector_desc_check(d) || !vattr_insert_tail(selector->std_desc, name, d))
			selector = NULL;
		refer_free(d);
		return selector;
	}
	return NULL;
}

vkaa_std_selector_s* vkaa_std_selector_append_any(vkaa_std_selector_s *restrict selector, const char *restrict name, vkaa_function_f function, refer_t pri_data, vkaa_std_selector_output_t output, vkaa_std_selector_convert_t convert, uintptr_t this_typeid, uintptr_t output_typeid, const vkaa_std_selector_desc_input_t *restrict must, const vkaa_std_selector_desc_input_t *restrict maybe)
{
	vkaa_std_selector_desc_s *restrict d;
	if (name && (d = vkaa_std_selector_desc_alloc_by_param(must, maybe)))
	{
		d->function = function;
		d->pri_data = refer_save(pri_data);
		d->output = output;
		d->convert = convert;
		d->this_typeid = this_typeid;
		d->output_typeid = output_typeid;
		if (!vkaa_std_selector_desc_check(d) || !vattr_insert_tail(selector->std_desc, name, d))
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
	const vkaa_std_selector_desc_s *restrict desc;
	if (s->selector == (vkaa_selector_f) vkaa_std_selector_selector)
	{
		for (vl = ((const vkaa_std_selector_s *) s)->std_desc->vattr; vl; vl = vl->vattr_next)
		{
			if ((desc = (const vkaa_std_selector_desc_s *) vl->value) && !desc->input_number)
				return (desc->convert == vkaa_std_selector_convert_promotion)?s:NULL;
		}
	}
	return NULL;
}

// -1 fail
//  0 normal
//  1 promotion
//  2 hit
static int vkaa_std_selector_test_input_hit(const vkaa_selector_param_t *restrict param, vkaa_var_s *restrict var, uintptr_t input_number, const uintptr_t *restrict input_typeid, uintptr_t *restrict rhit_type)
{
	const vkaa_selector_s *restrict s;
	uintptr_t i, rt;
	int rr;
	rr = -1;
	rt = 0;
	for (i = 0; i < input_number; ++i)
	{
		if (!input_typeid[i] || var->type_id == input_typeid[i])
		{
			rr = 2;
			rt = var->type_id;
			break;
		}
		else if (param->exec && (s = vkaa_std_convert_test_by_typeid(var->type, input_typeid[i], param->tpool)))
		{
			if (vkaa_std_selector_test_convert_is_promotion(s))
			{
				if (rr < 1)
				{
					rr = 1;
					rt = input_typeid[i];
				}
			}
			else
			{
				if (rr < 0)
				{
					rr = 0;
					rt = input_typeid[i];
				}
			}
		}
	}
	if (rhit_type) *rhit_type = rt;
	return rr;
}

static const vkaa_selector_param_t* vkaa_std_selector_test_input(const vkaa_selector_param_t *restrict param, uintptr_t input_number, const uintptr_t *restrict input_typeid, uintptr_t *restrict score)
{
	vkaa_var_s *const *restrict input_list;
	const uintptr_t *restrict maybe_typeid;
	const vkaa_selector_s *restrict s;
	uintptr_t i, n, sr, sp;
	input_list = param->input_list;
	sp = (uintptr_t) 0x80000000;
	if (~input_number)
	{
		if (input_number != param->input_number)
			goto label_fail;
		for (i = sr = 0; i < input_number; ++i)
		{
			if (input_list[i]->type_id == input_typeid[i])
				sr |= sp;
			else if (!param->exec || !(s = vkaa_std_convert_test_by_typeid(input_list[i]->type, input_typeid[i], param->tpool)))
				goto label_fail;
			else if (vkaa_std_selector_test_convert_is_promotion(s))
				sr |= (sp >> 1);
			sp >>= 2;
		}
	}
	else
	{
		input_number = param->input_number;
		for (i = sr = 0; i < input_number && (n = *input_typeid); ++i)
		{
			switch (vkaa_std_selector_test_input_hit(param, input_list[i], n, ++input_typeid, NULL))
			{
				case 2: sr |= sp; break;
				case 1: sr |= (sp >> 1); // fall through
				case 0: break;
				default: goto label_fail;
			}
			input_typeid += n;
			sp >>= 2;
		}
		if (!*input_typeid++)
		{
			if (i == input_number)
				goto label_okay;
			if (!*input_typeid)
				goto label_fail;
			maybe_typeid = input_typeid;
			while (i < input_number)
			{
				if (!(n = *input_typeid))
					n = *(input_typeid = maybe_typeid);
				switch (vkaa_std_selector_test_input_hit(param, input_list[i], n, ++input_typeid, NULL))
				{
					case 2: sr |= sp; break;
					case 1: sr |= (sp >> 1); // fall through
					case 0: break;
					default: goto label_fail;
				}
				input_typeid += n;
				sp >>= 2;
				++i;
			}
			if (*input_typeid)
				goto label_fail;
			if (i < input_number)
				goto label_fail;
		}
	}
	label_okay:
	if (score) *score = sr;
	return param;
	label_fail:
	return NULL;
}

static const vkaa_selector_param_t* vkaa_std_selector_fix_input(const vkaa_selector_param_t *restrict param, uintptr_t input_number, const uintptr_t *restrict input_typeid, vkaa_function_s *restrict function)
{
	vkaa_var_s *const *restrict input_list;
	const uintptr_t *restrict maybe_typeid;
	vkaa_function_s *restrict cf;
	uintptr_t i, n, rt;
	input_list = function->input_list;
	if (~input_number)
	{
		if (input_number != function->input_number)
			goto label_fail;
		for (i = 0; i < input_number; ++i)
		{
			if (input_list[i]->type_id == input_typeid[i])
				continue;
			if (!param->exec || !(cf = vkaa_std_convert_by_typeid(param->exec, param->tpool, input_list[i], input_typeid[i])))
				goto label_fail;
			if (!vkaa_function_set_input(function, i, vkaa_function_okay(cf)))
				goto label_fail;
		}
	}
	else
	{
		input_number = function->input_number;
		for (i = 0; i < input_number && (n = *input_typeid); ++i)
		{
			switch (vkaa_std_selector_test_input_hit(param, input_list[i], n, ++input_typeid, &rt))
			{
				case 2: continue;
				case 1:
				case 0:
					if (!(cf = vkaa_std_convert_by_typeid(param->exec, param->tpool, input_list[i], rt)))
						goto label_fail;
					if (!vkaa_function_set_input(function, i, vkaa_function_okay(cf)))
						goto label_fail;
					break;
				default: goto label_fail;
			}
			input_typeid += n;
		}
		if (!*input_typeid++)
		{
			if (i == input_number)
				goto label_okay;
			if (!*input_typeid)
				goto label_fail;
			maybe_typeid = input_typeid;
			while (i < input_number)
			{
				if (!(n = *input_typeid))
					n = *(input_typeid = maybe_typeid);
				switch (vkaa_std_selector_test_input_hit(param, input_list[i], n, ++input_typeid, &rt))
				{
					case 2: break;
					case 1:
					case 0:
						if (!(cf = vkaa_std_convert_by_typeid(param->exec, param->tpool, input_list[i], rt)))
							goto label_fail;
						if (!vkaa_function_set_input(function, i, vkaa_function_okay(cf)))
							goto label_fail;
						break;
					default: goto label_fail;
				}
				input_typeid += n;
				++i;
			}
			if (*input_typeid)
				goto label_fail;
			if (i < input_number)
				goto label_fail;
		}
	}
	label_okay:
	return param;
	label_fail:
	return NULL;
}

const vkaa_std_selector_desc_s* vkaa_std_selector_test(const vkaa_std_selector_desc_s *restrict desc, const vkaa_selector_param_t *restrict param, uintptr_t *restrict score)
{
	if (desc->this_typeid && (!param->this || param->this->type_id != desc->this_typeid))
		goto label_fail;
	switch (desc->output)
	{
		case vkaa_std_selector_output_any:
		case vkaa_std_selector_output_new:
		case vkaa_std_selector_output_this:
		case vkaa_std_selector_output_input_first:
			break;
		case vkaa_std_selector_output_must_first:
			if (param->input_number && param->input_list[0]->type_id == desc->output_typeid)
				break;
			// fall through
		default: goto label_fail;
	}
	if (vkaa_std_selector_test_input(param, desc->input_number, desc->input_typeid, score))
		return desc;
	label_fail:
	if (score) *score = 0;
	return NULL;
}

vkaa_function_s* vkaa_std_selector_create(const vkaa_selector_param_t *restrict param, const vkaa_std_selector_desc_s *restrict desc)
{
	vkaa_function_s *restrict rf;
	if ((rf = vkaa_function_alloc(desc->pri_data, desc->function,
		vkaa_tpool_find_id(param->tpool, desc->output_typeid),
		param->input_number, param->input_list)))
	{
		if (desc->this_typeid)
		{
			if (!param->this || param->this->type_id != desc->this_typeid)
				goto label_fail;
			vkaa_function_set_this(rf, param->this);
		}
		if (vkaa_std_selector_fix_input(param, desc->input_number, desc->input_typeid, rf))
		{
			switch (desc->output)
			{
				case vkaa_std_selector_output_any:
					break;
				case vkaa_std_selector_output_this:
					if (!vkaa_function_set_output(rf, rf->this))
						goto label_fail;
					break;
				case vkaa_std_selector_output_input_first:
					if (!vkaa_function_set_output(rf, rf->input_list[0]))
						goto label_fail;
					break;
				case vkaa_std_selector_output_must_first:
					if (rf->input_list[0] != param->input_list[0])
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
		}
		label_fail:
		refer_free(rf);
	}
	return NULL;
}

vkaa_function_s* vkaa_std_selector_selector(const vkaa_std_selector_s *restrict selector, const vkaa_selector_param_t *restrict param)
{
	vattr_vlist_t *restrict vl;
	const vkaa_std_selector_desc_s *restrict desc, *restrict hit_desc;
	uintptr_t score, max_score;
	hit_desc = NULL;
	max_score = 0;
	for (vl = selector->std_desc->vattr; vl; vl = vl->vattr_next)
	{
		if ((desc = (const vkaa_std_selector_desc_s *) vl->value) &&
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
		return vkaa_std_selector_create(param, hit_desc);
	return NULL;
}
