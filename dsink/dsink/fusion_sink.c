#include "inner.h"

uintptr_t dsink_inner_fusion_sink(register dsink_fusion_t *restrict fusion, register uintptr_t n, register uintptr_t threshold)
{
	register uintptr_t i, p;
	i = 0;
	for (i = 0, p = 1; p < n; ++p)
	{
		if (fusion[p].bits >= fusion[i].bits)
		{
			if ((uintptr_t) (fusion[p].bits - fusion[i].bits) * fusion[i].length < threshold)
			{
				fusion[i].bits = fusion[p].bits;
				fusion[i].length += fusion[p].length;
				if (i) goto label_refilter;
				continue;
			}
		}
		else
		{
			if ((uintptr_t) (fusion[i].bits - fusion[p].bits) * fusion[p].length < threshold)
			{
				fusion[i].length += fusion[p].length;
				if (i) goto label_refilter;
				continue;
			}
		}
		++i;
		fusion[i].bits = fusion[p].bits;
		fusion[i].length = fusion[p].length;
		continue;
		label_refilter:
		if (fusion[i].bits >= fusion[i - 1].bits)
		{
			if ((uintptr_t) (fusion[i].bits - fusion[i - 1].bits) * fusion[i - 1].length < threshold)
			{
				--i;
				fusion[i].bits = fusion[i + 1].bits;
				fusion[i].length += fusion[i + 1].length;
				if (i) goto label_refilter;
			}
		}
		else
		{
			if ((uintptr_t) (fusion[i - 1].bits - fusion[i].bits) * fusion[i].length < threshold)
			{
				--i;
				fusion[i].length += fusion[i + 1].length;
				if (i) goto label_refilter;
			}
		}
		
	}
	return i + 1;
}
