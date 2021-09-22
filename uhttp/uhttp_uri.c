#include "uhttp_type.h"

typedef struct uhttp_uri_param_s {
	uintptr_t key_length;
	uintptr_t kdata_length;
	uintptr_t vdata_length;
	uintptr_t value_length;
	const char* key;
	const char* kdata;
	const char* vdata;
	const char* value;
} uhttp_uri_param_s;

static const uint8_t uhttp_uri_param_value_keep[256] = {
	['-'] = 1, ['.'] = 1,
	['0' ... '9'] = 1,
	['A' ... 'Z'] = 1,
	['_'] = 1,
	['a' ... 'z'] = 1,
	['~'] = 1
};

static const uint8_t uhttp_uri_param_vdata_ishex[256] = {
	['0' ... '9'] = 1,
	['A' ... 'F'] = 1,
	['a' ... 'f'] = 1
};

uintptr_t uhttp_uri_param_length_value2vdata(const char *restrict value, uintptr_t value_length)
{
	uintptr_t i, nk;
	for (nk = i = 0; i < value_length; ++i)
		nk += !uhttp_uri_param_value_keep[((const uint8_t *) value)[i]];
	return value_length + (nk << 1);
}

uintptr_t uhttp_uri_param_length_vdata2value(const char *restrict vdata, uintptr_t vdata_length)
{
	uintptr_t i, n;
	n = vdata_length;
	for (i = 0; i < vdata_length; ++i)
	{
		if (vdata[i] != '%')
			continue;
		if (i + 3 <= vdata_length &&
			uhttp_uri_param_vdata_ishex[((const uint8_t *) vdata)[i + 1]] &&
			uhttp_uri_param_vdata_ishex[((const uint8_t *) vdata)[i + 2]])
		{
			n -= 2;
			i += 2;
		}
	}
	return n;
}

char* uhttp_uri_param_write_value2vdata(char *restrict vdata, const char *restrict value, uintptr_t value_length)
{
	static const char hex2c[16] = "0123456789abcdef";
	uintptr_t i;
	uint8_t c;
	for (i = 0; i < value_length; ++i)
	{
		if (uhttp_uri_param_value_keep[c = ((const uint8_t *) value)[i]])
			*vdata++ = (char) c;
		else
		{
			*vdata++ = '%';
			*vdata++ = hex2c[c >> 4];
			*vdata++ = hex2c[c & 15];
		}
	}
	return vdata;
}

char* uhttp_uri_param_write_vdata2value(char *restrict value, const char *restrict vdata, uintptr_t vdata_length)
{
	static const uint8_t c2hex[256] = {
		['0'] =  0, ['1'] =  1, ['2'] =  2, ['3'] =  3, ['4'] =  4,
		['5'] =  5, ['6'] =  6, ['7'] =  7, ['8'] =  8, ['9'] =  9,
		['A'] = 10, ['B'] = 11, ['C'] = 12, ['D'] = 13, ['E'] = 14, ['F'] = 15,
		['a'] = 10, ['b'] = 11, ['c'] = 12, ['d'] = 13, ['e'] = 14, ['f'] = 15
	};
	uintptr_t i;
	char c;
	for (i = 0; i < vdata_length; ++i)
	{
		if ((c = vdata[i]) != '%')
		{
			label_normal:
			*value++ = c;
		}
		else if (i + 3 > vdata_length)
			goto label_normal;
		else if (!uhttp_uri_param_vdata_ishex[((const uint8_t *) vdata)[i + 1]])
			goto label_normal;
		else if (!uhttp_uri_param_vdata_ishex[((const uint8_t *) vdata)[i + 2]])
			goto label_normal;
		else
		{
			*value++ = (char) ((c2hex[((const uint8_t *) vdata)[i + 1]] << 4) |
						c2hex[((const uint8_t *) vdata)[i + 2]]);
			i += 2;
		}
	}
	return value;
}

static uhttp_uri_param_s* uhttp_uri_param_alloc_by_value(const char *restrict key, uintptr_t key_length, const char *restrict value, uintptr_t value_length)
{
	uhttp_uri_param_s *restrict r;
	char *p;
	uintptr_t n_kd, n_vd;
	n_kd = uhttp_uri_param_length_value2vdata(key, key_length);
	n_vd = uhttp_uri_param_length_value2vdata(value, value_length);
	if ((r = (uhttp_uri_param_s *) refer_alloc(sizeof(uhttp_uri_param_s) + 4 + key_length + n_kd + n_vd + value_length)))
	{
		p = (char *) (r + 1);
		r->key_length = key_length;
		r->kdata_length = n_kd;
		r->vdata_length = n_vd;
		r->value_length = value_length;
		// key
		r->key = p;
		if (key_length)
			memcpy(p, key, key_length);
		p += key_length;
		*p++ = 0;
		// kdata
		r->kdata = p;
		if (n_kd) p = uhttp_uri_param_write_value2vdata(p, key, key_length);
		*p++ = 0;
		// vdata
		r->vdata = p;
		if (n_vd) p = uhttp_uri_param_write_value2vdata(p, value, value_length);
		*p++ = 0;
		// value
		r->value = p;
		if (value_length)
			memcpy(p, value, value_length);
		*(p += value_length) = 0;
	}
	return r;
}

static uhttp_uri_param_s* uhttp_uri_param_alloc_by_vdata(const char *restrict kdata, uintptr_t kdata_length, const char *restrict vdata, uintptr_t vdata_length)
{
	uhttp_uri_param_s *restrict r;
	char *p;
	uintptr_t n_ky, n_va;
	n_ky = uhttp_uri_param_length_vdata2value(kdata, kdata_length);
	n_va = uhttp_uri_param_length_vdata2value(vdata, vdata_length);
	if ((r = (uhttp_uri_param_s *) refer_alloc(sizeof(uhttp_uri_param_s) + 4 + n_ky + kdata_length + vdata_length + n_va)))
	{
		p = (char *) (r + 1);
		r->key_length = n_ky;
		r->kdata_length = kdata_length;
		r->vdata_length = vdata_length;
		r->value_length = n_va;
		// key
		r->key = p;
		if (n_ky) p = uhttp_uri_param_write_vdata2value(p, kdata, kdata_length);
		*p++ = 0;
		// kdata
		r->kdata = p;
		if (kdata_length)
			memcpy(p, kdata, kdata_length);
		p += kdata_length;
		*p++ = 0;
		// vdata
		r->vdata = p;
		if (vdata_length)
			memcpy(p, vdata, vdata_length);
		p += vdata_length;
		*p++ = 0;
		// value
		r->value = p;
		if (n_va) p = uhttp_uri_param_write_vdata2value(p, vdata, vdata_length);
		*p = 0;
	}
	return r;
}

static vattr_s* uhttp_uri_parse_param(const char *restrict uri, uintptr_t uri_length, vattr_s *restrict param)
{
	uhttp_uri_param_s *restrict vp;
	const char *pn, *pv;
	uintptr_t n, nk;
	while (uri_length)
	{
		pn = (const char *) memchr(uri, '&', uri_length);
		if (pn) n = (uintptr_t) pn - (uintptr_t) uri;
		else n = uri_length;
		pv = (const char *) memchr(uri, '=', n);
		if (pv) nk = (uintptr_t) pv - (uintptr_t) uri;
		else nk = n;
		vp = uhttp_uri_param_alloc_by_vdata(uri, nk, pv?(uri + nk + 1):NULL, pv?(n - nk - 1):0);
		if (!vp) goto label_fail;
		if (!vattr_insert_tail(param, vp->key, vp))
			goto label_fail;
		refer_free(vp);
		vp = NULL;
		if (pn) ++n;
		uri += n;
		uri_length -= n;
	}
	return param;
	label_fail:
	if (vp) refer_free(vp);
	return NULL;
}

static refer_nstring_t uhttp_uri_parse(refer_nstring_t uri, refer_nstring_t *restrict path, vattr_s *restrict param)
{
	const char *restrict p;
	uintptr_t n;
	*path = NULL;
	p = (const char *) memchr(uri->string, '?', uri->length);
	if (p)
	{
		n = (uintptr_t) p - (uintptr_t) uri->string;
		if (!(*path = refer_dump_nstring_with_length(uri->string, n)))
			goto label_fail;
		++p;
		n = uri->length - n - 1;
		if (!uhttp_uri_parse_param(p, n, param))
			goto label_fail;
	}
	else *path = (refer_nstring_t) refer_save(uri);
	return uri;
	label_fail:
	return NULL;
}

static refer_nstring_t uhttp_uri_build(refer_nstring_t path, const vattr_s *restrict param)
{
	struct refer_nstring_s *restrict r;
	const vattr_vlist_t *restrict vl;
	const uhttp_uri_param_s *restrict vp;
	char *restrict p;
	uintptr_t length;
	length = 0;
	if (path) length = path->length;
	for (vl = param->vattr; vl; vl = vl->vattr_next)
	{
		vp = (const uhttp_uri_param_s *) vl->value;
		length += vp->kdata_length + vp->vdata_length + 2;
	}
	if (!path && param->vattr)
		--length;
	r = (struct refer_nstring_s *) refer_alloc(sizeof(struct refer_nstring_s) + 1 + length);
	if (r)
	{
		r->length = length;
		p = r->string;
		if (path && path->length)
		{
			memcpy(p, path->string, path->length);
			p += path->length;
		}
		if ((vl = param->vattr))
		{
			if (path) *p++ = '?';
			goto label_build_param_entry;
			do {
				*p++ = '&';
				label_build_param_entry:
				vp = (const uhttp_uri_param_s *) vl->value;
				if (vp->kdata_length)
				{
					memcpy(p, vp->kdata, vp->kdata_length);
					p += vp->kdata_length;
				}
				*p++ = '=';
				if (vp->vdata_length)
				{
					memcpy(p, vp->vdata, vp->vdata_length);
					p += vp->vdata_length;
				}
			} while ((vl = vl->vattr_next));
		}
		*p = 0;
	}
	return r;
}

static void uhttp_uri_free_func(uhttp_uri_s *restrict r)
{
	if (r->uri_build)
		refer_free(r->uri_build);
	if (r->uri_path)
		refer_free(r->uri_path);
	if (r->param)
		refer_free(r->param);
}

uhttp_uri_s* uhttp_uri_alloc(void)
{
	uhttp_uri_s *restrict r;
	r = (uhttp_uri_s *) refer_alloz(sizeof(uhttp_uri_s));
	if (r)
	{
		refer_set_free(r, (refer_free_f) uhttp_uri_free_func);
		if ((r->param = vattr_alloc()))
			return r;
	}
	return NULL;
}

void uhttp_uri_clear(uhttp_uri_s *restrict uri)
{
	if (uri->uri_build)
	{
		refer_free(uri->uri_build);
		uri->uri_build = NULL;
	}
	if (uri->uri_path)
	{
		refer_free(uri->uri_path);
		uri->uri_path = NULL;
	}
	if (uri->param)
		vattr_clear(uri->param);
}

#define d_set(_name)  \
	refer_nstring_t uhttp_uri_set_##_name(uhttp_uri_s *restrict uri, const char *restrict value)\
	{\
		refer_nstring_t r, v;\
		v = NULL;\
		if (value) v = refer_dump_nstring(value);\
		r = uhttp_uri_refer_##_name(uri, v);\
		if (v) refer_free(v);\
		return r;\
	}

refer_nstring_t uhttp_uri_refer_uri(uhttp_uri_s *restrict uri, refer_nstring_t value)
{
	if (value)
	{
		uhttp_uri_clear(uri);
		if (uhttp_uri_parse(value, &uri->uri_path, uri->param))
			return (uri->uri_build = (refer_nstring_t) refer_save(value));
	}
	uhttp_uri_clear(uri);
	return NULL;
}

d_set(uri)

#define uhttp_uri_clear_build(_uri)  if (_uri->uri_build) refer_free(_uri->uri_build), _uri->uri_build = NULL

refer_nstring_t uhttp_uri_refer_path(uhttp_uri_s *restrict uri, refer_nstring_t value)
{
	uhttp_uri_clear_build(uri);
	if (uri->uri_path)
		refer_free(uri->uri_path);
	return (uri->uri_path = (refer_nstring_t) refer_save(value));
}

d_set(path)

#undef d_set

uhttp_uri_s* uhttp_uri_parse_data(uhttp_uri_s *restrict uri, const char *restrict param_data, uintptr_t param_size)
{
	uhttp_uri_clear_build(uri);
	if (uhttp_uri_parse_param(param_data, param_size, uri->param))
		return uri;
	return NULL;
}

uhttp_uri_s* uhttp_uri_set_param(uhttp_uri_s *restrict uri, const char *restrict key, const char *restrict value)
{
	uhttp_uri_s *r;
	uhttp_uri_param_s *restrict p;
	uhttp_uri_clear_build(uri);
	r = NULL;
	p = uhttp_uri_param_alloc_by_value(key, strlen(key), value, strlen(value));
	if (p)
	{
		if (vattr_set(uri->param, p->key, p))
			r = uri;
		refer_free(p);
	}
	return r;
}

uhttp_uri_s* uhttp_uri_insert_param(uhttp_uri_s *restrict uri, const char *restrict key, const char *restrict value)
{
	uhttp_uri_s *r;
	uhttp_uri_param_s *restrict p;
	uhttp_uri_clear_build(uri);
	r = NULL;
	p = uhttp_uri_param_alloc_by_value(key, strlen(key), value, strlen(value));
	if (p)
	{
		if (vattr_insert_tail(uri->param, p->key, p))
			r = uri;
		refer_free(p);
	}
	return r;
}

void uhttp_uri_delete_param(uhttp_uri_s *restrict uri, const char *restrict key)
{
	uhttp_uri_clear_build(uri);
	vattr_delete(uri->param, key);
}

void uhttp_uri_delete_param_index(uhttp_uri_s *restrict uri, const char *restrict key, uintptr_t index)
{
	uhttp_uri_clear_build(uri);
	vattr_delete_index(uri->param, key, index);
}

#undef uhttp_uri_clear_build

refer_nstring_t uhttp_uri_get_uri(uhttp_uri_s *restrict uri)
{
	if (uri->uri_build)
		return uri->uri_build;
	return (uri->uri_build = uhttp_uri_build(uri->uri_path, uri->param));
}

refer_nstring_t uhttp_uri_get_path(const uhttp_uri_s *restrict uri)
{
	return uri->uri_path;
}

const char* uhttp_uri_get_param(const uhttp_uri_s *restrict uri, const char *restrict key, uintptr_t *restrict value_length)
{
	const uhttp_uri_param_s *restrict p;
	if ((p = (const uhttp_uri_param_s *) vattr_get_first(uri->param, key)))
	{
		if (value_length)
			*value_length = p->value_length;
		return p->value;
	}
	if (value_length)
		*value_length = 0;
	return NULL;
}

const char* uhttp_uri_get_param_index(const uhttp_uri_s *restrict uri, const char *restrict key, uintptr_t index, uintptr_t *restrict value_length)
{
	const uhttp_uri_param_s *restrict p;
	if ((p = (const uhttp_uri_param_s *) vattr_get_index(uri->param, key, index)))
	{
		if (value_length)
			*value_length = p->value_length;
		return p->value;
	}
	if (value_length)
		*value_length = 0;
	return NULL;
}

// uri cut

int uhttp_uri_tear_with_length(const char *restrict full_uri, uintptr_t length, refer_nstring_t *restrict protocol, refer_nstring_t *restrict host, uint32_t *restrict port, refer_nstring_t *restrict uri)
{
	const char *p, *p_protocol, *p_host, *p_port, *p_uri;
	uintptr_t n, n_protocol, n_host, n_port, n_uri;
	int ret;
	p_protocol = p_host = p_port = p_uri = NULL;
	p = memchr(full_uri, '/', length);
	if (!p) goto label_not_protocol_and_uri;
	if (p > full_uri && *(p - 1) == ':' && p + 1 < full_uri + length && *(p + 1) == '/')
	{
		// this protocol
		p_protocol = full_uri;
		n_protocol = (uintptr_t) p - (uintptr_t) full_uri - 1;
		n = n_protocol + 3;
		full_uri += n;
		length -= n;
		p = memchr(full_uri, '/', length);
		if (!p)
		{
			label_not_protocol_and_uri:
			p = full_uri + length;
		}
		goto label_set_host_and_port;
	}
	else
	{
		// no protocol
		label_set_host_and_port:
		p_host = full_uri;
		n_host = (uintptr_t) p - (uintptr_t) full_uri;
		full_uri += n_host;
		length -= n_host;
	}
	// uri
	p_uri = full_uri;
	n_uri = length;
	// tear host and port
	p = memchr(p_host, ':', n_host);
	if (p)
	{
		n = (uintptr_t) p - (uintptr_t) p_host;
		p_port = p + 1;
		n_port = n_host - n - 1;
		n_host = n;
	}
	// build protocol host port uri
	ret = 0;
	if (protocol)
	{
		if (!(*protocol = (p_protocol?refer_dump_nstring_with_length(p_protocol, n_protocol):refer_dump_nstring("http"))))
			--ret;
	}
	if (host)
	{
		if (!(*host = refer_dump_nstring_with_length(p_host, n_host)))
			--ret;
	}
	if (port)
	{
		*port = 0;
		if (p_port)
		{
			for (n = 0; n < n_port && p_port[n] >= '0' && p_port[n] <= '9'; ++n)
				*port = (*port * 10) + (p_port[n] - '0');
			*port &= 0xffff;
		}
	}
	if (uri)
	{
		if (!(*uri = ((p_uri && n_uri)?refer_dump_nstring_with_length(p_uri, n_uri):refer_dump_nstring("/"))))
			--ret;
	}
	return ret;
}

int uhttp_uri_tear(const char *restrict full_uri, refer_nstring_t *restrict protocol, refer_nstring_t *restrict host, uint32_t *restrict port, refer_nstring_t *restrict uri)
{
	return uhttp_uri_tear_with_length(full_uri, strlen(full_uri), protocol, host, port, uri);
}
