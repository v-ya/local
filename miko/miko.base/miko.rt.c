#include "miko.rt.h"

static miko_process_runtime_s* miko_rt_process_builder_func(const miko_process_s *restrict process, miko_bool_t *restrict isokay)
{
	miko_process_create_f create;
	miko_process_runtime_s *value;
	if (process && (create = process->create))
	{
		if ((value = create(process)))
			return value;
		*isokay = 0;
	}
	return NULL;
}

static void miko_rt_free_func(miko_rt_s *restrict r)
{
	refer_ck_free(r->dynamic.segment);
	miko_wink_ck_free(r->dynamic.stack);
	refer_ck_free(r->process);
	refer_ck_free(r->runtime);
	refer_ck_free(r->gomi);
	refer_ck_free(r->mlog);
	refer_ck_free(r->program);
}

miko_rt_s* miko_rt_alloc(miko_program_s *restrict program, mlog_s *restrict mlog, uintptr_t stack_max_limit)
{
	miko_rt_s *restrict r;
	if (program && (r = (miko_rt_s *) refer_alloz(sizeof(miko_rt_s))))
	{
		refer_hook_free(r, rt);
		r->program = (miko_program_s *) refer_save(program);
		r->mlog = (mlog_s *) refer_save(mlog);
		r->gomi = (miko_wink_gomi_s *) refer_save(program->gomi);
		r->runtime = (const miko_env_runtime_s *) refer_save(program->runtime);
		if (r->gomi && r->runtime &&
			(r->process = miko_table_create_builder(r->runtime->process,
				(miko_table_builder_f) miko_rt_process_builder_func)) &&
			(r->dynamic.stack = miko_form_alloc(r->gomi, stack_max_limit)) &&
			(r->dynamic.segment = miko_form_table_alloc(r->runtime->segment->count)))
			return r;
		refer_free(r);
	}
	return NULL;
}

void miko_rt_reset(miko_rt_s *restrict r)
{
	miko_process_runtime_s *const *restrict p;
	miko_process_runtime_s *restrict v;
	uintptr_t i, n;
	r->dynamic.context.instr_array = NULL;
	r->dynamic.context.instr_count = 0;
	r->dynamic.context.instr_index = 0;
	miko_form_table_clear(r->dynamic.segment);
	miko_form_keep_count(r->dynamic.stack, 0);
	p = (miko_process_runtime_s *const *) r->process->table;
	n = r->process->count;
	for (i = 0; i < n; ++i)
	{
		if ((v = p[i]) && v->reset)
			v->reset(v);
	}
}

miko_rt_s* miko_rt_load(miko_rt_s *restrict r, const char *restrict name, miko_count_t argc, const miko_program_argv_t *restrict argv)
{
	miko_program_symbol_f symbol;
	miko_rt_reset(r);
	if ((symbol = r->program->symbol) && symbol(&r->dynamic, r->program, name, argc, argv))
		return r;
	return NULL;
}

miko_rt_s* miko_rt_exec(miko_rt_s *restrict r)
{
	volatile miko_rt_context_t *context;
	const miko_instr_t *restrict instr;
	miko_form_table_t segment;
	miko_interrupt_t interrupt;
	uintptr_t index;
	context = (volatile miko_rt_context_t *) &r->dynamic.context;
	segment = r->dynamic.segment->table_array;
	while ((index = context->instr_index) < context->instr_count)
	{
		instr = context->instr_array[index];
		context->instr_index = index + 1;
		if (!(interrupt = instr->instr_func(instr, segment, r)))
			continue;
		if (!r->program->interrupt || !r->program->interrupt(r, interrupt))
			goto label_fail;
	}
	return r;
	label_fail:
	return NULL;
}

const miko_rt_backup_s* miko_rt_backup(miko_rt_s *restrict r)
{
	return miko_rt_backup_alloc(r);
}

miko_rt_s* miko_rt_goback(miko_rt_s *restrict r, const miko_rt_backup_s *restrict backup)
{
	return miko_rt_backup_goback(backup, r);
}

uintptr_t miko_rt_backup_only_instr_index(miko_rt_s *restrict r)
{
	return r->dynamic.context.instr_index;
}

void miko_rt_goback_only_instr_index(miko_rt_s *restrict r, uintptr_t instr_index)
{
	r->dynamic.context.instr_index = instr_index;
}
