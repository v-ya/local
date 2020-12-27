#ifndef _iyii_shader_h_
#define _iyii_shader_h_

#ifndef iyii_shader_include
#include <stdint.h>
#define iyii_shader_include(_name, _path) \
	extern uint8_t iyii_shader$##_name##$data[];\
	extern uint32_t iyii_shader$##_name##$size
#endif

iyii_shader_include(vert, "iyii/shader/iyii.vert.spv");
iyii_shader_include(frag, "iyii/shader/iyii.frag.spv");

#endif
