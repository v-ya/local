#include "inner.h"

int64_t dsink_inner_dot_sign(register const int32_t *restrict data, register uintptr_t n, register uintptr_t quarter_cycle, register uintptr_t phase)
{
	register int64_t r = 0;
	if (!n || !quarter_cycle) goto label_return;
	quarter_cycle <<= 1;
	phase %= (quarter_cycle << 1);
	if (phase < quarter_cycle)
		goto label_sin_0_pi;
	phase -= quarter_cycle;
	goto label_sin_pi_2pi;
	do {
		label_sin_0_pi:
		do {
			r += *data++;
			if (!--n) goto label_return;
		} while (++phase < quarter_cycle);
		phase = 0;
		label_sin_pi_2pi:
		do {
			r -= *data++;
			if (!--n) goto label_return;
		} while (++phase < quarter_cycle);
		phase = 0;
	} while (1);
	label_return:
	return r;
}
