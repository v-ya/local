#ifndef _chord_zzz_h_
#define _chord_zzz_h_

#include <stdint.h>
#include <math.h>

#include "../chord_aq_builder.h"

#define chord_zzz_block    128
#define chord_zzz_s32_max  0x7fffffff
#define chord_zzz_s16_max  0x7fff

void chord_zzz_add_sin_f64(double *restrict p, uintptr_t n, double a, double w, double q);
void chord_zzz_add_sin_f32(float *restrict p, uintptr_t n, float a, float w, float q);
void chord_zzz_add_sin_s32(int32_t *restrict p, uintptr_t n, double a, double w, double q);
void chord_zzz_add_sin_s16(int16_t *restrict p, uintptr_t n, float a, float w, float q);

void chord_zzz_add_cute_sin_f64(double *restrict p, uintptr_t n, chord_aq_builder_s *restrict aq, intptr_t aq_index);
void chord_zzz_add_cute_sin_f32(float *restrict p, uintptr_t n, chord_aq_builder_s *restrict aq, intptr_t aq_index);
void chord_zzz_add_cute_sin_s32(int32_t *restrict p, uintptr_t n, chord_aq_builder_s *restrict aq, intptr_t aq_index);
void chord_zzz_add_cute_sin_s16(int16_t *restrict p, uintptr_t n, chord_aq_builder_s *restrict aq, intptr_t aq_index);

#endif
