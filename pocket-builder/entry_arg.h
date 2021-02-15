#ifndef _entry_arg_h_
#define _entry_arg_h_

#include <stdint.h>
#include <stddef.h>

typedef struct args_t {
	const char *input;
	const char *output;
} args_t;

void args_help(const char *exec);
int args_init(args_t *pri, int argc, const char *argv[]);

#endif
