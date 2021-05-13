#ifndef _av1_obu_quantization_params_h_
#define _av1_obu_quantization_params_h_

#include "obu.h"

struct av1_obu_sequence_header_t;

typedef struct av1_quantization_params_t {
	uint8_t base_q_idx;     // [0, 255]
	uint8_t diff_uv_delta;  // 0|1
	uint8_t using_qmatrix;  // 0|1
	uint32_t qm_y;          // [0, 15]
	uint32_t qm_u;          // [0, 15]
	uint32_t qm_v;          // [0, 15]
	int32_t delta_q_ydc;    // [-64, 63]
	int32_t delta_q_udc;    // [-64, 63]
	int32_t delta_q_uac;    // [-64, 63]
	int32_t delta_q_vdc;    // [-64, 63]
	int32_t delta_q_vac;    // [-64, 63]
} av1_quantization_params_t;

av1_quantization_params_t* av1_quantization_params_init(av1_quantization_params_t *restrict qparam);
av1_quantization_params_t* av1_quantization_params_read(av1_quantization_params_t *restrict qparam, av1_bits_reader_t *restrict reader, const struct av1_obu_sequence_header_t *restrict sh);
const av1_quantization_params_t* av1_quantization_params_write(const av1_quantization_params_t *restrict qparam, av1_bits_writer_t *restrict writer, const struct av1_obu_sequence_header_t *restrict sh);
uint64_t av1_quantization_params_bits(const av1_quantization_params_t *restrict qparam, const struct av1_obu_sequence_header_t *restrict sh);
void av1_quantization_params_dump(const av1_quantization_params_t *restrict qparam, mlog_s *restrict mlog);

#endif
