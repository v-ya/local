#ifndef _vkaa_error_h_
#define _vkaa_error_h_

#include "vkaa.h"
#include <hashmap.h>
#include <exbuffer.h>
#include <mlog.h>

struct vkaa_error_s {
	hashmap_t n2i;
	hashmap_t i2n;
};

typedef struct vkaa_elog_exec_t {
	uintptr_t index_skip;
	uintptr_t index_parent;
	uintptr_t exec_pos_start;
	uintptr_t exec_pos_stop;
	const vkaa_syntax_source_s *source;
	uint32_t syntax_pos_start;
	uint32_t syntax_pos_stop;
} vkaa_elog_exec_t;

struct vkaa_elog_s {
	exbuffer_t exec_buffer;
	vkaa_elog_exec_t *exec_array;
	uintptr_t exec_number;
	mlog_s *parse;
	mlog_s *exec;
	const vkaa_error_s *error;
	uintptr_t exec_curr;
};

vkaa_error_s* vkaa_error_alloc(void);
vkaa_error_s* vkaa_error_add_error(vkaa_error_s *restrict error, const char *restrict name);
uintptr_t vkaa_error_get_id(const vkaa_error_s *restrict error, const char *restrict name);
const char* vkaa_error_get_name(const vkaa_error_s *restrict error, uintptr_t id);

vkaa_elog_s* vkaa_elog_alloc(mlog_s *parse, mlog_s *exec, const vkaa_error_s *error);
vkaa_elog_s* vkaa_elog_push(vkaa_elog_s *restrict elog, const vkaa_syntax_source_s *source, uintptr_t exec_pos_start, uint32_t syntax_pos_start);
vkaa_elog_s* vkaa_elog_fence(vkaa_elog_s *restrict elog, const vkaa_syntax_source_s *source, uintptr_t exec_pos, uint32_t syntax_pos);
vkaa_elog_s* vkaa_elog_pop(vkaa_elog_s *restrict elog, uintptr_t exec_pos_stop, uint32_t syntax_pos_stop);
const vkaa_elog_exec_t* vkaa_elog_find(const vkaa_elog_s *restrict elog, uintptr_t exec_pos);
void vkaa_elog_print_parse(const vkaa_elog_s *restrict elog, const vkaa_syntax_source_s *restrict source, uint32_t syntax_pos, uintptr_t exec_pos, const char *restrict message);
void vkaa_elog_print_exec(const vkaa_elog_s *restrict elog, uintptr_t exec_pos, const char *restrict message);

#endif
