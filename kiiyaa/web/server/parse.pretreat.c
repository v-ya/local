#include "server.h"
#include <dylink.h>

const char* check_special_and_escape(const char *restrict name);;

refer_t pretreat_insert_header_by_const(refer_t pretreat, const char *restrict name, const char *restrict value);
refer_t pretreat_insert_header_by_value(refer_t pretreat, const char *restrict name, inst_web_server_value_s *restrict value);
refer_t pretreat_insert_trigger(refer_t pretreat, const char *restrict name, inst_web_server_value_s *restrict value);
void pretreat_delete_header(refer_t pretreat, const char *restrict name);
void pretreat_delete_trigger(refer_t pretreat, const char *restrict name);
void pretreat_clear_header(refer_t pretreat);
void pretreat_clear_trigger(refer_t pretreat);

static refer_t web_server_parse_header_item_insert_string(const pocket_attr_t *restrict item, const char *restrict name, refer_t pretreat, const web_server_s *server, const dylink_pool_t *restrict pool)
{
	// by value
	inst_web_server_value_allocer_f allocer;
	inst_web_server_value_s *restrict value;
	if (item->data.ptr && (allocer = dylink_pool_get_symbol(pool, (const char *) item->data.ptr, NULL)) &&
		(value = allocer(server)))
	{
		pretreat = pretreat_insert_header_by_value(pretreat, name, value);
		refer_free(value);
		return pretreat;
	}
	return NULL;
}

static refer_t web_server_parse_header_item_insert_text(const pocket_attr_t *restrict item, const char *restrict name, refer_t pretreat)
{
	// by const
	if (item->data.ptr && item->size && !((const char *) item->data.ptr)[item->size - 1])
		return pretreat_insert_header_by_const(pretreat, name, (const char *) item->data.ptr);
	return NULL;
}

static refer_t web_server_parse_trigger_item_insert_string(const pocket_attr_t *restrict item, const char *restrict name, refer_t pretreat, const web_server_s *server, const dylink_pool_t *restrict pool)
{
	// by value
	inst_web_server_value_allocer_f allocer;
	inst_web_server_value_s *restrict value;
	if (item->data.ptr && (allocer = dylink_pool_get_symbol(pool, (const char *) item->data.ptr, NULL)) &&
		(value = allocer(server)))
	{
		pretreat = pretreat_insert_trigger(pretreat, name, value);
		refer_free(value);
		return pretreat;
	}
	return NULL;
}

static const pocket_attr_t* web_server_parse_header_item(pocket_s *restrict pocket, const pocket_attr_t *restrict item, refer_t pretreat, const web_server_s *server, const dylink_pool_t *restrict pool)
{
	const char *restrict name;
	uint64_t n;
	if (!item->name.string)
		goto label_fail;
	if ((name = check_special_and_escape(item->name.string)))
	{
		switch (pocket_preset_tag(pocket, item))
		{
			case pocket_tag$index:
				n = item->size;
				item = (const pocket_attr_t *) item->data.ptr;
				while (n)
				{
					--n;
					if (!item->name.string)
						goto label_fail;
					switch (pocket_preset_tag(pocket, item))
					{
						case pocket_tag$string:
							if (!web_server_parse_header_item_insert_string(item, name, pretreat, server, pool))
								goto label_fail;
							break;
						case pocket_tag$text:
							if (!web_server_parse_header_item_insert_text(item, name, pretreat))
								goto label_fail;
							break;
						default: goto label_fail;
					}
					++item;
				}
				break;
			case pocket_tag$string:
				if (!web_server_parse_header_item_insert_string(item, name, pretreat, server, pool))
					goto label_fail;
				break;
			case pocket_tag$text:
				if (!web_server_parse_header_item_insert_text(item, name, pretreat))
					goto label_fail;
				break;
			default:
				label_fail:
				return item;
		}
	}
	return NULL;
}

static const pocket_attr_t* web_server_parse_trigger_item(pocket_s *restrict pocket, const pocket_attr_t *restrict item, refer_t pretreat, const web_server_s *server, const dylink_pool_t *restrict pool)
{
	const char *restrict name;
	uint64_t n;
	if (!item->name.string)
		goto label_fail;
	if ((name = check_special_and_escape(item->name.string)))
	{
		switch (pocket_preset_tag(pocket, item))
		{
			case pocket_tag$index:
				n = item->size;
				item = (const pocket_attr_t *) item->data.ptr;
				while (n)
				{
					--n;
					if (!item->name.string)
						goto label_fail;
					switch (pocket_preset_tag(pocket, item))
					{
						case pocket_tag$string:
							if (!web_server_parse_trigger_item_insert_string(item, name, pretreat, server, pool))
								goto label_fail;
							break;
						default: goto label_fail;
					}
					++item;
				}
				break;
			case pocket_tag$string:
				if (!web_server_parse_trigger_item_insert_string(item, name, pretreat, server, pool))
					goto label_fail;
				break;
			default:
				label_fail:
				return item;
		}
	}
	return NULL;
}

static const pocket_attr_t* web_server_parse_pretreat_clear(pocket_s *restrict pocket, const pocket_attr_t *restrict item, refer_t pretreat, void (*delete)(refer_t pretreat, const char *restrict name), void (*clear)(refer_t pretreat))
{
	uint64_t n;
	if ((item = pocket_find(pocket, item, "#clear")))
	{
		switch (pocket_preset_tag(pocket, item))
		{
			case pocket_tag$null:
				clear(pretreat);
				break;
			case pocket_tag$index:
				n = item->size;
				item = (const pocket_attr_t *) item->data.ptr;
				while (n)
				{
					--n;
					if (!item->name.string)
						goto label_fail;
					if (pocket_preset_tag(pocket, item) != pocket_tag$null)
						goto label_fail;
					delete(pretreat, item->name.string);
					++item;
				}
				break;
			default:
				label_fail:
				return item;
		}
	}
	return NULL;
}

const pocket_attr_t* web_server_parse_header(pocket_s *restrict pocket, const pocket_attr_t *restrict root, refer_t pretreat, const web_server_s *server, const dylink_pool_t *restrict pool)
{
	const pocket_attr_t *restrict error;
	uint64_t n;
	if ((error = web_server_parse_pretreat_clear(pocket, root, pretreat, pretreat_delete_header, pretreat_clear_header)))
		goto label_fail;
	n = root->size;
	root = (const pocket_attr_t *) root->data.ptr;
	while (n)
	{
		--n;
		if ((error = web_server_parse_header_item(pocket, root, pretreat, server, pool)))
			goto label_fail;
		++root;
	}
	return NULL;
	label_fail:
	return error;
}

const pocket_attr_t* web_server_parse_trigger(pocket_s *restrict pocket, const pocket_attr_t *restrict root, refer_t pretreat, const web_server_s *server, const dylink_pool_t *restrict pool)
{
	const pocket_attr_t *restrict error;
	uint64_t n;
	if ((error = web_server_parse_pretreat_clear(pocket, root, pretreat, pretreat_delete_trigger, pretreat_clear_trigger)))
		goto label_fail;
	n = root->size;
	root = (const pocket_attr_t *) root->data.ptr;
	while (n)
	{
		--n;
		if ((error = web_server_parse_trigger_item(pocket, root, pretreat, server, pool)))
			goto label_fail;
		++root;
	}
	return NULL;
	label_fail:
	return error;
}
