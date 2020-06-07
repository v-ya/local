#include <phoneme/phoneme.h>

static dyl_used void get_float(double *r, json_inode_t *arg, const char *restrict jpath)
{
	if (arg)
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
}
dyl_export(get_float, vya.common.get_float);

