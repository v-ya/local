#include "miko.std.h"

struct miko_iset_s* miko_std_env_register(miko_env_s *restrict env, struct miko_log_s *restrict log)
{
	return miko_iset_alloc(miko_std_name);
}
