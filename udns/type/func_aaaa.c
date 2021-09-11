#include "func.h"
#include "inner.h"
#include <memory.h>

typedef struct udns_type_arg_stack_t {
	uint8_t ipv6[16];
} udns_type_arg_stack_t;

inner_type_initial(aaaa)
{
	memset(arg->ipv6, 0, sizeof(arg->ipv6));
}

inner_type_parse_length(aaaa)
{
	if (!data)
		goto label_okay;
	if (pos + 16 == size)
	{
		data += pos;
		memcpy(arg->ipv6, data, sizeof(arg->ipv6));
		label_okay:
		return 40;
	}
	return ~(uintptr_t) 0;
}

inner_type_build_length(aaaa)
{
	if (!parse || udns_inner_get_ipv6(parse, arg->ipv6))
		return 16;
	return ~(uintptr_t) 0;
}

inner_type_parse_write(aaaa)
{
	static const char hex[16] = "0123456789abcdef";
	uintptr_t i, p;
	for (i = 8, p = 0; i; --i)
	{
		*parse++ = hex[arg->ipv6[p] >> 4];
		*parse++ = hex[arg->ipv6[p++] & 15];
		*parse++ = hex[arg->ipv6[p] >> 4];
		*parse++ = hex[arg->ipv6[p++] & 15];
		*parse++ = ':';
	}
	*--parse = 0;
	return arg;
}

inner_type_build_write(aaaa)
{
	memcpy(data, arg->ipv6, sizeof(arg->ipv6));
	return arg;
}
