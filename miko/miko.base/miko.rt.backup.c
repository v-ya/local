#include "miko.rt.h"

static refer_t miko_rt_backup_process_backup_builder_func(miko_process_runtime_s *restrict process, miko_bool_t *restrict isokay)
{
	miko_process_runtime_backup_f backup;
	refer_t value;
	if (process && (backup = process->backup))
	{
		if ((value = backup(process)))
			return value;
		*isokay = 0;
	}
	return NULL;
}

static void miko_rt_backup_free_func(miko_rt_backup_s *restrict r)
{
	refer_ck_free(r->dynamic_backup.segment);
	miko_wink_ck_free(r->dynamic_backup.stack);
	refer_ck_free(r->process_backup);
}

const miko_rt_backup_s* miko_rt_backup_alloc(const miko_rt_s *restrict runtime)
{
	miko_rt_backup_s *restrict r;
	const miko_form_w *restrict stack;
	const miko_form_table_s *restrict segment;
	if (runtime && (r = (miko_rt_backup_s *) refer_alloz(sizeof(miko_rt_backup_s))))
	{
		refer_hook_free(r, rt_backup);
		r->runtime = (const miko_rt_s *) refer_save(runtime);
		if (!(r->process_backup = miko_table_create_builder(runtime->process,
			(miko_table_builder_f) miko_rt_backup_process_backup_builder_func)))
			goto label_fail;
		if ((stack = runtime->dynamic.stack)->count)
		{
			if (!((r->dynamic_backup.stack = miko_form_alloc(runtime->gomi, stack->count))))
				goto label_fail;
			if (!miko_form_push_n_copy(r->dynamic_backup.stack, stack->form, stack->count))
				goto label_fail;
		}
		if (!(r->dynamic_backup.segment = miko_form_table_alloc((segment = runtime->dynamic.segment)->table_count)))
			goto label_fail;
		if (!miko_form_table_copy(r->dynamic_backup.segment, segment))
			goto label_fail;
		r->dynamic_backup.context.instr_array = runtime->dynamic.context.instr_array;
		r->dynamic_backup.context.instr_count = runtime->dynamic.context.instr_count;
		r->dynamic_backup.context.instr_index = runtime->dynamic.context.instr_index;
		return r;
		label_fail:
		refer_free(r);
	}
	return NULL;
}

miko_rt_s* miko_rt_backup_goback(const miko_rt_backup_s *restrict r, miko_rt_s *restrict runtime)
{
	miko_process_runtime_s *const *restrict pr;
	miko_process_runtime_s *restrict v;
	const miko_form_w *restrict stack;
	const refer_t *restrict pb;
	uintptr_t i, n;
	if (r->runtime == runtime)
	{
		pr = (miko_process_runtime_s *const *) runtime->process->table;
		pb = r->process_backup->table;
		n = r->process_backup->count;
		for (i = 0; i < n; ++i)
		{
			if ((v = pr[i]) && v->goback)
			{
				if (!v->goback(v, pb[i]))
					goto label_fail;
			}
		}
		miko_form_keep_count(runtime->dynamic.stack, 0);
		if ((stack = r->dynamic_backup.stack))
		{
			if (!miko_form_push_n_copy(runtime->dynamic.stack, stack->form, stack->count))
				goto label_fail;
		}
		if (!miko_form_table_copy(runtime->dynamic.segment, r->dynamic_backup.segment))
			goto label_fail;
		runtime->dynamic.context.instr_array = r->dynamic_backup.context.instr_array;
		runtime->dynamic.context.instr_count = r->dynamic_backup.context.instr_count;
		runtime->dynamic.context.instr_index = r->dynamic_backup.context.instr_index;
		return runtime;
	}
	label_fail:
	return NULL;
}
