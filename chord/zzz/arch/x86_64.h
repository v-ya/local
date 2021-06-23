#ifndef _chord_zzz_arch_h_
#define _chord_zzz_arch_h_

#include <stdint.h>

#define chord_zzz_sin_block  128

void chord_zzz_arch_inner_line_f64(double *restrict r, uintptr_t n, uintptr_t i, double k, double b);
void chord_zzz_arch_inner_cfma_f64(double *restrict r, const double *restrict s, uintptr_t n, double k);

void chord_zzz_arch_sin_nf64(double *restrict r, const double *restrict v, uintptr_t block, uintptr_t n);

#endif
