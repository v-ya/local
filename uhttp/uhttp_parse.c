#include "uhttp_type.h"

static const uint8_t uhttp_parse_is_space[256] = {
	['\t'] = 1,
	['\n'] = 1,
	['\r'] = 1,
	[' '] = 1
};

static const char* uhttp_parse_get_line(const char *restrict p, uintptr_t n, uintptr_t *restrict end_length, uintptr_t *restrict line_length)
{
	uintptr_t pos;
	if ((pos = (uintptr_t) memchr(p, '\n', n)))
	{
		pos -= (uintptr_t) p;
		*line_length = pos + 1;
		if (pos && p[pos - 1] == '\r')
			--pos;
		*end_length = pos;
		return p;
	}
	*end_length = *line_length = n;
	return NULL;
}

static inline uintptr_t uhttp_parse_skip_nspace(const char *restrict p, uintptr_t pos, uintptr_t n)
{
	while (pos < n && !uhttp_parse_is_space[((const uint8_t *) p)[pos]])
		++pos;
	return pos;
}

static inline uintptr_t uhttp_parse_skip_space(const char *restrict p, uintptr_t pos, uintptr_t n)
{
	while (pos < n && uhttp_parse_is_space[((const uint8_t *) p)[pos]])
		++pos;
	return pos;
}

static inline uintptr_t uhttp_parse_redge_nspace(const char *restrict p, uintptr_t pos)
{
	while (pos && !uhttp_parse_is_space[((const uint8_t *) p)[pos - 1]])
		--pos;
	return pos;
}

static inline uintptr_t uhttp_parse_redge_space(const char *restrict p, uintptr_t pos)
{
	while (pos && uhttp_parse_is_space[((const uint8_t *) p)[pos - 1]])
		--pos;
	return pos;
}

static inline refer_nstring_t uhttp_parse_inner_check_version(refer_nstring_t version, const char *restrict p, uintptr_t n)
{
	if (version->length == n && !memcmp(version->string, p, n))
		return version;
	return NULL;
}

static uhttp_parse_stuts_t uhttp_parse_inner_line(uhttp_s *restrict uhttp, const char *restrict p, uintptr_t n)
{
	refer_nstring_t s1, s2;
	uintptr_t pos1, pos2;
	s1 = s2 = NULL;
	if (!n) goto label_error_parse;
	// check first key
	// [?]...
	pos1 = uhttp_parse_skip_nspace(p, 0, n);
	if (!pos1 || pos1 >= n)
		goto label_error_parse;
	// check version
	if (uhttp_parse_inner_check_version(uhttp->version, p, pos1))
	{
		char buffer[64];
		int code;
		// response
		// p[version]1 {code} [desc]n
		pos1 = uhttp_parse_skip_space(p, pos1, n);
		// p[version] 1{code} [desc]n
		pos2 = uhttp_parse_skip_nspace(p, pos1, n);
		// p[version] 1{code}2 [desc]n
		pos2 -= pos1;
		if (pos2 >= sizeof(buffer))
			goto label_error_parse;
		memcpy(buffer, p + pos1, pos2);
		buffer[pos2] = 0;
		code = (int) strtol(buffer, NULL, 10);
		pos2 = uhttp_parse_skip_space(p, pos1 + pos2, n);
		// p[version] 1{code} 2[desc]n
		// dump [desc]
		s1 = refer_dump_nstring_with_length(p + pos2, n - pos2);
		if (!s1) goto label_error_inner;
		// set response
		uhttp_refer_response(uhttp, code, s1);
	}
	else
	{
		// request
		// p[method]1 [uri] [version]n
		pos2 = uhttp_parse_redge_nspace(p, n);
		// p[method]1 [uri] 2[version]n
		// check version ...
		if (!uhttp_parse_inner_check_version(uhttp->version, p + pos2, n - pos2))
			goto label_error_parse;
		// check version okay
		// dump [method]
		s1 = refer_dump_nstring_with_length(p, pos1);
		if (!s1) goto label_error_inner;
		// pos [uri]
		pos1 = uhttp_parse_skip_space(p, pos1, n);
		// p[method] 1[uri] 2[version]n
		pos2 = uhttp_parse_redge_space(p, pos2);
		// p[method] 1[uri]2 [version]n
		// dump [uri]
		if (pos1 >= pos2)
			goto label_error_parse;
		s2 = refer_dump_nstring_with_length(p + pos1, pos2 - pos1);
		if (!s2) goto label_error_inner;
		// set request
		uhttp_refer_request(uhttp, s1, s2);
	}
	// clear
	refer_free(s1);
	if (s2) refer_free(s2);
	return uhttp_parse_stuts_okay;
	label_error_parse:
	if (s1) refer_free(s1);
	if (s2) refer_free(s2);
	return uhttp_parse_stuts_error_parse;
	label_error_inner:
	if (s1) refer_free(s1);
	if (s2) refer_free(s2);
	return uhttp_parse_stuts_error_inner;
}

static uhttp_parse_stuts_t uhttp_parse_inner_header(uhttp_s *restrict uhttp, const char *restrict p, uintptr_t n)
{
	refer_nstring_t name;
	uhttp_header_s *restrict header;
	uintptr_t pos1, pos2;
	name = NULL;
	header = NULL;
	if (!n) goto label_error_parse;
	// p[name] : [value]n
	pos1 = (uintptr_t) memchr(p, ':', n);
	if (!pos1) goto label_error_parse;
	pos1 -= (uintptr_t) p;
	// p[name] 1: [value]n
	pos2 = pos1 + 1;
	// p[name] 1:2 [value]n
	pos1 = uhttp_parse_redge_space(p, pos1);
	// p[name]1 :2 [value]n
	pos2 = uhttp_parse_skip_space(p, pos2, n);
	// p[name]1 : 2[value]n
	if (!pos1) goto label_error_parse;
	// dump [name]
	name = refer_dump_nstring_with_length(p, pos1);
	if (!name) goto label_error_inner;
	// create header
	header = uhttp_header_refer_with_length(name, p + pos2, n - pos2);
	if (!header) goto label_error_inner;
	// insert header
	if (!uhttp_refer_header_tail(uhttp, header))
		goto label_error_inner;
	// clear
	refer_free(name);
	refer_free(header);
	return uhttp_parse_stuts_okay;
	label_error_parse:
	if (name) refer_free(name);
	if (header) refer_free(header);
	return uhttp_parse_stuts_error_parse;
	label_error_inner:
	if (name) refer_free(name);
	if (header) refer_free(header);
	return uhttp_parse_stuts_error_inner;
}

uhttp_parse_stuts_t uhttp_parse_line(uhttp_s *restrict uhttp, const char *restrict p, uintptr_t n, uintptr_t *restrict pos)
{
	uintptr_t n_end, n_line;
	p = uhttp_parse_get_line(p, n, &n_end, &n_line);
	if (pos) *pos = n_line;
	if (p) return uhttp_parse_inner_line(uhttp, p, n_end);
	return uhttp_parse_stuts_error_parse;
}

uhttp_parse_stuts_t uhttp_parse_header(uhttp_s *restrict uhttp, const char *restrict p, uintptr_t n, uintptr_t *restrict pos)
{
	uintptr_t n_end, n_line, n_pos;
	uhttp_parse_stuts_t r;
	n_pos = 0;
	while (uhttp_parse_get_line(p + n_pos, n, &n_end, &n_line))
	{
		if (!n_end)
			return uhttp_parse_stuts_okay;
		if ((r = uhttp_parse_inner_header(uhttp, p, n_end)) != uhttp_parse_stuts_okay)
			return r;
		n_pos += n_line;
	}
	if (pos) *pos = n_pos;
	return uhttp_parse_stuts_error_parse;
}

uhttp_parse_stuts_t uhttp_parse(uhttp_s *restrict uhttp, const char *restrict p, uintptr_t n, uintptr_t *restrict pos)
{
	uintptr_t np;
	uhttp_parse_stuts_t r;
	r = uhttp_parse_line(uhttp, p, n, &np);
	if (r == uhttp_parse_stuts_okay)
		return uhttp_parse_header(uhttp, p + np, n - np, pos);
	if (pos) *pos = np;
	return r;
}

void uhttp_parse_context_init(uhttp_parse_context_t *restrict context, uintptr_t pos)
{
	context->data = NULL;
	context->size = 0;
	context->pos = pos;
	context->pos_check = pos;
	context->step = uhttp_parse_stuts_wait_line;
}

void uhttp_parse_context_set(uhttp_parse_context_t *restrict context, const void *restrict data, uintptr_t length)
{
	context->data = (const char *) data;
	context->size = length;
}

static inline const char* uhttp_parse_context_get_line(uhttp_parse_context_t *restrict context, uintptr_t *restrict length)
{
	if (context->size > context->pos_check)
	{
		const char *restrict r;
		uintptr_t end_length, line_length;
		if (uhttp_parse_get_line((r = context->data) + context->pos_check, context->size - context->pos_check, &end_length, &line_length))
		{
			r += context->pos;
			*length = (context->pos_check - context->pos) + end_length;
			context->pos_check += line_length;
			context->pos = context->pos_check;
			return r;
		}
	}
	return NULL;
}

uhttp_parse_stuts_t uhttp_parse_context_try(uhttp_s *restrict uhttp, uhttp_parse_context_t *restrict context)
{
	const char *restrict p;
	uintptr_t n;
	uhttp_parse_stuts_t r;
	switch (context->step)
	{
		case uhttp_parse_stuts_wait_line:
			if (!(p = uhttp_parse_context_get_line(context, &n)))
				return uhttp_parse_stuts_wait_line;
			r = uhttp_parse_inner_line(uhttp, p, n);
			if (r < 0) goto label_fail_must;
			context->step = uhttp_parse_stuts_wait_header;
			// fall through
		case uhttp_parse_stuts_wait_header:
			while ((p = uhttp_parse_context_get_line(context, &n)))
			{
				if (!n) goto label_okay;
				r = uhttp_parse_inner_header(uhttp, p, n);
				if (r < 0) goto label_fail_maybe;
			}
			return uhttp_parse_stuts_wait_header;
		default:
			return context->step;
	}
	label_fail_must:
	context->step = r;
	label_fail_maybe:
	return r;
	label_okay:
	return (context->step = uhttp_parse_stuts_okay);
}
