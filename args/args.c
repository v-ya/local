#include "args.h"

hashmap_vlist_t* args_set_command(hashmap_t *a, const char *command, args_deal_f deal_func)
{
	return hashmap_set_name(a, command, deal_func, NULL);
}

hashmap_vlist_t* args_set_index(hashmap_t *a, int index, args_deal_f deal_func)
{
	return hashmap_set_head(a, index, deal_func, NULL);
}

int args_deal(int argc, const char *argv[], hashmap_t *a, args_deal_f default_func, void *pri)
{
	register args_deal_f deal;
	const char *command, *value;
	int index, r;
	index = 0;
	while (index < argc)
	{
		command = NULL;
		value = argv[index];
		if (!(deal = hashmap_get_head(a, index)))
		{
			if (!index || !(deal = hashmap_get_name(a, value)))
			{
				if (!(deal = default_func))
				{
					++index;
					continue;
				}
			}
			else
			{
				command = value;
				if (index + 1 < argc) value = argv[index + 1];
				else value = NULL;
			}
		}
		if ((r = deal(pri, &index, command, value, argc, argv))) return r;
		++index;
		if (index < 0) index = 0;
	}
	return 0;
}
