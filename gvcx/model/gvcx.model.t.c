#include "gvcx.model.h"

void gvcx_model_type_free_func(gvcx_model_type_s *restrict r)
{
	if (r->name) refer_free(r->name);
}

const gvcx_model_type_s* gvcx_model_type_alloc(const char *restrict name, uint32_t type_major, uint32_t type_minor, uintptr_t tsize, gvcx_model_type_initial_f initial, const void *restrict pri)
{
	gvcx_model_type_s *restrict r;
	if (tsize >= sizeof(gvcx_model_type_s) && name &&
		type_major != gvcx_model_type__unknow &&
		type_major != gvcx_model_type__any &&
		gvcx_model_type_name_enum(name) == type_major &&
		(r = (gvcx_model_type_s *) refer_alloz(tsize)))
	{
		refer_set_free(r, (refer_free_f) gvcx_model_type_free_func);
		r->type_major = type_major;
		r->type_minor = type_minor;
		if ((r->name = refer_dump_string(name)) && (!initial || initial(r, pri)))
			return r;
		refer_free(r);
	}
	return NULL;
}

gvcx_model_type_t gvcx_model_type_name_enum(const char *restrict name)
{
	if (name && name[0] && (name[1] == 0 || name[1] == ':'))
	{
		switch (*name)
		{
			case '*': return gvcx_model_type__any;
			case 'u': return gvcx_model_type__uint;
			case 'i': return gvcx_model_type__int;
			case 'f': return gvcx_model_type__float;
			case 'b': return gvcx_model_type__boolean;
			case 's': return gvcx_model_type__string;
			case 'd': return gvcx_model_type__data;
			case 'e': return gvcx_model_type__enum;
			case 'a': return gvcx_model_type__array;
			case 'o': return gvcx_model_type__object;
			case '.': return gvcx_model_type__preset;
			default: break;
		}
	}
	return gvcx_model_type__unknow;
}

const char* gvcx_model_type_name_inherit(const char *restrict parent, const char *restrict child)
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

const char* gvcx_model_type_inherit(const vattr_s *restrict pool_any, const char *restrict parent, const char *restrict child)
{
	const gvcx_model_any_s *restrict any;
	gvcx_model_type_t type;
	if (!parent)
		return child;
	else if ((type = gvcx_model_type_name_enum(parent)) != gvcx_model_type__unknow)
	{
		if (type != gvcx_model_type__any)
			return gvcx_model_type_name_inherit(parent, child);
		else if (pool_any && (any = (const gvcx_model_any_s *) vattr_get_first(pool_any, parent)))
			return gvcx_model_any_inherit(any, child);
	}
	return NULL;
}
