#include "inner.h"

uintptr_t dsink_inner_fusion(register const int32_t *restrict data, register uintptr_t n, register dsink_fusion_t *restrict fusion)
{
	uintptr_t nf;
	register uintptr_t i;
	register uint32_t bits;
	register uint32_t length;
	register uint32_t nbits;
	nf = i = 0;
	if (!n) goto label_return;
	if ((nbits = (uint32_t) *data))
		nbits = 32 - __builtin_clrsb(nbits);
	goto label_entry;
	do {
		if ((nbits = (uint32_t) data[i]))
			nbits = 32 - __builtin_clrsb(nbits);
		if (nbits != bits)
		{
			fusion[nf].bits = bits;
			fusion[nf].length = length;
			++nf;
			label_entry:
			bits = nbits;
			length = 0;
		}
		++length;
	} while (++i < n);
	fusion[nf].bits = bits;
	fusion[nf].length = length;
	++nf;
	label_return:
	return nf;
}
