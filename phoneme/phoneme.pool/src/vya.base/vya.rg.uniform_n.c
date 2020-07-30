#include <phoneme/phoneme.h>
#include <math.h>
#include "../vya.common/random_generate.h"

typedef struct rg_uniform_n_s {
	size_t n;
} rg_uniform_n_s;

static dyl_used random_generate_func(rg_uniform_n, register rg_uniform_n_s*restrict)
{
	// n = rsrc + ... + rsrc |pri->n
	// = (n / pri->n - 0.5) * sqrt(12 * pri->n) * sigma + mu
	register size_t i;
	register double n;
	n = 0;
	if (pri && (i = pri->n))
	{
		while (i)
		{
			--i;
			n += rsrc_generate(rsrc);
		}
		i = pri->n;
		n = (n / i - 0.5) * sqrt(12.0 * i) * sigma + mu;
	}
	return n;
}
dyl_export(rg_uniform_n, vya.rg.uniform_n);

static dyl_used phoneme_arg2pri_func(rg_uniform_n_arg, rg_uniform_n_s*)
{
	register rg_uniform_n_s *restrict r;
	r = refer_alloc(sizeof(rg_uniform_n_s));
	if (r)
	{
		int64_t n;
		n = 1;
		if (arg) json_get_integer(arg, ".n", &n);
		if (n < 1) n = 1;
		if (n > 16) n = 16;
		r->n = n;
	}
	return r;
}
dyl_export(rg_uniform_n_arg, vya.rg.uniform_n.arg);
