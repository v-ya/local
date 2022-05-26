#include "../vkaa.execute.h"
#include "../vkaa.syntax.h"
#include "../vkaa.error.h"
#include "../vkaa.function.h"

typedef struct vkaa_execute_label_s {
	exbuffer_t jumper_last_func_pos;
	uintptr_t label_pos;
} vkaa_execute_label_s;

// label

static void vkaa_execute_label_free_func(vkaa_execute_label_s *restrict r)
{
	exbuffer_uini(&r->jumper_last_func_pos);
}

static vkaa_execute_label_s* vkaa_execute_label_alloc(void)
{
	vkaa_execute_label_s *restrict r;
	if ((r = (vkaa_execute_label_s *) refer_alloz(sizeof(vkaa_execute_label_s))))
	{
		refer_set_free(r, (refer_free_f) vkaa_execute_label_free_func);
		r->label_pos = ~(uintptr_t) 0;
		if (exbuffer_init(&r->jumper_last_func_pos, 0))
			return r;
		refer_free(r);
	}
	return NULL;
}

static vkaa_execute_label_s* vkaa_execute_label_append_jumper(vkaa_execute_label_s *restrict r, uintptr_t jumper_pos)
{
	if (exbuffer_append(&r->jumper_last_func_pos, &jumper_pos, sizeof(jumper_pos)))
		return r;
	return NULL;
}

static vkaa_execute_label_s* vkaa_execute_label_okay(vkaa_execute_label_s *restrict r, vkaa_execute_t *restrict array, uintptr_t number)
{
	const uintptr_t *restrict jumper;
	uintptr_t i, n, j;
	if (r->label_pos > number)
		goto label_fail;
	jumper = (const uintptr_t *) r->jumper_last_func_pos.data;
	n = r->jumper_last_func_pos.used / sizeof(*jumper);
	for (i = 0; i < n; ++i)
	{
		if ((j = jumper[i]) >= number)
			goto label_fail;
		array[j].jump = r->label_pos;
	}
	r->jumper_last_func_pos.used = 0;
	return r;
	label_fail:
	return NULL;
}

// execute

static void vkaa_execute_free_func(vkaa_execute_s *restrict r)
{
	vkaa_execute_t *restrict array;
	uintptr_t i, n;
	array = r->execute_array;
	n = r->execute_number;
	for (i = 0; i < n; ++i)
		refer_free(array[i].func);
	if (r->label) refer_free(r->label);
	if (r->elog) refer_free(r->elog);
	exbuffer_uini(&r->buffer);
}

vkaa_execute_s* vkaa_execute_alloc(void)
{
	vkaa_execute_s *restrict r;
	if ((r = (vkaa_execute_s *) refer_alloz(sizeof(vkaa_execute_s))))
	{
		refer_set_free(r, (refer_free_f) vkaa_execute_free_func);
		if (exbuffer_init(&r->buffer, 0) &&
			(r->label = vattr_alloc()))
			return r;
		refer_free(r);
	}
	return NULL;
}

vkaa_execute_t* vkaa_execute_get_last_item(const vkaa_execute_s *restrict exec)
{
	if (exec->execute_number)
		return &exec->execute_array[exec->execute_number - 1];
	return NULL;
}

vkaa_function_s* vkaa_execute_get_last_function(const vkaa_execute_s *restrict exec)
{
	if (exec->execute_number)
		return exec->execute_array[exec->execute_number - 1].func;
	return NULL;
}

vkaa_var_s* vkaa_execute_get_last_var(const vkaa_execute_s *restrict exec)
{
	vkaa_function_s *restrict func;
	if ((func = vkaa_execute_get_last_function(exec)))
		return vkaa_function_okay(func);
	return NULL;
}

uintptr_t vkaa_execute_next_pos(vkaa_execute_s *restrict exec)
{
	return exec->execute_number;
}

vkaa_execute_s* vkaa_execute_push_label(vkaa_execute_s *restrict exec, const char *restrict label)
{
	vkaa_execute_label_s *restrict el;
	vkaa_execute_s *rr;
	rr = NULL;
	if ((el = vkaa_execute_label_alloc()))
	{
		if (vattr_insert_first(exec->label, label, el))
			rr = exec;
		refer_free(el);
	}
	return rr;
}

vkaa_execute_s* vkaa_execute_pop_label(vkaa_execute_s *restrict exec, const char *restrict label, uintptr_t label_pos)
{
	vattr_vlist_t *restrict vl;
	vkaa_execute_label_s *restrict el;
	if ((vl = vattr_get_vlist_first(exec->label, label)) &&
		(el = (vkaa_execute_label_s *) vl->value))
	{
		el->label_pos = label_pos;
		if (vkaa_execute_label_okay(el, exec->execute_array, exec->execute_number))
		{
			vattr_delete_vlist(vl);
			return exec;
		}
	}
	return NULL;
}

vkaa_execute_s* vkaa_execute_jump_to_label(vkaa_execute_s *restrict exec, const char *restrict label, uintptr_t stack_pos, uintptr_t jumper_pos)
{
	vkaa_execute_label_s *restrict el;
	if ((el = (vkaa_execute_label_s *) vattr_get_index(exec->label, label, stack_pos)) &&
		vkaa_execute_label_append_jumper(el, jumper_pos))
		return exec;
	return NULL;
}

vkaa_execute_s* vkaa_execute_set_label_without_exist(vkaa_execute_s *restrict exec, const char *restrict label, uintptr_t label_pos)
{
	vkaa_execute_label_s *restrict el;
	vattr_vslot_t *restrict vslot;
	vattr_vlist_t *restrict vl;
	if ((vslot = vattr_get_vslot(exec->label, label)))
	{
		if (vslot->number == 1)
		{
			el = (vkaa_execute_label_s *) vslot->vslot->value;
			if (!~el->label_pos)
			{
				label_set_label_pos:
				el->label_pos = label_pos;
				return exec;
			}
		}
	}
	else if ((el = vkaa_execute_label_alloc()))
	{
		vl = vattr_set(exec->label, label, el);
		refer_free(el);
		if (vl) goto label_set_label_pos;
	}
	return NULL;
}

vkaa_execute_s* vkaa_execute_set_unlabel(vkaa_execute_s *restrict exec, const char *restrict label)
{
	vattr_vslot_t *restrict vslot;
	if ((vslot = vattr_get_vslot(exec->label, label)) && vslot->number == 1)
	{
		if (vkaa_execute_label_okay((vkaa_execute_label_s *) vslot->vslot->value, exec->execute_array, exec->execute_number))
		{
			vattr_delete_vslot(vslot);
			return exec;
		}
	}
	return NULL;
}

vkaa_execute_s* vkaa_execute_add_jump_any(vkaa_execute_s *restrict exec, const char *restrict label, uintptr_t jumper_pos)
{
	vkaa_execute_label_s *restrict el;
	vattr_vlist_t *restrict vl;
	if ((el = (vkaa_execute_label_s *) vattr_get_first(exec->label, label)))
	{
		label_append_jumper:
		if (vkaa_execute_label_append_jumper(el, jumper_pos))
			return exec;
	}
	else if ((el = vkaa_execute_label_alloc()))
	{
		vl = vattr_set(exec->label, label, el);
		refer_free(el);
		if (vl) goto label_append_jumper;
	}
	return NULL;
}

vkaa_execute_s* vkaa_execute_push(vkaa_execute_s *restrict exec, vkaa_function_s *restrict func)
{
	vkaa_execute_t data;
	vkaa_execute_t *restrict array;
	data.func = func;
	data.jump = ~(uintptr_t) 0;
	if (func && vkaa_execute_okay_last_function(exec) &&
		(array = (vkaa_execute_t *) exbuffer_append(&exec->buffer, &data, sizeof(data))))
	{
		exec->execute_array = array;
		exec->execute_number += 1;
		refer_save(func);
		return exec;
	}
	return NULL;
}

vkaa_function_s* vkaa_execute_pop(vkaa_execute_s *restrict exec)
{
	if (exec->execute_number)
	{
		exec->buffer.used -= sizeof(vkaa_execute_t);
		return exec->execute_array[--exec->execute_number].func;
	}
	return NULL;
}

vkaa_execute_s* vkaa_execute_okay_last_function(vkaa_execute_s *restrict exec)
{
	vkaa_function_s *restrict last;
	if (!(last = vkaa_execute_get_last_function(exec)) || vkaa_function_okay(last))
		return exec;
	return NULL;
}

vkaa_execute_s* vkaa_execute_okay_label(vkaa_execute_s *restrict exec)
{
	vattr_vlist_t *restrict vl, *restrict vd;
	vl = exec->label->vattr;
	while ((vd = vl))
	{
		if (!vkaa_execute_label_okay((vkaa_execute_label_s *) vl->value, exec->execute_array, exec->execute_number))
			goto label_fail;
		vl = vl->vattr_next;
		vattr_delete_vlist(vd);
	}
	return exec;
	label_fail:
	return NULL;
}

vkaa_execute_s* vkaa_execute_okay(vkaa_execute_s *restrict exec)
{
	if (vkaa_execute_okay_last_function(exec) && vkaa_execute_okay_label(exec))
		return exec;
	return NULL;
}

uintptr_t vkaa_execute_do(const vkaa_execute_s *restrict exec, const volatile uintptr_t *running)
{
	vkaa_execute_t *restrict array;
	vkaa_function_s *restrict func;
	vkaa_execute_control_t c;
	uintptr_t n, error;
	c.next_pos = 0;
	c.array = array = exec->execute_array;
	c.number = n = exec->execute_number;
	if (!(c.running = running))
	{
		while (c.next_pos < n)
		{
			func = array[c.next_pos++].func;
			if ((error = func->function(func, &c)))
				goto label_fail;
		}
	}
	else
	{
		while (*running && c.next_pos < n)
		{
			func = array[c.next_pos++].func;
			if ((error = func->function(func, &c)))
				goto label_fail;
		}
	}
	return 0;
	label_fail:
	vkaa_execute_elog_print_rtime(exec, c.next_pos - 1, error);
	return error;
}

void vkaa_execute_clear(vkaa_execute_s *restrict exec)
{
	vkaa_execute_t *restrict array;
	uintptr_t i, n;
	array = exec->execute_array;
	n = exec->execute_number;
	exec->execute_number = 0;
	exec->execute_array = NULL;
	for (i = 0; i < n; ++i)
		refer_free(array[i].func);
	vattr_clear(exec->label);
}

vkaa_execute_s* vkaa_execute_elog_enable(vkaa_execute_s *restrict exec, mlog_s *ctime, mlog_s *rtime, const vkaa_error_s *error)
{
	if (!exec->elog)
	{
		if ((exec->elog = vkaa_elog_alloc(ctime, rtime, error)))
			return exec;
	}
	return NULL;
}

vkaa_execute_s* vkaa_execute_elog_enable_by_exec(vkaa_execute_s *restrict exec, const vkaa_execute_s *restrict src)
{
	vkaa_elog_s *restrict elog;
	if (!exec->elog)
	{
		if (!(elog = src->elog))
			goto label_okay;
		else if (!elog->parse && !elog->exec)
			goto label_okay;
		else if ((exec->elog = vkaa_elog_alloc(elog->parse, elog->exec, elog->error)))
		{
			label_okay:
			return exec;
		}
	}
	return NULL;
}

vkaa_execute_s* vkaa_execute_elog_push(vkaa_execute_s *restrict exec, const vkaa_syntax_source_s *source, const vkaa_syntax_t *restrict syntax, uintptr_t number)
{
	if (!exec->elog || !number || vkaa_elog_push(exec->elog, source, exec->execute_number, syntax[0].pos))
		return exec;
	return NULL;
}

vkaa_execute_s* vkaa_execute_elog_fence(vkaa_execute_s *restrict exec, const vkaa_syntax_source_s *source, const vkaa_syntax_t *restrict syntax, uintptr_t number, uintptr_t pos)
{
	if (!exec->elog || pos >= number || vkaa_elog_fence(exec->elog, source, exec->execute_number, syntax[pos].pos))
		return exec;
	return NULL;
}

vkaa_execute_s* vkaa_execute_elog_pop(vkaa_execute_s *restrict exec, const vkaa_syntax_t *restrict syntax, uintptr_t number)
{
	if (!exec->elog || !number || vkaa_elog_pop(exec->elog, exec->execute_number, syntax[number - 1].pos))
		return exec;
	return NULL;
}

void vkaa_execute_elog_print_ctime(const vkaa_execute_s *restrict exec, const vkaa_syntax_source_s *restrict source, uint32_t syntax_pos, const char *restrict message)
{
	if (exec->elog)
		vkaa_elog_print_parse(exec->elog, source, syntax_pos, exec->execute_number, message);
}

void vkaa_execute_elog_print_rtime(const vkaa_execute_s *restrict exec, uintptr_t pos, uintptr_t error)
{
	vkaa_elog_s *restrict elog;
	if ((elog = exec->elog))
		vkaa_elog_print_exec(elog, pos, elog->error?vkaa_error_get_name(elog->error, error):NULL);
}
