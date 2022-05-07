#include "std.h"
#include <memory.h>

const char* vkaa_std_op_pre_name(vkaa_std_op_pre_t *restrict cache, refer_nstring_t op)
{
	if (op->length <= sizeof(vkaa_std_op_pre_t) - sizeof(vkaa_std_op_preprocess_suffix))
	{
		memcpy(cache->op_pre_name, op->string, op->length);
		memcpy(cache->op_pre_name + op->length, vkaa_std_op_preprocess_suffix, sizeof(vkaa_std_op_preprocess_suffix));
		return cache->op_pre_name;
	}
	return NULL;
}
