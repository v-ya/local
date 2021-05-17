#include "uncompressed_header.h"
#include "obu_sequence_header.h"
#include "obu_header.h"
#include "../av1_ref.h"

#define ALL_FRAMES  ((1u << NUM_REF_FRAMES) - 1)

av1_uncompressed_header_t* av1_uncompressed_header_init(av1_uncompressed_header_t *restrict header)
{
	if (av1_frame_render_size_init(&header->extra.frame_render_size) &&
		av1_tile_info_init(&header->extra.tile_info) &&
		av1_quantization_params_init(&header->extra.quantization_params) &&
		av1_segmentation_params_init(&header->extra.segmentation_params))
	{
		uintptr_t i;
		memset(header, 0, offsetof(av1_uncompressed_header_t, extra));
		for (i = 0; i < REFS_PER_FRAME; ++i)
			header->ref_frame_idx[i] = REF_NONE_FRAME;
		header->interpolation_filter = av1_interpolation_filter_switchable;
		return header;
	}
	return NULL;
}

uint32_t av1_uncompressed_header_FrameIsIntra(const av1_uncompressed_header_t *restrict header)
{
	return (header->frame_type == av1_frame_type_key ||
		header->frame_type == av1_frame_type_intra_only);
}

av1_uncompressed_header_t* av1_uncompressed_header_read(av1_uncompressed_header_t *restrict header, av1_bits_reader_t *restrict reader, const struct av1_obu_header_t *restrict oh, av1_env_t *restrict env)
{
	const av1_obu_sequence_header_t *restrict sh;
	av1_ref_t *restrict ref;
	uint64_t v;
	uintptr_t i, n;
	uint32_t id_len; // [3, 25]
	uint32_t frame_is_intra;
	uint32_t ref_frame;
	uint32_t ref_index;
	uint32_t u32;
	sh = env->sequence_header;
	ref = env->ref;
	id_len = 0;
	if (sh->frame_id_numbers_present_flag)
		id_len = sh->additional_frame_id_length + sh->delta_frame_id_length;
	if (sh->reduced_still_picture_header)
	{
		header->show_existing_frame = 0;
		header->frame_type = av1_frame_type_key;
		header->show_frame = 1;
		header->showable_frame = 0;
	}
	else
	{
		// (1) show_existing_frame
		if (!av1_bits_flag_read(reader, &header->show_existing_frame))
			goto label_fail;
		if (header->show_existing_frame)
		{
			// (3) frame_to_show_map_idx
			if (!av1_bits_uint_read(reader, &v, 3))
				goto label_fail;
			header->frame_to_show_map_idx = (uint8_t) v;
			if (sh->decoder_model_info_present_flag && !sh->extra.timing_info.num_ticks_per_picture)
			{
				// n = frame_presentation_time_length
				// (n) frame_presentation_time
				if (!av1_bits_uint_read(reader, &v, sh->extra.decode_model_info.frame_presentation_time_length))
					goto label_fail;
				header->frame_presentation_time = (uint32_t) v;
			}
			header->refresh_frame_flags = 0;
			if (sh->frame_id_numbers_present_flag)
			{
				// (id_len) display_frame_id
				if (!av1_bits_uint_read(reader, &v, id_len))
					goto label_fail;
				header->display_frame_id = (uint32_t) v;
			}
			header->frame_type = ref->frame_type[header->frame_to_show_map_idx];
			if (header->frame_type == av1_frame_type_key)
				header->refresh_frame_flags = ALL_FRAMES;
			if (sh->film_grain_params_present)
			{
				/// TODO: load_grain_params(frame_to_show_map_idx)
			}
			goto label_return;
		}
		// (2) frame_type
		// (1) show_frame
		if (!av1_bits_uint_read(reader, &v, 3))
			goto label_fail;
		header->frame_type = (av1_frame_type_t) (v >> 1);
		header->show_frame = !!(v & 0x01);
		if (header->show_frame && sh->decoder_model_info_present_flag && !sh->extra.timing_info.num_ticks_per_picture)
		{
			// n = frame_presentation_time_length
			// (n) frame_presentation_time
			if (!av1_bits_uint_read(reader, &v, sh->extra.decode_model_info.frame_presentation_time_length))
				goto label_fail;
			header->frame_presentation_time = (uint32_t) v;
		}
		if (header->show_frame)
			header->showable_frame = (header->frame_type != av1_frame_type_key);
		else
		{
			// (1) showable_frame
			if (!av1_bits_flag_read(reader, &header->showable_frame))
				goto label_fail;
		}
		if (header->frame_type == av1_frame_type_switch ||
			(header->frame_type == av1_frame_type_key && header->show_frame))
			header->error_resilient_mode = 1;
		else
		{
			// (1) error_resilient_mode
			if (!av1_bits_flag_read(reader, &header->error_resilient_mode))
				goto label_fail;
		}
	}
	frame_is_intra = av1_uncompressed_header_FrameIsIntra(header);
	if (header->frame_type == av1_frame_type_key && header->show_frame)
	{
		// clear ref ?
		for (i = 0; i < NUM_REF_FRAMES; ++i)
		{
			ref->valid[i] = 0;
			ref->order_hint[i] = 0;
		}
		for (i = REF_LAST_FRAME; i < NUM_REF_FRAMES; ++i)
			header->order_hints[i] = 0;
	}
	// (1) disable_cdf_update
	if (!av1_bits_flag_read(reader, &header->disable_cdf_update))
		goto label_fail;
	if (sh->seq_force_screen_content_tools == SELECT_SCREEN_CONTENT_TOOLS)
	{
		// (1) allow_screen_content_tools
		if (!av1_bits_flag_read(reader, &header->allow_screen_content_tools))
			goto label_fail;
	}
	else header->allow_screen_content_tools = sh->seq_force_screen_content_tools;
	if (header->allow_screen_content_tools)
	{
		if (sh->seq_force_integer_mv == SELECT_INTEGER_MV)
		{
			// (1) force_integer_mv
			if (!av1_bits_flag_read(reader, &header->force_integer_mv))
				goto label_fail;
		}
		else header->force_integer_mv = sh->seq_force_integer_mv;
	}
	else header->force_integer_mv = 0;
	if (frame_is_intra)
		header->force_integer_mv = 1;
	if (sh->frame_id_numbers_present_flag)
	{
		/// TODO: PrevFrameID = current_frame_id
		// (id_len) current_frame_id
		if (!av1_bits_uint_read(reader, &v, id_len))
			goto label_fail;
		header->current_frame_id = (uint32_t) v;
		av1_ref_mark_ref_frames(ref, header->current_frame_id, sh->additional_frame_id_length, sh->delta_frame_id_length);
	}
	else header->current_frame_id = 0;
	if (header->frame_type == av1_frame_type_switch)
		header->frame_size_override_flag = 1;
	else if (sh->reduced_still_picture_header)
		header->frame_size_override_flag = 0;
	else
	{
		// (1) frame_size_override_flag
		if (!av1_bits_flag_read(reader, &header->frame_size_override_flag))
			goto label_fail;
	}
	// (order_hint_bits) order_hint
	if (!av1_bits_uint_read(reader, &v, sh->order_hint_bits))
		goto label_fail;
	header->order_hints[REF_INTRA_FRAME] = (uint32_t) v;
	if (frame_is_intra || header->error_resilient_mode)
		header->primary_ref_frame = PRIMARY_REF_NONE;
	else
	{
		// (3) primary_ref_frame
		if (!av1_bits_uint_read(reader, &v, 3))
			goto label_fail;
		header->primary_ref_frame = (uint8_t) v;
	}
	if (sh->decoder_model_info_present_flag)
	{
		// (1) buffer_removal_time_present_flag
		if (!av1_bits_flag_read(reader, &header->buffer_removal_time_present_flag))
			goto label_fail;
		if (header->buffer_removal_time_present_flag)
		{
			n = (uintptr_t) sh->operating_points_cnt;
			for (i = 0; i < n; ++i)
			{
				if (sh->decoder_model_present_for_this_op[i])
				{
					if (!sh->operating_point_idc[i] ||
						((sh->operating_point_idc[i] & (1 << oh->temporal_id)) &&
							(sh->operating_point_idc[i] & (1 << (oh->spatial_id + 8)))))
					{
						// n = buffer_removal_time_length
						// (n) buffer_removal_time[i]
						if (!av1_bits_uint_read(reader, &v, sh->extra.decode_model_info.buffer_removal_time_length))
							goto label_fail;
						header->buffer_removal_time[i] = (uint32_t) v;
					}
				}
			}
		}
	}
	header->allow_high_precision_mv = 0;
	header->use_ref_frame_mvs = 0;
	header->allow_intrabc = 0;
	if (header->frame_type == av1_frame_type_switch ||
		(header->frame_type == av1_frame_type_key && header->show_frame))
		header->refresh_frame_flags = ALL_FRAMES;
	else
	{
		// (8) refresh_frame_flags
		if (!av1_bits_uint_read(reader, &v, 8))
			goto label_fail;
		header->refresh_frame_flags = (uint32_t) v;
	}
	if (!frame_is_intra || header->refresh_frame_flags != ALL_FRAMES)
	{
		if (header->error_resilient_mode && sh->enable_order_hint)
		{
			for (i = 0; i < NUM_REF_FRAMES; ++i)
			{
				// (order_hint_bits) ref_order_hint[i]
				if (!av1_bits_uint_read(reader, &v, sh->order_hint_bits))
					goto label_fail;
				header->ref_order_hint[i] = (uint32_t) v;
				if (header->ref_order_hint[i] != ref->order_hint[i])
					ref->valid[i] = 0;
			}
		}
	}
	if (frame_is_intra)
	{
		if (!av1_frame_render_size_read(&header->extra.frame_render_size, reader, sh, header->frame_size_override_flag))
			goto label_fail;
		if (header->allow_screen_content_tools &&
			header->extra.frame_render_size.UpscaledWidth == header->extra.frame_render_size.FrameWidth)
		{
			// (1) allow_intrabc
			if (!av1_bits_flag_read(reader, &header->allow_intrabc))
				goto label_fail;
		}
	}
	else
	{
		if (!sh->enable_order_hint)
			header->frame_refs_short_signaling = 0;
		else
		{
			// (1) frame_refs_short_signaling
			if (!av1_bits_flag_read(reader, &header->frame_refs_short_signaling))
				goto label_fail;
			if (header->frame_refs_short_signaling)
			{
				// (3) last_frame_idx
				// (3) gold_frame_idx
				if (!av1_bits_uint_read(reader, &v, 6))
					goto label_fail;
				header->last_frame_idx = (uint8_t) (v >> 3);
				header->gold_frame_idx = (uint8_t) (v & 7);
				/// TODO: set_frame_refs()
				for (i = 0; i < REFS_PER_FRAME; ++i)
					header->ref_frame_idx[i] = REF_NONE_FRAME;
				header->ref_frame_idx[REF_LAST_FRAME - REF_LAST_FRAME] = header->last_frame_idx;
				header->ref_frame_idx[REF_GOLDEN_FRAME - REF_LAST_FRAME] = header->gold_frame_idx;
				// the values stored within the RefOrderHint array (these values represent the least significant bits of the expected output order of the frames).
			}
		}
		for (i = 0; i < REFS_PER_FRAME; ++i)
		{
			if (!header->frame_refs_short_signaling)
			{
				// (3) ref_frame_idx[i]
				if (!av1_bits_uint_read(reader, &v, 3))
					goto label_fail;
				header->ref_frame_idx[i] = (uint32_t) v;
			}
			if (sh->frame_id_numbers_present_flag)
			{
				// n = delta_frame_id_length
				// (n) delta_frame_id_minus_1
				if (!av1_bits_uint_read(reader, &v, sh->delta_frame_id_length))
					goto label_fail;
				header->delta_frame_id[i] = (uint32_t) (v + 1);
			}
		}
		if (header->frame_size_override_flag && !header->error_resilient_mode)
		{
			for (i = 0; i < REFS_PER_FRAME; ++i)
			{
				// (1) found_ref
				if (!av1_bits_flag_read(reader, &header->found_ref))
					goto label_fail;
				if (header->found_ref)
				{
					if (~(ref_index = header->ref_frame_idx[i]))
					{
						header->extra.frame_render_size.UpscaledWidth = ref->upscaled_width[ref_index];
						header->extra.frame_render_size.FrameWidth = header->extra.frame_render_size.UpscaledWidth;
						header->extra.frame_render_size.RenderWidth = ref->render_width[ref_index];
						header->extra.frame_render_size.RenderHeight = ref->render_height[ref_index];
					}
					header->found_ref_bits = (uint8_t) (i + 1);
					if (!av1_frame_render_size_read_superres_params(&header->extra.frame_render_size, reader, sh))
						goto label_fail;
					goto label_found_ref_next;
				}
			}
			header->found_ref_bits = REFS_PER_FRAME;
		}
		if (!av1_frame_render_size_read(&header->extra.frame_render_size, reader, sh, header->frame_size_override_flag))
			goto label_fail;
		label_found_ref_next:
		if (header->force_integer_mv)
			header->allow_high_precision_mv = 0;
		else
		{
			// (1) allow_high_precision_mv
			if (!av1_bits_flag_read(reader, &header->allow_high_precision_mv))
				goto label_fail;
		}
		// (1) is_filter_switchable
		if (!av1_bits_flag_read(reader, &header->is_filter_switchable))
			goto label_fail;
		if (header->is_filter_switchable)
			header->interpolation_filter = av1_interpolation_filter_switchable;
		else
		{
			// (2) interpolation_filter
			if (!av1_bits_uint_read(reader, &v, 2))
				goto label_fail;
			header->interpolation_filter = (av1_interpolation_filter_t) v;
		}
		// (1) is_motion_mode_switchable
		if (!av1_bits_flag_read(reader, &header->is_motion_mode_switchable))
			goto label_fail;
		if (header->error_resilient_mode || !sh->enable_ref_frame_mvs)
			header->use_ref_frame_mvs = 0;
		else
		{
			// (1) use_ref_frame_mvs
			if (!av1_bits_flag_read(reader, &header->use_ref_frame_mvs))
				goto label_fail;
		}
		for (i = 0; i < REFS_PER_FRAME; ++i)
		{
			ref_frame = (uint32_t) (REF_LAST_FRAME + i);
			u32 = 0;
			if (~(ref_index = header->ref_frame_idx[i]))
				u32 = ref->order_hint[ref_index];
			header->order_hints[ref_frame] = u32;
			if (!sh->enable_order_hint)
				ref->frame_sign_bias[ref_frame] = 0;
			else ref->frame_sign_bias[ref_frame] =
				(av1_get_relative_dist(u32, header->order_hints[REF_INTRA_FRAME], sh->order_hint_bits) > 0);
		}
	}
	if (sh->reduced_still_picture_header || header->disable_cdf_update)
		header->disable_frame_end_update_cdf = 1;
	else
	{
		// (1) disable_frame_end_update_cdf
		if (!av1_bits_flag_read(reader, &header->disable_frame_end_update_cdf))
			goto label_fail;
	}
	if (header->primary_ref_frame == PRIMARY_REF_NONE)
	{
		/// TODO: init_non_coeff_cdfs()
		/// TODO: setup_past_independence()
	}
	else
	{
		/// TODO: load_cdfs(ref_frame_idx[primary_ref_frame])
		/// TODO: load_previous()
	}
	if (header->use_ref_frame_mvs)
	{
		/// TODO: motion_field_estimation()
	}
	// tile_info()
	if (!av1_tile_info_read(&header->extra.tile_info, reader, &header->extra.frame_render_size, sh->use_128x128_superblock))
		goto label_fail;
	// quantization_params()
	if (!av1_quantization_params_read(&header->extra.quantization_params, reader, sh))
		goto label_fail;
	// segmentation_params()
	if (!av1_segmentation_params_read(&header->extra.segmentation_params, reader, header->primary_ref_frame))
		goto label_fail;
	label_return:
	return header;
	label_fail:
	return NULL;
}

const av1_uncompressed_header_t* av1_uncompressed_header_write(const av1_uncompressed_header_t *restrict header, av1_bits_writer_t *restrict writer, const struct av1_obu_header_t *restrict oh, av1_env_t *restrict env)
{
	const av1_obu_sequence_header_t *restrict sh;
	uintptr_t i, n;
	uint32_t id_len; // [3, 25]
	uint32_t frame_is_intra;
	sh = env->sequence_header;
	id_len = 0;
	if (sh->frame_id_numbers_present_flag)
		id_len = sh->additional_frame_id_length + sh->delta_frame_id_length;
	if (!sh->reduced_still_picture_header)
	{
		// (1) show_existing_frame
		if (!av1_bits_flag_write(writer, header->show_existing_frame))
			goto label_fail;
		if (header->show_existing_frame)
		{
			// (3) frame_to_show_map_idx
			if (!av1_bits_uint_write(writer, header->frame_to_show_map_idx, 3))
				goto label_fail;
			if (sh->decoder_model_info_present_flag && !sh->extra.timing_info.num_ticks_per_picture)
			{
				// n = frame_presentation_time_length
				// (n) frame_presentation_time
				if (!av1_bits_uint_write(writer, header->frame_presentation_time, sh->extra.decode_model_info.frame_presentation_time_length))
					goto label_fail;
			}
			if (sh->frame_id_numbers_present_flag)
			{
				// (id_len) display_frame_id
				if (!av1_bits_uint_write(writer, header->display_frame_id, id_len))
					goto label_fail;
			}
			goto label_return;
		}
		// (2) frame_type
		// (1) show_frame
		if (!av1_bits_uint_write(writer,
			((uint64_t) header->frame_type << 1) |
			((uint64_t) header->show_frame),
			3))
			goto label_fail;
		if (header->show_frame && sh->decoder_model_info_present_flag && !sh->extra.timing_info.num_ticks_per_picture)
		{
			// n = frame_presentation_time_length
			// (n) frame_presentation_time
			if (!av1_bits_uint_write(writer, header->frame_presentation_time, sh->extra.decode_model_info.frame_presentation_time_length))
				goto label_fail;
		}
		if (!header->show_frame)
		{
			// (1) showable_frame
			if (!av1_bits_flag_write(writer, header->showable_frame))
				goto label_fail;
		}
		if (header->frame_type == av1_frame_type_switch ||
			(header->frame_type == av1_frame_type_key && header->show_frame)) ;
		else
		{
			// (1) error_resilient_mode
			if (!av1_bits_flag_write(writer, header->error_resilient_mode))
				goto label_fail;
		}
	}
	frame_is_intra = av1_uncompressed_header_FrameIsIntra(header);
	// (1) disable_cdf_update
	if (!av1_bits_flag_write(writer, header->disable_cdf_update))
		goto label_fail;
	if (sh->seq_force_screen_content_tools == SELECT_SCREEN_CONTENT_TOOLS)
	{
		// (1) allow_screen_content_tools
		if (!av1_bits_flag_write(writer, header->allow_screen_content_tools))
			goto label_fail;
	}
	if (header->allow_screen_content_tools)
	{
		if (sh->seq_force_integer_mv == SELECT_INTEGER_MV)
		{
			// (1) force_integer_mv
			if (!av1_bits_flag_write(writer, header->force_integer_mv))
				goto label_fail;
		}
	}
	if (sh->frame_id_numbers_present_flag)
	{
		// (id_len) current_frame_id
		if (!av1_bits_uint_write(writer, header->current_frame_id, id_len))
			goto label_fail;
	}
	if (header->frame_type != av1_frame_type_switch && !sh->reduced_still_picture_header)
	{
		// (1) frame_size_override_flag
		if (!av1_bits_flag_write(writer, header->frame_size_override_flag))
			goto label_fail;
	}
	// (order_hint_bits) order_hint
	if (!av1_bits_uint_write(writer, header->order_hints[REF_INTRA_FRAME], sh->order_hint_bits))
		goto label_fail;
	if (!frame_is_intra && !header->error_resilient_mode)
	{
		// (3) primary_ref_frame
		if (!av1_bits_uint_write(writer, header->primary_ref_frame, 3))
			goto label_fail;
	}
	if (sh->decoder_model_info_present_flag)
	{
		// (1) buffer_removal_time_present_flag
		if (!av1_bits_flag_write(writer, header->buffer_removal_time_present_flag))
			goto label_fail;
		if (header->buffer_removal_time_present_flag)
		{
			n = (uintptr_t) sh->operating_points_cnt;
			for (i = 0; i < n; ++i)
			{
				if (sh->decoder_model_present_for_this_op[i])
				{
					if (!sh->operating_point_idc[i] ||
						((sh->operating_point_idc[i] & (1 << oh->temporal_id)) &&
							(sh->operating_point_idc[i] & (1 << (oh->spatial_id + 8)))))
					{
						// n = buffer_removal_time_length
						// (n) buffer_removal_time[i]
						if (!av1_bits_uint_write(writer, header->buffer_removal_time[i], sh->extra.decode_model_info.buffer_removal_time_length))
							goto label_fail;
					}
				}
			}
		}
	}
	if (header->frame_type == av1_frame_type_switch ||
		(header->frame_type == av1_frame_type_key && header->show_frame)) ;
	else
	{
		// (8) refresh_frame_flags
		if (!av1_bits_uint_write(writer, header->refresh_frame_flags, 8))
			goto label_fail;
	}
	if (!frame_is_intra || header->refresh_frame_flags != ALL_FRAMES)
	{
		if (header->error_resilient_mode && sh->enable_order_hint)
		{
			for (i = 0; i < NUM_REF_FRAMES; ++i)
			{
				// (order_hint_bits) ref_order_hint[i]
				if (!av1_bits_uint_write(writer, header->ref_order_hint[i], sh->order_hint_bits))
					goto label_fail;
			}
		}
	}
	if (frame_is_intra)
	{
		if (!av1_frame_render_size_write(&header->extra.frame_render_size, writer, sh, header->frame_size_override_flag))
			goto label_fail;
		if (header->allow_screen_content_tools &&
			header->extra.frame_render_size.UpscaledWidth == header->extra.frame_render_size.FrameWidth)
		{
			// (1) allow_intrabc
			if (!av1_bits_flag_write(writer, header->allow_intrabc))
				goto label_fail;
		}
	}
	else
	{
		if (sh->enable_order_hint)
		{
			// (1) frame_refs_short_signaling
			if (!av1_bits_flag_write(writer, header->frame_refs_short_signaling))
				goto label_fail;
			if (header->frame_refs_short_signaling)
			{
				// (3) last_frame_idx
				// (3) gold_frame_idx
				if (!av1_bits_uint_write(writer,
					((uint64_t) header->last_frame_idx << 3) |
					((uint64_t) header->gold_frame_idx),
					6)) goto label_fail;
			}
		}
		for (i = 0; i < REFS_PER_FRAME; ++i)
		{
			if (!header->frame_refs_short_signaling)
			{
				// (3) ref_frame_idx[i]
				if (!av1_bits_uint_write(writer, header->ref_frame_idx[i], 3))
					goto label_fail;
			}
			if (sh->frame_id_numbers_present_flag)
			{
				// n = delta_frame_id_length
				// (n) delta_frame_id_minus_1
				if (!av1_bits_uint_write(writer, header->delta_frame_id[i] - 1, sh->delta_frame_id_length))
					goto label_fail;
			}
		}
		if (header->frame_size_override_flag && !header->error_resilient_mode)
		{
			// (found_ref_bits) 0...1
			if (!av1_bits_uint_write(writer, header->found_ref, header->found_ref_bits))
				goto label_fail;
			if (header->found_ref)
			{
				if (!av1_frame_render_size_write_superres_params(&header->extra.frame_render_size, writer, sh))
					goto label_fail;
				goto label_found_ref_next;
			}
		}
		if (!av1_frame_render_size_write(&header->extra.frame_render_size, writer, sh, header->frame_size_override_flag))
			goto label_fail;
		label_found_ref_next:
		if (!header->force_integer_mv)
		{
			// (1) allow_high_precision_mv
			if (!av1_bits_flag_write(writer, header->allow_high_precision_mv))
				goto label_fail;
		}
		// (1) is_filter_switchable
		if (!av1_bits_flag_write(writer, header->is_filter_switchable))
			goto label_fail;
		if (!header->is_filter_switchable)
		{
			// (2) interpolation_filter
			if (!av1_bits_uint_write(writer, header->interpolation_filter, 2))
				goto label_fail;
		}
		// (1) is_motion_mode_switchable
		if (!av1_bits_flag_write(writer, header->is_motion_mode_switchable))
			goto label_fail;
		if (!header->error_resilient_mode && sh->enable_ref_frame_mvs)
		{
			// (1) use_ref_frame_mvs
			if (!av1_bits_flag_write(writer, header->use_ref_frame_mvs))
				goto label_fail;
		}
	}
	if (!sh->reduced_still_picture_header && !header->disable_cdf_update)
	{
		// (1) disable_frame_end_update_cdf
		if (!av1_bits_flag_write(writer, header->disable_frame_end_update_cdf))
			goto label_fail;
	}
	// tile_info()
	if (!av1_tile_info_write(&header->extra.tile_info, writer, &header->extra.frame_render_size, sh->use_128x128_superblock))
		goto label_fail;
	// quantization_params()
	if (!av1_quantization_params_write(&header->extra.quantization_params, writer, sh))
		goto label_fail;
	// segmentation_params()
	if (!av1_segmentation_params_write(&header->extra.segmentation_params, writer, header->primary_ref_frame))
		goto label_fail;
	label_return:
	return header;
	label_fail:
	return NULL;
}

uint64_t av1_uncompressed_header_bits(const av1_uncompressed_header_t *restrict header, const struct av1_obu_header_t *restrict oh, av1_env_t *restrict env)
{
	const av1_obu_sequence_header_t *restrict sh;
	uint64_t size;
	uintptr_t i, n;
	uint32_t id_len; // [3, 25]
	uint32_t frame_is_intra;
	sh = env->sequence_header;
	size = 0;
	id_len = 0;
	if (sh->frame_id_numbers_present_flag)
		id_len = sh->additional_frame_id_length + sh->delta_frame_id_length;
	if (!sh->reduced_still_picture_header)
	{
		// (1) show_existing_frame
		size += 1;
		if (header->show_existing_frame)
		{
			// (3) frame_to_show_map_idx
			size += 3;
			if (sh->decoder_model_info_present_flag && !sh->extra.timing_info.num_ticks_per_picture)
			{
				// n = frame_presentation_time_length
				// (n) frame_presentation_time
				size += sh->extra.decode_model_info.frame_presentation_time_length;
			}
			if (sh->frame_id_numbers_present_flag)
			{
				// (id_len) display_frame_id
				size += id_len;
			}
			goto label_return;
		}
		// (2) frame_type
		// (1) show_frame
		size += 3;
		if (header->show_frame && sh->decoder_model_info_present_flag && !sh->extra.timing_info.num_ticks_per_picture)
		{
			// n = frame_presentation_time_length
			// (n) frame_presentation_time
			size += sh->extra.decode_model_info.frame_presentation_time_length;
		}
		if (!header->show_frame)
		{
			// (1) showable_frame
			size += 1;
		}
		if (header->frame_type == av1_frame_type_switch ||
			(header->frame_type == av1_frame_type_key && header->show_frame)) ;
		else
		{
			// (1) error_resilient_mode
			size += 1;
		}
	}
	frame_is_intra = av1_uncompressed_header_FrameIsIntra(header);
	// (1) disable_cdf_update
	size += 1;
	if (sh->seq_force_screen_content_tools == SELECT_SCREEN_CONTENT_TOOLS)
	{
		// (1) allow_screen_content_tools
		size += 1;
	}
	if (header->allow_screen_content_tools)
	{
		if (sh->seq_force_integer_mv == SELECT_INTEGER_MV)
		{
			// (1) force_integer_mv
			size += 1;
		}
	}
	if (sh->frame_id_numbers_present_flag)
	{
		// (id_len) current_frame_id
		size += id_len;
	}
	if (header->frame_type != av1_frame_type_switch && !sh->reduced_still_picture_header)
	{
		// (1) frame_size_override_flag
		size += 1;
	}
	// (order_hint_bits) order_hint
	size += sh->order_hint_bits;
	if (!frame_is_intra && !header->error_resilient_mode)
	{
		// (3) primary_ref_frame
		size += 3;
	}
	if (sh->decoder_model_info_present_flag)
	{
		// (1) buffer_removal_time_present_flag
		size += 1;
		if (header->buffer_removal_time_present_flag)
		{
			n = (uintptr_t) sh->operating_points_cnt;
			for (i = 0; i < n; ++i)
			{
				if (sh->decoder_model_present_for_this_op[i])
				{
					if (!sh->operating_point_idc[i] ||
						((sh->operating_point_idc[i] & (1 << oh->temporal_id)) &&
							(sh->operating_point_idc[i] & (1 << (oh->spatial_id + 8)))))
					{
						// n = buffer_removal_time_length
						// (n) buffer_removal_time[i]
						size += sh->extra.decode_model_info.buffer_removal_time_length;
					}
				}
			}
		}
	}
	if (header->frame_type == av1_frame_type_switch ||
		(header->frame_type == av1_frame_type_key && header->show_frame)) ;
	else
	{
		// (8) refresh_frame_flags
		size += 8;
	}
	if (!frame_is_intra || header->refresh_frame_flags != ALL_FRAMES)
	{
		if (header->error_resilient_mode && sh->enable_order_hint)
		{
			for (i = 0; i < NUM_REF_FRAMES; ++i)
			{
				// (order_hint_bits) ref_order_hint[i]
				size += sh->order_hint_bits;
			}
		}
	}
	if (frame_is_intra)
	{
		size += av1_frame_render_size_bits(&header->extra.frame_render_size, sh, header->frame_size_override_flag);
		if (header->allow_screen_content_tools &&
			header->extra.frame_render_size.UpscaledWidth == header->extra.frame_render_size.FrameWidth)
		{
			// (1) allow_intrabc
			size += 1;
		}
	}
	else
	{
		if (sh->enable_order_hint)
		{
			// (1) frame_refs_short_signaling
			size += 1;
			if (header->frame_refs_short_signaling)
			{
				// (3) last_frame_idx
				// (3) gold_frame_idx
				size += 6;
			}
		}
		if (!header->frame_refs_short_signaling)
		{
			// REFS_PER_FRAME * (3) ref_frame_idx[i]
			size += 3 * REFS_PER_FRAME;
		}
		if (sh->frame_id_numbers_present_flag)
		{
			// n = delta_frame_id_length
			// REFS_PER_FRAME * (n) delta_frame_id_minus_1
			size += (uint64_t) sh->delta_frame_id_length * REFS_PER_FRAME;
		}
		if (header->frame_size_override_flag && !header->error_resilient_mode)
		{
			// (found_ref_bits) 0...1
			size += header->found_ref_bits;
			if (header->found_ref)
			{
				size += av1_frame_render_size_bits_superres_params(&header->extra.frame_render_size, sh);
				goto label_found_ref_next;
			}
		}
		size += av1_frame_render_size_bits(&header->extra.frame_render_size, sh, header->frame_size_override_flag);
		label_found_ref_next:
		if (!header->force_integer_mv)
		{
			// (1) allow_high_precision_mv
			size += 1;
		}
		// (1) is_filter_switchable
		size += 1;
		if (!header->is_filter_switchable)
		{
			// (2) interpolation_filter
			size += 2;
		}
		// (1) is_motion_mode_switchable
		size += 1;
		if (!header->error_resilient_mode && sh->enable_ref_frame_mvs)
		{
			// (1) use_ref_frame_mvs
			size += 1;
		}
	}
	if (!sh->reduced_still_picture_header && !header->disable_cdf_update)
	{
		// (1) disable_frame_end_update_cdf
		size += 1;
	}
	// tile_info()
	size += av1_tile_info_bits(&header->extra.tile_info, &header->extra.frame_render_size, sh->use_128x128_superblock);
	// quantization_params()
	size += av1_quantization_params_bits(&header->extra.quantization_params, sh);
	// segmentation_params()
	size += av1_segmentation_params_bits(&header->extra.segmentation_params, header->primary_ref_frame);
	label_return:
	return size;
}

void av1_uncompressed_header_dump(const av1_uncompressed_header_t *restrict header, mlog_s *restrict mlog, av1_env_t *restrict env)
{
	const av1_obu_sequence_header_t *restrict sh;
	// uintptr_t i, n;
	sh = env->sequence_header;
	mlog_printf(mlog, "show_existing_frame[0, 1]:                                      %u\n", header->show_existing_frame);
	mlog_printf(mlog, "frame_type[0, 3]:                                               %u (%s)\n",
		header->frame_type, av1_string_frame_type(header->frame_type));
	mlog_printf(mlog, "show_frame[0, 1]:                                               %u\n", header->show_frame);
	mlog_printf(mlog, "frame_to_show_map_idx[0, 7]:                                    %u\n", header->frame_to_show_map_idx);
	mlog_printf(mlog, "frame_presentation_time[0, 2^frame_presentation_time_length-1]: %u\n", header->frame_presentation_time);
	mlog_printf(mlog, "showable_frame[0, 1]:                                           %u\n", header->showable_frame);
	mlog_printf(mlog, "error_resilient_mode[0, 1]:                                     %u\n", header->error_resilient_mode);
	mlog_printf(mlog, "disable_cdf_update[0, 1]:                                       %u\n", header->disable_cdf_update);
	mlog_printf(mlog, "allow_screen_content_tools[0, 1]:                               %u\n", header->allow_screen_content_tools);
	mlog_printf(mlog, "force_integer_mv[0, 1]:                                         %u\n", header->force_integer_mv);
	mlog_printf(mlog, "current_frame_id[0, 2^(%2u+%2u)-1]:                               %08x (%u, %u)\n",
		sh->additional_frame_id_length, sh->delta_frame_id_length,
		header->current_frame_id,
		(header->current_frame_id >> sh->delta_frame_id_length) & ((1u << sh->additional_frame_id_length) - 1),
		header->current_frame_id & ((1u << sh->delta_frame_id_length) - 1));
	mlog_printf(mlog, "display_frame_id[0, 2^(%2u+%2u)-1]:                               %08x (%u, %u)\n",
		sh->additional_frame_id_length, sh->delta_frame_id_length,
		header->display_frame_id,
		(header->display_frame_id >> sh->delta_frame_id_length) & ((1u << sh->additional_frame_id_length) - 1),
		header->display_frame_id & ((1u << sh->delta_frame_id_length) - 1));
	mlog_printf(mlog, "frame_size_override_flag[0, 1]:                                 %u\n", header->frame_size_override_flag);
	mlog_printf(mlog, "primary_ref_frame[0, 7]:                                        %u\n", header->primary_ref_frame);
	mlog_printf(mlog, "buffer_removal_time_present_flag[0, 1]:                         %u\n", header->buffer_removal_time_present_flag);
	mlog_printf(mlog, "refresh_frame_flags[0, 255]:                                    0x%02x\n", header->refresh_frame_flags);
	mlog_printf(mlog, "frame_refs_short_signaling[0, 1]:                               %u\n", header->frame_refs_short_signaling);
	mlog_printf(mlog, "last_frame_idx[0, 7]:                                           %u\n", header->last_frame_idx);
	mlog_printf(mlog, "gold_frame_idx[0, 7]:                                           %u\n", header->gold_frame_idx);
	av1_frame_render_size_dump(&header->extra.frame_render_size, mlog);
	mlog_printf(mlog, "allow_intrabc[0, 1]:                                            %u\n", header->allow_intrabc);
	mlog_printf(mlog, "allow_high_precision_mv[0, 1]:                                  %u\n", header->allow_high_precision_mv);
	mlog_printf(mlog, "is_filter_switchable[0, 1]:                                     %u\n", header->is_filter_switchable);
	mlog_printf(mlog, "interpolation_filter[0, 1]:                                     %u (%s)\n",
		header->interpolation_filter, av1_string_interpolation_filter(header->interpolation_filter));
	mlog_printf(mlog, "is_motion_mode_switchable[0, 1]:                                %u\n", header->is_motion_mode_switchable);
	mlog_printf(mlog, "use_ref_frame_mvs[0, 1]:                                        %u\n", header->use_ref_frame_mvs);
	mlog_printf(mlog, "disable_frame_end_update_cdf[0, 1]:                             %u\n", header->disable_frame_end_update_cdf);
	av1_tile_info_dump(&header->extra.tile_info, mlog);
	av1_quantization_params_dump(&header->extra.quantization_params, mlog);
	av1_segmentation_params_dump(&header->extra.segmentation_params, mlog);
}
