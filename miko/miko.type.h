#ifndef _miko_type_h_
#define _miko_type_h_

#include <stdint.h>

// type base

typedef uintptr_t miko_index_t;
typedef uintptr_t miko_xpos_t;
typedef struct miko_vector_s *miko_vector_s_t;
typedef struct miko_strpool_s *miko_strpool_s_t;

// type hide

typedef struct miko_log_s miko_log_s;
typedef struct miko_iset_s miko_iset_s;
typedef struct vattr_s miko_iset_pool_s;

// type enum

typedef enum miko_log_level_t miko_log_level_t;
typedef enum miko_instruction_attr_field_t miko_instruction_attr_field_t;
typedef enum miko_major_vtype_t miko_major_vtype_t;

// type union

typedef union miko_instruction_attr_func_t miko_instruction_attr_func_t;

// type struct

typedef struct miko_xpos_prefix_t miko_xpos_prefix_t;
typedef struct miko_instruction_attr_t miko_instruction_attr_t;
typedef struct miko_minor_s miko_minor_s;

// type function

typedef void (*miko_func_f)(void);
typedef void* (*miko_vector_initial_f)(void *restrict v);
typedef refer_free_f miko_vector_finally_f;
typedef miko_rt_s* (*miko_instruction_attr_call_f)(miko_rt_s *restrict r, miko_index_t instruction, const miko_xpos_t xpos[]);

// type define

enum miko_log_level_t {
	miko_log_level__error,
	miko_log_level__waring,
	miko_log_level__notify,
	miko_log_level__debug,
	miko_log_level_max
};

enum miko_instruction_attr_field_t {
	miko_instruction_attr_field__null,
	miko_instruction_attr_field__call,
	miko_instruction_attr_field_max
};

enum miko_major_vtype_t {
	miko_major_vtype__real,
	miko_major_vtype__refer,
	miko_major_vtype__wink
};

union miko_instruction_attr_func_t {
	miko_func_f func;
	miko_instruction_attr_call_f call;
};

// this modifier prefix of a real param (miko_xpos_t xpos)
struct miko_xpos_prefix_t {
	const char *score;  // xpos-score  (const, static, stack, this)
	const char *style;  // xpos-style  (read, write, exec)
	const char *major;  // major-type  (in miko.iset.type)
	const char *minor;  // minor-type  (in major-type)
};

struct miko_instruction_attr_t {
	miko_instruction_attr_field_t field;
	miko_instruction_attr_func_t func;
};

// var-type creater
struct miko_minor_s {
	refer_string_t name;  // miko.major.name
	miko_index_t minor;   // miko.major[major].minor[minor]
};

#endif
