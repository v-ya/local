#ifndef _vkaa_h_
#define _vkaa_h_

#include <refer.h>

typedef struct vkaa_syntaxor_s vkaa_syntaxor_s;
typedef struct vkaa_syntax_source_s vkaa_syntax_source_s;
typedef struct vkaa_syntax_s vkaa_syntax_s;
typedef struct vkaa_oplevel_s vkaa_oplevel_s;
typedef struct vkaa_level_s vkaa_level_s;
typedef struct vkaa_error_s vkaa_error_s;
typedef struct vkaa_elog_s vkaa_elog_s;
typedef struct vkaa_tpool_s vkaa_tpool_s;
typedef struct vkaa_type_s vkaa_type_s;
typedef struct vkaa_var_s vkaa_var_s;
typedef struct vkaa_vclear_s vkaa_vclear_s;
typedef struct vkaa_scope_s vkaa_scope_s;
typedef struct vkaa_selector_s vkaa_selector_s;
typedef struct vkaa_function_s vkaa_function_s;
typedef struct vkaa_execute_s vkaa_execute_s;
typedef struct vkaa_parse_s vkaa_parse_s;

typedef enum vkaa_syntax_type_t vkaa_syntax_type_t;
typedef struct vkaa_syntax_t vkaa_syntax_t;
typedef struct vkaa_execute_control_t vkaa_execute_control_t;

// return error id, 0 is okay
typedef uintptr_t (*vkaa_function_f)(const vkaa_function_s *restrict r, vkaa_execute_control_t *restrict control);

#endif
