#include "largeN.h"

largeN_s* largeN_measure_effective(largeN_s *restrict n)
{
	unit_t e;
	e = n->n;
	do {
		if (n->le[e - 1])
			break;
	} while (--e);
	n->e = e;
	return n;
}
