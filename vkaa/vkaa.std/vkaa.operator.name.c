#include "std.h"
#include <string.h>

const char* vkaa_std_op_testeval_name(vkaa_std_op_name_t *restrict cache, const char *restrict op)
{
	uintptr_t n;
	if ((n = strlen(op)) <= sizeof(vkaa_std_op_name_t) - sizeof(vkaa_std_op_testeval_suffix))
	{
		memcpy(cache->op_name, op, n);
		memcpy(cache->op_name + n, vkaa_std_op_testeval_suffix, sizeof(vkaa_std_op_testeval_suffix));
		return cache->op_name;
	}
	return NULL;
}
