#ifndef _miko_type_h_
#define _miko_type_h_

#include <stdint.h>

typedef enum miko_instruction_attr_field_t miko_instruction_attr_field_t;
typedef union miko_instruction_attr_func_t miko_instruction_attr_func_t;
typedef struct miko_instruction_argv_t miko_instruction_argv_t;
typedef struct miko_instruction_attr_t miko_instruction_attr_t;
typedef struct miko_iset_s miko_iset_s;
typedef struct vattr_s miko_iset_pool_s;

typedef void (*miko_func_f)(void);
typedef miko_rt_s* (*miko_instruction_attr_call_f)(miko_rt_s *restrict r, uintptr_t instruction, const uintptr_t argv[]);

enum miko_instruction_attr_field_t {
	miko_instruction_attr_field__null,
	miko_instruction_attr_field__call,
	miko_instruction_attr_field_max
};

union miko_instruction_attr_func_t {
	miko_func_f func;
	miko_instruction_attr_call_f call;
};

struct miko_instruction_argv_t {
	const char *xfind_type;
	const char *value_type;
};

struct miko_instruction_attr_t {
	miko_instruction_attr_field_t field;
	miko_instruction_attr_func_t func;
};

#endif
