#ifndef _chord_aq_builder_h_
#define _chord_aq_builder_h_

#include <refer.h>

typedef struct chord_aq_builder_s chord_aq_builder_s;

typedef void (*chord_aq_builder_get_f64_f)(chord_aq_builder_s *restrict builder, uintptr_t n, double *restrict a, double *restrict q, intptr_t index);
typedef void (*chord_aq_builder_get_f32_f)(chord_aq_builder_s *restrict builder, uintptr_t n, float *restrict a, float *restrict q, intptr_t index);

struct chord_aq_builder_s {
	chord_aq_builder_get_f64_f get64;
	chord_aq_builder_get_f32_f get32;
};

#endif
