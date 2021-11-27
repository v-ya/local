#include "server.h"

const pocket_attr_t* get_u32_1(pocket_s *restrict pocket, const pocket_attr_t *restrict item, uint32_t *restrict value);

static const pocket_attr_t* web_server_parse_bind_item(pocket_s *restrict pocket, const pocket_attr_t *restrict item, web_server_s *server)
{
	const pocket_attr_t *restrict v;
	const char *restrict ip;
	uint32_t port, nlisten;
	pocket_tag_t tag;
	port = 0;
	nlisten = 4096;
	tag = pocket_preset_tag(pocket, item);
	if (tag == pocket_tag$string)
	{
		if ((ip = (const char *) item->data.ptr))
			goto label_add_bind;
	}
	else if (tag == pocket_tag$index)
	{
		if ((v = pocket_find_tag(pocket, item, "address", pocket_tag$string, NULL)) &&
			(ip = (const char *) v->data.ptr))
		{
			if ((v = pocket_find(pocket, item, "port")) && !get_u32_1(pocket, v, &port))
				goto label_fail;
			if ((v = pocket_find(pocket, item, "nlisten")) && !get_u32_1(pocket, v, &nlisten))
				goto label_fail;
			label_add_bind:
			if (web_server_add_bind(server, ip, port, nlisten))
				return item;
		}
	}
	label_fail:
	return NULL;
}

const pocket_attr_t* web_server_parse_bind(pocket_s *restrict pocket, const pocket_attr_t *restrict root, web_server_s *server)
{
	uint64_t n;
	n = root->size;
	root = (const pocket_attr_t *) root->data.ptr;
	while (n)
	{
		--n;
		if (!web_server_parse_bind_item(pocket, root, server))
			return root;
		++root;
	}
	return NULL;
}
