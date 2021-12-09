#include "server.h"
#include <web/web-header.h>
#include <vattr.h>
#include <yaw.h>

typedef struct pretreat_header_s {
	refer_nstring_t header_name;
	uhttp_header_s *header;
	inst_web_server_value_s *value;
} pretreat_header_s;

typedef struct pretreat_s {
	yaw_lock_s *read;
	yaw_lock_s *write;
	vattr_s *trigger;  // trigger-name => (inst_web_server_value_s *)
	vattr_s *header;   // header-name  => (pretreat_header_s *)
	vattr_s *pre_name; // header-name  => (refer_nstring_t)
	volatile uintptr_t used;
} pretreat_s;

#define me(_r)  ((pretreat_s *) _r)

web_server_request_t* web_server_pretreat(web_server_request_t *restrict request, pretreat_s *restrict p)
{
	if (p->used)
	{
		vattr_vlist_t *restrict vl;
		pretreat_header_s *restrict header;
		inst_web_server_value_s *restrict value;
		uhttp_header_s *restrict h;
		yaw_lock_lock(p->read);
		// header
		for (vl = p->header->vattr; vl; vl = vl->vattr_next)
		{
			header = (pretreat_header_s *) vl->value;
			if (header->header)
				uhttp_refer_header_last(request->response_http, header->header);
			else if ((value = header->value))
			{
				if ((h = value->value(value, header->header_name, request)))
				{
					uhttp_refer_header_last(request->response_http, h);
					refer_free(h);
				}
			}
		}
		// trigger
		for (vl = p->trigger->vattr; vl; vl = vl->vattr_next)
		{
			value = (inst_web_server_value_s *) vl->value;
			if ((h = value->value(value, NULL, request)))
				refer_free(h);
		}
		yaw_lock_unlock(p->read);
	}
	return request;
}

static void pretreat_free_func(pretreat_s *restrict r)
{
	if (r->read) refer_free(r->read);
	if (r->write) refer_free(r->write);
	if (r->trigger) refer_free(r->trigger);
	if (r->header) refer_free(r->header);
	if (r->pre_name) refer_free(r->pre_name);
}

static vattr_s* pretreat_alloc_set_pre_name(vattr_s *restrict pre_name, web_header_s *restrict headers)
{
	refer_nstring_t name;
	web_header_id_t i;
	for (i = 0; i < web_header_id_max; ++i)
	{
		if ((name = web_header_get_name(headers, i)) &&
			!vattr_insert_tail(pre_name, name->string, name))
			goto label_fail;
	}
	return pre_name;
	label_fail:
	return NULL;
}

refer_t pretreat_alloc(const struct web_header_s *restrict headers)
{
	pretreat_s *restrict r;
	r = (pretreat_s *) refer_alloz(sizeof(pretreat_s));
	if (r)
	{
		refer_set_free(r, (refer_free_f) pretreat_free_func);
		if (yaw_lock_alloc_rwlock(&r->read, &r->write))
			goto label_fail;
		if (!(r->trigger = vattr_alloc())) goto label_fail;
		if (!(r->header = vattr_alloc())) goto label_fail;
		if (!(r->pre_name = vattr_alloc())) goto label_fail;
		if (pretreat_alloc_set_pre_name(r->pre_name, headers))
			return r;
		label_fail:
		refer_free(r);
	}
	return NULL;
}

static void pretreat_header_free_func(pretreat_header_s *restrict r)
{
	if (r->header_name) refer_free(r->header_name);
	if (r->header) refer_free(r->header);
	if (r->value) refer_free(r->value);
}

static pretreat_header_s* pretreat_header_alloc(vattr_s *restrict pre_name, const char *restrict name, const char *restrict cv, inst_web_server_value_s *restrict vv)
{
	pretreat_header_s *restrict r;
	r = (pretreat_header_s *) refer_alloz(sizeof(pretreat_header_s));
	if (r)
	{
		refer_set_free(r, (refer_free_f) pretreat_header_free_func);
		if ((r->header_name = vattr_get_first(pre_name, name)))
			refer_save(r->header_name);
		else if (!(r->header_name = refer_dump_nstring(name)))
			goto label_fail;
		if (cv && !(r->header = uhttp_header_refer(r->header_name, cv)))
			goto label_fail;
		if (vv) r->value = (inst_web_server_value_s *) refer_save(vv);
		return r;
		label_fail:
		refer_free(r);
	}
	return NULL;
}

static refer_t pretreat_insert_header_and_free_header(refer_t pretreat, const char *restrict name, pretreat_header_s *restrict header)
{
	yaw_lock_s *restrict lk;
	lk = me(pretreat)->write;
	yaw_lock_lock(lk);
	if (vattr_insert_tail(me(pretreat)->header, name, header))
		me(pretreat)->used = 1;
	else pretreat = NULL;
	yaw_lock_unlock(lk);
	refer_free(header);
	return pretreat;
}

refer_t pretreat_insert_header_by_const(refer_t pretreat, const char *restrict name, const char *restrict value)
{
	pretreat_header_s *restrict header;
	if ((header = pretreat_header_alloc(me(pretreat)->pre_name, name, value, NULL)))
		return pretreat_insert_header_and_free_header(pretreat, name, header);
	return NULL;
}

refer_t pretreat_insert_header_by_value(refer_t pretreat, const char *restrict name, inst_web_server_value_s *restrict value)
{
	pretreat_header_s *restrict header;
	if ((header = pretreat_header_alloc(me(pretreat)->pre_name, name, NULL, value)))
		return pretreat_insert_header_and_free_header(pretreat, name, header);
	return NULL;
}

refer_t pretreat_insert_trigger(refer_t pretreat, const char *restrict name, inst_web_server_value_s *restrict value)
{
	yaw_lock_s *restrict lk;
	lk = me(pretreat)->write;
	yaw_lock_lock(lk);
	if (vattr_insert_tail(me(pretreat)->trigger, name, value))
		me(pretreat)->used = 1;
	else pretreat = NULL;
	yaw_lock_unlock(lk);
	return pretreat;
}

static inline void pretreat_used_update(pretreat_s *restrict r)
{
	r->used = (r->header->vattr || r->trigger->vattr);
}

void pretreat_delete_header(refer_t pretreat, const char *restrict name)
{
	yaw_lock_s *restrict lk;
	lk = me(pretreat)->write;
	yaw_lock_lock(lk);
	vattr_delete(me(pretreat)->header, name);
	pretreat_used_update(me(pretreat));
	yaw_lock_unlock(lk);
}

void pretreat_delete_trigger(refer_t pretreat, const char *restrict name)
{
	yaw_lock_s *restrict lk;
	lk = me(pretreat)->write;
	yaw_lock_lock(lk);
	vattr_delete(me(pretreat)->trigger, name);
	pretreat_used_update(me(pretreat));
	yaw_lock_unlock(lk);
}

void pretreat_clear_header(refer_t pretreat)
{
	yaw_lock_s *restrict lk;
	lk = me(pretreat)->write;
	yaw_lock_lock(lk);
	vattr_clear(me(pretreat)->header);
	pretreat_used_update(me(pretreat));
	yaw_lock_unlock(lk);
}

void pretreat_clear_trigger(refer_t pretreat)
{
	yaw_lock_s *restrict lk;
	lk = me(pretreat)->write;
	yaw_lock_lock(lk);
	vattr_clear(me(pretreat)->trigger);
	pretreat_used_update(me(pretreat));
	yaw_lock_unlock(lk);
}
