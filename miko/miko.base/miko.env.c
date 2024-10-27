#include "miko.env.h"

static void miko_env_free_func(miko_env_s *restrict r)
{
	refer_ck_free(r->mlog);
	refer_ck_free(r->pool);
}

miko_env_s* miko_env_alloc(mlog_s *restrict mlog)
{
	miko_env_s *restrict r;
	if ((r = (miko_env_s *) refer_alloz(sizeof(miko_env_s))))
	{
		refer_hook_free(r, env);
		r->mlog = (mlog_s *) refer_save(mlog);
		if ((r->pool = vattr_alloc()))
			return r;
		refer_free(r);
	}
	return NULL;
}
