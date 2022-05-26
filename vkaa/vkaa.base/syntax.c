#include "../vkaa.syntax.h"
#include <tparse/tmapping.h>
#include <tparse/tstack.h>
#include <tparse/tstring.h>
#include <tparse/tword.h>
#include <string.h>

typedef struct vkaa_syntaxor_context_t vkaa_syntaxor_context_t;
typedef struct vkaa_syntaxor_parse_s vkaa_syntaxor_parse_s;
typedef struct vkaa_syntaxor_stack_t vkaa_syntaxor_stack_t;

typedef vkaa_syntaxor_parse_s* (*vkaa_syntaxor_parse_f)(vkaa_syntaxor_parse_s *restrict p, vkaa_syntaxor_context_t *restrict context);

struct vkaa_syntaxor_s {
	tparse_tmapping_s *mapping;
	tparse_tstack_s *stack;
	vkaa_syntaxor_parse_s *status[256];
};

struct vkaa_syntaxor_context_t {
	tparse_tstack_s *stack;
	vkaa_syntax_s *syntax;
	const char *data;
	uintptr_t size;
	uintptr_t pos;
};

struct vkaa_syntaxor_parse_s {
	vkaa_syntaxor_parse_f parse;
};

struct vkaa_syntaxor_stack_t {
	vkaa_syntax_s *syntax;
	char end_char;
};

// syntax

static void vkaa_syntax_free_func(vkaa_syntax_s *restrict r)
{
	const vkaa_syntax_t *restrict s;
	uintptr_t i, n;
	if ((s = r->syntax_array))
	{
		n = r->syntax_number;
		for (i = 0; i < n; ++i)
		{
			if (s[i].data.data)
				refer_free(s[i].data.data);
		}
	}
	exbuffer_uini(&r->buffer);
	if (r->source) refer_free(r->source);
}

vkaa_syntax_s* vkaa_syntax_alloc_empty(void)
{
	vkaa_syntax_s *restrict r;
	if ((r = (vkaa_syntax_s *) refer_alloz(sizeof(vkaa_syntax_s))))
	{
		if (exbuffer_init(&r->buffer, 0))
		{
			refer_set_free(r, (refer_free_f) vkaa_syntax_free_func);
			return r;
		}
		refer_free(r);
	}
	return NULL;
}

static vkaa_syntax_s* vkaa_syntax_push(vkaa_syntax_s *restrict r, vkaa_syntax_type_t type, uint32_t pos, refer_t data)
{
	vkaa_syntax_t *restrict s;
	uintptr_t index;
	if ((s = exbuffer_need(&r->buffer, ((index = r->syntax_number) + 1) * sizeof(vkaa_syntax_t))))
	{
		s[index].type = type;
		s[index].pos = pos;
		s[index].data.data = data;
		r->syntax_array = s;
		r->syntax_number = index + 1;
		return r;
	}
	return NULL;
}

vkaa_syntax_s* vkaa_syntax_push_none(vkaa_syntax_s *restrict syntax, vkaa_syntax_type_t type, uint32_t pos)
{
	switch (type)
	{
		case vkaa_syntax_type_comma:
		case vkaa_syntax_type_semicolon:
			return vkaa_syntax_push(syntax, type, pos, NULL);
		default: break;
	}
	return NULL;
}

refer_nstring_t vkaa_syntax_push_nstring(vkaa_syntax_s *restrict syntax, vkaa_syntax_type_t type, uint32_t pos, const char *restrict data, uintptr_t length)
{
	refer_nstring_t ns;
	switch (type)
	{
		case vkaa_syntax_type_keyword:
		case vkaa_syntax_type_operator:
		case vkaa_syntax_type_string:
		case vkaa_syntax_type_multichar:
		case vkaa_syntax_type_number:
			if ((ns = refer_dump_nstring_with_length(data, length)))
			{
				if (vkaa_syntax_push(syntax, type, pos, ns))
					return ns;
				refer_free(ns);
			}
			// fall through
		default: break;
	}
	return NULL;
}

vkaa_syntax_s* vkaa_syntax_push_syntax(vkaa_syntax_s *restrict syntax, vkaa_syntax_type_t type, uint32_t pos)
{
	vkaa_syntax_s *restrict s;
	switch (type)
	{
		case vkaa_syntax_type_scope:
		case vkaa_syntax_type_brackets:
		case vkaa_syntax_type_square:
			if ((s = vkaa_syntax_alloc_empty()))
			{
				s->source = (const vkaa_syntax_source_s *) refer_save(syntax->source);
				if (vkaa_syntax_push(syntax, type, pos, s))
					return s;
				refer_free(s);
			}
			// fall through
		default: break;
	}
	return NULL;
}

const vkaa_syntax_t* vkaa_syntax_test(const vkaa_syntax_t *restrict syntax, vkaa_syntax_type_t type, const char *restrict string)
{
	if (syntax->type == type)
	{
		switch (type)
		{
			case vkaa_syntax_type_keyword:
			case vkaa_syntax_type_operator:
			case vkaa_syntax_type_string:
			case vkaa_syntax_type_multichar:
			case vkaa_syntax_type_number:
				if (string && strcmp(syntax->data.string->string, string))
					goto label_fail;
				// fall through
			default: return syntax;
		}
	}
	label_fail:
	return NULL;
}

// parse (keyword operator number)

typedef struct vkaa_syntaxor_parse_match_s {
	vkaa_syntaxor_parse_s p;
	tparse_tword_edge_s *edge;
	vkaa_syntax_type_t type;
} vkaa_syntaxor_parse_match_s;

static vkaa_syntaxor_parse_s* vkaa_syntaxor_parse_match_parse_func(vkaa_syntaxor_parse_match_s *restrict p, vkaa_syntaxor_context_t *restrict context)
{
	uintptr_t p0, p1;
	p1 = tparse_tword_skip(p->edge, context->data, context->size, p0 = context->pos);
	if (p1 > p0)
	{
		if (vkaa_syntax_push_nstring(context->syntax, p->type, (uint32_t) p0, context->data + p0, p1 - p0))
		{
			context->pos = p1;
			return &p->p;
		}
	}
	return NULL;
}

static void vkaa_syntaxor_parse_match_free_func(vkaa_syntaxor_parse_match_s *restrict r)
{
	if (r->edge) refer_free(r->edge);
}

static vkaa_syntaxor_parse_match_s* vkaa_syntaxor_parse_match_alloc_empty(vkaa_syntax_type_t type)
{
	vkaa_syntaxor_parse_match_s *restrict r;
	if ((r = (vkaa_syntaxor_parse_match_s *) refer_alloz(sizeof(vkaa_syntaxor_parse_match_s))))
	{
		refer_set_free(r, (refer_free_f) vkaa_syntaxor_parse_match_free_func);
		if ((r->edge = tparse_tword_edge_alloc()))
		{
			r->p.parse = (vkaa_syntaxor_parse_f) vkaa_syntaxor_parse_match_parse_func;
			r->type = type;
			return r;
		}
		refer_free(r);
	}
	return NULL;
}

static vkaa_syntaxor_parse_s* vkaa_syntaxor_parse_create_keyword(void)
{
	vkaa_syntaxor_parse_match_s *restrict r;
	if ((r = vkaa_syntaxor_parse_match_alloc_empty(vkaa_syntax_type_keyword)))
	{
		tparse_tword_edge_s *restrict e;
		uintptr_t i, n;
		e = r->edge;
		e->edge['_'] = tparse_tword_edge_head | tparse_tword_edge_inner | tparse_tword_edge_tail;
		for (i = 'A', n = 'Z'; i <= n; ++i)
			e->edge[i] = tparse_tword_edge_head | tparse_tword_edge_inner | tparse_tword_edge_tail;
		for (i = 'a', n = 'z'; i <= n; ++i)
			e->edge[i] = tparse_tword_edge_head | tparse_tword_edge_inner | tparse_tword_edge_tail;
		for (i = 0x80, n = 0xff; i <= n; ++i)
			e->edge[i] = tparse_tword_edge_head | tparse_tword_edge_inner | tparse_tword_edge_tail;
		for (i = '0', n = '9'; i <= n; ++i)
			e->edge[i] = tparse_tword_edge_inner | tparse_tword_edge_tail;
		return &r->p;
	}
	return NULL;
}

static vkaa_syntaxor_parse_s* vkaa_syntaxor_parse_create_operator(void)
{
	static const uint8_t s_edge[] = "!#$%^&*+-./:<=>?@\\^`|~";
	vkaa_syntaxor_parse_match_s *restrict r;
	if ((r = vkaa_syntaxor_parse_match_alloc_empty(vkaa_syntax_type_operator)))
	{
		tparse_tword_edge_s *restrict e;
		uintptr_t i, n;
		e = r->edge;
		for (i = 0, n = sizeof(s_edge) - 1; i < n; ++i)
			e->edge[s_edge[i]] = tparse_tword_edge_head | tparse_tword_edge_inner | tparse_tword_edge_tail;
		return &r->p;
	}
	return NULL;
}

static vkaa_syntaxor_parse_s* vkaa_syntaxor_parse_number_parse_func(vkaa_syntaxor_parse_match_s *restrict p, vkaa_syntaxor_context_t *restrict context)
{
	uintptr_t p0, p1, p2;
	p1 = tparse_tword_skip(p->edge, context->data, context->size, p0 = context->pos);
	if (p1 > p0)
	{
		if (context->data[p1 - 1] == 'e' || context->data[p1 - 1] == 'E')
		{
			if (p1 < context->size)
			{
				if (context->data[p1] == '+' || context->data[p1] == '-')
				{
					p2 = tparse_tword_skip(p->edge, context->data, context->size, p1 + 1);
					if (p2 > p1 + 1) p1 = p2;
				}
			}
		}
		if (vkaa_syntax_push_nstring(context->syntax, p->type, (uint32_t) p0, context->data + p0, p1 - p0))
		{
			context->pos = p1;
			return &p->p;
		}
	}
	return NULL;
}

static vkaa_syntaxor_parse_s* vkaa_syntaxor_parse_create_number(void)
{
	static const uint8_t s_edge[] = ".BbEeXx";
	vkaa_syntaxor_parse_match_s *restrict r;
	if ((r = vkaa_syntaxor_parse_match_alloc_empty(vkaa_syntax_type_number)))
	{
		tparse_tword_edge_s *restrict e;
		uintptr_t i, n;
		r->p.parse = (vkaa_syntaxor_parse_f) vkaa_syntaxor_parse_number_parse_func;
		e = r->edge;
		for (i = '0', n = '9'; i <= n; ++i)
			e->edge[i] = tparse_tword_edge_head | tparse_tword_edge_inner | tparse_tword_edge_tail;
		for (i = 0, n = sizeof(s_edge) - 1; i < n; ++i)
			e->edge[s_edge[i]] = tparse_tword_edge_inner | tparse_tword_edge_tail;
		return &r->p;
	}
	return NULL;
}

// parse (comma semicolon)

typedef struct vkaa_syntaxor_parse_single_s {
	vkaa_syntaxor_parse_s p;
	vkaa_syntax_type_t type;
} vkaa_syntaxor_parse_single_s;

static vkaa_syntaxor_parse_s* vkaa_syntaxor_parse_single_parse_func(vkaa_syntaxor_parse_single_s *restrict p, vkaa_syntaxor_context_t *restrict context)
{
	if (vkaa_syntax_push(context->syntax, p->type, (uint32_t) context->pos, NULL))
	{
		if (context->pos < context->size)
			context->pos += 1;
		return &p->p;
	}
	return NULL;
}

static vkaa_syntaxor_parse_single_s* vkaa_syntaxor_parse_single_alloc(vkaa_syntax_type_t type)
{
	vkaa_syntaxor_parse_single_s *restrict r;
	if ((r = (vkaa_syntaxor_parse_single_s *) refer_alloz(sizeof(vkaa_syntaxor_parse_single_s))))
	{
		r->p.parse = (vkaa_syntaxor_parse_f) vkaa_syntaxor_parse_single_parse_func;
		r->type = type;
		return r;
	}
	return NULL;
}

static vkaa_syntaxor_parse_s* vkaa_syntaxor_parse_create_comma(void)
{
	return &vkaa_syntaxor_parse_single_alloc(vkaa_syntax_type_comma)->p;
}

static vkaa_syntaxor_parse_s* vkaa_syntaxor_parse_create_semicolon(void)
{
	return &vkaa_syntaxor_parse_single_alloc(vkaa_syntax_type_semicolon)->p;
}

// parse (string multichar)

typedef struct vkaa_syntaxor_parse_string_s {
	vkaa_syntaxor_parse_s p;
	tparse_tstring_s *ts;
	vkaa_syntax_type_t type;
} vkaa_syntaxor_parse_string_s;

static vkaa_syntaxor_parse_s* vkaa_syntaxor_parse_string_parse_func(vkaa_syntaxor_parse_string_s *restrict p, vkaa_syntaxor_context_t *restrict context)
{
	uintptr_t p0, p1, length;
	const char *restrict data;
	tparse_tstring_clear(p->ts);
	if (tparse_tstring_transport(p->ts, context->data, context->size, p0 = context->pos, &p1) && p1 > p0)
	{
		if ((data = tparse_tstring_get_data(p->ts, &length)) &&
			vkaa_syntax_push_nstring(context->syntax, p->type, (uint32_t) p0, data, length))
		{
			context->pos = p1;
			return &p->p;
		}
	}
	return NULL;
}

static void vkaa_syntaxor_parse_string_free_func(vkaa_syntaxor_parse_string_s *restrict r)
{
	if (r->ts) refer_free(r->ts);
}

static vkaa_syntaxor_parse_string_s* vkaa_syntaxor_parse_string_alloc(vkaa_syntax_type_t type, tparse_tstring_s* (*ts_alloc_func)(void))
{
	vkaa_syntaxor_parse_string_s *restrict r;
	if ((r = (vkaa_syntaxor_parse_string_s *) refer_alloz(sizeof(vkaa_syntaxor_parse_string_s))))
	{
		refer_set_free(r, (refer_free_f) vkaa_syntaxor_parse_string_free_func);
		if ((r->ts = ts_alloc_func()))
		{
			r->p.parse = (vkaa_syntaxor_parse_f) vkaa_syntaxor_parse_string_parse_func;
			r->type = type;
			return r;
		}
		refer_free(r);
	}
	return NULL;
}

static vkaa_syntaxor_parse_s* vkaa_syntaxor_parse_create_string(void)
{
	return &vkaa_syntaxor_parse_string_alloc(vkaa_syntax_type_string, tparse_tstring_alloc_c_parse_multi_quotes)->p;
}

static vkaa_syntaxor_parse_s* vkaa_syntaxor_parse_create_multichar(void)
{
	return &vkaa_syntaxor_parse_string_alloc(vkaa_syntax_type_multichar, tparse_tstring_alloc_c_parse_single_quotes)->p;
}

// parse (scope brackets square)

static void vkaa_syntaxor_stack_free_func(vkaa_syntaxor_stack_t *restrict r)
{
	if (r->syntax) refer_free(r->syntax);
}

static vkaa_syntaxor_parse_s* vkaa_syntaxor_parse_inside_parse_func(vkaa_syntaxor_parse_s *restrict p, vkaa_syntaxor_context_t *restrict context)
{
	vkaa_syntaxor_stack_t *restrict stack;
	vkaa_syntax_s *restrict syntax;
	vkaa_syntax_type_t type;
	char end_char;
	switch (context->data[context->pos])
	{
		case '(': type = vkaa_syntax_type_brackets; end_char = ')'; break;
		case '[': type = vkaa_syntax_type_square; end_char = ']'; break;
		case '{': type = vkaa_syntax_type_scope; end_char = '}'; break;
		default: goto label_fail;
	}
	if ((stack = tparse_tstack_push(context->stack, sizeof(vkaa_syntaxor_stack_t), (tparse_tstack_free_f) vkaa_syntaxor_stack_free_func)))
	{
		if ((syntax = vkaa_syntax_push_syntax(context->syntax, type, context->pos)))
		{
			stack->syntax = (vkaa_syntax_s *) refer_save(context->syntax);
			stack->end_char = end_char;
			context->syntax = syntax;
			context->pos += 1;
			return p;
		}
		tparse_tstack_pop(context->stack);
	}
	label_fail:
	return NULL;
}

static vkaa_syntaxor_parse_s* vkaa_syntaxor_parse_inside_outside_func(vkaa_syntaxor_parse_s *restrict p, vkaa_syntaxor_context_t *restrict context)
{
	vkaa_syntaxor_stack_t *restrict stack;
	vkaa_syntax_s *restrict syntax;
	if ((stack = tparse_tstack_layer(context->stack, 0, NULL)))
	{
		if (context->data[context->pos] == stack->end_char)
		{
			syntax = stack->syntax;
			context->syntax = syntax;
			context->pos += 1;
			tparse_tstack_pop(context->stack);
			return p;
		}
	}
	return NULL;
}

static vkaa_syntaxor_parse_s* vkaa_syntaxor_parse_onlyparse_alloc(vkaa_syntaxor_parse_f parse)
{
	vkaa_syntaxor_parse_s *restrict r;
	if ((r = (vkaa_syntaxor_parse_s *) refer_alloz(sizeof(vkaa_syntaxor_parse_s))))
		r->parse = parse;
	return r;
}

static vkaa_syntaxor_parse_s* vkaa_syntaxor_parse_create_inside(void)
{
	return vkaa_syntaxor_parse_onlyparse_alloc(vkaa_syntaxor_parse_inside_parse_func);
}

static vkaa_syntaxor_parse_s* vkaa_syntaxor_parse_create_outside(void)
{
	return vkaa_syntaxor_parse_onlyparse_alloc(vkaa_syntaxor_parse_inside_outside_func);
}

// initial

static inline void vkaa_syntaxor_set_parse_status(vkaa_syntaxor_parse_s *status[], uint8_t index, vkaa_syntaxor_parse_s *restrict p)
{
	if (status[index]) refer_free(status[index]);
	status[index] = (vkaa_syntaxor_parse_s *) refer_save(p);
}

static tparse_tmapping_s* vkaa_syntaxor_initial_parse_once(vkaa_syntaxor_parse_s *status[], tparse_tmapping_s *restrict mapping, uint8_t c, vkaa_syntaxor_parse_s *restrict p)
{
	char c2[2];
	c2[0] = (char) c;
	c2[1] = 0;
	vkaa_syntaxor_set_parse_status(status, c, p);
	return mapping->add(mapping, c2, p);
}

static tparse_tmapping_s* vkaa_syntaxor_initial_parse_keys(vkaa_syntaxor_parse_s *status[], tparse_tmapping_s *restrict mapping, const char *restrict keys, vkaa_syntaxor_parse_s *restrict p)
{
	char c2[2];
	c2[1] = 0;
	while ((c2[0] = *keys))
	{
		vkaa_syntaxor_set_parse_status(status, (uint8_t) c2[0], p);
		if (!mapping->add(mapping, c2, p))
			goto label_fail;
		++keys;
	}
	return mapping;
	label_fail:
	return NULL;
}

static tparse_tmapping_s* vkaa_syntaxor_initial_parse_range(vkaa_syntaxor_parse_s *status[], tparse_tmapping_s *restrict mapping, uintptr_t c0, uintptr_t c1, vkaa_syntaxor_parse_s *restrict p)
{
	char c2[2];
	c2[1] = 0;
	while (c0 <= c1)
	{
		c2[0] = (char) c0;
		vkaa_syntaxor_set_parse_status(status, (uint8_t) c0, p);
		if (!mapping->add(mapping, c2, p))
			goto label_fail;
		++c0;
	}
	return mapping;
	label_fail:
	return NULL;
}

static vkaa_syntaxor_s* vkaa_syntaxor_initial(vkaa_syntaxor_s *restrict r)
{
	vkaa_syntaxor_parse_s *restrict p;
	// keyword
	if (!(p = vkaa_syntaxor_parse_create_keyword())) goto label_fail;
	if (!vkaa_syntaxor_initial_parse_range(r->status, r->mapping, 'A', 'Z', p)) goto label_fail;
	if (!vkaa_syntaxor_initial_parse_range(r->status, r->mapping, 'a', 'z', p)) goto label_fail;
	if (!vkaa_syntaxor_initial_parse_once(r->status, r->mapping, '_', p)) goto label_fail;
	if (!vkaa_syntaxor_initial_parse_range(r->status, r->mapping, 0x80, 0xff, p)) goto label_fail;
	refer_free(p);
	// operator
	if (!(p = vkaa_syntaxor_parse_create_operator())) goto label_fail;
	if (!vkaa_syntaxor_initial_parse_keys(r->status, r->mapping, "!#$%^&*+-./:<=>?@\\^`|~", p)) goto label_fail;
	refer_free(p);
	// number
	if (!(p = vkaa_syntaxor_parse_create_number())) goto label_fail;
	if (!vkaa_syntaxor_initial_parse_range(r->status, r->mapping, '0', '9', p)) goto label_fail;
	refer_free(p);
	// comma
	if (!(p = vkaa_syntaxor_parse_create_comma())) goto label_fail;
	if (!vkaa_syntaxor_initial_parse_once(r->status, r->mapping, ',', p)) goto label_fail;
	refer_free(p);
	// semicolon
	if (!(p = vkaa_syntaxor_parse_create_semicolon())) goto label_fail;
	if (!vkaa_syntaxor_initial_parse_once(r->status, r->mapping, ';', p)) goto label_fail;
	refer_free(p);
	// string
	if (!(p = vkaa_syntaxor_parse_create_string())) goto label_fail;
	if (!vkaa_syntaxor_initial_parse_once(r->status, r->mapping, '\"', p)) goto label_fail;
	refer_free(p);
	// multichar
	if (!(p = vkaa_syntaxor_parse_create_multichar())) goto label_fail;
	if (!vkaa_syntaxor_initial_parse_once(r->status, r->mapping, '\'', p)) goto label_fail;
	refer_free(p);
	// scope brackets square
	if (!(p = vkaa_syntaxor_parse_create_inside())) goto label_fail;
	if (!vkaa_syntaxor_initial_parse_keys(r->status, r->mapping, "([{", p)) goto label_fail;
	refer_free(p);
	if (!(p = vkaa_syntaxor_parse_create_outside())) goto label_fail;
	if (!vkaa_syntaxor_initial_parse_keys(r->status, r->mapping, ")]}", p)) goto label_fail;
	refer_free(p);
	return r;
	label_fail:
	if (p) refer_free(p);
	return NULL;
}

// syntaxor

static void vkaa_syntaxor_free_func(vkaa_syntaxor_s *restrict r)
{
	uintptr_t i, n;
	if (r->mapping) refer_free(r->mapping);
	if (r->stack) refer_free(r->stack);
	n = sizeof(r->status) / sizeof(*r->status);
	for (i = 0; i < n; ++i)
	{
		if (r->status[i])
			refer_free(r->status[i]);
	}
}

vkaa_syntaxor_s* vkaa_syntaxor_alloc(void)
{
	vkaa_syntaxor_s *restrict r;
	if ((r = (vkaa_syntaxor_s *) refer_alloz(sizeof(vkaa_syntaxor_s))))
	{
		refer_set_free(r, (refer_free_f) vkaa_syntaxor_free_func);
		if ((r->mapping = tparse_tmapping_alloc_single()) &&
			(r->stack = tparse_tstack_alloc()) &&
			vkaa_syntaxor_initial(r))
			return r;
		refer_free(r);
	}
	return NULL;
}

typedef struct vkaa_syntaxor_parse_comment_s {
	vkaa_syntaxor_parse_s p;
	vkaa_syntaxor_parse_s *next;
	refer_nstring_t start;
	refer_nstring_t stop;
} vkaa_syntaxor_parse_comment_s;

static vkaa_syntaxor_parse_s* vkaa_syntaxor_parse_comment_parse_func(vkaa_syntaxor_parse_comment_s *restrict p, vkaa_syntaxor_context_t *restrict context)
{
	const char *restrict s, *restrict t, *restrict u;
	uintptr_t n, m, l;
	s = context->data + context->pos;
	n = context->size - context->pos;
	if ((m = p->start->length) > n)
		goto label_not_hit;
	if (memcmp(s, p->start->string, m))
		goto label_not_hit;
	s += m;
	n -= m;
	t = p->stop->string;
	m = p->stop->length;
	while ((u = memchr(s, *t, n)))
	{
		l = (uintptr_t) u - (uintptr_t) s;
		s += l;
		n -= l;
		if (m > n) break;
		if (!memcmp(s, t, m))
		{
			n -= m;
			context->pos = context->size - n;
			return &p->p;
		}
		++s;
		--n;
	}
	context->pos = context->size;
	return &p->p;
	label_not_hit:
	if (!p->next || p->next->parse(p->next, context))
		return &p->p;
	return NULL;
}

static void vkaa_syntaxor_parse_comment_free_func(vkaa_syntaxor_parse_comment_s *restrict r)
{
	if (r->next) refer_free(r->next);
	if (r->start) refer_free(r->start);
	if (r->stop) refer_free(r->stop);
}

static vkaa_syntaxor_parse_s* vkaa_syntaxor_parse_comment_alloc(const char *restrict start, const char *restrict stop, vkaa_syntaxor_parse_s *restrict next)
{
	vkaa_syntaxor_parse_comment_s *restrict r;
	if (start && stop && *start && *stop &&
		(r = (vkaa_syntaxor_parse_comment_s *) refer_alloz(sizeof(vkaa_syntaxor_parse_comment_s))))
	{
		refer_set_free(r, (refer_free_f) vkaa_syntaxor_parse_comment_free_func);
		r->next = (vkaa_syntaxor_parse_s *) refer_save(next);
		if ((r->start = refer_dump_nstring(start)) &&
			(r->stop = refer_dump_nstring(stop)))
		{
			r->p.parse = (vkaa_syntaxor_parse_f) vkaa_syntaxor_parse_comment_parse_func;
			return &r->p;
		}
		refer_free(r);
	}
	return NULL;
}

vkaa_syntaxor_s* vkaa_syntaxor_add_comment(vkaa_syntaxor_s *restrict syntaxor, const char *restrict start, const char *restrict stop)
{
	vkaa_syntaxor_s *rr;
	vkaa_syntaxor_parse_s *restrict comment;
	char c2[2];
	rr = NULL;
	if (start && stop && *start && *stop)
	{
		if ((comment = vkaa_syntaxor_parse_comment_alloc(start, stop, syntaxor->status[*(uint8_t *) start])))
		{
			c2[0] = *start;
			c2[1] = 0;
			if (syntaxor->mapping->add(syntaxor->mapping, c2, comment))
			{
				vkaa_syntaxor_set_parse_status(syntaxor->status, *(uint8_t *) start, comment);
				rr = syntaxor;
			}
			refer_free(comment);
		}
	}
	return rr;
}

// source

static vkaa_syntax_source_s* vkaa_syntax_source_append_linepos(vkaa_syntax_source_s *restrict r, const char *restrict source, uintptr_t length)
{
	uintptr_t i;
	uint32_t *restrict linepos_array;
	for (i = 0; i < length; ++i)
	{
		if (source[i] == '\n')
		{
			if (!(linepos_array = exbuffer_need(&r->linepos_buffer, sizeof(uint32_t) * (r->linepos_number + 1))))
				goto label_fail;
			r->linepos_array = linepos_array;
			linepos_array[r->linepos_number++] = (uint32_t) (i + 1);
		}
	}
	return r;
	label_fail:
	return NULL;
}

static void vkaa_syntax_source_free_func(vkaa_syntax_source_s *restrict r)
{
	if (r->source) refer_free(r->source);
	if (r->name) refer_free(r->name);
	exbuffer_uini(&r->linepos_buffer);
}

vkaa_syntax_source_s* vkaa_syntax_source_alloc(refer_nstring_t source)
{
	vkaa_syntax_source_s *restrict r;
	if (source && (r = (vkaa_syntax_source_s *) refer_alloz(sizeof(vkaa_syntax_source_s))))
	{
		refer_set_free(r, (refer_free_f) vkaa_syntax_source_free_func);
		if (exbuffer_init(&r->linepos_buffer, 0) &&
			vkaa_syntax_source_append_linepos(r, source->string, source->length))
		{
			r->source = (refer_nstring_t) refer_save(source);
			return r;
		}
		refer_free(r);
	}
	return NULL;
}

vkaa_syntax_source_s* vkaa_syntax_source_alloc_by_data(const char *restrict source_data, uintptr_t source_length)
{
	refer_nstring_t source;
	vkaa_syntax_source_s *restrict r;
	if ((source = refer_dump_nstring_with_length(source_data, source_length)))
	{
		r = vkaa_syntax_source_alloc(source);
		refer_free(source);
		return r;
	}
	return NULL;
}

refer_string_t vkaa_syntax_source_refer_name(vkaa_syntax_source_s *restrict source, refer_string_t name)
{
	if (source->name)
		refer_free(source->name);
	if ((source->name = name))
		refer_save(name);
	return name;
}

refer_string_t vkaa_syntax_source_set_name(vkaa_syntax_source_s *restrict source, const char *restrict name)
{
	refer_string_t rs;
	if (!name) return vkaa_syntax_source_refer_name(source, NULL);
	else if ((rs = refer_dump_string(name)))
	{
		vkaa_syntax_source_refer_name(source, rs);
		refer_free(rs);
	}
	return rs;
}

uint32_t vkaa_syntax_source_linepos(const vkaa_syntax_source_s *restrict source, uint32_t pos, uint32_t *restrict cpos)
{
	const uint32_t *restrict p;
	uintptr_t i, t, n;
	uint32_t linepos;
	p = source->linepos_array;
	n = source->linepos_number;
	i = 0;
	while (n)
	{
		t = n >> 1;
		linepos = p[t];
		if (linepos < pos)
		{
			if (t)
			{
				i += t;
				p += t;
				n -= t;
			}
			else
			{
				i += 1;
				break;
			}
		}
		else if (linepos > pos)
			n = t;
		else
		{
			i += t + 1;
			break;
		}
	}
	if (cpos) *cpos = (i?(pos - source->linepos_array[i - 1]):pos) + 1;
	return (uint32_t) (i + 1);
}

// create syntax

const vkaa_syntax_s* vkaa_syntax_alloc(vkaa_syntaxor_s *restrict syntaxor, const vkaa_syntax_source_s *restrict source)
{
	vkaa_syntaxor_context_t c;
	vkaa_syntax_s *restrict syntax;
	tparse_tmapping_s *restrict mapping;
	vkaa_syntaxor_parse_s *restrict p;
	const char *restrict s;
	uintptr_t n;
	c.stack = syntaxor->stack;
	syntaxor->mapping->clear(syntaxor->mapping);
	tparse_tstack_clear(c.stack);
	if ((c.syntax = syntax = vkaa_syntax_alloc_empty()))
	{
		syntax->source = (const vkaa_syntax_source_s *) refer_save(source);
		c.data = s = source->source->string;
		c.size = n = source->source->length;
		c.pos = 0;
		mapping = syntaxor->mapping;
		while (c.pos < n)
		{
			if ((p = (vkaa_syntaxor_parse_s *) mapping->test(mapping, s[c.pos])))
			{
				if (!p->parse(p, &c))
					goto label_fail;
			}
			else ++c.pos;
		}
		if (!tparse_tstack_layer_number(c.stack))
			return syntax;
		label_fail:
		tparse_tstack_clear(c.stack);
		refer_free(syntax);
	}
	return NULL;
}

// syntax convert

#include <stdlib.h>

const vkaa_syntax_t* vkaa_syntax_convert_uintptr(const vkaa_syntax_t *restrict syntax, uintptr_t *restrict value, int base)
{
	char *endptr;
	uintptr_t v;
	if (syntax->type == vkaa_syntax_type_number)
	{
		v = (uintptr_t) strtoull(syntax->data.number->string, &endptr, base);
		if (endptr && !*endptr)
		{
			if (value) *value = v;
			return syntax;
		}
	}
	return NULL;
}

const vkaa_syntax_t* vkaa_syntax_convert_intptr(const vkaa_syntax_t *restrict syntax, intptr_t *restrict value, int base)
{
	char *endptr;
	intptr_t v;
	if (syntax->type == vkaa_syntax_type_number)
	{
		v = (intptr_t) strtoll(syntax->data.number->string, &endptr, base);
		if (endptr && !*endptr)
		{
			if (value) *value = v;
			return syntax;
		}
	}
	return NULL;
}

const vkaa_syntax_t* vkaa_syntax_convert_double(const vkaa_syntax_t *restrict syntax, double *restrict value)
{
	char *endptr;
	double v;
	if (syntax->type == vkaa_syntax_type_number)
	{
		v = strtod(syntax->data.number->string, &endptr);
		if (endptr && !*endptr)
		{
			if (value) *value = v;
			return syntax;
		}
	}
	return NULL;
}

const vkaa_syntax_t* vkaa_syntax_convert_float(const vkaa_syntax_t *restrict syntax, float *restrict value)
{
	char *endptr;
	float v;
	if (syntax->type == vkaa_syntax_type_number)
	{
		v = strtof(syntax->data.number->string, &endptr);
		if (endptr && !*endptr)
		{
			if (value) *value = v;
			return syntax;
		}
	}
	return NULL;
}
