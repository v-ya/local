#include "largeN.h"

largeN_s* largeN_measure_effective(largeN_s *restrict n)
{
	unit_t e;
	e = n->n;
	while (--e && !n->le[e]) ;
	n->e = e + !!n->le[e];
	return n;
}
