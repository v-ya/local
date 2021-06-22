#ifndef _chord_zzz_h_
#define _chord_zzz_h_

#include <stdint.h>
#include <math.h>

void chord_zzz_add_sin_f64(double *restrict p, uintptr_t n, double a, double w, double q);
void chord_zzz_add_sin_f32(float *restrict p, uintptr_t n, float a, float w, float q);
void chord_zzz_add_sin_s32(int32_t *restrict p, uintptr_t n, double a, double w, double q);
void chord_zzz_add_sin_s16(int16_t *restrict p, uintptr_t n, float a, float w, float q);

#endif
