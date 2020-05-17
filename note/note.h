#ifndef _note_h_
#define _note_h_

#include <stdint.h>
#include <refer.h>
#include "note_details.h"

typedef double (*note_envelope_f)(double t, double volume, refer_t pri);
typedef double (*note_base_frequency_f)(double t, double a, refer_t pri);
typedef void (*note_details_f)(note_details_s *d, double w, refer_t pri);

typedef struct note_details_stage_t {
	note_details_f func;
	refer_t pri;
} note_details_stage_t;

typedef struct note_s {
	// 细节
	note_details_s *details;
	// 包络
	note_envelope_f envelope;
	refer_t envelope_pri;
	// 类周期基频
	note_base_frequency_f base_frequency;
	refer_t base_frequency_pri;
	// 类周期细节生成链
	uint32_t stage_used;
	uint32_t stage_max;
	note_details_stage_t stage[];
} note_s;

note_s* note_alloc(uint32_t details_max, uint32_t stage_max);
void note_set_envelope(note_s *restrict n, note_envelope_f envelope, refer_t pri);
void note_set_base_frequency(note_s *restrict n, note_base_frequency_f base_frequency, refer_t pri);
note_s* note_set_stage(note_s *restrict n, uint32_t i, note_details_f func, refer_t pri);
note_s* note_append_stage(note_s *restrict n, note_details_f func, refer_t pri);
void note_random_phase(note_s *restrict n);
void note_gen(note_s *restrict n, double t, double volume, double *v, uint32_t frames, uint32_t sampfre);
void note_gen_with_pos(note_s *restrict n, double t, double volume, double *v, uint32_t frames, uint32_t sampfre, uint32_t pos);

#endif
