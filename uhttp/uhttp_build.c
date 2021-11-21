#include "uhttp_type.h"

static inline uintptr_t uhttp_build_code_length(intptr_t status_code)
{
	char buffer[64];
	return (uintptr_t) sprintf(buffer, "%" PRIdPTR, status_code);
}

static inline char* uhttp_build_code_write(char *restrict data, intptr_t status_code)
{
	return data + (uintptr_t) sprintf(data, "%" PRIdPTR, status_code);
}

uintptr_t uhttp_build_line_length(const uhttp_s *restrict uhttp)
{
	if (uhttp->request_method && uhttp->request_uri)
	{
		// request
		// [method] [uri] [version]\r\n
		return uhttp->request_method->length + uhttp->request_uri->length + uhttp->version->length + 4;
	}
	else if (uhttp->status_desc)
	{
		// response
		// [version] {code} [desc]\r\n
		return uhttp_build_code_length(uhttp->status_code) + uhttp->status_desc->length + uhttp->version->length + 4;
	}
	return 2;
}

char* uhttp_build_line_write(const uhttp_s *restrict uhttp, char *restrict data)
{
	uintptr_t n;
	if (uhttp->request_method && uhttp->request_uri)
	{
		// request
		// [method] [uri] [version]\r\n
		memcpy(data, uhttp->request_method->string, n = uhttp->request_method->length);
		data += n;
		*data++ = ' ';
		memcpy(data, uhttp->request_uri->string, n = uhttp->request_uri->length);
		data += n;
		*data++ = ' ';
		memcpy(data, uhttp->version->string, n = uhttp->version->length);
		data += n;
	}
	else if (uhttp->status_desc)
	{
		// response
		// [version] {code} [desc]\r\n
		memcpy(data, uhttp->version->string, n = uhttp->version->length);
		data += n;
		*data++ = ' ';
		data = uhttp_build_code_write(data, uhttp->status_code);
		*data++ = ' ';
		memcpy(data, uhttp->status_desc->string, n = uhttp->status_desc->length);
		data += n;
	}
	*data++ = '\r';
	*data++ = '\n';
	return data;
}

uintptr_t uhttp_build_header_length(const uhttp_s *restrict uhttp)
{
	const vattr_vlist_t *restrict v;
	uhttp_header_s *restrict header;
	uintptr_t n;
	n = 0;
	for (v = uhttp->header->vattr; v; v = v->vattr_next)
	{
		// [name]: [value]\r\n
		header = (uhttp_header_s *) v->value;
		n += header->name->length + header->value_length + 4;
	}
	// \r\n
	n += 2;
	return n;
}

char* uhttp_build_header_write(const uhttp_s *restrict uhttp, char *restrict data)
{
	const vattr_vlist_t *restrict v;
	uhttp_header_s *restrict header;
	uintptr_t n;
	n = 0;
	for (v = uhttp->header->vattr; v; v = v->vattr_next)
	{
		// [name]: [value]\r\n
		header = (uhttp_header_s *) v->value;
		if ((n = header->name->length))
			memcpy(data, header->name->string, n);
		data += n;
		*data++ = ':';
		*data++ = ' ';
		if ((n = header->value_length))
			memcpy(data, header->value, n);
		data += n;
		*data++ = '\r';
		*data++ = '\n';
	}
	// \r\n
	*data++ = '\r';
	*data++ = '\n';
	return data;
}

uintptr_t uhttp_build_length(const uhttp_s *restrict uhttp)
{
	return uhttp_build_line_length(uhttp) + uhttp_build_header_length(uhttp);
}

char* uhttp_build_write(const uhttp_s *restrict uhttp, char *restrict data)
{
	return uhttp_build_header_write(uhttp, uhttp_build_line_write(uhttp, data));
}

char* uhttp_build(const uhttp_s *restrict uhttp, exbuffer_t *restrict message, uintptr_t *restrict length)
{
	char *restrict p;
	uintptr_t n;
	n = uhttp_build_length(uhttp);
	if ((p = (char *) exbuffer_need(message, n + 1)))
		*uhttp_build_write(uhttp, p) = 0;
	else n = 0;
	message->used = n;
	if (length)
		*length = n;
	return p;
}
