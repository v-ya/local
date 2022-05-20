#ifndef _vkaa_syntax_h_
#define _vkaa_syntax_h_

#include "vkaa.h"
#include <exbuffer.h>

enum vkaa_syntax_type_t {
	vkaa_syntax_type_keyword,   // [A-Za-z_][A-Za-z_0-9]*
	vkaa_syntax_type_operator,  // !#$%^&*+-./:<=>?@\^`|~
	vkaa_syntax_type_comma,     // ,
	vkaa_syntax_type_semicolon, // ;
	vkaa_syntax_type_scope,     // {}
	vkaa_syntax_type_brackets,  // ()
	vkaa_syntax_type_square,    // []
	vkaa_syntax_type_string,    // ""
	vkaa_syntax_type_multichar, // ''
	vkaa_syntax_type_number,    // [0-9][0-9.BbEeXx_]*
};

typedef union vkaa_syntax_data_t {
	refer_t data;
	refer_nstring_t keyword;
	refer_nstring_t operator;
	const vkaa_syntax_s *scope;
	const vkaa_syntax_s *brackets;
	const vkaa_syntax_s *square;
	refer_nstring_t string;
	refer_nstring_t multichar;
	refer_nstring_t number;
} vkaa_syntax_data_t;

struct vkaa_syntax_t {
	vkaa_syntax_type_t type;
	vkaa_syntax_data_t data;
};

struct vkaa_syntax_s {
	exbuffer_t buffer;
	const vkaa_syntax_t *syntax_array;
	uintptr_t syntax_number;
};

vkaa_syntax_s* vkaa_syntax_alloc_empty(void);
vkaa_syntax_s* vkaa_syntax_push_none(vkaa_syntax_s *restrict syntax, vkaa_syntax_type_t type);
refer_nstring_t vkaa_syntax_push_nstring(vkaa_syntax_s *restrict syntax, vkaa_syntax_type_t type, const char *restrict data, uintptr_t length);
vkaa_syntax_s* vkaa_syntax_push_syntax(vkaa_syntax_s *restrict syntax, vkaa_syntax_type_t type);

const vkaa_syntax_t* vkaa_syntax_test(const vkaa_syntax_t *restrict syntax, vkaa_syntax_type_t type, const char *restrict string);

vkaa_syntaxor_s* vkaa_syntaxor_alloc(void);
vkaa_syntaxor_s* vkaa_syntaxor_add_comment(vkaa_syntaxor_s *restrict syntaxor, const char *restrict start, const char *restrict stop);

const vkaa_syntax_s* vkaa_syntax_alloc(vkaa_syntaxor_s *restrict syntaxor, const char *restrict source_data, uintptr_t source_length);

const vkaa_syntax_t* vkaa_syntax_convert_uintptr(const vkaa_syntax_t *restrict syntax, uintptr_t *restrict value, int base);
const vkaa_syntax_t* vkaa_syntax_convert_intptr(const vkaa_syntax_t *restrict syntax, intptr_t *restrict value, int base);
const vkaa_syntax_t* vkaa_syntax_convert_double(const vkaa_syntax_t *restrict syntax, double *restrict value);
const vkaa_syntax_t* vkaa_syntax_convert_float(const vkaa_syntax_t *restrict syntax, float *restrict value);

#endif
