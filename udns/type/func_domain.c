#include "func.h"
#include "inner.h"
#include <memory.h>

typedef struct udns_type_arg_stack_t {
	const uint8_t *data;
	const char *string;
	uintptr_t n_data;
	uintptr_t n_string;
	udns_inner_labels_t labels_data;
	udns_inner_labels_t labels_string;
} udns_type_arg_stack_t;

inner_type_initial(domain)
{
	arg->data = NULL;
	arg->string = NULL;
	arg->n_data = 0;
	arg->n_string = 0;
}

inner_type_parse_length(domain)
{
	if (!data)
	{
		if (arg->n_string)
			goto label_okay;
	}
	else if (udns_inner_labels2string(&arg->labels_string, data, size, &pos, &arg->n_string) &&
		udns_inner_labels4string(&arg->labels_data, (const char *) arg->labels_string.data, &arg->n_data))
	{
		arg->n_string += 1;
		arg->data = (const uint8_t *) arg->labels_data.data;
		arg->string = (const char *) arg->labels_string.data;
		label_okay:
		return arg->n_string;
	}
	return ~(uintptr_t) 0;
}

inner_type_build_length(domain)
{
	if (!parse)
	{
		if (arg->n_data)
			goto label_okay;
	}
	else if (udns_inner_labels4string(&arg->labels_data, parse, &arg->n_data))
	{
		arg->data = (const uint8_t *) arg->labels_data.data;
		label_okay:
		return arg->n_data;
	}
	return ~(uintptr_t) 0;
}

inner_type_parse_write(domain)
{
	if (arg->string)
	{
		memcpy(parse, arg->string, arg->n_string);
		return arg;
	}
	return NULL;
}

inner_type_build_write(domain)
{
	if (arg->data)
	{
		memcpy(data, arg->data, arg->n_data);
		return arg;
	}
	return NULL;
}
