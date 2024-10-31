#ifndef _miko_type_instruction_h_
#define _miko_type_instruction_h_

#include "miko.type.base.h"
#include "miko.type.instr.h"

typedef enum miko_instruction_attr_field_t miko_instruction_attr_field_t;
typedef union miko_instruction_attr_value_t miko_instruction_attr_value_t;
typedef struct miko_instruction_attr_t miko_instruction_attr_t;

typedef miko_instr_f miko_instruction_attr_call_f;

enum miko_instruction_attr_field_t {
	miko_instruction_attr_field__null,
	miko_instruction_attr_field__data,
	miko_instruction_attr_field__call,
	miko_instruction_attr_field_max
};

union miko_instruction_attr_value_t {
	void *null;
	refer_t data;
	miko_instruction_attr_call_f call;
};

struct miko_instruction_attr_t {
	miko_instruction_attr_field_t field;
	miko_instruction_attr_value_t vaule;
};

#endif
