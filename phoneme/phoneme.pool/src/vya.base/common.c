#include <phoneme/phoneme.h>

static dyl_used void json_set_float(double *r, json_inode_t *arg, const char *restrict jpath)
{
	arg = json_find(arg, jpath);
	if (arg)
	{
		switch (arg->type)
		{
			case json_inode_integer:
				*r = arg->value.integer;
				break;
			case json_inode_floating:
				*r = arg->value.floating;
				break;
		}
	}
}
dyl_export(json_set_float, vya.common.json_set_float);

