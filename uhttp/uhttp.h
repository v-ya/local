#ifndef _uhttp_h_
#define _uhttp_h_

#include <refer.h>
#include <exbuffer.h>

struct vattr_s;

typedef struct uhttp_inst_s uhttp_inst_s;
typedef struct uhttp_s uhttp_s;

typedef struct uhttp_iter_header_id_t *uhttp_iter_header_id_t;
typedef struct uhttp_iter_header_all_t *uhttp_iter_header_all_t;

typedef enum uhttp_parse_status_t {
	uhttp_parse_status_error_inner = -2,
	uhttp_parse_status_error_parse = -1,
	uhttp_parse_status_okay        = 0,
	uhttp_parse_status_wait_header = 1,
	uhttp_parse_status_wait_line   = 2,
} uhttp_parse_status_t;

typedef const struct uhttp_header_s {
	refer_nstring_t name;
	const char *id;
	const char *value;
	uintptr_t value_length;
} uhttp_header_s;

typedef struct uhttp_parse_context_t {
	// user set && uhttp const
	const char *data;
	uintptr_t size;
	// user init && uhttp modify
	uintptr_t pos;
	uintptr_t pos_check;
	uhttp_parse_status_t step;
} uhttp_parse_context_t;

// uhttp inst control
uhttp_inst_s* uhttp_inst_alloc_empty(void);
refer_nstring_t uhttp_inst_refer_version(uhttp_inst_s *restrict inst, refer_nstring_t version);
refer_nstring_t uhttp_inst_set_version(uhttp_inst_s *restrict inst, const char *restrict version);
refer_nstring_t uhttp_inst_get_version(const uhttp_inst_s *restrict inst);
refer_nstring_t uhttp_inst_refer_status(uhttp_inst_s *restrict inst, int status_code, refer_nstring_t status_desc);
refer_nstring_t uhttp_inst_set_status(uhttp_inst_s *restrict inst, int status_code, const char *restrict status_desc);
refer_nstring_t uhttp_inst_get_status(const uhttp_inst_s *restrict inst, int status_code);

// uhttp inst alloc
uhttp_inst_s* uhttp_inst_alloc_http11(void);
uhttp_inst_s* uhttp_inst_alloc_http11_without_status(void);

// uhttp header alloc
uhttp_header_s* uhttp_header_refer_with_length(refer_nstring_t name, const char *restrict value, uintptr_t length);
uhttp_header_s* uhttp_header_refer(refer_nstring_t name, const char *restrict value);
uhttp_header_s* uhttp_header_alloc_with_length(const char *restrict name, const char *restrict value, uintptr_t length);
uhttp_header_s* uhttp_header_alloc_with_length2(const char *restrict name, uintptr_t name_length, const char *restrict value, uintptr_t value_length);
uhttp_header_s* uhttp_header_alloc(const char *restrict name, const char *restrict value);
uhttp_header_s* uhttp_header_refer_integer(refer_nstring_t name, int64_t value);
uhttp_header_s* uhttp_header_alloc_integer(const char *restrict name, int64_t value);
refer_string_t uhttp_header_new_id_with_length(const char *restrict name, uintptr_t length);
refer_string_t uhttp_header_new_id_by_nstring(refer_nstring_t name);
refer_string_t uhttp_header_new_id(const char *restrict name);

// uhttp control
uhttp_s* uhttp_alloc(const uhttp_inst_s *restrict inst);
uhttp_s* uhttp_dump(const uhttp_s *restrict uhttp);
void uhttp_clear_line(uhttp_s *restrict uhttp);
void uhttp_clear_header(uhttp_s *restrict uhttp);
void uhttp_clear(uhttp_s *restrict uhttp);
void uhttp_copy_line(uhttp_s *restrict dst, const uhttp_s *restrict src);
uhttp_s* uhttp_copy_header(uhttp_s *restrict dst, const uhttp_s *restrict src);
uhttp_s* uhttp_append_header(uhttp_s *restrict dst, const uhttp_s *restrict src);
uhttp_s* uhttp_copy(uhttp_s *restrict dst, const uhttp_s *restrict src);
void uhttp_refer_request(uhttp_s *restrict uhttp, refer_nstring_t method, refer_nstring_t uri);
void uhttp_refer_response(uhttp_s *restrict uhttp, int code, refer_nstring_t desc);
uhttp_s* uhttp_set_request_refer_method(uhttp_s *restrict uhttp, refer_nstring_t method, const char *restrict uri);
uhttp_s* uhttp_set_request_refer_uri(uhttp_s *restrict uhttp, const char *restrict method, refer_nstring_t uri);
uhttp_s* uhttp_set_request(uhttp_s *restrict uhttp, const char *restrict method, const char *restrict uri);
uhttp_s* uhttp_set_response(uhttp_s *restrict uhttp, int code, const char *restrict desc);
refer_nstring_t uhttp_get_request_method(const uhttp_s *restrict uhttp);
refer_nstring_t uhttp_get_request_uri(const uhttp_s *restrict uhttp);
int uhttp_get_response_code(const uhttp_s *restrict uhttp);
refer_nstring_t uhttp_get_response_desc(const uhttp_s *restrict uhttp);
uhttp_s* uhttp_refer_header_reset(uhttp_s *restrict uhttp, uhttp_header_s *restrict header);
uhttp_s* uhttp_refer_header_first(uhttp_s *restrict uhttp, uhttp_header_s *restrict header);
uhttp_s* uhttp_refer_header_last(uhttp_s *restrict uhttp, uhttp_header_s *restrict header);
uhttp_s* uhttp_refer_header_tail(uhttp_s *restrict uhttp, uhttp_header_s *restrict header);
uhttp_s* uhttp_refer_header_index(uhttp_s *restrict uhttp, uhttp_header_s *restrict header, uintptr_t index);
uhttp_s* uhttp_set_header_refer_name(uhttp_s *restrict uhttp, refer_nstring_t name, const char *restrict value);
uhttp_s* uhttp_insert_header_refer_name(uhttp_s *restrict uhttp, refer_nstring_t name, const char *restrict value);
uhttp_s* uhttp_set_header(uhttp_s *restrict uhttp, const char *restrict name, const char *restrict value);
uhttp_s* uhttp_insert_header(uhttp_s *restrict uhttp, const char *restrict name, const char *restrict value);
uhttp_s* uhttp_set_header_integer_refer_name(uhttp_s *restrict uhttp, refer_nstring_t name, int64_t value);
uhttp_s* uhttp_insert_header_integer_refer_name(uhttp_s *restrict uhttp, refer_nstring_t name, int64_t value);
uhttp_s* uhttp_set_header_integer(uhttp_s *restrict uhttp, const char *restrict name, int64_t value);
uhttp_s* uhttp_insert_header_integer(uhttp_s *restrict uhttp, const char *restrict name, int64_t value);
void uhttp_delete_header(uhttp_s *restrict uhttp, const char *restrict id);
void uhttp_delete_header_first(uhttp_s *restrict uhttp, const char *restrict id);
void uhttp_delete_header_tail(uhttp_s *restrict uhttp, const char *restrict id);
void uhttp_delete_header_index(uhttp_s *restrict uhttp, const char *restrict id, uintptr_t index);
uhttp_header_s* uhttp_find_header_first(const uhttp_s *restrict uhttp, const char *restrict id);
uhttp_header_s* uhttp_find_header_tail(const uhttp_s *restrict uhttp, const char *restrict id);
uhttp_header_s* uhttp_find_header_index(const uhttp_s *restrict uhttp, const char *restrict id, uintptr_t index);
const char* uhttp_get_header_first(const uhttp_s *restrict uhttp, const char *restrict id);
const char* uhttp_get_header_tail(const uhttp_s *restrict uhttp, const char *restrict id);
const char* uhttp_get_header_index(const uhttp_s *restrict uhttp, const char *restrict id, uintptr_t index);
int64_t uhttp_get_header_integer_first(const uhttp_s *restrict uhttp, const char *restrict id);
int64_t uhttp_get_header_integer_tail(const uhttp_s *restrict uhttp, const char *restrict id);
int64_t uhttp_get_header_integer_index(const uhttp_s *restrict uhttp, const char *restrict id, uintptr_t index);
uintptr_t uhttp_get_header_number(const uhttp_s *restrict uhttp, const char *restrict id);
uhttp_s* uhttp_copy_header_by_id(uhttp_s *restrict dst, const uhttp_s *restrict src, const char *restrict id);
const struct vattr_s* uhttp_header_vattr(const uhttp_s *restrict uhttp);
uhttp_iter_header_all_t uhttp_iter_header_all_init(const uhttp_s *restrict uhttp);
uhttp_iter_header_id_t uhttp_iter_header_id_init(const uhttp_s *restrict uhttp, const char *restrict id);
uhttp_header_s* uhttp_iter_header_all_next(uhttp_iter_header_all_t *restrict iter);
uhttp_header_s* uhttp_iter_header_id_next(uhttp_iter_header_id_t *restrict iter);

// uhttp build
uintptr_t uhttp_build_line_length(const uhttp_s *restrict uhttp);
char* uhttp_build_line_write(const uhttp_s *restrict uhttp, char *restrict data);
uintptr_t uhttp_build_header_length(const uhttp_s *restrict uhttp);
char* uhttp_build_header_write(const uhttp_s *restrict uhttp, char *restrict data);
uintptr_t uhttp_build_length(const uhttp_s *restrict uhttp);
char* uhttp_build_write(const uhttp_s *restrict uhttp, char *restrict data);
char* uhttp_build(const uhttp_s *restrict uhttp, exbuffer_t *restrict message, uintptr_t *restrict length);

// uhttp parse
uhttp_parse_status_t uhttp_parse_line(uhttp_s *restrict uhttp, const char *restrict p, uintptr_t n, uintptr_t *restrict pos);
uhttp_parse_status_t uhttp_parse_header(uhttp_s *restrict uhttp, const char *restrict p, uintptr_t n, uintptr_t *restrict pos);
uhttp_parse_status_t uhttp_parse(uhttp_s *restrict uhttp, const char *restrict p, uintptr_t n, uintptr_t *restrict pos);
void uhttp_parse_context_init(uhttp_parse_context_t *restrict context, uintptr_t pos);
void uhttp_parse_context_set(uhttp_parse_context_t *restrict context, const void *restrict data, uintptr_t length);
uhttp_parse_status_t uhttp_parse_context_try(uhttp_s *restrict uhttp, uhttp_parse_context_t *restrict context);

#endif
