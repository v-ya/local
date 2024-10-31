#include "miko.env.h"
#include "miko.iset.h"

static void miko_env_free_func(miko_env_s *restrict r)
{
	refer_ck_free(r->mlog);
	refer_ck_free(r->log);
	refer_ck_free(r->pool);
	refer_ck_free(r->segment);
	refer_ck_free(r->action);
	refer_ck_free(r->major);
	refer_ck_free(r->instruction);
}

static miko_env_s* miko_env_alloc(mlog_s *restrict mlog)
{
	miko_env_s *restrict r;
	if ((r = (miko_env_s *) refer_alloz(sizeof(miko_env_s))))
	{
		refer_hook_free(r, env);
		r->mlog = (mlog_s *) refer_save(mlog);
		if ((!mlog || (r->log = miko_log_alloc())) &&
			(r->pool = vattr_alloc()) &&
			(r->segment = vattr_alloc()) &&
			(r->action = vattr_alloc()) &&
			(r->major = vattr_alloc()) &&
			(r->instruction = vattr_alloc()))
			return r;
		refer_free(r);
	}
	return NULL;
}

static miko_env_s* miko_env_add_iset(miko_env_s *restrict r, miko_env_register_f register_func)
{
	miko_iset_s *restrict iset;
	miko_env_s *result;
	result = NULL;
	if ((iset = register_func(r, r->log)))
	{
		if (!vattr_get_vslot(r->pool, iset->name))
		{
			if (vattr_insert_tail(r->pool, iset->name, iset))
			{
				miko_log_add_key(r->log, miko_log_level__notify, miko_status_register_okay, iset->name);
				result = r;
			}
			else miko_log_add_key(r->log, miko_log_level__error, miko_status_memless, iset->name);
		}
		else miko_log_add_key(r->log, miko_log_level__error, miko_status_conflict, iset->name);
		refer_free(iset);
	}
	else miko_log_add_key(r->log, miko_log_level__error, miko_status_create_fail, NULL);
	return result;
}

miko_env_s* miko_env_create(mlog_s *restrict mlog, miko_env_register_f register_array[])
{
	miko_env_s *restrict r;
	miko_env_s *result;
	uintptr_t i, into;
	if ((r = miko_env_alloc(mlog)))
	{
		result = r;
		into = miko_log_into_name(r->log, miko_stage_env_register);
		for (i = 0; result && register_array[i]; ++i)
			result = miko_env_add_iset(r, register_array[i]);
		miko_log_back(r->log, into);
		if (!result) goto label_fail;
		into = miko_log_into_name(r->log, miko_stage_env_merge);
		result = miko_env_okay(r);
		miko_log_back(r->log, into);
		if (!result) goto label_fail;
		miko_log_print(r->log, r->mlog);
		return r;
		label_fail:
		miko_log_print(r->log, r->mlog);
		refer_free(r);
	}
	return NULL;
}
