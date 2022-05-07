#ifndef _vkaa_execute_h_
#define _vkaa_execute_h_

#include "vkaa.h"
#include <exbuffer.h>
#include <vattr.h>

typedef struct vkaa_execute_t {
	vkaa_function_s *func;
	uintptr_t jump;
} vkaa_execute_t;

struct vkaa_execute_control_t {
	uintptr_t next_pos;
	const vkaa_execute_t *array;
	uintptr_t number;
};

struct vkaa_execute_s {
	exbuffer_t buffer;
	vkaa_execute_t *execute_array;
	uintptr_t execute_number;
	vattr_s *label;
};

vkaa_execute_s* vkaa_execute_alloc(void);
vkaa_function_s* vkaa_execute_get_last_function(const vkaa_execute_s *restrict exec);
vkaa_var_s* vkaa_execute_get_last_var(const vkaa_execute_s *restrict exec);

uintptr_t vkaa_execute_next_pos(vkaa_execute_s *restrict exec);

// allow loop(label), jump_to_label only add at (the label must exist)
vkaa_execute_s* vkaa_execute_push_label(vkaa_execute_s *restrict exec, const char *restrict label);
vkaa_execute_s* vkaa_execute_pop_label(vkaa_execute_s *restrict exec, const char *restrict label, uintptr_t label_pos);
vkaa_execute_s* vkaa_execute_jump_to_label(vkaa_execute_s *restrict exec, const char *restrict label, uintptr_t stack_pos, uintptr_t jumper_pos);

// label must only, add_jump_any allow add at (the label don't exist)
vkaa_execute_s* vkaa_execute_set_label_without_exist(vkaa_execute_s *restrict exec, const char *restrict label, uintptr_t label_pos);
vkaa_execute_s* vkaa_execute_add_jump_any(vkaa_execute_s *restrict exec, const char *restrict label, uintptr_t jumper_pos);

vkaa_execute_s* vkaa_execute_push(vkaa_execute_s *restrict exec, vkaa_function_s *restrict func);

vkaa_execute_s* vkaa_execute_okay_last_function(vkaa_execute_s *restrict exec);
vkaa_execute_s* vkaa_execute_okay_label(vkaa_execute_s *restrict exec);
vkaa_execute_s* vkaa_execute_okay(vkaa_execute_s *restrict exec);

uintptr_t vkaa_execute_do(const vkaa_execute_s *restrict exec);

void vkaa_execute_clear(vkaa_execute_s *restrict exec);

#endif
