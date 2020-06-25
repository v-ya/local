#ifndef _phloop_entry_arg_h_
#define _phloop_entry_arg_h_

#include <stdint.h>
#include <stddef.h>
#include "phloop.h"

typedef struct args_t {
	phloop_arg_t phloop;
	const char *input;
	uint32_t update_time;
	uint32_t thread;
	double nice;
} args_t;

void args_help(const char *exec, const char *type);
int args_init(args_t *pri, int argc, const char *argv[]);
int args_check(args_t *pri);

#endif
