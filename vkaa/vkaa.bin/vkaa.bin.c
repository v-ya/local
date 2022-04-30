#include "../vkaa.std.h"
#include "../vkaa.syntax.h"
#include <fsys.h>
#include <stdio.h>

void dump_syntax(const vkaa_syntax_s *restrict syntax, int n)
{
	const vkaa_syntax_t *restrict s;
	for (uintptr_t i = 0; i < syntax->syntax_number; ++i)
	{
		s = syntax->syntax_array + i;
		switch (s->type)
		{
			case vkaa_syntax_type_keyword:
				printf("%*c[%zu] keyword: %s\n", n, 0, i, s->data.keyword->string);
				break;
			case vkaa_syntax_type_operator:
				printf("%*c[%zu] operator: %s\n", n, 0, i, s->data.operator->string);
				break;
			case vkaa_syntax_type_comma:
				printf("%*c[%zu] ,\n", n, 0, i);
				break;
			case vkaa_syntax_type_semicolon:
				printf("%*c[%zu] ;\n", n, 0, i);
				break;
			case vkaa_syntax_type_scope:
				printf("%*c[%zu] {}\n", n, 0, i);
				dump_syntax(s->data.scope, n + 4);
				break;
			case vkaa_syntax_type_brackets:
				printf("%*c[%zu] ()\n", n, 0, i);
				dump_syntax(s->data.brackets, n + 4);
				break;
			case vkaa_syntax_type_square:
				printf("%*c[%zu] []\n", n, 0, i);
				dump_syntax(s->data.square, n + 4);
				break;
			case vkaa_syntax_type_string:
				printf("%*c[%zu] \"\": %s\n", n, 0, i, s->data.string->string);
				break;
			case vkaa_syntax_type_multichar:
				printf("%*c[%zu] \'\': %s\n", n, 0, i, s->data.multichar->string);
				break;
			case vkaa_syntax_type_number:
				printf("%*c[%zu] 0: %s\n", n, 0, i, s->data.number->string);
				break;
			default:
				break;
		}
	}
}

refer_nstring_t load_file(const char *restrict path)
{
	fsys_file_s *restrict fp;
	refer_nstring_t r;
	r = NULL;
	if ((fp = fsys_file_alloc(path, fsys_file_flag_read)))
	{
		r = fsys_file_aread_all(fp);
		refer_free(fp);
	}
	return r;
}

int main(void)
{
	vkaa_std_s *restrict std;
	refer_nstring_t code;
	const vkaa_syntax_s *restrict syntax;
	if ((std = vkaa_std_alloc()))
	{
		if ((code = load_file("../test.vkaa")))
		{
			if ((syntax = vkaa_syntax_alloc(std->syntaxor, code->string, code->length)))
			{
				dump_syntax(syntax, 0);
				refer_free(syntax);
			}
			refer_free(code);
		}
		refer_free(std);
	}
	return 0;
}
