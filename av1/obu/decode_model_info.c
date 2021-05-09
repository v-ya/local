#include "decode_model_info.h"

av1_decode_model_info_t* av1_decode_model_info_init(av1_decode_model_info_t *restrict dm_info)
{
	dm_info->buffer_delay_length = 1;
	dm_info->num_units_in_decoding_tick = 0;
	dm_info->buffer_removal_time_length = 1;
	dm_info->frame_presentation_time_length = 1;
	return dm_info;
}

av1_decode_model_info_t* av1_decode_model_info_read(av1_decode_model_info_t *restrict dm_info, av1_bits_reader_t *restrict reader)
{
	uint64_t v;
	// ( 5) buffer_delay_length_minus_1
	// (32) num_units_in_decoding_tick
	// ( 5) buffer_removal_time_length_minus_1
	// ( 5) frame_presentation_time_length_minus_1
	if (!av1_bits_uint_read(reader, &v, 5))
		goto label_fail;
	dm_info->buffer_delay_length = (uint32_t) (v + 1);
	if (!av1_bits_uint_read(reader, &v, 32))
		goto label_fail;
	dm_info->num_units_in_decoding_tick = (uint32_t) v;
	if (!av1_bits_uint_read(reader, &v, 5))
		goto label_fail;
	dm_info->buffer_removal_time_length = (uint32_t) (v + 1);
	if (!av1_bits_uint_read(reader, &v, 5))
		goto label_fail;
	dm_info->frame_presentation_time_length = (uint32_t) (v + 1);
	return dm_info;
	label_fail:
	return NULL;
}

const av1_decode_model_info_t* av1_decode_model_info_write(const av1_decode_model_info_t *restrict dm_info, av1_bits_writer_t *restrict writer)
{
	// ( 5) buffer_delay_length_minus_1
	// (32) num_units_in_decoding_tick
	// ( 5) buffer_removal_time_length_minus_1
	// ( 5) frame_presentation_time_length_minus_1
	if (!av1_bits_uint_write(writer, dm_info->buffer_delay_length - 1, 5))
		goto label_fail;
	if (!av1_bits_uint_write(writer, dm_info->num_units_in_decoding_tick, 32))
		goto label_fail;
	if (!av1_bits_uint_write(writer, dm_info->buffer_removal_time_length - 1, 5))
		goto label_fail;
	if (!av1_bits_uint_write(writer, dm_info->frame_presentation_time_length - 1, 5))
		goto label_fail;
	return dm_info;
	label_fail:
	return NULL;
}

uint64_t av1_decode_model_info_bits(const av1_decode_model_info_t *restrict dm_info)
{
	return 47;
}

void av1_decode_model_info_dump(const av1_decode_model_info_t *restrict dm_info, mlog_s *restrict mlog)
{
	mlog_printf(mlog, "buffer_delay_length[1, 32]:            %u\n", dm_info->buffer_delay_length);
	mlog_printf(mlog, "num_units_in_decoding_tick[0, 2^32-1]: %u\n", dm_info->num_units_in_decoding_tick);
	mlog_printf(mlog, "buffer_removal_time_length[1, 32]:     %u\n", dm_info->buffer_removal_time_length);
	mlog_printf(mlog, "frame_presentation_time_length[1, 32]: %u\n", dm_info->frame_presentation_time_length);
}
