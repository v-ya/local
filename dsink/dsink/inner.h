#ifndef _dsink_inner_h_
#define _dsink_inner_h_

#include <stdint.h>

typedef struct dsink_fusion_t {
	uint32_t bits;
	uint32_t length;
} dsink_fusion_t;

void dsink_inner_difference(const int32_t *restrict data, int32_t *restrict diff, uintptr_t n, int32_t s);
uintptr_t dsink_inner_fusion(const int32_t *restrict data, uintptr_t n, dsink_fusion_t *restrict fusion);

#endif
