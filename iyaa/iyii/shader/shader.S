#define iyii_shader_include(_name, _path) \
	.align 16;\
	.L##_name##_data:\
	.incbin _path;\
	.L##_name##_end:\
	.type .L##_name##_data, @object;\
	.size .L##_name##_data, .-.L##_name##_data;\
	.align 8;\
	.L##_name##_size:\
	.long .L##_name##_end-.L##_name##_data;\
	.type .L##_name##_size, @object;\
	.size .L##_name##_size, 4;\
	.globl iyii_shader$##_name##$data;\
	.globl iyii_shader$##_name##$size;\
	.set iyii_shader$##_name##$data, .L##_name##_data;\
	.set iyii_shader$##_name##$size, .L##_name##_size

.data
#include "shader.h"
