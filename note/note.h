#ifndef _note_h_
#define _note_h_

#include <stdint.h>
#include <refer.h>
#include "note_details.h"

typedef double (*note_envelope_f)(double t, refer_t pri);
typedef double (*note_base_frequency_f)(double t, double a, refer_t pri);
typedef void (*note_details_f)(note_details_s *d, double w, refer_t pri);

typedef struct note_s {
	// 包络
	note_envelope_f envelope;
	refer_t envelope_pri;
	// 类周期基频
	note_base_frequency_f base_frequency;
	refer_t base_frequency_pri;
	// 类周期细节调整: 音素层
	note_details_f phoneme;
	refer_t phoneme_pri;
	// 类周期细节填充: 音色层
	note_details_f tone;
	refer_t tone_pri;
	// 细节
	note_details_s *details;
} note_s;

note_s* note_alloc(uint32_t details_max);
void note_set_envelope(note_s *n, note_envelope_f envelope, refer_t pri);
void note_set_base_frequency(note_s *n, note_base_frequency_f base_frequency, refer_t pri);
void note_set_phoneme(note_s *n, note_details_f phoneme, refer_t pri);
void note_set_tone(note_s *n, note_details_f tone, refer_t pri);
void note_random_phase(note_s *n);
void note_gen(note_s *n, double t, double *v, uint32_t frames, uint32_t sampfre);
void note_gen_with_pos(note_s *n, double t, double *v, uint32_t frames, uint32_t sampfre, uint32_t pos);

#endif
