#include "../vkaa.syntax.h"
#include <tparse/tmapping.h>
#include <tparse/tstack.h>
#include <tparse/tstring.h>
#include <tparse/tword.h>

typedef struct vkaa_syntaxor_context_t vkaa_syntaxor_context_t;
typedef struct vkaa_syntaxor_parse_s vkaa_syntaxor_parse_s;

typedef vkaa_syntaxor_parse_s* (*vkaa_syntaxor_parse_f)(vkaa_syntaxor_parse_s *restrict p, vkaa_syntaxor_context_t *restrict context);

struct vkaa_syntaxor_s {
	tparse_tmapping_s *mapping;
	tparse_tstack_s *stack;
};

struct vkaa_syntaxor_context_t {
	vkaa_syntax_s *syntax;
	const char *data;
	uintptr_t size;
	uintptr_t pos;
};

struct vkaa_syntaxor_parse_s {
	vkaa_syntaxor_parse_f parse;
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
	if (r->source) refer_free(r->source);
	exbuffer_uini(&r->buffer);
}

static vkaa_syntax_s* vkaa_syntax_alloc_empty(refer_nstring_t source, uintptr_t offset, uintptr_t length)
{
	vkaa_syntax_s *restrict r;
	if (source && (r = (vkaa_syntax_s *) refer_alloz(sizeof(vkaa_syntax_s))))
	{
		if (exbuffer_init(&r->buffer, 0))
		{
			refer_set_free(r, (refer_free_f) vkaa_syntax_free_func);
			r->source = (refer_nstring_t) refer_save(source);
			r->source_offset = offset;
			r->source_length = length;
			return r;
		}
		refer_free(r);
	}
	return NULL;
}

static vkaa_syntax_s* vkaa_syntax_push(vkaa_syntax_s *restrict r, vkaa_syntax_type_t type, refer_t data)
{
	vkaa_syntax_t *restrict s;
	uintptr_t index;
	if ((s = exbuffer_need(&r->buffer, ((index = r->syntax_number) + 1) * sizeof(vkaa_syntax_t))))
	{
		s[index].type = type;
		s[index].data.data = data;
		r->syntax_array = s;
		r->syntax_number = index + 1;
	}
	return NULL;
}

static vkaa_syntax_s* vkaa_syntax_push_nstring(vkaa_syntax_s *restrict r, vkaa_syntax_type_t type, const char *restrict data, uintptr_t length)
{
	refer_nstring_t ns;
	if ((ns = refer_dump_nstring_with_length(data, length)))
	{
		if (vkaa_syntax_push(r, type, ns))
			return r;
		refer_free(ns);
	}
	return NULL;
}

static vkaa_syntax_s* vkaa_syntax_push_syntax(vkaa_syntax_s *restrict r, vkaa_syntax_type_t type, refer_nstring_t source, uintptr_t offset, uintptr_t length)
{
	vkaa_syntax_s *restrict syntax;
	if ((syntax = vkaa_syntax_alloc_empty(source, offset, length)))
	{
		if (vkaa_syntax_push(r, type, syntax))
			return r;
		refer_free(syntax);
	}
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
		if (vkaa_syntax_push_nstring(context->syntax, p->type, context->data + p0, p1 - p0))
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

static vkaa_syntaxor_parse_s* vkaa_syntaxor_parse_create_number(void)
{
	static const uint8_t s_edge[] = ".BbEeXx";
	vkaa_syntaxor_parse_match_s *restrict r;
	if ((r = vkaa_syntaxor_parse_match_alloc_empty(vkaa_syntax_type_number)))
	{
		tparse_tword_edge_s *restrict e;
		uintptr_t i, n;
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
	if (vkaa_syntax_push(context->syntax, p->type, NULL))
	{
		if (context->pos < context->size)
			context->pos += 1;
		return &p->p;
	}
	return NULL;
}

static vkaa_syntaxor_parse_single_s* vkaa_syntaxor_parse_single_alloc_empty(vkaa_syntax_type_t type)
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
	return &vkaa_syntaxor_parse_single_alloc_empty(vkaa_syntax_type_comma)->p;
}

static vkaa_syntaxor_parse_s* vkaa_syntaxor_parse_create_semicolon(void)
{
	return &vkaa_syntaxor_parse_single_alloc_empty(vkaa_syntax_type_semicolon)->p;
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
			vkaa_syntax_push_nstring(context->syntax, p->type, data, length))
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

static vkaa_syntaxor_parse_string_s* vkaa_syntaxor_parse_string_alloc_empty(vkaa_syntax_type_t type, tparse_tstring_s* (*ts_alloc_func)(void))
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
	return &vkaa_syntaxor_parse_string_alloc_empty(vkaa_syntax_type_string, tparse_tstring_alloc_c_parse_multi_quotes)->p;
}

static vkaa_syntaxor_parse_s* vkaa_syntaxor_parse_create_multichar(void)
{
	return &vkaa_syntaxor_parse_string_alloc_empty(vkaa_syntax_type_multichar, tparse_tstring_alloc_c_parse_single_quotes)->p;
}

// parse (scope brackets square)

typedef struct vkaa_syntaxor_parse_inside_s {
	vkaa_syntaxor_parse_s p;
	tparse_tstack_s *stack;
} vkaa_syntaxor_parse_inside_s;

typedef struct vkaa_syntaxor_parse_outside_s {
	vkaa_syntaxor_parse_s p;
	tparse_tstack_s *stack;
} vkaa_syntaxor_parse_outside_s;

// initial

static tparse_tmapping_s* vkaa_syntaxor_initial_parse_once(tparse_tmapping_s *restrict mapping, uint8_t c, vkaa_syntaxor_parse_s *restrict p)
{
	char c2[2];
	c2[0] = (char) c;
	c2[1] = 0;
	return mapping->add(mapping, c2, p);
}

static tparse_tmapping_s* vkaa_syntaxor_initial_parse_keys(tparse_tmapping_s *restrict mapping, const char *restrict keys, vkaa_syntaxor_parse_s *restrict p)
{
	char c2[2];
	c2[1] = 0;
	while ((c2[0] = *keys))
	{
		if (!mapping->add(mapping, c2, p))
			goto label_fail;
	}
	return mapping;
	label_fail:
	return NULL;
}

static tparse_tmapping_s* vkaa_syntaxor_initial_parse_range(tparse_tmapping_s *restrict mapping, uintptr_t c0, uintptr_t c1, vkaa_syntaxor_parse_s *restrict p)
{
	char c2[2];
	c2[1] = 0;
	while (c0 <= c1)
	{
		c2[0] = (char) c0;
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
	if (!vkaa_syntaxor_initial_parse_range(r->mapping, 'A', 'Z', p)) goto label_fail;
	if (!vkaa_syntaxor_initial_parse_range(r->mapping, 'a', 'z', p)) goto label_fail;
	if (!vkaa_syntaxor_initial_parse_once(r->mapping, '_', p)) goto label_fail;
	if (!vkaa_syntaxor_initial_parse_range(r->mapping, 0x80, 0xff, p)) goto label_fail;
	refer_free(p);
	// operator
	if (!(p = vkaa_syntaxor_parse_create_operator())) goto label_fail;
	if (!vkaa_syntaxor_initial_parse_keys(r->mapping, "!#$%^&*+-./:<=>?@\\^`|~", p)) goto label_fail;
	refer_free(p);
	// number
	if (!(p = vkaa_syntaxor_parse_create_number())) goto label_fail;
	if (!vkaa_syntaxor_initial_parse_range(r->mapping, '0', '9', p)) goto label_fail;
	refer_free(p);
	// comma
	if (!(p = vkaa_syntaxor_parse_create_comma())) goto label_fail;
	if (!vkaa_syntaxor_initial_parse_once(r->mapping, ',', p)) goto label_fail;
	refer_free(p);
	// semicolon
	if (!(p = vkaa_syntaxor_parse_create_semicolon())) goto label_fail;
	if (!vkaa_syntaxor_initial_parse_once(r->mapping, ';', p)) goto label_fail;
	refer_free(p);
	// string
	if (!(p = vkaa_syntaxor_parse_create_string())) goto label_fail;
	if (!vkaa_syntaxor_initial_parse_once(r->mapping, '\"', p)) goto label_fail;
	refer_free(p);
	// multichar
	if (!(p = vkaa_syntaxor_parse_create_multichar())) goto label_fail;
	if (!vkaa_syntaxor_initial_parse_once(r->mapping, '\'', p)) goto label_fail;
	refer_free(p);
	// scope brackets square
	return r;
	label_fail:
	if (p) refer_free(p);
	return NULL;
}

static void vkaa_syntaxor_free_func(vkaa_syntaxor_s *restrict r)
{
	if (r->mapping) refer_free(r->mapping);
	if (r->stack) refer_free(r->stack);
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

// create syntax

vkaa_syntax_s* vkaa_syntax_alloc(vkaa_syntaxor_s *restrict syntaxor, refer_nstring_t source)
{
	vkaa_syntaxor_context_t c;
	tparse_tmapping_s *restrict mapping;
	vkaa_syntaxor_parse_s *restrict p;
	const char *restrict data;
	uintptr_t size;
	syntaxor->mapping->clear(syntaxor->mapping);
	tparse_tstack_clear(syntaxor->stack);
	if ((c.syntax = vkaa_syntax_alloc_empty(source, 0, source->length)))
	{
		c.data = data = source->string;
		c.size = size = source->length;
		c.pos = 0;
		mapping = syntaxor->mapping;
		while (c.pos < size)
		{
			if ((p = (vkaa_syntaxor_parse_s *) mapping->test(mapping, data[c.pos])))
			{
				if (!p->parse(p, &c))
					goto label_fail;
			}
			else ++c.pos;
		}
		if (c.syntax)
			return c.syntax;
	}
	label_fail:
	tparse_tstack_clear(syntaxor->stack);
	return NULL;
}
