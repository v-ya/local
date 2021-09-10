#include "func.h"
#include <stdio.h>

typedef struct udns_type_arg_stack_t {
	uint8_t ipv4[4];
} udns_type_arg_stack_t;

static uint8_t* inner_get_ipv4(const char *restrict s_ipv4, uint8_t *restrict a_ipv4)
{
	uint32_t a, i;
	for (i = 4; i; ++s_ipv4)
	{
		a = 0;
		for (;;)
		{
			if (*s_ipv4 >= '0' && *s_ipv4 <= '9')
				a = a * 10 + *s_ipv4 - '0';
			else if (!*s_ipv4 || *s_ipv4 == '.')
				break;
			else goto label_fail;
			if (a > 255)
				goto label_fail;
			++s_ipv4;
		}
		*a_ipv4++ = (uint8_t) a;
		--i;
		if (!*s_ipv4)
			break;
	}
	if (!i) return a_ipv4;
	label_fail:
	return NULL;
}

inner_type_initial(a)
{
	*(uint32_t *) arg->ipv4 = 0;
}

inner_type_parse_length(a)
{
	if (length == 4)
	{
		arg->ipv4[0] = data[0];
		arg->ipv4[1] = data[1];
		arg->ipv4[2] = data[2];
		arg->ipv4[3] = data[3];
		return 16;
	}
	return ~(uintptr_t) 0;
}

inner_type_build_length(a)
{
	if (inner_get_ipv4(parse, arg->ipv4))
		return 4;
	return ~(uintptr_t) 0;
}

inner_type_parse_write(a)
{
	if (sprintf(parse, "%u.%u.%u.%u", arg->ipv4[0], arg->ipv4[1], arg->ipv4[2], arg->ipv4[3]) >= 0)
		return arg;
	return NULL;
}

inner_type_build_write(a)
{
	data[0] = arg->ipv4[0];
	data[1] = arg->ipv4[1];
	data[2] = arg->ipv4[2];
	data[3] = arg->ipv4[3];
	return arg;
}
