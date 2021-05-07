#include "reader.h"

av1_bits_reader_t* av1_bits_reader_init(register av1_bits_reader_t *restrict reader, const uint8_t *restrict data, register uint64_t size, uint32_t reserve_bits)
{
	size <<= 3;
	if (size >= reserve_bits)
		size -= reserve_bits;
	else size = 0;
	reader->saver.cache = 0;
	reader->saver.cbits = 0;
	reader->saver.rbits = size;
	reader->saver.pos = 0;
	reader->data = data;
	reader->nbits = size;
	return reader;
}

void av1_bits_reader_final(av1_bits_reader_t *restrict reader)
{
	av1_bits_reader_init(reader, NULL, 0, 0);
}

void av1_bits_reader_load(av1_bits_reader_t *restrict reader)
{
	const uint8_t *restrict p;
	register uint64_t cache;
	reader->saver.rbits -= reader->saver.cbits;
	p = reader->data + reader->saver.pos;
	if (reader->saver.rbits >= 64)
	{
		cache = p[0];
		cache = (cache << 8) | p[1];
		cache = (cache << 8) | p[2];
		cache = (cache << 8) | p[3];
		cache = (cache << 8) | p[4];
		cache = (cache << 8) | p[5];
		cache = (cache << 8) | p[6];
		cache = (cache << 8) | p[7];
		reader->saver.cache = cache;
		reader->saver.cbits = 64;
		reader->saver.pos += 8;
		return ;
	}
	else if (reader->saver.rbits)
	{
		register uint32_t rb;
		cache = 0;
		switch ((reader->saver.cbits = (uint32_t) (reader->saver.rbits >> 3)) & 7)
		{
			case 7:
				cache = (cache << 8) | *p++;
				// fall through
			case 6:
				cache = (cache << 8) | *p++;
				// fall through
			case 5:
				cache = (cache << 8) | *p++;
				// fall through
			case 4:
				cache = (cache << 8) | *p++;
				// fall through
			case 3:
				cache = (cache << 8) | *p++;
				// fall through
			case 2:
				cache = (cache << 8) | *p++;
				// fall through
			case 1:
				cache = (cache << 8) | *p++;
				// fall through
			default:
				if ((rb = reader->saver.rbits & 3))
					cache = (cache << rb) | (*p >> (8 - rb));
				reader->saver.cache = cache;
				reader->saver.pos += (reader->saver.rbits >> 3) + !!rb;
				return ;
		}
	}
	reader->saver.cache = 0;
	reader->saver.cbits = 0;
}
