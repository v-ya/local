#include "miko.instruction.h"
#include "miko.instruction.prefix.h"

static void miko_instruction_free_func(miko_instruction_s *restrict r)
{
	refer_ck_free(r->name);
	refer_ck_free(r->iset);
	refer_ck_free(r->data);
	refer_ck_free(r->prefix);
}

miko_instruction_s* miko_instruction_alloc(const char *restrict name, refer_string_t iset)
{
	miko_instruction_s *restrict r;
	if (name && iset && (r = (miko_instruction_s *) refer_alloz(sizeof(miko_instruction_s))))
	{
		refer_hook_free(r, instruction);
		if ((r->name = refer_dump_string(name)) &&
			(r->iset = (refer_string_t) refer_save(iset)))
			return r;
		refer_free(r);
	}
	return NULL;
}

miko_instruction_s* miko_instruction_set_prefix(miko_instruction_s *restrict r, const miko_iset_pool_s *restrict pool, const miko_access_prefix_t prefix[])
{
	if (!r->prefix && (r->prefix = miko_instruction_prefix_alloc(pool, prefix)))
		return r;
	return NULL;
}

miko_instruction_s* miko_instruction_set_attr(miko_instruction_s *restrict r, const miko_instruction_attr_t attr[])
{
	if (attr)
	{
		while (attr->field && attr->vaule.null)
		{
			switch (attr->field)
			{
				case miko_instruction_attr_field__data:
					if (r->data || !attr->vaule.data)
						goto label_fail;
					r->data = refer_save(attr->vaule.data);
					break;
				#define miko_case_func(_field_)  \
					case miko_instruction_attr_field__##_field_:\
						if (r->func._field_) goto label_fail;\
						r->func._field_ = attr->vaule._field_;\
						break
				miko_case_func(call);
				#undef miko_case_func
				default: goto label_fail;
			}
			++attr;
		}
		if (!attr->field && !attr->vaule.null)
			return r;
	}
	label_fail:
	return NULL;
}
