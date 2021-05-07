#ifndef _av1_bits_reader_h_
#define _av1_bits_reader_h_

#include "bits.h"

static inline uint64_t av1_bits_reader_read(register av1_bits_reader_t *restrict reader, register uint32_t bits_must_le_64)
{
	void av1_bits_reader_load(av1_bits_reader_t *restrict reader);
	register uint64_t r;
	if (bits_must_le_64 <= reader->saver.cbits)
	{
		label_re_read:
		r = reader->saver.cache >> (64 - bits_must_le_64);
		reader->saver.cache <<= bits_must_le_64;
		reader->saver.cbits -= bits_must_le_64;
		reader->saver.rbits -= bits_must_le_64;
		return r;
	}
	else if (bits_must_le_64 <= reader->saver.rbits)
	{
		if (reader->saver.cbits)
		{
			r = reader->saver.cache >> (64 - reader->saver.cbits);
			bits_must_le_64 -= reader->saver.cbits;
			av1_bits_reader_load(reader);
			r = (r << bits_must_le_64) | (reader->saver.cache >> (64 - bits_must_le_64));
			reader->saver.cache <<= bits_must_le_64;
			reader->saver.cbits -= bits_must_le_64;
			reader->saver.rbits -= bits_must_le_64;
		}
		else
		{
			av1_bits_reader_load(reader);
			goto label_re_read;
		}
	}
	return 0;
}

#endif
