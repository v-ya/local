#ifndef _av1_obu_uncompressed_header_h_
#define _av1_obu_uncompressed_header_h_

#include "obu.h"
#include "frame_render_size.h"
#include "tile_info.h"
#include "quantization_params.h"
#include "segmentation_params.h"

struct av1_obu_header_t;

typedef struct av1_uncompressed_header_extra_t {
	av1_frame_render_size_t frame_render_size;
	av1_tile_info_t tile_info;
	av1_quantization_params_t quantization_params;
	av1_segmentation_params_t segmentation_params;
} av1_uncompressed_header_extra_t;

typedef struct av1_uncompressed_header_t {
	uint8_t show_existing_frame;                      // 0|1
	uint8_t show_frame;                               // 0|1
	uint8_t showable_frame;                           // 0|1
	uint8_t error_resilient_mode;                     // 0|1
	uint8_t disable_cdf_update;                       // 0|1
	uint8_t allow_screen_content_tools;               // 0|1
	uint8_t force_integer_mv;                         // 0|1
	uint8_t frame_size_override_flag;                 // 0|1
	uint8_t buffer_removal_time_present_flag;         // 0|1
	uint8_t allow_intrabc;                            // 0|1
	uint8_t frame_refs_short_signaling;               // 0|1
	uint8_t found_ref;                                // 0|1
	uint8_t allow_high_precision_mv;                  // 0|1
	uint8_t is_filter_switchable;                     // 0|1
	uint8_t is_motion_mode_switchable;                // 0|1
	uint8_t use_ref_frame_mvs;                        // 0|1
	uint8_t disable_frame_end_update_cdf;             // 0|1
	uint8_t primary_ref_frame;                        // [0, 7]
	uint8_t frame_to_show_map_idx;                    // [0, 7]
	uint8_t last_frame_idx;                           // [0, 7]
	uint8_t gold_frame_idx;                           // [0, 7]
	uint8_t found_ref_bits;                           // [1, 7]
	uint8_t refresh_frame_flags;                      // [0, 0xff]
	av1_frame_type_t frame_type;                      // [0, 3]
	av1_interpolation_filter_t interpolation_filter;  // [0, 4]
	uint32_t frame_presentation_time;                 // [0, 2^(frame_presentation_time_length = [1, 32]))
	uint32_t display_frame_id;                        // [0, 2^((additional_frame_id_length = [1, 8]) + (delta_frame_id_length = [2, 17])))
	uint32_t current_frame_id;                        // [0, 2^((additional_frame_id_length = [1, 8]) + (delta_frame_id_length = [2, 17])))
	uint32_t order_hints[NUM_REF_FRAMES];             // [0, 2^(order_hint_bits = [1, 8]))
	uint32_t ref_order_hint[NUM_REF_FRAMES];          // [0, 2^(order_hint_bits = [1, 8]))
	uint32_t delta_frame_id[NUM_REF_FRAMES];          // [1, 2^(delta_frame_id_length = [2, 17])]
	uint32_t ref_frame_idx[REFS_PER_FRAME];           // [0, 7]
	uint32_t buffer_removal_time[32];                 // [0, 2^(decode_model_info.buffer_delay_length = [1, 32]))
	av1_uncompressed_header_extra_t extra;
} av1_uncompressed_header_t;

av1_uncompressed_header_t* av1_uncompressed_header_init(av1_uncompressed_header_t *restrict header);
uint32_t av1_uncompressed_header_FrameIsIntra(const av1_uncompressed_header_t *restrict header);
av1_uncompressed_header_t* av1_uncompressed_header_read(av1_uncompressed_header_t *restrict header, av1_bits_reader_t *restrict reader, const struct av1_obu_header_t *restrict oh, av1_env_t *restrict env);
const av1_uncompressed_header_t* av1_uncompressed_header_write(const av1_uncompressed_header_t *restrict header, av1_bits_writer_t *restrict writer, const struct av1_obu_header_t *restrict oh, av1_env_t *restrict env);
uint64_t av1_uncompressed_header_bits(const av1_uncompressed_header_t *restrict header, const struct av1_obu_header_t *restrict oh, av1_env_t *restrict env);
void av1_uncompressed_header_dump(const av1_uncompressed_header_t *restrict header, mlog_s *restrict mlog, av1_env_t *restrict env);

#endif
