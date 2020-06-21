#ifndef _phloop_h_
#define _phloop_h_

#include <stdint.h>
#include <refer.h>
#include <phoneme/phoneme_script.h>
#include <mlog.h>
#include <audioloop.h>

typedef struct phloop_arg_t {
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
	uint32_t usleep_time;
	size_t xmsize;
} phloop_arg_t;

typedef struct phloop_s {
	aloop_s *aloop;
	phoneme_output_s *output;
	mlog_s *mlog;
	phloop_arg_t arg;
} phloop_s;

phloop_s* phloop_alloc(phloop_arg_t *restrict arg, phoneme_output_s *restrict output, mlog_s *restrict mlog);
phloop_s* phloop_update(phloop_s *restrict phloop, const char *script_path);
void phloop_uini(void);

#endif
