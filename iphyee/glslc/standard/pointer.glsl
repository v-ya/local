#ifndef _iphyee_glsl_standard_pointer_
#define _iphyee_glsl_standard_pointer_

#extension GL_EXT_buffer_reference : require

#define _def_pointer_         layout(buffer_reference) buffer
#define _def_array_(_align_)  layout(buffer_reference, std430, buffer_reference_align = _align_) buffer

#endif
