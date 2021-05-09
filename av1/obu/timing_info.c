#include "timing_info.h"

av1_timing_info_t* av1_timing_info_init(av1_timing_info_t *restrict timing_info)
{
	return (av1_timing_info_t *) memset(timing_info, 0, sizeof(*timing_info));
}

av1_timing_info_t* av1_timing_info_read(av1_timing_info_t *restrict timing_info, av1_bits_reader_t *restrict reader)
{
	uint64_t v;
	uint32_t n;
	uint8_t f;
	// (32) num_units_in_display_tick
	// (32) time_scale
	// ( 1) equal_picture_interval
	if (!av1_bits_uint_read(reader, &v, 32))
		goto label_fail;
	timing_info->num_units_in_display_tick = (uint32_t) v;
	if (!av1_bits_uint_read(reader, &v, 32))
		goto label_fail;
	timing_info->time_scale = (uint32_t) v;
	if (!av1_bits_flag_read(reader, &f))
		goto label_fail;
	if (f)
	{
		// (uvlc()) num_ticks_per_picture_minus_1
		if (!av1_bits_uvlc_read(reader, &n))
			goto label_fail;
		if (~n) goto label_fail;
		timing_info->num_ticks_per_picture = n + 1;
	}
	else timing_info->num_ticks_per_picture = 0;
	return timing_info;
	label_fail:
	return NULL;
}

const av1_timing_info_t* av1_timing_info_write(const av1_timing_info_t *restrict timing_info, av1_bits_writer_t *restrict writer)
{
	// (32) num_units_in_display_tick
	// (32) time_scale
	// ( 1) equal_picture_interval
	if (!av1_bits_uint_write(writer, timing_info->num_units_in_display_tick, 32))
		goto label_fail;
	if (!av1_bits_uint_write(writer, timing_info->time_scale, 32))
		goto label_fail;
	if (!av1_bits_flag_write(writer, !!timing_info->num_ticks_per_picture))
		goto label_fail;
	if (timing_info->num_ticks_per_picture)
	{
		// (uvlc()) num_ticks_per_picture_minus_1
		if (!av1_bits_uvlc_write(writer, timing_info->num_ticks_per_picture - 1))
			goto label_fail;
	}
	return timing_info;
	label_fail:
	return NULL;
}

uint64_t av1_timing_info_bits(const av1_timing_info_t *restrict timing_info)
{
	return 65 + (timing_info->num_ticks_per_picture?av1_bits_uvlc_bits(timing_info->num_ticks_per_picture - 1):0);
}

void av1_timing_info_dump(const av1_timing_info_t *restrict timing_info, mlog_s *restrict mlog)
{
	mlog_printf(mlog, "num_units_in_display_tick[0, 2^32-1]: %u\n", timing_info->num_units_in_display_tick);
	mlog_printf(mlog, "time_scale[0, 2^32-1]:                %u\n", timing_info->time_scale);
	mlog_printf(mlog, "num_ticks_per_picture[0, 2^32-1]:     %u\n", timing_info->num_ticks_per_picture);
}
