#include "miko.env.h"
#include "miko.iset.h"
#include "miko.iset.pool.h"

const miko_iset_pool_s* miko_env_save_depend(miko_env_s *restrict r, miko_iset_s *restrict iset)
{
	return miko_iset_pool_save_depend(r->pool, iset, r->log);
}
