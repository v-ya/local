#include "random_generate.h"
#include <phoneme/phoneme.h>
#include <stdlib.h>
#include <string.h>
#include "../pthis.h"

static random_src_generate_func(rsg_default, random_src_s *restrict)
{
	static double k = (1.0 / (RAND_MAX + 1u));
	return (rand() + 0.5) * k;
}

static random_src_alloc_func(rsa_default, random_src_s*)
{
	register random_src_s *restrict r;
	r = (random_src_s *) refer_alloc(sizeof(random_src_s));
	if (r) r->rsrc_generate_func = rsg_default;
	return r;
}

static dyl_used random_src_s* rsrc_alloc(json_inode_t *restrict arg, register const char *restrict name)
{
	register random_src_alloc_f rsa_func;
	register json_inode_t *v;
	rsa_func = NULL;
	if (arg && arg->type == json_inode_object)
	{
		v = json_object_find(arg, "@");
		if (v && v->type == json_inode_string)
			name = v->value.string;
	}
	if (!name) rsa_func = rsa_default;
	else if (!strncmp(name, "vya.rs.", 7))
		rsa_func = (random_src_alloc_f) pthis_symbol(name);
	return rsa_func?rsa_func(arg):NULL;
}
dyl_export(rsrc_alloc, vya.common.rsrc_alloc);

