#include "miko.code.h"

static void miko_code_builder_free_func(miko_code_builder_s *restrict r)
{
	refer_ck_free(r->runtime);
	refer_ck_free(r->cache);
	exbuffer_uini(&r->prefix);
	exbuffer_uini(&r->access);
}

miko_code_builder_s* miko_code_builder_alloc(const miko_env_runtime_s *restrict runtime)
{
	miko_code_builder_s *restrict r;
	if (runtime && (r = (miko_code_builder_s *) refer_alloz(sizeof(miko_code_builder_s))))
	{
		refer_hook_free(r, code_builder);
		r->runtime = (const miko_env_runtime_s *) refer_save(runtime);
		if (exbuffer_init(&r->prefix, 0) &&
			exbuffer_init(&r->access, 0))
			return r;
		refer_free(r);
	}
	return NULL;
}

miko_code_builder_s* miko_code_builder_new_code(miko_code_builder_s *restrict r)
{
	refer_ck_free(r->cache);
	if ((r->cache = miko_code_alloc()))
		return r;
	return NULL;
}

void miko_code_builder_argv_reset(miko_code_builder_s *restrict r)
{
	r->prefix.used = 0;
	r->access.used = 0;
}

miko_code_builder_s* miko_code_builder_argv_push(miko_code_builder_s *restrict r, const miko_access_prefix_t *restrict prefix, miko_xpos_t xpos)
{
	miko_access_t access;
	access.xseg = 0;
	access.xpos = xpos;
	if (prefix->segment)
		access.xseg = miko_table_index(r->runtime->segment, prefix->segment);
	if (exbuffer_append(&r->prefix, prefix, sizeof(miko_access_prefix_t)) &&
		exbuffer_append(&r->access, &access, sizeof(access)))
		return r;
	return NULL;
}

static miko_code_builder_s* miko_code_builder_argv_end(miko_code_builder_s *restrict r)
{
	static const miko_access_prefix_t prefix = {
		.segment = NULL,
		.action = NULL,
		.major = NULL,
		.minor = NULL
	};
	if (exbuffer_append(&r->prefix, &prefix, sizeof(miko_access_prefix_t)))
		return r;
	return NULL;
}

miko_code_builder_s* miko_code_builder_code_push_instruction(miko_code_builder_s *restrict r, const char *restrict name)
{
	miko_index_t index;
	if (name && r->cache && miko_code_builder_argv_end(r) &&
		(index = miko_env_runtime_instruction_get_index(r->runtime,
			name, (const miko_access_prefix_t *) r->prefix.data)) &&
		miko_code_push_instruction(r->cache, r->runtime,
			index, (const miko_access_t *) r->access.data))
	{
		miko_code_builder_argv_reset(r);
		return r;
	}
	miko_code_builder_argv_reset(r);
	return NULL;
}

miko_code_builder_s* miko_code_builder_code_push_code(miko_code_builder_s *restrict r, const miko_code_s *restrict code)
{
	miko_code_builder_argv_reset(r);
	if (code && r->cache && miko_code_push_code(r->cache, code))
		return r;
	return NULL;
}

miko_code_s* miko_code_builder_save_code(miko_code_builder_s *restrict r)
{
	miko_code_s *restrict code;
	if (r->cache && (code = miko_code_okay(r->cache)))
	{
		r->cache = NULL;
		return code;
	}
	return NULL;
}
