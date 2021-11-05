#ifndef _cparse_h_
#define _cparse_h_

#include <refer.h>
#include <vattr.h>

typedef struct cparse_inst_s cparse_inst_s;
typedef struct cparse_scope_s cparse_scope_s;
typedef const struct cparse_value_s cparse_value_s;

struct cparse_scope_s {
	vattr_s *scope;       // => (cparse_value_s *)
	refer_nstring_t code; // code
	refer_string_t path;  // ex: dir/
	refer_string_t name;  // ex: code.c
};

typedef enum cparse_value_type_t {
	cparse_value$scope,       // {...}
	cparse_value$key,         // _abc$01
	cparse_value$string,      // "..."
	cparse_value$chars,       // '...'
	cparse_value$number,      // -1.23, +15u, 0x2233, ...
	cparse_value$operator,    // +-*/%...
	cparse_value$include$sys, // #include <...>
	cparse_value$include$cur, // #include "..."
	cparse_value$define,      // #define, #undef
	cparse_value$check,       // #if, ##elif
} cparse_value_type_t;

typedef union cparse_value_data_t {
	refer_t refer;
	cparse_scope_s *scope;
	refer_nstring_t string;
	refer_nstring_t chars;
	refer_nstring_t number;
	refer_string_t include;
} cparse_value_data_t;

struct cparse_value_s {
	cparse_value_type_t type;
	cparse_value_data_t data;
};

cparse_inst_s* cparse_inst_alloc(void);
void cparse_inst_clear(cparse_inst_s *restrict inst);

cparse_scope_s* cparse_scope_alloc(cparse_inst_s *restrict inst, refer_nstring_t code, refer_string_t path, refer_string_t name);
cparse_scope_s* cparse_scope_alloc_by_code_path(cparse_inst_s *restrict inst, refer_nstring_t code, const char *restrict code_path);
cparse_scope_s* cparse_scope_load(cparse_inst_s *restrict inst, const char *restrict code_path);

#endif
