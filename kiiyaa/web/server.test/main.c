#include "statistics.h"

statistics_s *statistics;

void finally(void)
{
	statistics_s *r;
	if ((r = statistics))
	{
		refer_free(r);
		statistics = NULL;
	}
}
