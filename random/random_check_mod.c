#include "random_check_pri.h"

struct random_check_layer_mod_s {
	random_check_layer_s layer;
	uint32_t mod;
	uint32_t sum[];
};

random_check_define_layer_check(mod)
{
	r->sum[x % r->mod] += 1;
}

random_check_define_layer_clear(mod)
{
	memset(r->sum, 0, sizeof(uint32_t) * r->mod);
}

random_check_define_layer_dump(mod)
{
	mlog_printf(ml, "[layer mod %u] ", r->mod);
	random_check_dump_statistics(ml, r->sum, r->mod);
}

random_check_s* random_check_layer_mod(random_check_s *restrict rc, uint32_t mod)
{
	if (mod)
	{
		register struct random_check_layer_mod_s *restrict r;
		r = (struct random_check_layer_mod_s *) refer_alloz(sizeof(struct random_check_layer_mod_s) + sizeof(uint32_t) * mod);
		if (r)
		{
			r->mod = mod;
			random_check_append_layer(rc, r->layer, mod);
			return rc;
		}
	}
	return NULL;
}
