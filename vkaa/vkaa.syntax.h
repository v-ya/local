#ifndef _vkaa_syntax_h_
#define _vkaa_syntax_h_

#include "vkaa.h"
#include <exbuffer.h>

typedef enum vkaa_syntax_type_t {
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
} vkaa_syntax_type_t;

typedef union vkaa_syntax_data_t {
	refer_t data;
	refer_nstring_t keyword;
	refer_nstring_t operator;
	vkaa_syntax_s  *scope;
	vkaa_syntax_s  *brackets;
	vkaa_syntax_s  *square;
	refer_nstring_t string;
	refer_nstring_t multichar;
	refer_nstring_t number;
} vkaa_syntax_data_t;

typedef struct vkaa_syntax_t {
	vkaa_syntax_type_t type;
	vkaa_syntax_data_t data;
} vkaa_syntax_t;

struct vkaa_syntax_s {
	exbuffer_t buffer;
	const vkaa_syntax_t *syntax_array;
	uintptr_t syntax_number;
	refer_nstring_t source;
	uintptr_t source_offset;
	uintptr_t source_length;
};

vkaa_syntaxor_s* vkaa_syntaxor_alloc(void);
vkaa_syntax_s* vkaa_syntax_alloc(vkaa_syntaxor_s *restrict syntaxor, refer_nstring_t source);

#endif
