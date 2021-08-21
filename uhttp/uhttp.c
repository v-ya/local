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
		0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
		0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
		0x60, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
		0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f,
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
		r->inst = (const uhttp_inst_s *) refer_save(inst);
		r->version = (refer_nstring_t) refer_save(inst->version);
		if ((r->header = vattr_alloc()))
			return r;
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

uhttp_s* uhttp_refer_header_reset(uhttp_s *restrict uhttp, uhttp_header_s *restrict header)
{
	if (vattr_set(uhttp->header, header->id, header))
		return uhttp;
	return NULL;
}

uhttp_s* uhttp_refer_header_first(uhttp_s *restrict uhttp, uhttp_header_s *restrict header)
{
	if (vattr_insert_first_vslot(uhttp->header, header->id, header))
		return uhttp;
	return NULL;
}

uhttp_s* uhttp_refer_header_last(uhttp_s *restrict uhttp, uhttp_header_s *restrict header)
{
	if (vattr_insert_tail_vslot(uhttp->header, header->id, header))
		return uhttp;
	return NULL;
}

uhttp_s* uhttp_refer_header_tail(uhttp_s *restrict uhttp, uhttp_header_s *restrict header)
{
	if (vattr_insert_tail(uhttp->header, header->id, header))
		return uhttp;
	return NULL;
}

uhttp_s* uhttp_refer_header_index(uhttp_s *restrict uhttp, uhttp_header_s *restrict header, uintptr_t index)
{
	if (vattr_insert_index_last(uhttp->header, header->id, index, header))
		return uhttp;
	return NULL;
}

uhttp_s* uhttp_set_header_refer_name(uhttp_s *restrict uhttp, refer_nstring_t name, const char *restrict value)
{
	uhttp_s *restrict r;
	uhttp_header_s *restrict header;
	r = NULL;
	if ((header = uhttp_header_refer(name, value)))
	{
		if (vattr_set(uhttp->header, header->id, header))
			r = uhttp;
		refer_free(header);
	}
	return r;
}

uhttp_s* uhttp_insert_header_refer_name(uhttp_s *restrict uhttp, refer_nstring_t name, const char *restrict value)
{
	uhttp_s *restrict r;
	uhttp_header_s *restrict header;
	r = NULL;
	if ((header = uhttp_header_refer(name, value)))
	{
		if (vattr_insert_tail(uhttp->header, header->id, header))
			r = uhttp;
		refer_free(header);
	}
	return r;
}

uhttp_s* uhttp_set_header(uhttp_s *restrict uhttp, const char *restrict name, const char *restrict value)
{
	uhttp_s *restrict r;
	uhttp_header_s *restrict header;
	r = NULL;
	if ((header = uhttp_header_alloc(name, value)))
	{
		if (vattr_set(uhttp->header, header->id, header))
			r = uhttp;
		refer_free(header);
	}
	return r;
}

uhttp_s* uhttp_insert_header(uhttp_s *restrict uhttp, const char *restrict name, const char *restrict value)
{
	uhttp_s *restrict r;
	uhttp_header_s *restrict header;
	r = NULL;
	if ((header = uhttp_header_alloc(name, value)))
	{
		if (vattr_insert_tail(uhttp->header, header->id, header))
			r = uhttp;
		refer_free(header);
	}
	return r;
}

void uhttp_delete_header(uhttp_s *restrict uhttp, const char *restrict id)
{
	vattr_delete(uhttp->header, id);
}

void uhttp_delete_header_first(uhttp_s *restrict uhttp, const char *restrict id)
{
	vattr_delete_first(uhttp->header, id);
}

void uhttp_delete_header_tail(uhttp_s *restrict uhttp, const char *restrict id)
{
	vattr_delete_tail(uhttp->header, id);
}

void uhttp_delete_header_index(uhttp_s *restrict uhttp, const char *restrict id, uintptr_t index)
{
	vattr_delete_index(uhttp->header, id, index);
}

uhttp_header_s* uhttp_find_header_first(uhttp_s *restrict uhttp, const char *restrict id)
{
	return (uhttp_header_s *) vattr_get_first(uhttp->header, id);
}

uhttp_header_s* uhttp_find_header_tail(uhttp_s *restrict uhttp, const char *restrict id)
{
	return (uhttp_header_s *) vattr_get_tail(uhttp->header, id);
}

uhttp_header_s* uhttp_find_header_index(uhttp_s *restrict uhttp, const char *restrict id, uintptr_t index)
{
	return (uhttp_header_s *) vattr_get_index(uhttp->header, id, index);
}

const char* uhttp_get_header_first(uhttp_s *restrict uhttp, const char *restrict id, uintptr_t *restrict length)
{
	uhttp_header_s *restrict header;
	if ((header = (uhttp_header_s *) vattr_get_first(uhttp->header, id)))
	{
		if (length)
			*length = header->value_length;
		return header->value;
	}
	if (length)
		*length = 0;
	return NULL;
}

const char* uhttp_get_header_tail(uhttp_s *restrict uhttp, const char *restrict id, uintptr_t *restrict length)
{
	uhttp_header_s *restrict header;
	if ((header = (uhttp_header_s *) vattr_get_tail(uhttp->header, id)))
	{
		if (length)
			*length = header->value_length;
		return header->value;
	}
	if (length)
		*length = 0;
	return NULL;
}

const char* uhttp_get_header_index(uhttp_s *restrict uhttp, const char *restrict id, uintptr_t index, uintptr_t *restrict length)
{
	uhttp_header_s *restrict header;
	if ((header = (uhttp_header_s *) vattr_get_index(uhttp->header, id, index)))
	{
		if (length)
			*length = header->value_length;
		return header->value;
	}
	if (length)
		*length = 0;
	return NULL;
}

uintptr_t uhttp_get_header_number(uhttp_s *restrict uhttp, const char *restrict id)
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

uhttp_iter_header_all_t uhttp_iter_header_all_init(uhttp_s *restrict uhttp)
{
	return (uhttp_iter_header_all_t) uhttp->header->vattr;
}

uhttp_iter_header_id_t uhttp_iter_header_id_init(uhttp_s *restrict uhttp, const char *restrict id)
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
