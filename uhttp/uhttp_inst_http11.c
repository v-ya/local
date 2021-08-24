#include "uhttp_type.h"

uhttp_inst_s* uhttp_inst_alloc_http11(void)
{
	uhttp_inst_s *restrict r;
	if ((r = uhttp_inst_alloc_empty()))
	{
		if (
			// 1xx
			uhttp_inst_set_status(r, 100, "Continue") &&
			uhttp_inst_set_status(r, 101, "Switching Protocols") &&
			uhttp_inst_set_status(r, 102, "Processing") &&
			uhttp_inst_set_status(r, 103, "Early Hints") &&
			// 2xx
			uhttp_inst_set_status(r, 200, "OK") &&
			uhttp_inst_set_status(r, 201, "Created") &&
			uhttp_inst_set_status(r, 202, "Accepted") &&
			uhttp_inst_set_status(r, 203, "Non-Authoritative Information") &&
			uhttp_inst_set_status(r, 204, "No Content") &&
			uhttp_inst_set_status(r, 205, "Reset Content") &&
			uhttp_inst_set_status(r, 206, "Partial Content") &&
			uhttp_inst_set_status(r, 207, "Multi-Status") &&
			uhttp_inst_set_status(r, 208, "Already Reported") &&
			uhttp_inst_set_status(r, 226, "IM Used") &&
			// 3xx
			uhttp_inst_set_status(r, 300, "Multiple Choices") &&
			uhttp_inst_set_status(r, 301, "Moved Permanently") &&
			uhttp_inst_set_status(r, 302, "Found") &&
			uhttp_inst_set_status(r, 303, "See Other") &&
			uhttp_inst_set_status(r, 304, "Not Modified") &&
			uhttp_inst_set_status(r, 305, "Use Proxy") &&
			uhttp_inst_set_status(r, 306, "Switch Proxy") &&
			uhttp_inst_set_status(r, 307, "Temporary Redirect") &&
			uhttp_inst_set_status(r, 308, "Permanent Redirect") &&
			// 4xx
			uhttp_inst_set_status(r, 400, "Bad Request") &&
			uhttp_inst_set_status(r, 401, "Unauthorized") &&
			uhttp_inst_set_status(r, 402, "Payment Required") &&
			uhttp_inst_set_status(r, 403, "Forbidden") &&
			uhttp_inst_set_status(r, 404, "Not Found") &&
			uhttp_inst_set_status(r, 405, "Method Not Allowed") &&
			uhttp_inst_set_status(r, 406, "Not Acceptable") &&
			uhttp_inst_set_status(r, 407, "Proxy Authentication Required") &&
			uhttp_inst_set_status(r, 408, "Request Timeout") &&
			uhttp_inst_set_status(r, 409, "Conflict") &&
			uhttp_inst_set_status(r, 410, "Gone") &&
			uhttp_inst_set_status(r, 411, "Length Required") &&
			uhttp_inst_set_status(r, 412, "Precondition Failed") &&
			uhttp_inst_set_status(r, 413, "Request Entity Too Large") &&
			uhttp_inst_set_status(r, 414, "Request-URI Too Long") &&
			uhttp_inst_set_status(r, 415, "Unsupported Media Type") &&
			uhttp_inst_set_status(r, 416, "Requested Range Not Satisfiable") &&
			uhttp_inst_set_status(r, 417, "Expectation Failed") &&
			uhttp_inst_set_status(r, 418, "I'm a teapot") &&
			uhttp_inst_set_status(r, 421, "Misdirected Request") &&
			uhttp_inst_set_status(r, 422, "Unprocessable Entity") &&
			uhttp_inst_set_status(r, 423, "Locked") &&
			uhttp_inst_set_status(r, 424, "Failed Dependency") &&
			uhttp_inst_set_status(r, 425, "Too Early") &&
			uhttp_inst_set_status(r, 426, "Upgrade Required") &&
			uhttp_inst_set_status(r, 427, "Precondition Required") &&
			uhttp_inst_set_status(r, 428, "Precondition Required") &&
			uhttp_inst_set_status(r, 429, "Too Many Requests") &&
			uhttp_inst_set_status(r, 431, "Request Header Fields Too Large") &&
			uhttp_inst_set_status(r, 440, "Login Time-out") &&
			uhttp_inst_set_status(r, 451, "Unavailable For Legal Reasons") &&
			// 5xx
			uhttp_inst_set_status(r, 500, "Internal Server Error") &&
			uhttp_inst_set_status(r, 501, "Not Implemented") &&
			uhttp_inst_set_status(r, 502, "Bad Gateway") &&
			uhttp_inst_set_status(r, 503, "Service Unavailable") &&
			uhttp_inst_set_status(r, 504, "Gateway Timeout") &&
			uhttp_inst_set_status(r, 505, "HTTP Version Not Supported") &&
			uhttp_inst_set_status(r, 506, "Variant Also Negotiates") &&
			uhttp_inst_set_status(r, 507, "Insufficient Storage") &&
			uhttp_inst_set_status(r, 508, "Loop Detected") &&
			uhttp_inst_set_status(r, 510, "Not Extended") &&
			uhttp_inst_set_status(r, 511, "Network Authentication Required") &&
			// version
			uhttp_inst_set_version(r, "HTTP/1.1")
			) return r;
		refer_free(r);
	}
	return NULL;
}

uhttp_inst_s* uhttp_inst_alloc_http11_without_status(void)
{
	uhttp_inst_s *restrict r;
	if ((r = uhttp_inst_alloc_empty()))
	{
		if (uhttp_inst_set_version(r, "HTTP/1.1"))
			return r;
		refer_free(r);
	}
	return NULL;
}
