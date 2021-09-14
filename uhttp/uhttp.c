#include "uhttp_type.h"

// uhttp header

static void uhttp_header_free_func(uhttp_header_s *restrict r)
{
	if (r->name)
		refer_free(r->name);
}

static inline void uhttp_header_build_id(char *restrict id, const char *restrict name, uintptr_t length)
{
	static const char n2i[256] = {
		0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
		0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
		0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
		0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
		0x40, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
		0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
		0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
		0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f,
		0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
		0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f,
		0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf,
		0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,
		0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,
		0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf,
		0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,
		0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff,
	};
	for (uintptr_t i = 0; i < length; ++i)
		id[i] = n2i[((uint8_t *) name)[i]];
}

static inline uintptr_t uhttp_header_build_integer(char *restrict buffer, int64_t value)
{
	return (uintptr_t) sprintf(buffer, "%" PRId64, value);
}

uhttp_header_s* uhttp_header_refer_with_length(refer_nstring_t name, const char *restrict value, uintptr_t length)
{
	struct uhttp_header_s *restrict r;
	char *restrict p;
	r = (struct uhttp_header_s *) refer_alloc((sizeof(struct uhttp_header_s) + 2) + name->length + length);
	if (r)
	{
		refer_set_free(r, (refer_free_f) uhttp_header_free_func);
		r->name = (refer_nstring_t) refer_save(name);
		p = (char *) (r + 1);
		r->id = p;
		if (name->length)
		{
			uhttp_header_build_id(p, name->string, name->length);
			p += name->length;
		}
		*p++ = 0;
		r->value = p;
		memcpy(p, value, length);
		p[length] = 0;
		r->value_length = length;
	}
	return r;
}

uhttp_header_s* uhttp_header_refer(refer_nstring_t name, const char *restrict value)
{
	return uhttp_header_refer_with_length(name, value, value?strlen(value):0);
}

uhttp_header_s* uhttp_header_alloc_with_length(const char *restrict name, const char *restrict value, uintptr_t length)
{
	uhttp_header_s *restrict r;
	refer_nstring_t n;
	r = NULL;
	if ((n = refer_dump_nstring(name)))
	{
		r = uhttp_header_refer_with_length(n, value, length);
		refer_free(n);
	}
	return r;
}

uhttp_header_s* uhttp_header_alloc_with_length2(const char *restrict name, uintptr_t name_length, const char *restrict value, uintptr_t value_length)
{
	uhttp_header_s *restrict r;
	refer_nstring_t n;
	r = NULL;
	if ((n = refer_dump_nstring_with_length(name, name_length)))
	{
		r = uhttp_header_refer_with_length(n, value, value_length);
		refer_free(n);
	}
	return r;
}

uhttp_header_s* uhttp_header_alloc(const char *restrict name, const char *restrict value)
{
	return uhttp_header_alloc_with_length(name, value, value?strlen(value):0);
}

uhttp_header_s* uhttp_header_refer_integer(refer_nstring_t name, int64_t value)
{
	char buffer[64];
	return uhttp_header_refer_with_length(name, buffer, uhttp_header_build_integer(buffer, value));
}

uhttp_header_s* uhttp_header_alloc_integer(const char *restrict name, int64_t value)
{
	char buffer[64];
	return uhttp_header_alloc_with_length(name, buffer, uhttp_header_build_integer(buffer, value));
}

refer_string_t uhttp_header_new_id_with_length(const char *restrict name, uintptr_t length)
{
	char *restrict id;
	id = (char *) refer_alloc(length + 1);
	if (id)
	{
		if (length)
			uhttp_header_build_id(id, name, length);
		id[length] = 0;
	}
	return id;
}

refer_string_t uhttp_header_new_id_by_nstring(refer_nstring_t name)
{
	return uhttp_header_new_id_with_length(name->string, name->length);
}

refer_string_t uhttp_header_new_id(const char *restrict name)
{
	return uhttp_header_new_id_with_length(name, strlen(name));
}

// uhttp

static void uhttp_free_func(uhttp_s *restrict r)
{
	if (r->inst)
		refer_free(r->inst);
	if (r->version)
		refer_free(r->version);
	if (r->request_method)
		refer_free(r->request_method);
	if (r->request_uri)
		refer_free(r->request_uri);
	if (r->status_desc)
		refer_free(r->status_desc);
	if (r->header)
		refer_free(r->header);
}

uhttp_s* uhttp_alloc(const uhttp_inst_s *restrict inst)
{
	uhttp_s *restrict r;
	if ((r = (uhttp_s *) refer_alloz(sizeof(uhttp_s))))
	{
		refer_set_free(r, (refer_free_f) uhttp_free_func);
		if (inst)
			r->inst = (const uhttp_inst_s *) refer_save(inst);
		else r->inst = inst = uhttp_inst_alloc_http11_without_status();
		if (r->inst)
		{
			r->version = (refer_nstring_t) refer_save(inst->version?inst->version:inst->empty);
			if ((r->header = vattr_alloc()))
				return r;
		}
		refer_free(r);
	}
	return NULL;
}

uhttp_s* uhttp_dump(const uhttp_s *restrict uhttp)
{
	uhttp_s *restrict r;
	if ((r = uhttp_alloc(uhttp->inst)))
	{
		if (uhttp->request_method)
			r->request_method = (refer_nstring_t) refer_save(uhttp->request_method);
		if (uhttp->request_uri)
			r->request_uri = (refer_nstring_t) refer_save(uhttp->request_uri);
		if (uhttp->status_desc)
			r->status_desc = (refer_nstring_t) refer_save(uhttp->status_desc);
		r->status_code = uhttp->status_code;
		if (vattr_copy(r->header, uhttp->header))
			return r;
		refer_free(r);
	}
	return NULL;
}

void uhttp_clear_line(uhttp_s *restrict uhttp)
{
	if (uhttp->request_method)
	{
		refer_free(uhttp->request_method);
		uhttp->request_method = NULL;
	}
	if (uhttp->request_uri)
	{
		refer_free(uhttp->request_uri);
		uhttp->request_uri = NULL;
	}
	if (uhttp->status_desc)
	{
		refer_free(uhttp->status_desc);
		uhttp->status_desc = NULL;
	}
	uhttp->status_code = 0;
}

void uhttp_clear_header(uhttp_s *restrict uhttp)
{
	vattr_clear(uhttp->header);
}

void uhttp_clear(uhttp_s *restrict uhttp)
{
	uhttp_clear_line(uhttp);
	uhttp_clear_header(uhttp);
}

void uhttp_copy_line(uhttp_s *restrict dst, const uhttp_s *restrict src)
{
	uhttp_clear_line(dst);
	if (src->request_method)
		dst->request_method = (refer_nstring_t) refer_save(src->request_method);
	if (src->request_uri)
		dst->request_uri = (refer_nstring_t) refer_save(src->request_uri);
	if (src->status_desc)
		dst->status_desc = (refer_nstring_t) refer_save(src->status_desc);
	dst->status_code = src->status_code;
}

uhttp_s* uhttp_copy_header(uhttp_s *restrict dst, const uhttp_s *restrict src)
{
	if (vattr_copy(dst->header, src->header))
		return dst;
	return NULL;
}

uhttp_s* uhttp_append_header(uhttp_s *restrict dst, const uhttp_s *restrict src)
{
	if (vattr_append(dst->header, src->header))
		return dst;
	return NULL;
}

uhttp_s* uhttp_copy(uhttp_s *restrict dst, const uhttp_s *restrict src)
{
	uhttp_copy_line(dst, src);
	return uhttp_copy_header(dst, src);
}

void uhttp_refer_request(uhttp_s *restrict uhttp, refer_nstring_t method, refer_nstring_t uri)
{
	const uhttp_inst_s *restrict inst;
	inst = uhttp->inst;
	if (!method) method = inst->empty;
	if (!uri) uri = inst->empty;
	uhttp_clear_line(uhttp);
	uhttp->request_method = (refer_nstring_t) refer_save(method);
	uhttp->request_uri = (refer_nstring_t) refer_save(uri);
}

void uhttp_refer_response(uhttp_s *restrict uhttp, int code, refer_nstring_t desc)
{
	const uhttp_inst_s *restrict inst;
	inst = uhttp->inst;
	if (!desc)
	{
		if (!(desc = uhttp_inst_get_status(inst, code)))
			desc = inst->empty;
	}
	uhttp_clear_line(uhttp);
	uhttp->status_code = (intptr_t) code;
	uhttp->status_desc = (refer_nstring_t) refer_save(desc);
}

uhttp_s* uhttp_set_request_refer_method(uhttp_s *restrict uhttp, refer_nstring_t method, const char *restrict uri)
{
	refer_nstring_t u;
	u = NULL;
	if (uri)
	{
		if (!(u = refer_dump_nstring(uri)))
			goto label_fail_uri;
	}
	uhttp_refer_request(uhttp, method, u);
	if (u) refer_free(u);
	return uhttp;
	label_fail_uri:
	return NULL;
}

uhttp_s* uhttp_set_request_refer_uri(uhttp_s *restrict uhttp, const char *restrict method, refer_nstring_t uri)
{
	refer_nstring_t m;
	m = NULL;
	if (method)
	{
		if (!(m = refer_dump_nstring(method)))
			goto label_fail_method;
	}
	uhttp_refer_request(uhttp, m, uri);
	if (m) refer_free(m);
	return uhttp;
	label_fail_method:
	return NULL;
}

uhttp_s* uhttp_set_request(uhttp_s *restrict uhttp, const char *restrict method, const char *restrict uri)
{
	refer_nstring_t m, u;
	m = u = NULL;
	if (method)
	{
		if (!(m = refer_dump_nstring(method)))
			goto label_fail_method;
	}
	if (uri)
	{
		if (!(u = refer_dump_nstring(uri)))
			goto label_fail_uri;
	}
	uhttp_refer_request(uhttp, m, u);
	if (m) refer_free(m);
	if (u) refer_free(u);
	return uhttp;
	label_fail_uri:
	if (m) refer_free(m);
	label_fail_method:
	return NULL;
}

uhttp_s* uhttp_set_response(uhttp_s *restrict uhttp, int code, const char *restrict desc)
{
	refer_nstring_t d;
	d = NULL;
	if (desc)
	{
		if (!(d = refer_dump_nstring(desc)))
			goto label_fail_desc;
	}
	uhttp_refer_response(uhttp, code, d);
	if (d) refer_free(d);
	return uhttp;
	label_fail_desc:
	return NULL;
}

refer_nstring_t uhttp_get_request_method(const uhttp_s *restrict uhttp)
{
	return uhttp->request_method;
}

refer_nstring_t uhttp_get_request_uri(const uhttp_s *restrict uhttp)
{
	return uhttp->request_uri;
}

int uhttp_get_response_code(const uhttp_s *restrict uhttp)
{
	return (int) uhttp->status_code;
}

refer_nstring_t uhttp_get_response_desc(const uhttp_s *restrict uhttp)
{
	return uhttp->status_desc;
}

#define d_refer_header(_name, _call)  \
	uhttp_s* _name(uhttp_s *restrict uhttp, uhttp_header_s *restrict header)\
	{\
		if (_call(uhttp->header, header->id, header))\
			return uhttp;\
		return NULL;\
	}
#define d_refer_header_index(_name, _call)  \
	uhttp_s* _name(uhttp_s *restrict uhttp, uhttp_header_s *restrict header, uintptr_t index)\
	{\
		if (_call(uhttp->header, header->id, index, header))\
			return uhttp;\
		return NULL;\
	}

d_refer_header(uhttp_refer_header_reset, vattr_set)
d_refer_header(uhttp_refer_header_first, vattr_insert_first_vslot)
d_refer_header(uhttp_refer_header_last, vattr_insert_tail_vslot)
d_refer_header(uhttp_refer_header_tail, vattr_insert_tail)
d_refer_header_index(uhttp_refer_header_index, vattr_insert_index_last)

#undef d_refer_header
#undef d_refer_header_index

#define d_set_header(_name, _build, _set, _type_name, _type_value)  \
	uhttp_s* _name(uhttp_s *restrict uhttp, _type_name name, _type_value value)\
	{\
		uhttp_s *restrict r;\
		uhttp_header_s *restrict header;\
		r = NULL;\
		if ((header = _build(name, value)))\
		{\
			if (_set(uhttp->header, header->id, header))\
				r = uhttp;\
			refer_free(header);\
		}\
		return r;\
	}

d_set_header(uhttp_set_header_refer_name, uhttp_header_refer, vattr_set, refer_nstring_t, const char *restrict)
d_set_header(uhttp_insert_header_refer_name, uhttp_header_refer, vattr_insert_tail, refer_nstring_t, const char *restrict)
d_set_header(uhttp_set_header, uhttp_header_alloc, vattr_set, const char *restrict, const char *restrict)
d_set_header(uhttp_insert_header, uhttp_header_alloc, vattr_insert_tail, const char *restrict, const char *restrict)

d_set_header(uhttp_set_header_integer_refer_name, uhttp_header_refer_integer, vattr_set, refer_nstring_t, int64_t)
d_set_header(uhttp_insert_header_integer_refer_name, uhttp_header_refer_integer, vattr_insert_tail, refer_nstring_t, int64_t)
d_set_header(uhttp_set_header_integer, uhttp_header_alloc_integer, vattr_set, const char *restrict, int64_t)
d_set_header(uhttp_insert_header_integer, uhttp_header_alloc_integer, vattr_insert_tail, const char *restrict, int64_t)

#undef d_set_header

#define d_delete_header(_name, _call)  \
	void _name(uhttp_s *restrict uhttp, const char *restrict id)\
	{\
		_call(uhttp->header, id);\
	}
#define d_delete_header_index(_name, _call)  \
	void _name(uhttp_s *restrict uhttp, const char *restrict id, uintptr_t index)\
	{\
		_call(uhttp->header, id, index);\
	}

d_delete_header(uhttp_delete_header, vattr_delete)
d_delete_header(uhttp_delete_header_first, vattr_delete_first)
d_delete_header(uhttp_delete_header_tail, vattr_delete_tail)
d_delete_header_index(uhttp_delete_header_index, vattr_delete_index)

#undef d_delete_header
#undef d_delete_header_index

#define d_find_header(_name, _call)  \
	uhttp_header_s* _name(const uhttp_s *restrict uhttp, const char *restrict id)\
	{\
		return (uhttp_header_s *) _call(uhttp->header, id);\
	}
#define d_find_header_index(_name, _call)  \
	uhttp_header_s* _name(const uhttp_s *restrict uhttp, const char *restrict id, uintptr_t index)\
	{\
		return (uhttp_header_s *) _call(uhttp->header, id, index);\
	}

d_find_header(uhttp_find_header_first, vattr_get_first)
d_find_header(uhttp_find_header_tail, vattr_get_tail)
d_find_header_index(uhttp_find_header_index, vattr_get_index)

#undef d_find_header
#undef d_find_header_index

#define d_get_header(_name, _get, _ret_type, _ret_value, _ret_default)  \
	_ret_type _name(const uhttp_s *restrict uhttp, const char *restrict id)\
	{\
		uhttp_header_s *restrict header;\
		if ((header = (uhttp_header_s *) _get(uhttp->header, id)))\
			return _ret_value;\
		return _ret_default;\
	}
#define d_get_header_index(_name, _get, _ret_type, _ret_value, _ret_default)  \
	_ret_type _name(const uhttp_s *restrict uhttp, const char *restrict id, uintptr_t index)\
	{\
		uhttp_header_s *restrict header;\
		if ((header = (uhttp_header_s *) _get(uhttp->header, id, index)))\
			return _ret_value;\
		return _ret_default;\
	}

d_get_header(uhttp_get_header_first, vattr_get_first, const char*, header->value, NULL)
d_get_header(uhttp_get_header_tail, vattr_get_tail, const char*, header->value, NULL)
d_get_header_index(uhttp_get_header_index, vattr_get_index, const char*, header->value, NULL)

d_get_header(uhttp_get_header_integer_first, vattr_get_first, int64_t, (int64_t) strtoll(header->value, NULL, 0), 0)
d_get_header(uhttp_get_header_integer_tail, vattr_get_tail, int64_t, (int64_t) strtoll(header->value, NULL, 0), 0)
d_get_header_index(uhttp_get_header_integer_index, vattr_get_index, int64_t, (int64_t) strtoll(header->value, NULL, 0), 0)

#undef d_get_header
#undef d_get_header_index

uintptr_t uhttp_get_header_number(const uhttp_s *restrict uhttp, const char *restrict id)
{
	return vattr_get_vslot_number(uhttp->header, id);
}

uhttp_s* uhttp_copy_header_by_id(uhttp_s *restrict dst, const uhttp_s *restrict src, const char *restrict id)
{
	const vattr_vlist_t *restrict v;
	const vattr_vslot_t *restrict vslot;
	const vattr_s *restrict s;
	vattr_s *restrict d;
	s = src->header;
	d = dst->header;
	vattr_delete(d, id);
	if ((vslot = vattr_get_vslot(s, id)))
	{
		for (v = vslot->vslot; v; v = v->vslot_next)
		{
			if (!vattr_insert_tail(d, id, v->value))
				goto label_fail;
		}
	}
	return dst;
	label_fail:
	return NULL;
}

const struct vattr_s* uhttp_header_vattr(const uhttp_s *restrict uhttp)
{
	return uhttp->header;
}

uhttp_iter_header_all_t uhttp_iter_header_all_init(const uhttp_s *restrict uhttp)
{
	return (uhttp_iter_header_all_t) uhttp->header->vattr;
}

uhttp_iter_header_id_t uhttp_iter_header_id_init(const uhttp_s *restrict uhttp, const char *restrict id)
{
	vattr_vslot_t *restrict vslot;
	if ((vslot = vattr_get_vslot(uhttp->header, id)))
		return (uhttp_iter_header_id_t) vslot->vslot;
	return NULL;
}

uhttp_header_s* uhttp_iter_header_all_next(uhttp_iter_header_all_t *restrict iter)
{
	vattr_vlist_t *restrict v;
	if ((v = &(*iter)->iter))
	{
		*iter = (uhttp_iter_header_all_t) v->vattr_next;
		return (uhttp_header_s *) v->value;
	}
	return NULL;
}

uhttp_header_s* uhttp_iter_header_id_next(uhttp_iter_header_id_t *restrict iter)
{
	vattr_vlist_t *restrict v;
	if ((v = &(*iter)->iter))
	{
		*iter = (uhttp_iter_header_id_t) v->vslot_next;
		return (uhttp_header_s *) v->value;
	}
	return NULL;
}
