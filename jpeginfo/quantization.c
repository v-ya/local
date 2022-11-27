#include "quantization.h"

quantization_s* quantization_alloc(uintptr_t depth, const void *restrict q64)
{
	quantization_s *restrict r;
	uintptr_t i, n;
	if ((depth == 8 || depth == 16) &&
		(r = (quantization_s *) refer_alloc(sizeof(quantization_s))))
	{
		n = sizeof(r->q) / sizeof(*r->q);
		if (depth == 8)
		{
			for (i = 0; i < n; ++i)
				r->q[i] = (uint32_t) ((const uint8_t *) q64)[i];
			return r;
		}
		else if (depth == 16)
		{
			for (i = 0; i < n; ++i)
				r->q[i] = (uint32_t) ((const uint16_t *) q64)[i];
			return r;
		}
		refer_free(r);
	}
	return NULL;
}
