#include "miko.iset.h"
#include "miko.major.h"
#include "miko.instruction.h"

static void miko_iset_free_func(miko_iset_s *restrict r)
{
	refer_ck_free(r->name);
	refer_ck_free(r->depend);
	refer_ck_free(r->segment);
	refer_ck_free(r->action);
	refer_ck_free(r->major);
	refer_ck_free(r->instruction);
}

miko_iset_s* miko_iset_alloc(const char *restrict name)
{
	miko_iset_s *restrict r;
	if ((r = (miko_iset_s *) refer_alloz(sizeof(miko_iset_s))))
	{
		refer_hook_free(r, iset);
		if ((r->name = (name?refer_dump_string(name):NULL)) &&
			(r->depend = vattr_alloc()) &&
			(r->segment = vattr_alloc()) &&
			(r->action = vattr_alloc()) &&
			(r->major = vattr_alloc()) &&
			(r->instruction = vattr_alloc()))
			return r;
		refer_free(r);
	}
	return NULL;
}

#define miko_iset_add_string(_field_, _value_)  \
	{\
		refer_string_t _string_;\
		if (_value_ && !vattr_get_vslot(r->_field_, _value_) && (_string_ = refer_dump_string(_value_)))\
		{\
			if (!vattr_insert_tail(r->_field_, _value_, _string_))\
				r = NULL;\
			refer_free(_string_);\
			return r;\
		}\
		return NULL;\
	}

miko_iset_s* miko_iset_add_depend(miko_iset_s *restrict r, const char *restrict name)
	miko_iset_add_string(depend, name)

miko_iset_s* miko_iset_add_segment(miko_iset_s *restrict r, const char *restrict name)
	miko_iset_add_string(segment, name)

miko_iset_s* miko_iset_add_action(miko_iset_s *restrict r, const char *restrict name)
	miko_iset_add_string(action, name)

#undef miko_iset_add_string

miko_iset_s* miko_iset_add_major(miko_iset_s *restrict r, const char *restrict major_name, miko_major_vtype_t vtype)
{
	miko_major_s *restrict major;
	if (major_name && !vattr_get_vslot(r->major, major_name) &&
		(major = miko_major_alloc(major_name, r->name, vtype)))
	{
		if (!vattr_insert_tail(r->major, major_name, major))
			r = NULL;
		refer_free(major);
		return r;
	}
	return NULL;
}

miko_iset_s* miko_iset_add_minor(miko_iset_s *restrict r, const char *restrict major_name, miko_minor_s *restrict minor)
{
	miko_major_s *restrict major;
	if (major_name && (major = (miko_major_s *) vattr_get_first(r->major, major_name)) &&
		miko_major_add_minor(major, minor))
		return r;
	return NULL;
}

miko_iset_s* miko_iset_add_default_minor(miko_iset_s *restrict r, const char *restrict major_name, const char *restrict minor_name)
{
	miko_minor_s *restrict minor;
	if (major_name && minor_name && (minor = miko_minor_alloc(minor_name, sizeof(miko_minor_s))))
	{
		r = miko_iset_add_minor(r, major_name, minor);
		refer_free(minor);
		return r;
	}
	return NULL;
}

miko_iset_s* miko_iset_add_instruction(miko_iset_s *restrict r, const miko_iset_pool_s *restrict pool, const char *restrict name, const miko_access_prefix_t prefix[], const miko_instruction_attr_t attr[], refer_t pri)
{
	miko_instruction_s *restrict instruction;
	if (name && (instruction = miko_instruction_alloc(name, r->name, pri)))
	{
		if (miko_instruction_set_prefix(instruction, pool, prefix) &&
			miko_instruction_set_func(instruction, attr) &&
			vattr_insert_tail(r->instruction, name, instruction))
			;
		else r = NULL;
		refer_free(instruction);
		return r;
	}
	return NULL;
}
