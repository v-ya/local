#ifndef _dsink_inner_h_
#define _dsink_inner_h_

#include <stdint.h>

typedef struct dsink_fusion_t {
	uint32_t bits;
	uint32_t length;
} dsink_fusion_t;

typedef struct dsink_fourier_coefficient_t {
	int32_t sa;
	int32_t ca;
} dsink_fourier_coefficient_t;

typedef struct dsink_fourier_t {
	uintptr_t cycle;
	uint32_t rank;
	int32_t level;
	dsink_fourier_coefficient_t c[16];
} dsink_fourier_t;

void dsink_inner_difference(const int32_t *restrict data, int32_t *restrict diff, uintptr_t n, int32_t s);

uintptr_t dsink_inner_fusion(const int32_t *restrict data, uintptr_t n, dsink_fusion_t *restrict fusion);
uintptr_t dsink_inner_fusion_sink(dsink_fusion_t *restrict fusion, uintptr_t n, uintptr_t threshold);
uintptr_t dsink_inner_fusion_bits(const dsink_fusion_t *restrict fusion, uintptr_t n, uintptr_t *restrict rlength);
void dsink_inner_fusion_limit_bits(dsink_fusion_t *restrict fusion, uintptr_t n, uint32_t max_bits);

int64_t dsink_inner_dot_one(const int32_t *restrict data, uintptr_t n);
void dsink_inner_add_one(int32_t *restrict data, uintptr_t n, int32_t amplitude);

int64_t dsink_inner_dot_sign(const int32_t *restrict data, uintptr_t n, uintptr_t quarter_cycle, uintptr_t phase);
void dsink_inner_add_sign(int32_t *restrict data, uintptr_t n, uintptr_t quarter_cycle, uintptr_t phase, int32_t amplitude);

uintptr_t dsink_inner_fourier_rank(uintptr_t n, uint32_t *restrict rank);
double dsink_inner_fourier_sign2sin(uint32_t rank_sign, uint32_t rank_sin, double q_sign, double q_sin);
void dsink_inner_fourier_filter_sign2sin(double *restrict sa, double *restrict ca, uintptr_t n);
void dsink_inner_fourier_filter_sin2sign(double *restrict sa, double *restrict ca, uintptr_t n);
void dsink_inner_fourier_get(dsink_fourier_t *restrict fourier, const int32_t *restrict data, uintptr_t n);
void dsink_inner_fourier_add(int32_t *restrict data, uintptr_t n, const dsink_fourier_t *restrict fourier);
void dsink_inner_fourier_sub(int32_t *restrict data, uintptr_t n, const dsink_fourier_t *restrict fourier);

#endif
