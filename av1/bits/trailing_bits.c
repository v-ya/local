#include "reader.h"
#include "writer.h"

av1_bits_reader_t* av1_bits_trailing_bits_read(av1_bits_reader_t *restrict r)
{
	register uint32_t bits;
	bits = 8 - ((r->nbits - r->saver.rbits) & 7);
	if (bits <= r->saver.rbits && av1_bits_reader_read(r, bits) == (1ul << (bits - 1)))
		return r;
	return NULL;
}

av1_bits_writer_t* av1_bits_trailing_bits_write(av1_bits_writer_t *restrict w)
{
	register uint32_t bits;
	bits = 8 - (w->saver.rbits & 7);
	return av1_bits_writer_write(w, 1ul << (bits - 1), bits);
}

uint64_t av1_bits_trailing_bits_bits(uint64_t bits)
{
	return 8 - (bits & 7);
}
