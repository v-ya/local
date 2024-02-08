#include "mat4x4.h"
#include <memory.h>

void iphyee_mat4x4_cpy(iphyee_mat4x4_t *restrict dst, const iphyee_mat4x4_t *restrict src)
{
	memcpy(dst, src, sizeof(iphyee_mat4x4_t));
}
