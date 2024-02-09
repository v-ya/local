#include "layer.model.h"

void layer_model_type_free_func(layer_model_type_s *restrict r)
{
	if (r->name) refer_free(r->name);
}

const layer_model_type_s* layer_model_type_alloc(const char *restrict name, uint32_t type_major, uint32_t type_minor, uintptr_t tsize, layer_model_type_initial_f initial, const void *restrict pri)
{
	layer_model_type_s *restrict r;
	if (tsize >= sizeof(layer_model_type_s) && name &&
		layer_model_type_name_enum(name) == type_major &&
		(r = (layer_model_type_s *) refer_alloz(tsize)))
	{
		refer_set_free(r, (refer_free_f) layer_model_type_free_func);
		r->type_major = type_major;
		r->type_minor = type_minor;
		if ((r->name = refer_dump_string(name)) && (!initial || initial(r, pri)))
			return r;
		refer_free(r);
	}
	return NULL;
}

layer_model_type_t layer_model_type_name_enum(const char *restrict name)
{
	if (name && name[0] && (name[1] == 0 || name[1] == ':'))
	{
		switch (*name)
		{
			case '*': return layer_model_type__any;
			case 'u': return layer_model_type__uint;
			case 'i': return layer_model_type__int;
			case 'f': return layer_model_type__float;
			case 'b': return layer_model_type__boolean;
			case 's': return layer_model_type__string;
			case 'd': return layer_model_type__data;
			case 'e': return layer_model_type__enum;
			case 'a': return layer_model_type__array;
			case 'o': return layer_model_type__object;
			case '.': return layer_model_type__preset;
			default: break;
		}
	}
	return layer_model_type__unknow;
}

const char* layer_model_type_name_inherit(const char *restrict parent, const char *restrict child)
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

const char* layer_model_type_inherit_name(refer_string_t cname, const layer_model_any_s *restrict cany, const char *restrict name)
{
	if (!cname)
		return name;
	else if (cany)
		return layer_model_any_inherit(cany, name);
	else return layer_model_type_name_inherit(cname, name);
}

const layer_model_type_s* layer_model_type_inherit_type(refer_string_t cname, const layer_model_any_s *restrict cany, const layer_model_type_s *restrict type)
{
	if (!cname || (type && (cname == type->name || (cany && layer_model_any_inherit(cany, type->name)))))
		return type;
	return NULL;
}
