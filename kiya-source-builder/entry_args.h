#ifndef _entry_args_h_
#define _entry_args_h_

#include <stdint.h>
#include <stddef.h>

typedef struct args_t {
	const char *input;
	const char *output;
	const char *verify;
	const char *version;
	const char *author;
	const char *time;
	const char *description;
	const char *flag;
	const char *sync;
	uintptr_t o_builder_srcipt;
	uintptr_t sync_quiet;
} args_t;

args_t* args_get(args_t *restrict pri, int argc, const char *argv[]);

#endif
