#ifndef _vkaa_std_h_
#define _vkaa_std_h_

#include "vkaa.h"
#include <tparse/tstack.h>

typedef const struct vkaa_std_typeid_s {
	uintptr_t id_void;
	uintptr_t id_null;
	uintptr_t id_scope;
	uintptr_t id_syntax;
	uintptr_t id_marco;
	uintptr_t id_function;
	uintptr_t id_string;
	uintptr_t id_bool;
	uintptr_t id_uint;
	uintptr_t id_int;
	uintptr_t id_float;
} vkaa_std_typeid_s;

typedef struct vkaa_std_s {
	vkaa_std_typeid_s *typeid;
	vkaa_syntaxor_s *syntaxor;
	vkaa_oplevel_s *oplevel;
	vkaa_tpool_s *tpool;
	vkaa_parse_s *parse;
	tparse_tstack_s *stack;
} vkaa_std_s;

typedef struct vkaa_std_context_s {
	const vkaa_std_s *std;
	vkaa_execute_s *exec;
	vkaa_scope_s *scope;
	vkaa_var_s *var;
} vkaa_std_context_s;

const vkaa_std_s* vkaa_std_alloc(void);

vkaa_std_context_s* vkaa_std_context_alloc(const vkaa_std_s *restrict std);
vkaa_std_context_s* vkaa_std_context_append_syntax(vkaa_std_context_s *restrict context, const vkaa_syntax_s *restrict syntax);
vkaa_std_context_s* vkaa_std_context_append_source(vkaa_std_context_s *restrict context, const char *restrict source_data, uintptr_t source_length);
uintptr_t vkaa_std_context_exec(vkaa_std_context_s *restrict context, const volatile uintptr_t *running);

#endif
