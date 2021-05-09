#include "uncompressed_header.h"
#include "obu_sequence_header.h"
#include "define.h"

#define ALL_FRAMES  ((1u << NUM_REF_FRAMES) - 1)

av1_uncompressed_header_t* av1_uncompressed_header_init(av1_uncompressed_header_t *restrict header)
{
	return (av1_uncompressed_header_t *) memset(header, 0, sizeof(*header));
}

uint32_t av1_uncompressed_header_FrameIsIntra(const av1_uncompressed_header_t *restrict header)
{
	return (header->frame_type == av1_frame_type_key ||
		header->frame_type == av1_frame_type_intra_only);
}

av1_uncompressed_header_t* av1_uncompressed_header_read(av1_uncompressed_header_t *restrict header, av1_bits_reader_t *restrict reader, av1_env_t *restrict env)
{
	const av1_obu_sequence_header_t *restrict sh;
	uint64_t v;
	uint32_t id_len; // [3, 25]
	id_len = 0;
	if (!(sh = env->sequence_header))
		goto label_fail;
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
			/// TODO: frame_type = RefFrameType[frame_to_show_map_idx]
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
	label_return:
	return header;
	label_fail:
	return NULL;
}

const av1_uncompressed_header_t* av1_uncompressed_header_write(const av1_uncompressed_header_t *restrict header, av1_bits_writer_t *restrict writer, av1_env_t *restrict env)
{
	const av1_obu_sequence_header_t *restrict sh;
	uint32_t id_len; // [3, 25]
	id_len = 0;
	if (!(sh = env->sequence_header))
		goto label_fail;
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
	label_return:
	return header;
	label_fail:
	return NULL;
}

uint64_t av1_uncompressed_header_bits(const av1_uncompressed_header_t *restrict header, av1_env_t *restrict env)
{
	const av1_obu_sequence_header_t *restrict sh;
	uint64_t size;
	uint32_t id_len; // [3, 25]
	size = 0;
	id_len = 0;
	if (!(sh = env->sequence_header))
		goto label_return;
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
	label_return:
	return size;
}

static const char* av1_string_frame_type(av1_frame_type_t t)
{
	static const char *s[av1_frame_type$max] = {
		[av1_frame_type_key]        = "key",
		[av1_frame_type_inter]      = "inter",
		[av1_frame_type_intra_only] = "intra_only",
		[av1_frame_type_switch]     = "switch"
	};
	return (t < av1_frame_type$max)?s[t]:NULL;
}

void av1_uncompressed_header_dump(const av1_uncompressed_header_t *restrict header, mlog_s *restrict mlog, av1_env_t *restrict env)
{
	const av1_obu_sequence_header_t *restrict sh;
	if (!(sh = env->sequence_header))
		goto label_return;
	mlog_printf(mlog, "show_existing_frame[0, 1]:                                      %u\n", header->show_existing_frame);
	mlog_printf(mlog, "frame_type[0, 3]:                                               %u (%s)\n",
		header->frame_type, av1_string_frame_type(header->frame_type));
	mlog_printf(mlog, "show_frame[0, 1]:                                               %u\n", header->show_frame);
	mlog_printf(mlog, "frame_to_show_map_idx[0, 7]:                                    %u\n", header->frame_to_show_map_idx);
	mlog_printf(mlog, "frame_presentation_time[0, 2^frame_presentation_time_length-1]: %u\n", header->frame_presentation_time);
	mlog_printf(mlog, "showable_frame[0, 1]:                                           %u\n", header->showable_frame);
	mlog_printf(mlog, "error_resilient_mode[0, 1]:                                     %u\n", header->error_resilient_mode);
	mlog_printf(mlog, "display_frame_id[0, 2^(%2u+%2u)-1]:                               %08x (%u, %u)\n",
		sh->additional_frame_id_length, sh->delta_frame_id_length,
		header->display_frame_id,
		(header->display_frame_id >> sh->delta_frame_id_length) & ((1u << sh->additional_frame_id_length) - 1),
		header->display_frame_id & ((1u << sh->delta_frame_id_length) - 1));
	label_return:
	return ;
}
