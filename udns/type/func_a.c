#include "func.h"
#include "inner.h"
#include <stdio.h>

typedef struct udns_type_arg_stack_t {
	uint8_t ipv4[4];
} udns_type_arg_stack_t;

inner_type_initial(a)
{
	*(uint32_t *) arg->ipv4 = 0;
}

inner_type_parse_length(a)
{
	if (!data)
		goto label_okay;
	if (pos + 4 == size)
	{
		data += pos;
		arg->ipv4[0] = data[0];
		arg->ipv4[1] = data[1];
		arg->ipv4[2] = data[2];
		arg->ipv4[3] = data[3];
		label_okay:
		return 16;
	}
	return ~(uintptr_t) 0;
}

inner_type_build_length(a)
{
	if (!parse || udns_inner_get_ipv4(parse, arg->ipv4))
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
