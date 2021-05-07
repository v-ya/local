#ifndef _av1_bits_writer_h_
#define _av1_bits_writer_h_

#include "bits.h"

static inline av1_bits_writer_t* av1_bits_writer_write(register av1_bits_writer_t *restrict writer, register uint64_t value, register uint32_t bits_must_le_64)
{
	av1_bits_writer_t* av1_bits_writer_store(av1_bits_writer_t *restrict writer);
	if (writer->saver.cbits + bits_must_le_64 < 64)
	{
		label_re_write:
		writer->saver.cache = (writer->saver.cache << bits_must_le_64) | value;
		writer->saver.cbits += bits_must_le_64;
		writer->saver.rbits += bits_must_le_64;
		return writer;
	}
	if (writer->saver.cbits)
	{
		register uint32_t sb = 64 - writer->saver.cbits;
		bits_must_le_64 -= sb;
		writer->saver.cache = (writer->saver.cache << sb) | (value >> bits_must_le_64);
		writer->saver.cbits += sb;
		writer->saver.rbits += sb;
		// must bits_must_le_64 < 64
		value &= ((uint64_t) 1 << bits_must_le_64) - 1;
		if (av1_bits_writer_store(writer))
			goto label_re_write;
		writer->saver.cache >>= sb;
		writer->saver.cbits -= sb;
		writer->saver.rbits -= sb;
		return NULL;
	}
	else
	{
		// must bits_must_le_64 == 64 and writer->cbits == 0
		writer->saver.cache = value;
		writer->saver.cbits = bits_must_le_64;
		writer->saver.rbits += bits_must_le_64;
		return av1_bits_writer_store(writer);
	}
}

#endif
