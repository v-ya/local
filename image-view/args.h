#ifndef _image_view_args_h_
#define _image_view_args_h_

#include <stdint.h>
#include <stddef.h>

typedef struct args_t {
	const char *image_path;
	uint32_t multicalc;
	uint32_t bgcolor;
	uint32_t shm_disable;
	uint32_t shm_size;
	uintptr_t have_help;
} args_t;

args_t* args_get(args_t *restrict args, int argc, const char *argv[]);

#endif
