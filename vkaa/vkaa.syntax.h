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

struct vkaa_syntax_source_s {
	exbuffer_t linepos_buffer;
	refer_nstring_t source;
	refer_string_t name;
	const uint32_t *restrict linepos_array;
	uintptr_t linepos_number;
};

struct vkaa_syntax_t {
	vkaa_syntax_type_t type;
	uint32_t pos;
	vkaa_syntax_data_t data;
};

struct vkaa_syntax_s {
	exbuffer_t buffer;
	const vkaa_syntax_t *syntax_array;
	uintptr_t syntax_number;
	const vkaa_syntax_source_s *source;
};

vkaa_syntax_s* vkaa_syntax_alloc_empty(void);
vkaa_syntax_s* vkaa_syntax_push_none(vkaa_syntax_s *restrict syntax, vkaa_syntax_type_t type, uint32_t pos);
refer_nstring_t vkaa_syntax_push_nstring(vkaa_syntax_s *restrict syntax, vkaa_syntax_type_t type, uint32_t pos, const char *restrict data, uintptr_t length);
vkaa_syntax_s* vkaa_syntax_push_syntax(vkaa_syntax_s *restrict syntax, vkaa_syntax_type_t type, uint32_t pos);

const vkaa_syntax_t* vkaa_syntax_test(const vkaa_syntax_t *restrict syntax, vkaa_syntax_type_t type, const char *restrict string);

vkaa_syntaxor_s* vkaa_syntaxor_alloc(void);
vkaa_syntaxor_s* vkaa_syntaxor_add_comment(vkaa_syntaxor_s *restrict syntaxor, const char *restrict start, const char *restrict stop);

vkaa_syntax_source_s* vkaa_syntax_source_alloc(refer_nstring_t source);
vkaa_syntax_source_s* vkaa_syntax_source_alloc_by_data(const char *restrict source_data, uintptr_t source_length);
refer_string_t vkaa_syntax_source_refer_name(vkaa_syntax_source_s *restrict source, refer_string_t name);
refer_string_t vkaa_syntax_source_set_name(vkaa_syntax_source_s *restrict source, const char *restrict name);
uint32_t vkaa_syntax_source_linepos(const vkaa_syntax_source_s *restrict source, uint32_t pos, uint32_t *restrict cpos);

const vkaa_syntax_s* vkaa_syntax_alloc(vkaa_syntaxor_s *restrict syntaxor, const vkaa_syntax_source_s *restrict source);

const vkaa_syntax_t* vkaa_syntax_convert_uintptr(const vkaa_syntax_t *restrict syntax, uintptr_t *restrict value, int base);
const vkaa_syntax_t* vkaa_syntax_convert_intptr(const vkaa_syntax_t *restrict syntax, intptr_t *restrict value, int base);
const vkaa_syntax_t* vkaa_syntax_convert_double(const vkaa_syntax_t *restrict syntax, double *restrict value);
const vkaa_syntax_t* vkaa_syntax_convert_float(const vkaa_syntax_t *restrict syntax, float *restrict value);

#endif
