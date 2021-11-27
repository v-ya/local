#include "server.h"
#include <web/web-method.h>
#include <fsys.h>

const pocket_attr_t* get_u32_1(pocket_s *restrict pocket, const pocket_attr_t *restrict item, uint32_t *restrict value);
const char* check_special_and_escape(const char *restrict name);

typedef struct web_server_parse_register_t {
	pocket_s *pocket;
	web_server_s *server;
	web_method_s *method;
	const hashmap_t *tags_local;
	const hashmap_t *tags_global;
	fsys_rpath_s *rpath;
	uintptr_t allow_method;
} web_server_parse_register_t;

static uintptr_t method_array_compare(const web_method_s *restrict method, const char *restrict m)
{
	const char *restrict a, *restrict b;
	uint32_t i;
	if (!*m)
		goto label_null;
	for (i = 0; i < web_method_id_custom; ++i)
	{
		a = m;
		if ((b = web_method_get(method, (web_method_id_t) i)->string))
		{
			while (*a == *b)
			{
				if (!*a)
					goto label_okay;
				++a;
				++b;
			}
		}
	}
	return ~(uintptr_t) 0;
	label_null:
	return 0;
	label_okay:
	return (uintptr_t) 1 << i;
}

static int response_code_get(const char *restrict code)
{
	uintptr_t i;
	int rcode;
	rcode = 0;
	if (*code > '0')
	{
		for (i = 0; i < 3; ++i)
		{
			if (code[i] && code[i] >= '0' && code[i] <= '9')
				rcode = rcode * 10 + (code[i] - '0');
		}
		if (!code[i])
			return rcode;
	}
	return 0;
}

static const web_server_route_s* get_route_save(const web_server_parse_register_t *restrict p, const pocket_attr_t *restrict item)
{
	inst_web_server_tag_s *tag;
	const web_server_route_s *route;
	refer_t pri;
	tag = NULL;
	route = NULL;
	if (p->tags_local) tag = (inst_web_server_tag_s *) hashmap_get_name(p->tags_local, item->tag.string);
	if (!tag) tag = (inst_web_server_tag_s *) hashmap_get_name(p->tags_global, item->tag.string);
	if (tag)
	{
		if (tag->route)
			route = (const web_server_route_s *) refer_save(tag->route);
		else
		{
			pri = NULL;
			if (tag->allocer && !(pri = tag->allocer(tag, p->pocket, item)))
				goto label_fail;
			route = web_server_route_alloc(tag->request_func, pri, tag->request_flag);
			if (pri) refer_free(pri);
		}
	}
	label_fail:
	return route;
}

static const pocket_attr_t* web_server_parse_register_response(const web_server_parse_register_t *restrict p, const pocket_attr_t *restrict item)
{
	const web_server_route_s *route;
	uint64_t n;
	int rcode;
	n = item->size;
	item = (const pocket_attr_t *) item->data.ptr;
	while (n)
	{
		--n;
		if (!item->name.string)
			goto label_fail;
		if (pocket_preset_tag(p->pocket, item) != pocket_tag$custom)
			goto label_fail;
		if (!(rcode = response_code_get(item->name.string)))
			goto label_fail;
		if ((route = get_route_save(p, item)))
		{
			rcode = !web_server_register_response_by_route(p->server, rcode, route);
			refer_free(route);
			if (rcode) goto label_fail;
		}
		++item;
	}
	return NULL;
	label_fail:
	return item;
}

static const pocket_attr_t* web_server_parse_register_request_item(const web_server_parse_register_t *restrict p, const pocket_attr_t *restrict item)
{
	const char *restrict uri;
	const pocket_attr_t *ret;
	const web_server_route_s *route;
	uint32_t i;
	ret = NULL;
	if (p->allow_method && pocket_preset_tag(p->pocket, item) == pocket_tag$custom &&
		(uri = fsys_rpath_get_full_path(p->rpath, check_special_and_escape(item->name.string))))
	{
		if ((route = get_route_save(p, item)))
		{
			for (i = 0; i < web_method_id_custom; ++i)
			{
				if (p->allow_method & ((uintptr_t) 1 << i))
				{
					if (!web_server_register_request_by_route(
						p->server,
						web_method_get(p->method, (web_method_id_t) i)->string,
						uri,
						route))
						goto label_fail_free_route;
				}
			}
			ret = item;
			label_fail_free_route:
			refer_free(route);
		}
	}
	return ret;
}

static const web_server_parse_register_t* web_server_parse_register_request_method(web_server_parse_register_t *restrict p, const pocket_attr_t *restrict item)
{
	const char *restrict m;
	uintptr_t mbit;
	uint64_t n;
	n = item->size;
	item = (const pocket_attr_t *) item->data.ptr;
	while (n)
	{
		--n;
		if (!(m = item->name.string))
			goto label_fail;
		switch (*m++)
		{
			case '+':
				mbit = method_array_compare(p->method, m);
				if (!mbit || !~mbit)
					goto label_fail;
				p->allow_method |= mbit;
				break;
			case '-':
				mbit = method_array_compare(p->method, m);
				if (!mbit || !~mbit)
					goto label_fail;
				p->allow_method &= ~mbit;
				break;
			case '=':
				mbit = method_array_compare(p->method, m);
				if (!~mbit)
					goto label_fail;
				p->allow_method = mbit;
				break;
			default: goto label_fail;
		}
		++item;
	}
	return p;
	label_fail:
	return NULL;
}

static const pocket_attr_t* web_server_parse_register_request(web_server_parse_register_t *restrict p, const pocket_attr_t *restrict item)
{
	uint64_t n;
	n = item->size;
	item = (const pocket_attr_t *) item->data.ptr;
	while (n)
	{
		--n;
		if (!item->name.string)
			goto label_fail;
		switch (pocket_preset_tag(p->pocket, item))
		{
			case pocket_tag$index:
				if (check_special_and_escape(item->name.string))
				{
					// into
					const pocket_attr_t *restrict e;
					uintptr_t allow_method;
					allow_method = p->allow_method;
					if (!fsys_rpath_push(p->rpath, check_special_and_escape(item->name.string)))
						goto label_fail;
					e = web_server_parse_register_request(p, item);
					fsys_rpath_pop(p->rpath);
					p->allow_method = allow_method;
					if (e)
					{
						item = e;
						goto label_fail;
					}
				}
				else if (item->name.string[1])
				{
					// modify method
					if (!web_server_parse_register_request_method(p, item))
						goto label_fail;
				}
				break;
			case pocket_tag$custom:
				if (!check_special_and_escape(item->name.string))
					goto label_fail;
				if (!web_server_parse_register_request_item(p, item))
					goto label_fail;
				break;
			default:
				if (check_special_and_escape(item->name.string))
					goto label_fail;
				break;
		}
		++item;
	}
	return NULL;
	label_fail:
	return item;
}

static fsys_rpath_s* web_server_parse_register_alloc_rpath(pocket_s *restrict pocket, const pocket_attr_t *restrict root)
{
	const pocket_attr_t *restrict v;
	uint32_t stack;
	stack = 64;
	if (!(v = pocket_find(pocket, root, "#stack")) || get_u32_1(pocket, v, &stack))
		return fsys_rpath_alloc("/", (uintptr_t) stack);
	return NULL;
}

const pocket_attr_t* web_server_parse_register(pocket_s *restrict pocket, const pocket_attr_t *restrict root, const inst_web_server_s *restrict inst, const hashmap_t *restrict tags_local)
{
	web_server_parse_register_t p;
	const pocket_attr_t *c;
	p.pocket = pocket;
	p.server = inst->server;
	p.method = inst->method;
	p.tags_local = tags_local;
	p.tags_global = &inst->tag;
	p.rpath = web_server_parse_register_alloc_rpath(pocket, root);
	p.allow_method = 0;
	c = root;
	if (p.rpath)
	{
		if ((c = pocket_find(pocket, root, "#")))
		{
			if (pocket_preset_tag(pocket, c) != pocket_tag$index)
				goto label_fail;
			if ((c = web_server_parse_register_response(&p, c)))
				goto label_fail;
		}
		c = web_server_parse_register_request(&p, root);
		label_fail:
		refer_free(p.rpath);
	}
	return c;
}
