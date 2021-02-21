#include <pocket/pocket.h>
#include <hashmap.h>
#include <dylink.h>

pocket_s* kiya$parse$export(pocket_s *restrict pocket, const pocket_attr_t *restrict root, const char *restrict name, const char *restrict *restrict error)
{
	extern hashmap_t *$dylink;
	dylink_pool_t *restrict pool;
	if ((pool = hashmap_get_name($dylink, name)) && (root = pocket_find_tag(pocket, root, "kiya.export", pocket_tag$index, NULL)))
	{
		const char *restrict upper_symbol;
		uint64_t n;
		n = root->size;
		root = (const pocket_attr_t *) root->data.ptr;
		while (n)
		{
			--n;
			if (!root->name.string)
				goto label_fail;
			switch (pocket_preset_tag(pocket, root))
			{
				case pocket_tag$null:
					upper_symbol = root->name.string;
					break;
				case pocket_tag$string:
					if (!(upper_symbol = (const char *) root->data.ptr))
						goto label_fail;
					break;
				default:
					goto label_fail;
			}
			if (dylink_pool_sync_symbol(pool, root->name.string, upper_symbol))
				goto label_fail;
			++root;
		}
		return pocket;
	}
	label_fail:
	*error = root->name.string;
	return NULL;
}
