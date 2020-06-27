#include <phoneme/phoneme.h>

static dyl_used void get_string(const char *restrict *restrict r, json_inode_t *restrict arg, const char *restrict jpath)
{
	if (arg)
	{
		arg = json_find(arg, jpath);
		if (arg)
		{
			switch (arg->type)
			{
				case json_inode_string:
					*r = arg->value.string;
					break;
				case json_inode_null:
					*r = NULL;
					break;
			}
		}
	}
}
dyl_export(get_string, vya.common.get_string);

