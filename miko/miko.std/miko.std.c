#include "miko.std.h"

struct miko_iset_s* miko_std_env_register(miko_env_s *restrict env, struct miko_log_s *restrict log)
{
	miko_iset_s *restrict r;
	if ((r = miko_iset_alloc(miko_std_name)))
	{
		// segment
		if (!miko_iset_add_segment(r, miko_std_segment__stack))
			goto label_fail;
		if (!miko_iset_add_segment(r, miko_std_segment__static))
			goto label_fail;
		if (!miko_iset_add_segment(r, miko_std_segment__this))
			goto label_fail;
		// action
		if (!miko_iset_add_action(r, miko_std_action__read))
			goto label_fail;
		if (!miko_iset_add_action(r, miko_std_action__write))
			goto label_fail;
		if (!miko_iset_add_action(r, miko_std_action__exec))
			goto label_fail;
		// major and minor
		// instruction
		return r;
		label_fail:
		refer_free(r);
	}
	miko_log_add_key_value(log, miko_log_level__error, miko_status_create_fail, miko_std_name);
	return NULL;
}
