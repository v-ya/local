#include "gvcx.model.h"

void gvcx_model_type_free_func(gvcx_model_type_s *restrict r)
{
	if (r->tname) refer_free(r->tname);
}

const gvcx_model_type_s* gvcx_model_type_alloc(const char *restrict tname, uintptr_t tsize, gvcx_model_type_initial_f initial, const void *restrict pri)
{
	gvcx_model_type_s *restrict r;
	if (tsize >= sizeof(gvcx_model_type_s) && tname && (r = (gvcx_model_type_s *) refer_alloz(tsize)))
	{
		refer_set_free(r, (refer_free_f) gvcx_model_type_free_func);
		if ((r->tname = refer_dump_string(tname)) && (!initial || initial(r, pri)))
			return r;
		refer_free(r);
	}
	return NULL;
}

gvcx_model_type_t gvcx_model_type_enum(const char *restrict tname)
{
	if (tname)
	{
		switch (*tname)
		{
			case '*': return gvcx_model_type__any;
			case 'u': return gvcx_model_type__u;
			case 'i': return gvcx_model_type__i;
			case 'f': return gvcx_model_type__f;
			case 'b': return gvcx_model_type__b;
			case 's': return gvcx_model_type__s;
			case 'd': return gvcx_model_type__d;
			case 'e': return gvcx_model_type__e;
			case 'a': return gvcx_model_type__a;
			case 'o': return gvcx_model_type__o;
			case '.': return gvcx_model_type__preset;
			default: break;
		}
	}
	return gvcx_model_type__any;
}

const char* gvcx_model_type_inherit(const char *restrict parent, const char *restrict child)
{
	uintptr_t i;
	if (parent && child)
	{
		for (i = 0; parent[i] && child[i]; ++i)
		{
			if (parent[i] != child[i])
				goto label_fail;
		}
		if (parent[i] == 0 && (child[i] == 0 || child[i] == ':'))
			return parent;
	}
	label_fail:
	return NULL;
}
