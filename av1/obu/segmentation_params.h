#ifndef _av1_obu_segmentation_params_h_
#define _av1_obu_segmentation_params_h_

#include "obu.h"

typedef struct av1_segmentation_params_t {
	uint8_t segmentation_enabled;                        // 0|1
	uint8_t segmentation_update_map;                     // 0|1
	uint8_t segmentation_temporal_update;                // 0|1
	uint8_t segmentation_update_data;                    // 0|1
	uint32_t SegIdPreSkip;
	uint32_t LastActiveSegId;
	uint8_t feature_enabled[MAX_SEGMENTS][SEG_LVL_MAX];  // 0|1
	int32_t feature_data[MAX_SEGMENTS][SEG_LVL_MAX];     // [-255, 255]
} av1_segmentation_params_t;

av1_segmentation_params_t* av1_segmentation_params_init(av1_segmentation_params_t *restrict sparam);
av1_segmentation_params_t* av1_segmentation_params_read(av1_segmentation_params_t *restrict sparam, av1_bits_reader_t *restrict reader, uint8_t primary_ref_frame);
const av1_segmentation_params_t* av1_segmentation_params_write(const av1_segmentation_params_t *restrict sparam, av1_bits_writer_t *restrict writer, uint8_t primary_ref_frame);
uint64_t av1_segmentation_params_bits(const av1_segmentation_params_t *restrict sparam, uint8_t primary_ref_frame);
void av1_segmentation_params_dump(const av1_segmentation_params_t *restrict sparam, mlog_s *restrict mlog);

#endif
