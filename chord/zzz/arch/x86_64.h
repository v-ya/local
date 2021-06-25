#ifndef _chord_zzz_arch_h_
#define _chord_zzz_arch_h_

#include "../_.h"

void chord_zzz_arch_inner_line_f64(double *restrict r, uintptr_t n, uintptr_t i, double k, double b);
void chord_zzz_arch_inner_cfma_f64(double *restrict r, const double *restrict s, uintptr_t n, double k);
void chord_zzz_arch_inner_cfma_f64s32(int32_t *restrict r, const double *restrict s, uintptr_t n, double k);

void chord_zzz_arch_inner_sin_nf64(double *restrict r, const double *restrict v, uintptr_t block, uintptr_t n);

void chord_zzz_arch_inner_line_f32(float *restrict r, uintptr_t n, uintptr_t i, float k, float b);
void chord_zzz_arch_inner_cfma_f32(float *restrict r, const float *restrict s, uintptr_t n, float k);
void chord_zzz_arch_inner_cfma_f32s16(int16_t *restrict r, const float *restrict s, uintptr_t n, float k);

void chord_zzz_arch_inner_sin_nf32(float *restrict r, const float *restrict v, uintptr_t block, uintptr_t n);

#endif
