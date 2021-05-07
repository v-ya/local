#include "decode_model_info.h"

av1_decode_model_info_t* av1_decode_model_info_init(av1_decode_model_info_t *restrict header)
{
	header->buffer_delay_length = 1;
	header->num_units_in_decoding_tick = 0;
	header->buffer_removal_time_length = 1;
	header->frame_presentation_time_length = 1;
	return header;
}

av1_decode_model_info_t* av1_decode_model_info_read(av1_decode_model_info_t *restrict header, av1_bits_reader_t *restrict reader)
{
	uint64_t v;
	// ( 5) buffer_delay_length_minus_1
	// (32) num_units_in_decoding_tick
	// ( 5) buffer_removal_time_length_minus_1
	// ( 5) frame_presentation_time_length_minus_1
	if (!av1_bits_uint_read(reader, &v, 5))
		goto label_fail;
	header->buffer_delay_length = (uint32_t) (v + 1);
	if (!av1_bits_uint_read(reader, &v, 32))
		goto label_fail;
	header->num_units_in_decoding_tick = (uint32_t) v;
	if (!av1_bits_uint_read(reader, &v, 5))
		goto label_fail;
	header->buffer_removal_time_length = (uint32_t) (v + 1);
	if (!av1_bits_uint_read(reader, &v, 5))
		goto label_fail;
	header->frame_presentation_time_length = (uint32_t) (v + 1);
	return header;
	label_fail:
	return NULL;
}

const av1_decode_model_info_t* av1_decode_model_info_write(const av1_decode_model_info_t *restrict header, av1_bits_writer_t *restrict writer)
{
	// ( 5) buffer_delay_length_minus_1
	// (32) num_units_in_decoding_tick
	// ( 5) buffer_removal_time_length_minus_1
	// ( 5) frame_presentation_time_length_minus_1
	if (!av1_bits_uint_write(writer, header->buffer_delay_length - 1, 5))
		goto label_fail;
	if (!av1_bits_uint_write(writer, header->num_units_in_decoding_tick, 32))
		goto label_fail;
	if (!av1_bits_uint_write(writer, header->buffer_removal_time_length - 1, 5))
		goto label_fail;
	if (!av1_bits_uint_write(writer, header->frame_presentation_time_length - 1, 5))
		goto label_fail;
	return header;
	label_fail:
	return NULL;
}

uint64_t av1_decode_model_info_bits(const av1_decode_model_info_t *restrict header)
{
	return 47;
}

void av1_decode_model_info_dump(const av1_decode_model_info_t *restrict header, mlog_s *restrict mlog)
{
	mlog_printf(mlog, "buffer_delay_length[1, 32]:            %u\n", header->buffer_delay_length);
	mlog_printf(mlog, "num_units_in_decoding_tick[0, 2^32-1]: %u\n", header->num_units_in_decoding_tick);
	mlog_printf(mlog, "buffer_removal_time_length[1, 32]:     %u\n", header->buffer_removal_time_length);
	mlog_printf(mlog, "frame_presentation_time_length[1, 32]: %u\n", header->frame_presentation_time_length);
}
