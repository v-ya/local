#ifndef _phoneme_entry_arg_h_
#define _phoneme_entry_arg_h_

#include <stdint.h>
#include <stddef.h>

typedef struct args_t {
	const char *input;
	const char *output;
	const char *core_path;
	const char *package_path;
	double base_time;
	double base_volume;
	double base_fre_line;
	double base_fre_step;
	double space_time;
	uint32_t sdmax;
	uint32_t dmax;
	uint32_t vstack_script_max;
	uint32_t vstack_phoneme_max;
	uint32_t sampfre;
	uint32_t thread;
	double nice;
	size_t xmsize;
} args_t;

void args_help(const char *exec, const char *type);
int args_init(args_t *pri, int argc, const char *argv[]);
int args_check(args_t *pri);

#endif
