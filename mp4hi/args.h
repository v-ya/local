#ifndef _mp4hi_args_h_
#define _mp4hi_args_h_

#include <stdint.h>
#include <stddef.h>

typedef struct args_t {
	const char *input;
	uintptr_t dump_samples;
	uintptr_t libmpeg4_verbose;
	uintptr_t have_help;
} args_t;

args_t* args_get(args_t *restrict args, int argc, const char *argv[]);

#endif
