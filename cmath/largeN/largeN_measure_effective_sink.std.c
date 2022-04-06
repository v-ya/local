#include "largeN.h"

largeN_s* largeN_measure_effective_sink(largeN_s *restrict n, unit_t last_e)
{
	if (last_e)
	{
		do {
			if (n->le[last_e - 1])
				break;
		} while (--last_e);
	}
	n->e = last_e;
	return n;
}
