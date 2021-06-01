#include "inner.h"

void dsink_inner_add_sin(register int32_t *restrict data, register uintptr_t n, register uintptr_t quarter_cycle, register uintptr_t phase, register int32_t amplitude)
{
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
			*data++ += amplitude;
			if (!--n) goto label_return;
		} while (++phase < quarter_cycle);
		phase = 0;
		label_sin_pi_2pi:
		do {
			*data++ -= amplitude;
			if (!--n) goto label_return;
		} while (++phase < quarter_cycle);
		phase = 0;
	} while (1);
	label_return:
	return ;
}
