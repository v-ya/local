#include "reader.h"
#include "writer.h"

static const uint64_t av1_bits_uint_mask[64] = {
	#define d_mask(_n)     [(_n)] = (~(uint64_t) 0 >> ((64 - (_n)) & 63))
	#define d_mask_4(_b)   d_mask(_b), d_mask(_b + 1), d_mask(_b + 2), d_mask(_b + 3)
	#define d_mask_16(_b)  d_mask_4(_b), d_mask_4(_b + 4), d_mask_4(_b + 8), d_mask_4(_b + 12)
	d_mask_16(0),
	d_mask_16(16),
	d_mask_16(32),
	d_mask_16(48)
	#undef d_mask_16
	#undef d_mask_4
	#undef d_mask
};

av1_bits_reader_t* av1_bits_uint_read(av1_bits_reader_t *restrict r, uint64_t *restrict value, uint32_t bits)
{
	if (bits <= r->saver.rbits)
	{
		*value = av1_bits_reader_read(r, bits);
		return r;
	}
	return NULL;
}

av1_bits_writer_t* av1_bits_uint_write(av1_bits_writer_t *restrict w, uint64_t value, uint32_t bits)
{
	return av1_bits_writer_write(w, value & av1_bits_uint_mask[bits & 63], bits);
}

av1_bits_reader_t* av1_bits_int_read(av1_bits_reader_t *restrict r, int64_t *restrict value, uint32_t bits)
{
	if (bits <= r->saver.rbits)
	{
		register uint64_t mask;
		if ((*value = av1_bits_reader_read(r, bits)) & ((mask = ~av1_bits_uint_mask[bits & 63]) >> 1))
			*value |= mask;
		return r;
	}
	return NULL;
}

av1_bits_writer_t* av1_bits_int_write(av1_bits_writer_t *restrict w, int64_t value, uint32_t bits)
{
	return av1_bits_writer_write(w, value & av1_bits_uint_mask[bits & 63], bits);
}
