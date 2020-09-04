#include "random_check_pri.h"

struct random_check_layer_bits_s {
	random_check_layer_s layer;
	uint32_t bits;
	uint32_t bits_mask;
	uint32_t number;
	uint32_t sum[];
};

random_check_define_layer_check(bits)
{
	register uint32_t *restrict sum;
	register uint32_t mask;
	register uint32_t i = 32;
	sum = r->sum;
	mask = r->bits_mask;
	do {
		sum[x & mask] += 1;
		x = (x << 1) | (x >> 31);
	} while (--i);
}

random_check_define_layer_clear(bits)
{
	memset(r->sum, 0, sizeof(uint32_t) * r->number);
}

random_check_define_layer_dump(bits)
{
	mlog_printf(ml, "[layer bits %2u] ", r->bits);
	random_check_dump_statistics(ml, r->sum, r->number);
}

random_check_s* random_check_layer_bits(random_check_s *restrict rc, uint32_t bits)
{
	if (bits && bits <= 24)
	{
		register struct random_check_layer_bits_s *restrict r;
		register uint32_t n;
		n = 1u << bits;
		r = (struct random_check_layer_bits_s *) refer_alloz(sizeof(struct random_check_layer_bits_s) + sizeof(uint32_t) * n);
		if (r)
		{
			r->bits = bits;
			r->bits_mask = n - 1;
			r->number = n;
			random_check_append_layer(rc, r->layer, bits);
			return rc;
		}
	}
	return NULL;
}
