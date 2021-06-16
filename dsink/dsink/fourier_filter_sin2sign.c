#define _DEFAULT_SOURCE
#include "inner.h"
#include <math.h>

/*
	r = dsink_inner_fourier_sign2sin(...);
	k = rank_sin / rank_sign = 2 * u + 1;
	Ss: r = 4 / (M_PI * k);
	Cc: ((n & 1)?-4:4) / (M_PI * k);
*/

void dsink_inner_fourier_filter_sin2sign(register double *restrict sa, register double *restrict ca, register uintptr_t n)
{
	static const double a = 1;
	register uintptr_t rank_sign, rank_sin, p, k;
	register double c;
	--sa, --ca;
	rank_sign = n;
	for (rank_sign = 1; rank_sign <= n; ++rank_sign)
	{
		// n == 0
		rank_sin = rank_sign;
		p = rank_sign << 1;
		k = 1;
		while ((rank_sin += p) <= n)
		{
			// n & 1 == 1
			k += 2;
			c = a / k;
			sa[rank_sin] -= sa[rank_sign] * c;
			ca[rank_sin] += ca[rank_sign] * c;
			if ((rank_sin += p) > n)
				break;
			// n & 1 == 0
			k += 2;
			c = a / k;
			sa[rank_sin] -= sa[rank_sign] * c;
			ca[rank_sin] -= ca[rank_sign] * c;
		}
	}
}
