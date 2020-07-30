#include <phoneme/phoneme.h>
#include "random_generate.inc"
#include "rgf_find.inc"

typedef struct random_s {
	random_src_s *rsrc;
	random_generate_f rg_func;
	refer_t rg_data;
} random_s;

static dyl_used double random(register random_s *restrict r, double mu, double sigma)
{
	return r->rg_func(r->rsrc, mu, sigma, r->rg_data);
}
dyl_export(random, vya.common.random);

static void random_free_func(register random_s *restrict r)
{
	if (r->rsrc) refer_free(r->rsrc);
	if (r->rg_data) refer_free(r->rg_data);
}

static dyl_used random_s* random_arg(register json_inode_t *restrict arg, const char *restrict rgname, const char *restrict rsname)
{
	register random_s *restrict r;
	register json_inode_t *v;
	phoneme_arg2pri_f arg2pri;
	r = (random_s *) refer_alloz(sizeof(random_s));
	if (r)
	{
		refer_set_free(r, (refer_free_f) random_free_func);
		v = json_object_find(arg, "@");
		if (v && v->type == json_inode_string)
			rgname = v->value.string;
		if (!rgname) goto label_miss;
		if (!(r->rsrc = rsrc_alloc(json_object_find(arg, "@rsrc"), rsname)))
			goto label_miss;
		if (!(r->rg_func = rgf_find(rgname, &arg2pri)))
			goto label_miss;
		if (arg2pri && !(r->rg_data = arg2pri(arg)))
			goto label_miss;
		return r;
		label_miss:
		refer_free(r);
	}
	return NULL;
}
dyl_export(random_arg, vya.common.random_arg);
