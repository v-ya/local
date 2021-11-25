#include "console_command.h"
#include <pocket/pocket.h>
#include <hashmap.h>
#include <vattr.h>
#include <dylink.h>

static const char parse_name[] = "console.main.parse.command";

static console_command_s* console_parse_command_add(const char *restrict name, console_command_s* (*allocer)(void))
{
	extern vattr_s *ccpool;
	console_command_s *r, *cc;
	r = NULL;
	if (!vattr_get_vslot(ccpool, name))
	{
		if ((cc = allocer()))
		{
			if (vattr_set(ccpool, name, cc))
				r = cc;
			refer_free(cc);
		}
	}
	return r;
}

static pocket_s* console_parse_command(pocket_s *restrict pocket, const pocket_attr_t *restrict root, const char *restrict name, const char *restrict *restrict error)
{
	extern hashmap_t *$dylink;
	dylink_pool_t *restrict pool;
	console_command_s* (*allocer)(void);
	uint64_t n;
	pocket_tag_t tag;
	if ((pool = hashmap_get_name($dylink, name)) && (root = pocket_find(pocket, root, parse_name)))
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
				if (tag != pocket_tag$string)
					goto label_fail;
				if (!root->data.ptr)
					goto label_fail;
				if (!(allocer = dylink_pool_get_symbol(pool, (const char *) root->data.ptr, NULL)))
					goto label_fail;
				if (!console_parse_command_add(root->name.string, allocer))
					goto label_fail;
				++root;
			}
			return pocket;
		}
	}
	label_fail:
	*error = root->name.string;
	return NULL;
}

hashmap_vlist_t* console_parse_initial(void)
{
	extern hashmap_t *$parse;
	hashmap_vlist_t *restrict vl;
	void *value;
	if ((vl = hashmap_put_name($parse, parse_name, value = console_parse_command, NULL)) &&
		vl->value == value)
		return vl;
	return NULL;
}

void console_parse_finally(void)
{
	extern hashmap_t *$parse;
	hashmap_delete_name($parse, parse_name);
}
