#include <phoneme/phoneme.h>
#include "random.inc"
#include "get_float.inc"

typedef struct random_const_s {
	refer_t r;
	double mu;
	double sigma;
} random_const_s;

static dyl_used double random_const(register random_const_s *restrict rc)
{
	return random(rc->r, rc->mu, rc->sigma);
}
dyl_export(random_const, vya.common.random_const);

static void random_const_free_func(register random_const_s *restrict rc)
{
	if (rc->r) refer_free(rc->r);
}

static dyl_used random_const_s* random_const_arg(json_inode_t *restrict arg, const char *restrict rgname, const char *restrict rsname)
{
	register random_const_s *restrict rc;
	rc = (random_const_s *) refer_alloc(sizeof(random_const_s));
	if (rc)
	{
		if ((rc->r = random_arg(json_object_find(arg, "type"), rgname, rsname)))
		{
			refer_set_free(rc, (refer_free_f) random_const_free_func);
			rc->mu = 0;
			rc->sigma = 1;
			get_float(&rc->mu, arg, ".u");
			get_float(&rc->sigma, arg, ".c");
			return rc;
		}
		refer_free(rc);
	}
	return NULL;
}
dyl_export(random_const_arg, vya.common.random_const_arg);
