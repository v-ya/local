#include "writer.h"
#include <stdlib.h>

av1_bits_writer_t* av1_bits_writer_init(register av1_bits_writer_t *restrict writer, register uint64_t init_size)
{
	writer->saver.cache = 0;
	writer->saver.cbits = 0;
	writer->saver.rbits = 0;
	writer->saver.pos = 0;
	writer->data = NULL;
	writer->size = 0;
	if (!init_size)
	{
		label_ok:
		return writer;
	}
	if (init_size < 64)
		init_size = 64;
	--init_size;
	init_size |= init_size >> 32;
	init_size |= init_size >> 16;
	init_size |= init_size >> 8;
	init_size |= init_size >> 4;
	init_size |= init_size >> 2;
	init_size |= init_size >> 1;
	++init_size;
	if ((writer->data = (uint64_t *) malloc(init_size)))
	{
		writer->size = init_size;
		goto label_ok;
	}
	return NULL;
}

void av1_bits_writer_final(av1_bits_writer_t *restrict writer)
{
	if (writer->data)
		free(writer->data);
	av1_bits_writer_init(writer, 0);
}

av1_bits_writer_t* av1_bits_writer_store(av1_bits_writer_t *restrict writer)
{
	uint64_t *restrict data;
	uint64_t size;
	if (writer->saver.pos < writer->size)
	{
		label_store:
		writer->data[writer->saver.pos >> 3] = __builtin_bswap64(writer->saver.cache << (64 - writer->saver.cbits));
		writer->saver.pos = (writer->saver.pos & ~(uint64_t) 3) + ((writer->saver.cbits + 7) >> 3);
		writer->saver.cache = 0;
		writer->saver.cbits = 0;
		return writer;
	}
	size = writer->size << 1;
	if (!size) size = 4096;
	if (size > writer->size && (data = (uint64_t *)(writer->data?realloc(writer->data, size):malloc(size))))
	{
		writer->data = data;
		writer->size = size;
		goto label_store;
	}
	return NULL;
}

const uint8_t* av1_bits_writer_end(av1_bits_writer_t *restrict writer, uint64_t *restrict size, uint32_t *restrict reserve_bits)
{
	if (reserve_bits) *reserve_bits = (64 - writer->saver.cbits) & 7;
	if (!writer->saver.cbits || av1_bits_writer_store(writer))
	{
		if (size) *size = writer->saver.pos;
		return (uint8_t *) writer->data;
	}
	return NULL;
}
