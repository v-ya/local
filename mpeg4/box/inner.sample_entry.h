#ifndef _mpeg4_inner_sample_entry_h_
#define _mpeg4_inner_sample_entry_h_

#include "box.include.h"

typedef struct inner_sample_entry_t {
	uint8_t reserved[6];
	uint16_t data_reference_index;
} __attribute__ ((packed)) inner_sample_entry_t;

typedef struct inner_visual_sample_entry_t {
	inner_sample_entry_t sample_entry;
	uint16_t pre_defined;
	uint16_t reserved;
	uint32_t pre_defined3[3];
	uint16_t width;
	uint16_t height;
	uint32_t horizre_solution; // 16.16
	uint32_t vertre_solution;  // 16.16
	uint32_t reserved2;
	uint16_t frame_count;
	uint8_t compressor_name[32];
	uint16_t depth;
	int16_t pre_defined_1;
} __attribute__ ((packed)) inner_visual_sample_entry_t;

typedef struct inner_audio_sample_entry_t {
	inner_sample_entry_t sample_entry;
	uint16_t entry_version;
	uint16_t reserved3[3];
	uint16_t channel_count;
	uint16_t sample_size;
	uint16_t pre_defined;
	uint16_t reserved;
	uint32_t sample_rate; // 16.16
} __attribute__ ((packed)) inner_audio_sample_entry_t;

typedef struct inner_visual_sample_t {
	uint16_t data_reference_index;
	uint16_t width;
	uint16_t height;
	uint16_t frame_count;
	uint16_t depth;
	double horizre_solution;
	double vertre_solution;
	char compressor_name[32];
} inner_visual_sample_t;

typedef struct inner_audio_sample_t {
	uint16_t data_reference_index;
	uint16_t entry_version;
	uint16_t channel_count;
	uint16_t sample_size;
	double sample_rate;
} inner_audio_sample_t;

inner_visual_sample_t* mpeg4$define(inner, visual_sample, get)(inner_visual_sample_t *restrict r, const uint8_t *restrict *restrict data, uint64_t *restrict size);
uint8_t* mpeg4$define(inner, visual_sample, set)(uint8_t *restrict data, const inner_visual_sample_t *restrict r);

inner_audio_sample_t* mpeg4$define(inner, audio_sample, get)(inner_audio_sample_t *restrict r, const uint8_t *restrict *restrict data, uint64_t *restrict size);
uint8_t* mpeg4$define(inner, audio_sample, set)(uint8_t *restrict data, const inner_audio_sample_t *restrict r);

#endif
