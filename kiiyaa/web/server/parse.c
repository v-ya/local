#include "server.h"
#include <dylink.h>

static const char parse_name[] = "web.server.parse";

const pocket_attr_t* get_u32_1(pocket_s *restrict pocket, const pocket_attr_t *restrict item, uint32_t *restrict value)
{
	if ((pocket_preset_tag(pocket, item) == pocket_tag$u32) && item->data.ptr && item->size == sizeof(uint32_t))
	{
		if (value) *value = *(uint32_t *) item->data.ptr;
		return item;
	}
	return NULL;
}

const char* check_special_and_escape(const char *restrict name)
{
	if (name[0] != '#') return name;
	else if (name[1] == '#') return name + 1;
	return NULL;
}

const pocket_attr_t* web_server_parse_header(pocket_s *restrict pocket, const pocket_attr_t *restrict root, refer_t pretreat, const web_server_s *server, const dylink_pool_t *restrict pool);
const pocket_attr_t* web_server_parse_trigger(pocket_s *restrict pocket, const pocket_attr_t *restrict root, refer_t pretreat, const web_server_s *server, const dylink_pool_t *restrict pool);
const pocket_attr_t* web_server_parse_tag(pocket_s *restrict pocket, const pocket_attr_t *restrict root, hashmap_t *restrict tags, const dylink_pool_t *restrict pool, const inst_web_server_s *restrict inst);
const pocket_attr_t* web_server_parse_register(pocket_s *restrict pocket, const pocket_attr_t *restrict root, const inst_web_server_s *restrict inst, const hashmap_t *restrict tags_local);
const pocket_attr_t* web_server_parse_bind(pocket_s *restrict pocket, const pocket_attr_t *restrict root, web_server_s *server);

pocket_s* web_server_parse(pocket_s *restrict pocket, const pocket_attr_t *restrict root, const char *restrict name, const char *restrict *restrict error)
{
	extern hashmap_t *$dylink;
	extern inst_web_server_s *inst;
	if ((root = pocket_find_tag(pocket, root, parse_name, pocket_tag$index, NULL)))
	{
		inst_web_server_s *restrict r;
		dylink_pool_t *restrict pool;
		const pocket_attr_t *restrict item;
		hashmap_t *restrict local_tag;
		hashmap_t local_tag_cache;
		r = inst;
		pool = hashmap_get_name($dylink, name);
		local_tag = NULL;
		// header
		if ((item = pocket_find(pocket, root, "header")))
		{
			if (pocket_preset_tag(pocket, item) != pocket_tag$index)
				goto label_fail;
			if ((item = web_server_parse_header(pocket, item, r->pretreat, r->server, pool)))
				goto label_fail;
		}
		// trigger
		if ((item = pocket_find(pocket, root, "trigger")))
		{
			if (pocket_preset_tag(pocket, item) != pocket_tag$index)
				goto label_fail;
			if ((item = web_server_parse_trigger(pocket, item, r->pretreat, r->server, pool)))
				goto label_fail;
		}
		// tag.global
		if ((item = pocket_find(pocket, root, "tag.global")))
		{
			if (pocket_preset_tag(pocket, item) != pocket_tag$index)
				goto label_fail;
			if (!pool)
				goto label_fail;
			if ((item = web_server_parse_tag(pocket, item, &r->tag, pool, r)))
				goto label_fail;
		}
		// tag.local
		if ((item = pocket_find(pocket, root, "tag.local")))
		{
			if (pocket_preset_tag(pocket, item) != pocket_tag$index)
				goto label_fail;
			if (!pool)
				goto label_fail;
			if (!hashmap_init(&local_tag_cache))
				goto label_fail;
			local_tag = &local_tag_cache;
			if ((item = web_server_parse_tag(pocket, item, local_tag, pool, r)))
				goto label_fail;
		}
		// register
		if ((item = pocket_find(pocket, root, "register")))
		{
			if (pocket_preset_tag(pocket, item) != pocket_tag$index)
				goto label_fail;
			if ((item = web_server_parse_register(pocket, item, r, local_tag)))
				goto label_fail;
		}
		// bind
		if ((item = pocket_find(pocket, root, "bind")))
		{
			if (pocket_preset_tag(pocket, item) != pocket_tag$index)
				goto label_fail;
			if ((item = web_server_parse_bind(pocket, item, r->server)))
				goto label_fail;
		}
		label_return:
		if (local_tag)
			hashmap_uini(local_tag);
		return pocket;
		label_fail:
		*error = item->name.string;
		pocket = NULL;
		goto label_return;
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
