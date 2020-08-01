#include <phoneme/phoneme.h>
#include "random.inc"
#include "polyline.inc"

typedef struct random_polyline_s {
	refer_t r;
	refer_t mu;
	refer_t sigma;
} random_polyline_s;

static dyl_used double random_polyline(register random_polyline_s *restrict rp, register double t)
{
	return random(rp->r, polyline(t, rp->mu), polyline(t, rp->sigma));
}
dyl_export(random_polyline, vya.common.random_polyline);

static void random_polyline_free_func(register random_polyline_s *restrict rp)
{
	if (rp->r) refer_free(rp->r);
	if (rp->mu) refer_free(rp->mu);
	if (rp->sigma) refer_free(rp->sigma);
}

static dyl_used random_polyline_s* random_polyline_arg(json_inode_t *restrict arg, const char *restrict rgname, const char *restrict rsname)
{
	register random_polyline_s *restrict rc;
	if (arg && arg->type == json_inode_object)
	{
		rc = (random_polyline_s *) refer_alloz(sizeof(random_polyline_s));
		if (rc)
		{
			if ((rc->r = random_arg(json_object_find(arg, "type"), rgname, rsname)))
			{
				refer_set_free(rc, (refer_free_f) random_polyline_free_func);
				if ((rc->mu = polyline_arg(json_object_find(arg, "u"), NULL, NULL)) &&
					(rc->sigma = polyline_arg(json_object_find(arg, "c"), NULL, NULL)))
				{
					return rc;
				}
			}
			refer_free(rc);
		}
	}
	return NULL;
}
dyl_export(random_polyline_arg, vya.common.random_polyline_arg);
