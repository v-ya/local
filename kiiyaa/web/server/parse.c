#include <pocket/pocket.h>
#include <hashmap.h>

static const char parse_name[] = "web.server.parse";

const pocket_attr_t* web_server_parse_bind(pocket_s *restrict pocket, const pocket_attr_t *restrict root, const char *restrict name);

const pocket_attr_t* get_u32_1(pocket_s *restrict pocket, const pocket_attr_t *restrict item, uint32_t *restrict value)
{
	if ((pocket_preset_tag(pocket, item) == pocket_tag$u32) && item->data.ptr && item->size == sizeof(uint32_t))
	{
		if (value) *value = *(uint32_t *) item->data.ptr;
		return item;
	}
	return NULL;
}

pocket_s* web_server_parse(pocket_s *restrict pocket, const pocket_attr_t *restrict root, const char *restrict name, const char *restrict *restrict error)
{
	if ((root = pocket_find_tag(pocket, root, parse_name, pocket_tag$index, NULL)))
	{
		const pocket_attr_t *restrict item;
		if ((item = pocket_find_tag(pocket, root, "bind", pocket_tag$index, NULL)))
		{
			if ((item = web_server_parse_bind(pocket, item, name)))
				goto label_fail;
		}
		return pocket;
		label_fail:
		*error = item->name.string;
	}
	return NULL;
}

hashmap_vlist_t* initial_parse(void)
{
	extern hashmap_t *$parse;
	hashmap_vlist_t *restrict vl;
	void *value;
	if ((vl = hashmap_put_name($parse, parse_name, value = web_server_parse, NULL)) &&
		vl->value == value)
		return vl;
	return NULL;
}

void finally_parse(void)
{
	extern hashmap_t *$parse;
	hashmap_delete_name($parse, parse_name);
}
