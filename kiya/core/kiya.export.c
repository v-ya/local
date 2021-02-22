#include <pocket/pocket.h>
#include <hashmap.h>
#include <dylink.h>
#include <string.h>

pocket_s* kiya$parse$export(pocket_s *restrict pocket, const pocket_attr_t *restrict root, const char *restrict name, const char *restrict *restrict error)
{
	extern hashmap_t *$dylink;
	dylink_pool_t *restrict pool;
	const char *restrict upper_symbol;
	uint64_t n;
	pocket_tag_t tag;
	if ((pool = hashmap_get_name($dylink, name)) && (root = pocket_find(pocket, root, "kiya.export")))
	{
		tag = pocket_preset_tag(pocket, root);
		if (tag == pocket_tag$index)
		{
			n = root->size;
			root = (const pocket_attr_t *) root->data.ptr;
			while (n)
			{
				--n;
				if (!root->name.string)
					goto label_fail;
				tag = pocket_preset_tag(pocket, root);
				if (tag == pocket_tag$null) upper_symbol = root->name.string;
				else if (tag == pocket_tag$string)
				{
					if (!(upper_symbol = (const char *) root->data.ptr))
						goto label_fail;
				}
				else goto label_fail;
				if (dylink_pool_sync_symbol(pool, root->name.string, upper_symbol))
					goto label_fail;
				++root;
			}
			label_ok:
			return pocket;
		}
		else if (tag == pocket_tag$custom && !root->size &&
			!strcmp(root->tag.string, "kiya.export.all") &&
			!dylink_pool_sync_symbol_all(pool))
			goto label_ok;
	}
	label_fail:
	*error = root->name.string;
	return NULL;
}
