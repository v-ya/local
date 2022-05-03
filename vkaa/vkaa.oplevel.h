#ifndef _vkaa_oplevel_h_
#define _vkaa_oplevel_h_

#include "vkaa.h"
#include <hashmap.h>

struct vkaa_level_s {
	uintptr_t level;
};

vkaa_oplevel_s* vkaa_oplevel_alloc(void);
void vkaa_oplevel_clear(vkaa_oplevel_s *restrict oplevel);
const vkaa_level_s* vkaa_oplevel_get(const vkaa_oplevel_s *restrict oplevel, const char *restrict name);
const vkaa_level_s* vkaa_oplevel_insert_first(vkaa_oplevel_s *restrict oplevel, const char *restrict name);
const vkaa_level_s* vkaa_oplevel_insert_tail(vkaa_oplevel_s *restrict oplevel, const char *restrict name);
const vkaa_level_s* vkaa_oplevel_insert_last(vkaa_oplevel_s *restrict oplevel, const char *restrict name, const char *restrict target);
const vkaa_level_s* vkaa_oplevel_insert_next(vkaa_oplevel_s *restrict oplevel, const char *restrict name, const char *restrict target);
void vkaa_oplevel_remove(vkaa_oplevel_s *restrict oplevel, const char *restrict name);

#endif
