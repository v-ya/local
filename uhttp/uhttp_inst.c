#include "uhttp_type.h"

static void uhttp_inst_free_func(uhttp_inst_s *restrict r)
{
	if (r->version)
		refer_free(r->version);
	if (r->empty)
		refer_free(r->empty);
	if (r->status)
		rbtree_clear(&r->status);
}

static void uhttp_inst_status_free_func(rbtree_t *restrict rb)
{
	if (rb->value)
		refer_free(rb->value);
}

uhttp_inst_s* uhttp_inst_alloc_empty(void)
{
	uhttp_inst_s *restrict r;
	r = (uhttp_inst_s *) refer_alloz(sizeof(uhttp_inst_s));
	if (r)
	{
		refer_set_free(r, (refer_free_f) uhttp_inst_free_func);
		if ((r->empty = refer_dump_nstring_with_length(NULL, 0)))
			return r;
		refer_free(r);
	}
	return NULL;
}

refer_nstring_t uhttp_inst_refer_version(uhttp_inst_s *restrict inst, refer_nstring_t version)
{
	if (inst->version)
		refer_free(inst->version);
	inst->version = refer_save(version);
	return version;
}

refer_nstring_t uhttp_inst_set_version(uhttp_inst_s *restrict inst, const char *restrict version)
{
	refer_nstring_t v;
	v = NULL;
	if (version)
	{
		if (!(v = refer_dump_nstring(version)))
			goto label_fail;
	}
	uhttp_inst_refer_version(inst, v);
	if (v) refer_free(v);
	label_fail:
	return v;
}

refer_nstring_t uhttp_inst_get_version(const uhttp_inst_s *restrict inst)
{
	return inst->version;
}

refer_nstring_t uhttp_inst_refer_status(uhttp_inst_s *restrict inst, int status_code, refer_nstring_t status_desc)
{
	uint64_t index = (uint64_t) status_code;
	rbtree_delete(&inst->status, NULL, index);
	if (status_desc)
	{
		if (rbtree_insert(&inst->status, NULL, index, status_desc, (rbtree_func_free_f) uhttp_inst_status_free_func))
			refer_save(status_desc);
		else status_desc = NULL;
	}
	return status_desc;
}

refer_nstring_t uhttp_inst_set_status(uhttp_inst_s *restrict inst, int status_code, const char *restrict status_desc)
{
	refer_nstring_t v;
	v = NULL;
	if (status_desc)
	{
		if (!(v = refer_dump_nstring(status_desc)))
			goto label_fail;
	}
	uhttp_inst_refer_status(inst, status_code, v);
	if (v) refer_free(v);
	label_fail:
	return v;
}

refer_nstring_t uhttp_inst_get_status(const uhttp_inst_s *restrict inst, int status_code)
{
	rbtree_t *restrict rv;
	if ((rv = rbtree_find(&inst->status, NULL, (uint64_t) status_code)))
		return rv->value;
	return NULL;
}
