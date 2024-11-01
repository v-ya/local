#include "miko.env.h"
#include "miko.instruction.h"

static void miko_env_table_free_func(miko_env_table_s *restrict r)
{
	refer_ck_free(r->segment);
	refer_ck_free(r->action);
	refer_ck_free(r->major);
	refer_ck_free(r->instruction);
}

const miko_env_table_s* miko_env_table_alloc(const miko_env_s *restrict env)
{
	miko_env_table_s *restrict r;
	if ((r = (miko_env_table_s *) refer_alloz(sizeof(miko_env_table_s))))
	{
		refer_hook_free(r, env_table);
		if ((r->segment = miko_table_create_single(env->segment)) &&
			(r->action = miko_table_create_single(env->action)) &&
			(r->major = miko_table_create_single(env->major)) &&
			(r->instruction = miko_table_create_multi(env->instruction)))
			return r;
		refer_free(r);
	}
	return NULL;
}

refer_string_t miko_env_table_segment_by_index(const miko_env_table_s *restrict r, miko_index_t index)
{
	if (index < r->segment->count)
		return (refer_string_t) r->segment->table[index];
	return NULL;
}

refer_string_t miko_env_table_segment_by_name(const miko_env_table_s *restrict r, const char *restrict name)
{
	return (refer_string_t) r->segment->table[miko_table_index(r->segment, name)];
}

refer_string_t miko_env_table_action_by_index(const miko_env_table_s *restrict r, miko_index_t index)
{
	if (index < r->action->count)
		return (refer_string_t) r->action->table[index];
	return NULL;
}

refer_string_t miko_env_table_action_by_name(const miko_env_table_s *restrict r, const char *restrict name)
{
	return (refer_string_t) r->action->table[miko_table_index(r->action, name)];
}

miko_major_s* miko_env_table_major_by_index(const miko_env_table_s *restrict r, miko_index_t index)
{
	if (index < r->major->count)
		return (miko_major_s *) r->major->table[index];
	return NULL;
}

miko_major_s* miko_env_table_major_by_name(const miko_env_table_s *restrict r, const char *restrict name)
{
	return (miko_major_s *) r->major->table[miko_table_index(r->major, name)];
}

const miko_instruction_s* miko_env_table_instruction_by_index(const miko_env_table_s *restrict r, miko_index_t index)
{
	if (index < r->instruction->count)
		return (const miko_instruction_s *) r->instruction->table[index];
	return NULL;
}

const miko_instruction_s* const* miko_env_table_instruction_by_name(const miko_env_table_s *restrict r, const char *restrict name, miko_count_t *restrict count)
{
	miko_index_t index;
	if (miko_table_range(r->instruction, name, &index, count))
		return (const miko_instruction_s *const *) r->instruction->table + index;
	return NULL;
}

miko_index_t miko_env_table_instruction_get_index(const miko_env_table_s *restrict r, const char *restrict name, const miko_access_prefix_t prefix[])
{
	const miko_instruction_s *const *restrict p;
	miko_index_t index;
	miko_count_t count;
	if (miko_table_range(r->instruction, name, &index, &count))
	{
		p = (const miko_instruction_s *const *) r->instruction->table;
		for (count += index; index < count; ++index)
		{
			if (miko_instruction_prefix_vaild(p[index]->prefix, prefix))
				return index;
		}
	}
	return 0;
}

const miko_instruction_s* miko_env_table_instruction_get_ptr(const miko_env_table_s *restrict r, const char *restrict name, const miko_access_prefix_t prefix[])
{
	const miko_instruction_s *const *restrict p;
	miko_index_t index;
	miko_count_t count;
	if (miko_table_range(r->instruction, name, &index, &count))
	{
		p = (const miko_instruction_s *const *) r->instruction->table;
		for (count += index; index < count; ++index)
		{
			if (miko_instruction_prefix_vaild(p[index]->prefix, prefix))
				return p[index];
		}
	}
	return 0;
}
