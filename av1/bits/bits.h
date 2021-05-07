#ifndef _av1_bits_h_
#define _av1_bits_h_

#include <stdint.h>
#include <stddef.h>

typedef struct av1_bits_saver_t {
	uint64_t cache;
	uint32_t cbits;
	uint64_t rbits;
	uint64_t pos;
} av1_bits_saver_t;

typedef struct av1_bits_reader_t {
	av1_bits_saver_t saver;
	const uint8_t *data;
	uint64_t nbits;
} av1_bits_reader_t;

typedef struct av1_bits_writer_t {
	av1_bits_saver_t saver;
	uint64_t *restrict data;
	uint64_t size;
} av1_bits_writer_t;

av1_bits_reader_t* av1_bits_reader_init(av1_bits_reader_t *restrict reader, const uint8_t *restrict data, uint64_t size, uint32_t reserve_bits);
av1_bits_writer_t* av1_bits_writer_init(av1_bits_writer_t *restrict writer, uint64_t init_size);

void av1_bits_reader_final(av1_bits_reader_t *restrict reader);
void av1_bits_writer_final(av1_bits_writer_t *restrict writer);

const uint8_t* av1_bits_writer_end(av1_bits_writer_t *restrict writer, uint64_t *restrict size, uint32_t *restrict reserve_bits);

// base bits type

av1_bits_reader_t* av1_bits_uint_read(av1_bits_reader_t *restrict r, uint64_t *restrict value, uint32_t bits);
av1_bits_writer_t* av1_bits_uint_write(av1_bits_writer_t *restrict w, uint64_t value, uint32_t bits);
av1_bits_reader_t* av1_bits_int_read(av1_bits_reader_t *restrict r, int64_t *restrict value, uint32_t bits);
av1_bits_writer_t* av1_bits_int_write(av1_bits_writer_t *restrict w, int64_t value, uint32_t bits);

av1_bits_reader_t* av1_bits_leb128_read(av1_bits_reader_t *restrict r, uint64_t *restrict value);
av1_bits_writer_t* av1_bits_leb128_write(av1_bits_writer_t *restrict w, uint64_t value);

#endif
