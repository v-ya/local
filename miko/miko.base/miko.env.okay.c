#include "miko.env.h"

static void miko_env_reset(miko_env_s *restrict r)
{
	vattr_clear(r->iset_type);
	vattr_clear(r->iset_instruction);
}

miko_env_s* miko_env_okay(miko_env_s *restrict r)
{
	miko_env_reset(r);
	return r;
}
