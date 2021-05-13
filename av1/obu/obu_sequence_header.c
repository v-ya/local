#include "obu_sequence_header.h"

av1_obu_sequence_header_t* av1_obu_sequence_header_init(av1_obu_sequence_header_t *restrict header)
{
	if (av1_timing_info_init(&header->extra.timing_info) &&
		av1_decode_model_info_init(&header->extra.decode_model_info))
	{
		memset(header, 0, offsetof(av1_obu_sequence_header_t, extra));
		header->operating_points_cnt = 1;
		header->frame_width_bits = 1;
		header->frame_height_bits = 1;
		header->max_frame_width = 1;
		header->max_frame_height = 1;
		header->delta_frame_id_length = 2;
		header->additional_frame_id_length = 1;
		header->seq_force_screen_content_tools = SELECT_SCREEN_CONTENT_TOOLS;
		header->seq_force_integer_mv = SELECT_INTEGER_MV;
		header->order_hint_bits = 1;
		header->color_config_color_primaries = av1_color_primaries_UNSPECIFIED;
		header->color_config_transfer_characteristics = av1_transfer_characteristics_UNSPECIFIED;
		header->color_config_matrix_coefficients = av1_matrix_coefficients_UNSPECIFIED;
		header->color_config_chroma_sample_position = av1_chroma_sample_position_UNKNOWN;
		return header;
	}
	return NULL;
}

uint32_t av1_obu_sequence_header_OrderHintBits(const av1_obu_sequence_header_t *restrict header)
{
	if (header->enable_order_hint)
		return header->order_hint_bits;
	return 0;
}

uint32_t av1_obu_sequence_header_BitDepth(const av1_obu_sequence_header_t *restrict header)
{
	if (header->seq_profile == 2 && header->color_config_high_bitdepth)
		return header->color_config_twelve_bit?12:10;
	else if (header->seq_profile <= 2)
		return header->color_config_high_bitdepth?10:8;
	return 0;
}

uint32_t av1_obu_sequence_header_NumPlanes(const av1_obu_sequence_header_t *restrict header)
{
	return header->color_config_mono_chrome?1:3;
}

av1_obu_sequence_header_t* av1_obu_sequence_header_read(av1_obu_sequence_header_t *restrict header, av1_bits_reader_t *restrict reader)
{
	uint64_t v;
	uintptr_t i, n;
	uint32_t l;
	uint8_t f;
	// (3) seq_profile
	// (1) still_picture
	// (1) reduced_still_picture_header
	if (!av1_bits_uint_read(reader, &v, 5))
		goto label_fail;
	header->seq_profile = (uint8_t) ((v >> 2) & 0x07);
	header->still_picture = !!(v & 0x02);
	header->reduced_still_picture_header = !!(v & 0x01);
	if (header->reduced_still_picture_header)
	{
		header->timing_info_present_flag = 0;
		header->decoder_model_info_present_flag = 0;
		header->initial_display_delay_present_flag = 0;
		header->operating_points_cnt = 1;
		header->operating_point_idc[0] = 0;
		// (5) seq_level_idx[0]
		if (!av1_bits_uint_read(reader, &v, 5))
			goto label_fail;
		header->seq_level_idx[0] = (uint8_t) v;
		header->seq_tier[0] = 0;
		header->decoder_model_present_for_this_op[0] = 0;
		header->initial_display_delay[0] = 0;
	}
	else
	{
		// (1) timing_info_present_flag;
		if (!av1_bits_flag_read(reader, &header->timing_info_present_flag))
			goto label_fail;
		if (header->timing_info_present_flag)
		{
			// (timing_info()) timing_info
			if (!av1_timing_info_read(&header->extra.timing_info, reader))
				goto label_fail;
			// (1) decoder_model_info_present_flag
			if (!av1_bits_flag_read(reader, &header->decoder_model_info_present_flag))
				goto label_fail;
			if (header->decoder_model_info_present_flag)
			{
				// (decode_model_info()) decode_model_info
				if (!av1_decode_model_info_read(&header->extra.decode_model_info, reader))
					goto label_fail;
			}
		}
		else header->decoder_model_info_present_flag = 0;
		// (1) initial_display_delay_present_flag
		// (5) operating_points_cnt_minus_1
		if (!av1_bits_flag_read(reader, &header->initial_display_delay_present_flag))
			goto label_fail;
		if (!av1_bits_uint_read(reader, &v, 5))
			goto label_fail;
		header->operating_points_cnt = (uint8_t) (++v);
		n = (uintptr_t) v;
		l = header->extra.decode_model_info.buffer_delay_length;
		for (i = 0; i < n; ++i)
		{
			// (12) operating_point_idc[i]
			// ( 5) seq_level_idx[i]
			if (!av1_bits_uint_read(reader, &v, 12))
				goto label_fail;
			header->operating_point_idc[i] = (uint16_t) v;
			if (!av1_bits_uint_read(reader, &v, 5))
				goto label_fail;
			header->seq_level_idx[i] = (uint8_t) v;
			if (header->seq_level_idx[i] > 7)
			{
				// (1) seq_tier[i]
				if (!av1_bits_flag_read(reader, header->seq_tier + i))
					goto label_fail;
			}
			else header->seq_tier[0] = 0;
			if (header->decoder_model_info_present_flag)
			{
				// (1) decoder_model_present_for_this_op[i]
				if (!av1_bits_flag_read(reader, header->decoder_model_present_for_this_op + i))
					goto label_fail;
				if (header->decoder_model_present_for_this_op[i])
				{
					// n = buffer_delay_length;
					// (n) decoder_buffer_delay[i]
					// (n) encoder_buffer_delay[i]
					// (1) low_delay_mode_flag[i]
					if (!av1_bits_uint_read(reader, &v, l))
						goto label_fail;
					header->decoder_buffer_delay[i] = (uint32_t) v;
					if (!av1_bits_uint_read(reader, &v, l))
						goto label_fail;
					header->encoder_buffer_delay[i] = (uint32_t) v;
					if (!av1_bits_flag_read(reader, header->low_delay_mode_flag + i))
						goto label_fail;
				}
			}
			else header->decoder_model_present_for_this_op[i] = 0;
			if (header->initial_display_delay_present_flag)
			{
				// (1) initial_display_delay_present_for_this_op[i]
				if (!av1_bits_flag_read(reader, &f))
					goto label_fail;
				if (f)
				{
					// (4) initial_display_delay_minus_1[i]
					if (!av1_bits_uint_read(reader, &v, 4))
						goto label_fail;
					header->initial_display_delay[i] = (uint8_t) (v + 1);
				}
			}
		}
	}
	// (4) frame_width_bits_minus_1
	// (4) frame_height_bits_minus_1
	// n = frame_width_bits
	// (n) max_frame_width_minus_1
	// n = frame_height_bits
	// (n) max_frame_height_minus_1
	if (!av1_bits_uint_read(reader, &v, 4))
		goto label_fail;
	header->frame_width_bits = (uint8_t) (v + 1);
	if (!av1_bits_uint_read(reader, &v, 4))
		goto label_fail;
	header->frame_height_bits = (uint8_t) (v + 1);
	if (!av1_bits_uint_read(reader, &v, header->frame_width_bits))
		goto label_fail;
	header->max_frame_width = (uint32_t) (v + 1);
	if (!av1_bits_uint_read(reader, &v, header->frame_height_bits))
		goto label_fail;
	header->max_frame_height = (uint32_t) (v + 1);
	if (!header->reduced_still_picture_header)
	{
		// (1) frame_id_numbers_present_flag
		if (!av1_bits_flag_read(reader, &header->frame_id_numbers_present_flag))
			goto label_fail;
	}
	else header->frame_id_numbers_present_flag = 0;
	if (header->frame_id_numbers_present_flag)
	{
		// (4) delta_frame_id_length_minus_2
		// (3) additional_frame_id_length_minus_1
		if (!av1_bits_uint_read(reader, &v, 4))
			goto label_fail;
		header->delta_frame_id_length = (uint8_t) (v + 2);
		if (!av1_bits_uint_read(reader, &v, 3))
			goto label_fail;
		header->additional_frame_id_length = (uint8_t) (v + 1);
	}
	// (1) use_128x128_superblock
	// (1) enable_filter_intra
	// (1) enable_intra_edge_filter
	if (!av1_bits_uint_read(reader, &v, 3))
		goto label_fail;
	header->use_128x128_superblock = !!(v & 0x04);
	header->enable_filter_intra = !!(v & 0x02);
	header->enable_intra_edge_filter = !!(v & 0x01);
	if (header->reduced_still_picture_header)
	{
		header->enable_interintra_compound = 0;
		header->enable_masked_compound = 0;
		header->enable_warped_motion = 0;
		header->enable_dual_filter = 0;
		header->enable_order_hint = 0;
		header->enable_jnt_comp = 0;
		header->enable_ref_frame_mvs = 0;
		header->seq_force_screen_content_tools = SELECT_SCREEN_CONTENT_TOOLS;
		header->seq_force_integer_mv = SELECT_INTEGER_MV;
	}
	else
	{
		// (1) enable_interintra_compound
		// (1) enable_masked_compound
		// (1) enable_warped_motion
		// (1) enable_dual_filter
		// (1) enable_order_hint
		if (!av1_bits_uint_read(reader, &v, 5))
			goto label_fail;
		header->enable_interintra_compound = !!(v & 0x10);
		header->enable_masked_compound = !!(v & 0x08);
		header->enable_warped_motion = !!(v & 0x04);
		header->enable_dual_filter = !!(v & 0x02);
		header->enable_order_hint = !!(v & 0x01);
		if (header->enable_order_hint)
		{
			// (1) enable_jnt_comp
			// (1) enable_ref_frame_mvs
			if (!av1_bits_uint_read(reader, &v, 2))
				goto label_fail;
			header->enable_jnt_comp = !!(v & 0x02);
			header->enable_ref_frame_mvs = !!(v & 0x01);
		}
		else
		{
			header->enable_jnt_comp = 0;
			header->enable_ref_frame_mvs = 0;
		}
		// (1) seq_choose_screen_content_tools
		if (!av1_bits_flag_read(reader, &f))
			goto label_fail;
		if (!f)
		{
			// (1) seq_force_screen_content_tools
			if (!av1_bits_flag_read(reader, &header->seq_force_screen_content_tools))
				goto label_fail;
		}
		else header->seq_force_screen_content_tools = SELECT_SCREEN_CONTENT_TOOLS;
		if (header->seq_force_screen_content_tools > 0)
		{
			// (1) seq_choose_integer_mv
			if (!av1_bits_flag_read(reader, &f))
				goto label_fail;
			if (!f)
			{
				// (1) seq_force_integer_mv
				if (!av1_bits_flag_read(reader, &header->seq_force_integer_mv))
					goto label_fail;
			}
			else header->seq_force_integer_mv = SELECT_INTEGER_MV;
		}
		else header->seq_force_integer_mv = SELECT_INTEGER_MV;
		if (header->enable_order_hint)
		{
			// (3) order_hint_bits_minus_1
			if (!av1_bits_uint_read(reader, &v, 3))
				goto label_fail;
			header->order_hint_bits = (uint8_t) (v + 1);
		}
	}
	// (1) enable_superres
	// (1) enable_cdef
	// (1) enable_restoration
	if (!av1_bits_uint_read(reader, &v, 3))
		goto label_fail;
	header->enable_superres = !!(v & 0x04);
	header->enable_cdef = !!(v & 0x02);
	header->enable_restoration = !!(v & 0x01);
	// (1) high_bitdepth
	if (!av1_bits_flag_read(reader, &header->color_config_high_bitdepth))
		goto label_fail;
	if (header->seq_profile == 2 && header->color_config_high_bitdepth)
	{
		// (1) twelve_bit
		if (!av1_bits_flag_read(reader, &header->color_config_twelve_bit))
			goto label_fail;
	}
	if (header->seq_profile == 1)
		header->color_config_mono_chrome = 0;
	else
	{
		// (1) mono_chrome
		if (!av1_bits_flag_read(reader, &header->color_config_mono_chrome))
			goto label_fail;
	}
	// (1) color_description_present_flag
	if (!av1_bits_flag_read(reader, &header->color_config_color_description_present_flag))
		goto label_fail;
	if (header->color_config_color_description_present_flag)
	{
		// (8) color_primaries
		// (8) transfer_characteristics
		// (8) matrix_coefficients
		if (!av1_bits_uint_read(reader, &v, 24))
			goto label_fail;
		header->color_config_color_primaries = (uint8_t) (v >> 16);
		header->color_config_transfer_characteristics = (uint8_t) (v >> 8);
		header->color_config_matrix_coefficients = (uint8_t) v;
	}
	else
	{
		header->color_config_color_primaries = av1_color_primaries_UNSPECIFIED;
		header->color_config_transfer_characteristics = av1_transfer_characteristics_UNSPECIFIED;
		header->color_config_matrix_coefficients = av1_matrix_coefficients_UNSPECIFIED;
	}
	if (header->color_config_mono_chrome)
	{
		// (1) color_range
		if (!av1_bits_flag_read(reader, &header->color_config_color_range))
			goto label_fail;
		header->color_config_subsampling_x = 1;
		header->color_config_subsampling_y = 1;
		header->color_config_chroma_sample_position = av1_chroma_sample_position_UNKNOWN;
		header->color_config_separate_uv_delta_q = 0;
		goto label_end_color_config;
	}
	else if (header->color_config_color_primaries == av1_color_primaries_BT_709 &&
		header->color_config_transfer_characteristics == av1_transfer_characteristics_SRGB &&
		header->color_config_matrix_coefficients == av1_matrix_coefficients_IDENTITY)
	{
		header->color_config_color_range = 1;
		header->color_config_subsampling_x = 0;
		header->color_config_subsampling_y = 0;
	}
	else
	{
		// (1) color_range
		if (!av1_bits_flag_read(reader, &header->color_config_color_range))
			goto label_fail;
		if (header->seq_profile == 0)
		{
			header->color_config_subsampling_x = 1;
			header->color_config_subsampling_y = 1;
		}
		else if (header->seq_profile == 1)
		{
			header->color_config_subsampling_x = 0;
			header->color_config_subsampling_y = 0;
		}
		else
		{
			if (av1_obu_sequence_header_BitDepth(header) == 12)
			{
				// (1) subsampling_x
				if (!av1_bits_flag_read(reader, &header->color_config_subsampling_x))
					goto label_fail;
				if (header->color_config_subsampling_x)
				{
					// (1) subsampling_y
					if (!av1_bits_flag_read(reader, &header->color_config_subsampling_y))
						goto label_fail;
				}
				else header->color_config_subsampling_y = 0;
			}
			else
			{
				header->color_config_subsampling_x = 1;
				header->color_config_subsampling_y = 0;
			}
		}
		if (header->color_config_subsampling_x && header->color_config_subsampling_y)
		{
			// (2) chroma_sample_position
			if (!av1_bits_uint_read(reader, &v, 2))
				goto label_fail;
			header->color_config_chroma_sample_position = (av1_chroma_sample_position_t) v;
		}
	}
	// (1) separate_uv_delta_q
	if (!av1_bits_flag_read(reader, &header->color_config_separate_uv_delta_q))
		goto label_fail;
	label_end_color_config:
	// (1) film_grain_params_present
	if (!av1_bits_flag_read(reader, &header->film_grain_params_present))
		goto label_fail;
	if (!av1_bits_trailing_bits_read(reader))
		goto label_fail;
	return header;
	label_fail:
	return NULL;
}

const av1_obu_sequence_header_t* av1_obu_sequence_header_write(const av1_obu_sequence_header_t *restrict header, av1_bits_writer_t *restrict writer)
{
	uintptr_t i, n;
	uint32_t l;
	// (3) seq_profile
	// (1) still_picture
	// (1) reduced_still_picture_header
	if (!av1_bits_uint_write(writer,
		((uint64_t) header->seq_profile << 2) |
		((uint64_t) header->still_picture << 1) |
		((uint64_t) header->reduced_still_picture_header)
		, 5))
		goto label_fail;
	if (header->reduced_still_picture_header)
	{
		// (5) seq_level_idx[0]
		if (!av1_bits_uint_write(writer, header->seq_level_idx[0], 5))
			goto label_fail;
	}
	else
	{
		// (1) timing_info_present_flag;
		if (!av1_bits_flag_write(writer, header->timing_info_present_flag))
			goto label_fail;
		if (header->timing_info_present_flag)
		{
			// (timing_info()) timing_info
			if (!av1_timing_info_write(&header->extra.timing_info, writer))
				goto label_fail;
			// (1) decoder_model_info_present_flag
			if (!av1_bits_flag_write(writer, header->decoder_model_info_present_flag))
				goto label_fail;
			if (header->decoder_model_info_present_flag)
			{
				// (decode_model_info()) decode_model_info
				if (!av1_decode_model_info_write(&header->extra.decode_model_info, writer))
					goto label_fail;
			}
		}
		// (1) initial_display_delay_present_flag
		// (5) operating_points_cnt_minus_1
		if (!av1_bits_flag_write(writer, header->initial_display_delay_present_flag))
			goto label_fail;
		if (!av1_bits_uint_write(writer, header->operating_points_cnt - 1, 5))
			goto label_fail;
		n = header->operating_points_cnt;
		l = header->extra.decode_model_info.buffer_delay_length;
		for (i = 0; i < n; ++i)
		{
			// (12) operating_point_idc[i]
			// ( 5) seq_level_idx[i]
			if (!av1_bits_uint_write(writer, header->operating_point_idc[i], 12))
				goto label_fail;
			if (!av1_bits_uint_write(writer, header->seq_level_idx[i], 5))
				goto label_fail;
			if (header->seq_level_idx[i] > 7)
			{
				// (1) seq_tier[i]
				if (!av1_bits_flag_write(writer, header->seq_tier[i]))
					goto label_fail;
			}
			if (header->decoder_model_info_present_flag)
			{
				// (1) decoder_model_present_for_this_op[i]
				if (!av1_bits_flag_write(writer, header->decoder_model_present_for_this_op[i]))
					goto label_fail;
				if (header->decoder_model_present_for_this_op[i])
				{
					// n = buffer_delay_length;
					// (n) decoder_buffer_delay[i]
					// (n) encoder_buffer_delay[i]
					// (1) low_delay_mode_flag[i]
					if (!av1_bits_uint_write(writer, header->decoder_buffer_delay[i], l))
						goto label_fail;
					if (!av1_bits_uint_write(writer, header->encoder_buffer_delay[i], l))
						goto label_fail;
					if (!av1_bits_flag_write(writer, header->low_delay_mode_flag[i]))
						goto label_fail;
				}
			}
			if (header->initial_display_delay_present_flag)
			{
				// (1) initial_display_delay_present_for_this_op[i]
				if (!av1_bits_flag_write(writer, !!header->initial_display_delay[i]))
					goto label_fail;
				if (header->initial_display_delay[i])
				{
					// (4) initial_display_delay_minus_1[i]
					if (!av1_bits_uint_write(writer, header->initial_display_delay[i], 4))
						goto label_fail;
				}
			}
		}
	}
	// (4) frame_width_bits_minus_1
	// (4) frame_height_bits_minus_1
	// n = frame_width_bits
	// (n) max_frame_width_minus_1
	// n = frame_height_bits
	// (n) max_frame_height_minus_1
	if (!av1_bits_uint_write(writer, header->frame_width_bits - 1, 4))
		goto label_fail;
	if (!av1_bits_uint_write(writer, header->frame_height_bits - 1, 4))
		goto label_fail;
	if (!av1_bits_uint_write(writer, header->max_frame_width - 1, header->frame_width_bits))
		goto label_fail;
	if (!av1_bits_uint_write(writer, header->max_frame_height - 1, header->frame_height_bits))
		goto label_fail;
	if (!header->reduced_still_picture_header)
	{
		// (1) frame_id_numbers_present_flag
		if (!av1_bits_flag_write(writer, header->frame_id_numbers_present_flag))
			goto label_fail;
	}
	if (header->frame_id_numbers_present_flag)
	{
		// (4) delta_frame_id_length_minus_2
		// (3) additional_frame_id_length_minus_1
		if (!av1_bits_uint_write(writer, header->delta_frame_id_length - 2, 4))
			goto label_fail;
		if (!av1_bits_uint_write(writer, header->additional_frame_id_length - 1, 3))
			goto label_fail;
	}
	// (1) use_128x128_superblock
	// (1) enable_filter_intra
	// (1) enable_intra_edge_filter
	if (!av1_bits_uint_write(writer,
		((uint64_t) header->use_128x128_superblock << 2) |
		((uint64_t) header->enable_filter_intra << 1) |
		((uint64_t) header->enable_intra_edge_filter)
		, 3))
		goto label_fail;
	if (!header->reduced_still_picture_header)
	{
		// (1) enable_interintra_compound
		// (1) enable_masked_compound
		// (1) enable_warped_motion
		// (1) enable_dual_filter
		// (1) enable_order_hint
		if (!av1_bits_uint_write(writer,
			((uint64_t) header->enable_interintra_compound << 4) |
			((uint64_t) header->enable_masked_compound << 3) |
			((uint64_t) header->enable_warped_motion << 2) |
			((uint64_t) header->enable_dual_filter << 1) |
			((uint64_t) header->enable_order_hint)
			, 5))
			goto label_fail;
		if (header->enable_order_hint)
		{
			// (1) enable_jnt_comp
			// (1) enable_ref_frame_mvs
			if (!av1_bits_uint_write(writer,
				((uint64_t) header->enable_jnt_comp << 1) |
				((uint64_t) header->enable_ref_frame_mvs)
				, 2))
				goto label_fail;
		}
		// (1) seq_choose_screen_content_tools
		if (!av1_bits_flag_write(writer, (header->seq_force_screen_content_tools > 1)))
			goto label_fail;
		if (header->seq_force_screen_content_tools < SELECT_SCREEN_CONTENT_TOOLS)
		{
			// (1) seq_force_screen_content_tools
			if (!av1_bits_flag_write(writer, header->seq_force_screen_content_tools))
				goto label_fail;
		}
		if (header->seq_force_screen_content_tools > 0)
		{
			// (1) seq_choose_integer_mv
			if (!av1_bits_flag_write(writer, (header->seq_force_integer_mv > 1)))
				goto label_fail;
			if (header->seq_force_integer_mv < SELECT_INTEGER_MV)
			{
				// (1) seq_force_integer_mv
				if (!av1_bits_flag_write(writer, header->seq_force_integer_mv))
					goto label_fail;
			}
		}
		if (header->enable_order_hint)
		{
			// (3) order_hint_bits_minus_1
			if (!av1_bits_uint_write(writer, header->order_hint_bits - 1, 3))
				goto label_fail;
		}
	}
	// (1) enable_superres
	// (1) enable_cdef
	// (1) enable_restoration
	if (!av1_bits_uint_write(writer,
		((uint64_t) header->enable_superres << 2) |
		((uint64_t) header->enable_cdef << 1) |
		((uint64_t) header->enable_restoration)
		, 3))
		goto label_fail;
	// (1) high_bitdepth
	if (!av1_bits_flag_write(writer, header->color_config_high_bitdepth))
		goto label_fail;
	if (header->seq_profile == 2 && header->color_config_high_bitdepth)
	{
		// (1) twelve_bit
		if (!av1_bits_flag_write(writer, header->color_config_twelve_bit))
			goto label_fail;
	}
	if (header->seq_profile != 1)
	{
		// (1) mono_chrome
		if (!av1_bits_flag_write(writer, header->color_config_mono_chrome))
			goto label_fail;
	}
	// (1) color_description_present_flag
	if (!av1_bits_flag_write(writer, header->color_config_color_description_present_flag))
		goto label_fail;
	if (header->color_config_color_description_present_flag)
	{
		// (8) color_primaries
		// (8) transfer_characteristics
		// (8) matrix_coefficients
		if (!av1_bits_uint_write(writer,
			((uint64_t) (header->color_config_color_primaries & 0xff) << 16) |
			((uint64_t) (header->color_config_transfer_characteristics & 0xff) << 8) |
			((uint64_t) (header->color_config_matrix_coefficients & 0xff))
			, 24))
			goto label_fail;
	}
	if (header->color_config_mono_chrome)
	{
		// (1) color_range
		if (!av1_bits_flag_write(writer, header->color_config_color_range))
			goto label_fail;
		goto label_end_color_config;
	}
	else if (header->color_config_color_primaries == av1_color_primaries_BT_709 &&
		header->color_config_transfer_characteristics == av1_transfer_characteristics_SRGB &&
		header->color_config_matrix_coefficients == av1_matrix_coefficients_IDENTITY) ;
	else
	{
		// (1) color_range
		if (!av1_bits_flag_write(writer, header->color_config_color_range))
			goto label_fail;
		if (header->seq_profile > 1)
		{
			if (av1_obu_sequence_header_BitDepth(header) == 12)
			{
				// (1) subsampling_x
				if (!av1_bits_flag_write(writer, header->color_config_subsampling_x))
					goto label_fail;
				if (header->color_config_subsampling_x)
				{
					// (1) subsampling_y
					if (!av1_bits_flag_write(writer, header->color_config_subsampling_y))
						goto label_fail;
				}
			}
		}
		if (header->color_config_subsampling_x && header->color_config_subsampling_y)
		{
			// (2) chroma_sample_position
			if (!av1_bits_uint_write(writer, header->color_config_chroma_sample_position, 2))
				goto label_fail;
		}
	}
	// (1) separate_uv_delta_q
	if (!av1_bits_flag_write(writer, header->color_config_separate_uv_delta_q))
		goto label_fail;
	label_end_color_config:
	// (1) film_grain_params_present
	if (!av1_bits_flag_write(writer, header->film_grain_params_present))
		goto label_fail;
	if (!av1_bits_trailing_bits_write(writer))
		goto label_fail;
	return header;
	label_fail:
	return NULL;
}

uint64_t av1_obu_sequence_header_bits(const av1_obu_sequence_header_t *restrict header)
{
	uint64_t size = 5;
	uintptr_t i, n;
	uint32_t l;
	if (header->reduced_still_picture_header) size += 5;
	else
	{
		size += 1;
		if (header->timing_info_present_flag)
		{
			size += av1_timing_info_bits(&header->extra.timing_info);
			size += 1;
			if (header->decoder_model_info_present_flag)
				size += av1_decode_model_info_bits(&header->extra.decode_model_info);
		}
		size += 6;
		n = header->operating_points_cnt;
		l = header->extra.decode_model_info.buffer_delay_length;
		for (i = 0; i < n; ++i)
		{
			size += 17;
			if (header->seq_level_idx[i] > 7)
				size += 1;
			if (header->decoder_model_info_present_flag)
			{
				size += 1;
				if (header->decoder_model_present_for_this_op[i])
					size += l * 2 + 1;
			}
			if (header->initial_display_delay_present_flag)
			{
				size += 1;
				if (header->initial_display_delay[i])
					size += 4;
			}
		}
	}
	size += 8;
	size += header->frame_width_bits;
	size += header->frame_height_bits;
	if (!header->reduced_still_picture_header)
		size += 1;
	if (header->frame_id_numbers_present_flag)
		size += 7;
	size += 3;
	if (!header->reduced_still_picture_header)
	{
		size += 5;
		if (header->enable_order_hint)
			size += 2;
		size += (header->seq_force_screen_content_tools > 1)?1:2;
		if (header->seq_force_screen_content_tools > 0)
			size += (header->seq_force_integer_mv > 1)?1:2;
		if (header->enable_order_hint)
			size += 3;
	}
	size += 3;
	size += 1;
	if (header->seq_profile == 2 && header->color_config_high_bitdepth)
		size += 1;
	if (header->seq_profile != 1)
		size += 1;
	size += 1;
	if (header->color_config_color_description_present_flag)
		size += 24;
	if (header->color_config_mono_chrome)
	{
		size += 1;
		goto label_end_color_config;
	}
	else if (header->color_config_color_primaries == av1_color_primaries_BT_709 &&
		header->color_config_transfer_characteristics == av1_transfer_characteristics_SRGB &&
		header->color_config_matrix_coefficients == av1_matrix_coefficients_IDENTITY) ;
	else
	{
		size += 1;
		if (header->seq_profile > 1)
		{
			if (av1_obu_sequence_header_BitDepth(header) == 12)
			{
				size += 1;
				if (header->color_config_subsampling_x)
					size += 1;
			}
		}
		if (header->color_config_subsampling_x && header->color_config_subsampling_y)
			size += 2;
	}
	size += 1;
	label_end_color_config:
	size += 1;
	size += av1_bits_trailing_bits_bits(size);
	return size;
}

void av1_obu_sequence_header_dump(const av1_obu_sequence_header_t *restrict header, mlog_s *restrict mlog)
{
	uintptr_t i, n;
	mlog_printf(mlog, "seq_profile[0, 7]:                                 %u\n", header->seq_profile);
	mlog_printf(mlog, "still_picture[0, 1]:                               %u\n", header->still_picture);
	mlog_printf(mlog, "reduced_still_picture_header[0, 1]:                %u\n", header->reduced_still_picture_header);
	mlog_printf(mlog, "timing_info_present_flag[0, 1]:                    %u\n", header->timing_info_present_flag);
	av1_timing_info_dump(&header->extra.timing_info, mlog);
	mlog_printf(mlog, "decoder_model_info_present_flag[0, 1]:             %u\n", header->decoder_model_info_present_flag);
	av1_decode_model_info_dump(&header->extra.decode_model_info, mlog);
	mlog_printf(mlog, "initial_display_delay_present_flag[0, 1]:          %u\n", header->initial_display_delay_present_flag);
	mlog_printf(mlog, "operating_points_cnt[1, 32]:                       %u\n", header->operating_points_cnt);
	n = header->operating_points_cnt;
	for (i = 0; i < n; ++i)
	{
		mlog_printf(mlog, "operating_point_idc(%2u)[0, 2^12-1]:                %u\n", (uint32_t) i, header->operating_point_idc[i]);
		mlog_printf(mlog, "seq_level_idx(%2u)[0, 31]:                          %u\n", (uint32_t) i, header->seq_level_idx[i]);
		mlog_printf(mlog, "seq_tier(%2u)[0, 1]:                                %u\n", (uint32_t) i, header->seq_tier[i]);
		mlog_printf(mlog, "decoder_model_present_for_this_op(%2u)[0, 1]:       %u\n", (uint32_t) i, header->decoder_model_present_for_this_op[i]);
		mlog_printf(mlog, "decoder_buffer_delay(%2u)[0, buffer_delay_length]:  %u\n", (uint32_t) i, header->decoder_buffer_delay[i]);
		mlog_printf(mlog, "encoder_buffer_delay(%2u)[0, buffer_delay_length]:  %u\n", (uint32_t) i, header->encoder_buffer_delay[i]);
		mlog_printf(mlog, "low_delay_mode_flag(%2u)[0, 1]:                     %u\n", (uint32_t) i, header->low_delay_mode_flag[i]);
		mlog_printf(mlog, "initial_display_delay(%2u)[0, 16]:                  %u\n", (uint32_t) i, header->initial_display_delay[i]);
	}
	mlog_printf(mlog, "frame_width_bits[1, 16]:                           %u\n", header->frame_width_bits);
	mlog_printf(mlog, "frame_height_bits[1, 16]:                          %u\n", header->frame_height_bits);
	mlog_printf(mlog, "max_frame_width[1, 2^frame_width_bits]:            %u\n", header->max_frame_width);
	mlog_printf(mlog, "max_frame_height[1, 2^frame_height_bits]:          %u\n", header->max_frame_height);
	mlog_printf(mlog, "frame_id_numbers_present_flag[0, 1]:               %u\n", header->frame_id_numbers_present_flag);
	mlog_printf(mlog, "delta_frame_id_length[2, 17]:                      %u\n", header->delta_frame_id_length);
	mlog_printf(mlog, "additional_frame_id_length[1, 8]:                  %u\n", header->additional_frame_id_length);
	mlog_printf(mlog, "use_128x128_superblock[0, 1]:                      %u\n", header->use_128x128_superblock);
	mlog_printf(mlog, "enable_filter_intra[0, 1]:                         %u\n", header->enable_filter_intra);
	mlog_printf(mlog, "enable_intra_edge_filter[0, 1]:                    %u\n", header->enable_intra_edge_filter);
	mlog_printf(mlog, "enable_interintra_compound[0, 1]:                  %u\n", header->enable_interintra_compound);
	mlog_printf(mlog, "enable_masked_compound[0, 1]:                      %u\n", header->enable_masked_compound);
	mlog_printf(mlog, "enable_warped_motion[0, 1]:                        %u\n", header->enable_warped_motion);
	mlog_printf(mlog, "enable_dual_filter[0, 1]:                          %u\n", header->enable_dual_filter);
	mlog_printf(mlog, "enable_order_hint[0, 1]:                           %u\n", header->enable_order_hint);
	mlog_printf(mlog, "enable_jnt_comp[0, 1]:                             %u\n", header->enable_jnt_comp);
	mlog_printf(mlog, "enable_ref_frame_mvs[0, 1]:                        %u\n", header->enable_ref_frame_mvs);
	mlog_printf(mlog, "seq_force_screen_content_tools[0, 2]:              %u\n", header->seq_force_screen_content_tools);
	mlog_printf(mlog, "seq_force_integer_mv[0, 2]:                        %u\n", header->seq_force_integer_mv);
	mlog_printf(mlog, "order_hint_bits[1, 8]:                             %u\n", header->order_hint_bits);
	mlog_printf(mlog, "enable_superres[0, 1]:                             %u\n", header->enable_superres);
	mlog_printf(mlog, "enable_cdef[0, 1]:                                 %u\n", header->enable_cdef);
	mlog_printf(mlog, "enable_restoration[0, 1]:                          %u\n", header->enable_restoration);
	mlog_printf(mlog, "color_config_high_bitdepth[0, 1]:                  %u\n", header->color_config_high_bitdepth);
	mlog_printf(mlog, "color_config_twelve_bit[0, 1]:                     %u\n", header->color_config_twelve_bit);
	mlog_printf(mlog, "color_config_mono_chrome[0, 1]:                    %u\n", header->color_config_mono_chrome);
	mlog_printf(mlog, "color_config_color_description_present_flag[0, 1]: %u\n", header->color_config_color_description_present_flag);
	mlog_printf(mlog, "color_config_color_primaries[0, 255]:              %u (%s)\n", header->color_config_color_primaries,
		av1_string_color_primaries(header->color_config_color_primaries));
	mlog_printf(mlog, "color_config_transfer_characteristics[0, 255]:     %u (%s)\n", header->color_config_transfer_characteristics,
		av1_string_transfer_characteristics(header->color_config_transfer_characteristics));
	mlog_printf(mlog, "color_config_matrix_coefficients[0, 255]:          %u (%s)\n", header->color_config_matrix_coefficients,
		av1_string_matrix_coefficients(header->color_config_matrix_coefficients));
	mlog_printf(mlog, "color_config_color_range[0, 1]:                    %u\n", header->color_config_color_range);
	mlog_printf(mlog, "color_config_subsampling_x[0, 1]:                  %u\n", header->color_config_subsampling_x);
	mlog_printf(mlog, "color_config_subsampling_y[0, 1]:                  %u\n", header->color_config_subsampling_y);
	mlog_printf(mlog, "color_config_chroma_sample_position[0, 3]:         %u (%s)\n", header->color_config_chroma_sample_position,
		av1_string_chroma_sample_position(header->color_config_chroma_sample_position));
	mlog_printf(mlog, "color_config_separate_uv_delta_q[0, 1]:            %u\n", header->color_config_separate_uv_delta_q);
	mlog_printf(mlog, "film_grain_params_present[0, 1]:                   %u\n", header->film_grain_params_present);
	mlog_printf(mlog, "OrderHintBits: %u\n", av1_obu_sequence_header_OrderHintBits(header));
	mlog_printf(mlog, "BitDepth:      %u\n", av1_obu_sequence_header_BitDepth(header));
	mlog_printf(mlog, "NumPlanes:     %u\n", av1_obu_sequence_header_NumPlanes(header));
}
