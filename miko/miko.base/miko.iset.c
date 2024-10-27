#include "miko.iset.h"
#include "miko.instruction.h"

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

static void miko_iset_free_func(miko_iset_s *restrict r)
{
	refer_ck_free(r->name);
	refer_ck_free(r->depend);
	refer_ck_free(r->type);
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
			(r->type = vattr_alloc()) &&
			(r->instruction = vattr_alloc()))
			return r;
		refer_free(r);
	}
	return NULL;
}

miko_iset_s* miko_iset_add_depend(miko_iset_s *restrict r, const char *restrict name)
	miko_iset_add_string(depend, name)

miko_iset_s* miko_iset_add_type(miko_iset_s *restrict r, const char *restrict name)
	miko_iset_add_string(type, name)

miko_iset_s* miko_iset_add_instruction(miko_iset_s *restrict r, const miko_iset_pool_s *restrict pool, const char *restrict name, const miko_instruction_argv_t argv[], const miko_instruction_attr_t attr[], refer_t pri)
{
	miko_instruction_s *restrict instruction;
	if (name && !vattr_get_vslot(r->instruction, name) &&
		(instruction = miko_instruction_alloc(name, r->name, pri)))
	{
		if (miko_instruction_set_args(instruction, pool, argv) &&
			miko_instruction_set_func(instruction, attr) &&
			vattr_insert_tail(r->instruction, name, instruction))
			;
		else r = NULL;
		refer_free(instruction);
		return r;
	}
	return NULL;
}
