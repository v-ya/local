#include "cparse.type.h"

typedef struct cparse_parse_operator_s {
	cparse_parse_s parse;
	cparse_value_s *value;
} cparse_parse_operator_s;

/*
	!	! !=
	%	% %=
	*	* *=
	=	= ==
	^	^ ^=
	/	/ /= /\* /\/
	&	& && &=
	|	| || |=
	+	+ ++ +=
	-	- -- -= ->
	<	< << <= <<=
	>	> >> >= >>=
	(	(
	)	)
	,	,
	.	.
	:	:
	;	;
	?	?
	[	[
	]	]
	~	~
*/

static cparse_parse_s* cparse_inner_parse_operator(cparse_parse_operator_s *restrict pri, cparse_inst_s *restrict inst, cparse_inst_context_t *restrict context)
{
	char operator[8];
	const char *restrict p;
	uintptr_t pos, n;
	n = 1;
	switch ((p = context->data)[(pos = context->pos) - 1])
	{
		case '!':
		case '%':
		case '*':
		case '=':
		case '^':
			if (pos >= context->size)
				break;
			label_check_equ:
			if (p[pos] == '=')
				++n;
			break;
		case '/':
			if (pos >= context->size)
				break;
			if (p[pos] == '*')
			{
				// block comment
				context->pos = pos + 1;
				return cparse_inner_parse_comment_block(&pri->parse, inst, context);
			}
			if (p[pos] == '/')
			{
				// line comment
				context->pos = pos + 1;
				return cparse_inner_parse_comment_line(&pri->parse, inst, context);
			}
			goto label_check_equ;
		case '&':
		case '|':
		case '+':
			if (pos >= context->size)
				break;
			label_check_overlapping_and_equ:
			if (p[pos] == p[pos - 1])
			{
				label_n_plus_and_break:
				++n;
				break;
			}
			goto label_check_equ;
		case '-':
			if (pos >= context->size)
				break;
			if (p[pos] == '>')
				goto label_n_plus_and_break;
			goto label_check_overlapping_and_equ;
		case '<':
		case '>':
			if (pos >= context->size)
				break;
			if (p[pos] == p[pos - 1])
			{
				if (++pos >= context->size || p[pos] != '=')
					goto label_n_plus_and_break;
				n += 2;
				break;
			}
			goto label_check_equ;
		case '(':
		case ')':
		case ',':
		case '.':
		case ':':
		case ';':
		case '?':
		case '[':
		case ']':
		case '~':
			break;
		default:
			// goto label_fail;
			break;
	}
	pos = context->pos - 1;
	context->pos = pos + n;
	operator[n] = 0;
	while (n) --n, operator[n] = p[pos + n];
	if (vattr_insert_tail(context->scope, operator, pri->value))
		return &pri->parse;
	// label_fail:
	return NULL;
}

static void cparse_parse_operator_free_func(cparse_parse_operator_s *restrict r)
{
	if (r->value)
		refer_free(r->value);
}

cparse_parse_s* cparse_inner_alloc_parse_operator(void)
{
	cparse_parse_operator_s *restrict r;
	if ((r = (cparse_parse_operator_s *) refer_alloz(sizeof(cparse_parse_operator_s))))
	{
		refer_set_free(r, (refer_free_f) cparse_parse_operator_free_func);
		r->parse.parse = (cparse_parse_f) cparse_inner_parse_operator;
		if ((r->value = cparse_inner_alloc_value_empty(cparse_value$operator)))
			return &r->parse;
		refer_free(r);
	}
	return NULL;
}
