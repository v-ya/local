#include "server.h"
#include <dylink.h>

static void hashmap_free_refer_func(hashmap_vlist_t *restrict vl)
{
	if (vl->value)
		refer_free(vl->value);
}

static const pocket_attr_t* web_server_parse_tag_item(pocket_s *restrict pocket, const pocket_attr_t *restrict item, hashmap_t *restrict tags, const dylink_pool_t *restrict pool, const hashmap_t *restrict flags)
{
	inst_web_server_tag_s *restrict t;
	const pocket_attr_t *restrict v;
	inst_web_server_tag_builder_f builder;
	inst_web_server_tag_allocer_f allocer;
	web_server_request_f request;
	web_server_request_flag_t request_flags;
	pocket_tag_t tag;
	if (!item->name.string)
		goto label_fail;
	if (hashmap_find_name(tags, item->name.string))
		goto label_fail;
	tag = pocket_preset_tag(pocket, item);
	if (tag == pocket_tag$string)
	{
		// builder
		if (item->data.ptr && ((builder = dylink_pool_get_symbol(pool, (const char *) item->data.ptr, NULL))))
		{
			if ((t = builder(pocket, item)))
				goto label_check_and_set;
		}
	}
	else if (tag == pocket_tag$index)
	{
		// compose
		builder = NULL;
		allocer = NULL;
		request = NULL;
		request_flags = 0;
		// builder
		if ((v = pocket_find(pocket, item, "builder")))
		{
			if (pocket_preset_tag(pocket, v) != pocket_tag$string)
				goto label_fail;
			if (!v->data.ptr)
				goto label_fail;
			if (!(builder = dylink_pool_get_symbol(pool, (const char *) v->data.ptr, NULL)))
				goto label_fail;
		}
		// request
		if ((v = pocket_find(pocket, item, "request")))
		{
			if (pocket_preset_tag(pocket, v) != pocket_tag$string)
				goto label_fail;
			if (!v->data.ptr)
				goto label_fail;
			if (!(request = dylink_pool_get_symbol(pool, (const char *) v->data.ptr, NULL)))
				goto label_fail;
		}
		// allocer
		if ((v = pocket_find(pocket, item, "allocer")))
		{
			if (pocket_preset_tag(pocket, v) != pocket_tag$string)
				goto label_fail;
			if (!v->data.ptr)
				goto label_fail;
			if (!(allocer = dylink_pool_get_symbol(pool, (const char *) v->data.ptr, NULL)))
				goto label_fail;
		}
		// flags
		if ((v = pocket_find(pocket, item, "flags")))
		{
			inst_web_server_flags_modify_f modify;
			uint64_t n;
			if (pocket_preset_tag(pocket, v) != pocket_tag$index)
				goto label_fail;
			n = v->size;
			v = (const pocket_attr_t *) v->data.ptr;
			while (n)
			{
				--n;
				if (!v->name.string)
					goto label_fail;
				if (pocket_preset_tag(pocket, v) != pocket_tag$null)
					goto label_fail;
				if (!(modify = hashmap_get_name(flags, v->name.string)))
					goto label_fail;
				request_flags = modify(request_flags);
				++v;
			}
		}
		// build
		if (builder)
			t = builder(pocket, item);
		else t = (inst_web_server_tag_s *) refer_alloz(sizeof(inst_web_server_tag_s));
		if (t)
		{
			if (request)
				t->request_func = request;
			if (allocer)
				t->allocer = allocer;
			t->request_flag = request_flags;
			label_check_and_set:
			if (t->request_func && hashmap_set_name(tags, item->name.string, t, hashmap_free_refer_func))
				return item;
			refer_free(t);
		}
	}
	label_fail:
	return NULL;
}

const pocket_attr_t* web_server_parse_tag(pocket_s *restrict pocket, const pocket_attr_t *restrict root, hashmap_t *restrict tags, const dylink_pool_t *restrict pool, const hashmap_t *restrict flags)
{
	uint64_t n;
	n = root->size;
	root = (const pocket_attr_t *) root->data.ptr;
	while (n)
	{
		--n;
		if (!web_server_parse_tag_item(pocket, root, tags, pool, flags))
			return root;
		++root;
	}
	return NULL;
}
