#ifndef _av1_obu_header_h_
#define _av1_obu_header_h_

#include "obu.h"

typedef struct av1_obu_header_t {
	av1_obu_type_t type;
	uint8_t extension_flag;  // 0|1
	uint8_t has_size_field;  // 0|1
	uint8_t temporal_id;     // [0, 7]
	uint8_t spatial_id;      // [0, 3]
	uint64_t size;
} av1_obu_header_t;

av1_obu_header_t* av1_obu_header_init(av1_obu_header_t *restrict header);
av1_obu_header_t* av1_obu_header_read(av1_obu_header_t *restrict header, av1_bits_reader_t *restrict reader);
const av1_obu_header_t* av1_obu_header_write(const av1_obu_header_t *restrict header, av1_bits_writer_t *restrict writer);
uint64_t av1_obu_header_bits(const av1_obu_header_t *restrict header);
void av1_obu_header_dump(const av1_obu_header_t *restrict header, mlog_s *restrict mlog);

#endif
