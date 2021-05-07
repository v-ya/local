#include "timing_info.h"

av1_timing_info_t* av1_timing_info_init(av1_timing_info_t *restrict header)
{
	return (av1_timing_info_t *) memset(header, 0, sizeof(*header));
}

av1_timing_info_t* av1_timing_info_read(av1_timing_info_t *restrict header, av1_bits_reader_t *restrict reader)
{
	uint64_t v;
	uint32_t n;
	uint8_t f;
	// (32) num_units_in_display_tick
	// (32) time_scale
	// ( 1) equal_picture_interval
	if (!av1_bits_uint_read(reader, &v, 32))
		goto label_fail;
	header->num_units_in_display_tick = (uint32_t) v;
	if (!av1_bits_uint_read(reader, &v, 32))
		goto label_fail;
	header->time_scale = (uint32_t) v;
	if (!av1_bits_flag_read(reader, &f))
		goto label_fail;
	if (f)
	{
		// (uvlc()) num_ticks_per_picture_minus_1
		if (!av1_bits_uvlc_read(reader, &n))
			goto label_fail;
		if (~n) goto label_fail;
		header->num_ticks_per_picture = n + 1;
	}
	else header->num_ticks_per_picture = 0;
	return header;
	label_fail:
	return NULL;
}

const av1_timing_info_t* av1_timing_info_write(const av1_timing_info_t *restrict header, av1_bits_writer_t *restrict writer)
{
	// (32) num_units_in_display_tick
	// (32) time_scale
	// ( 1) equal_picture_interval
	if (!av1_bits_uint_write(writer, header->num_units_in_display_tick, 32))
		goto label_fail;
	if (!av1_bits_uint_write(writer, header->time_scale, 32))
		goto label_fail;
	if (!av1_bits_flag_write(writer, !!header->num_ticks_per_picture))
		goto label_fail;
	if (header->num_ticks_per_picture)
	{
		// (uvlc()) num_ticks_per_picture_minus_1
		if (!av1_bits_uvlc_write(writer, header->num_ticks_per_picture - 1))
			goto label_fail;
	}
	return header;
	label_fail:
	return NULL;
}

uint64_t av1_timing_info_bits(const av1_timing_info_t *restrict header)
{
	return 65 + (header->num_ticks_per_picture?av1_bits_uvlc_bits(header->num_ticks_per_picture - 1):0);
}

void av1_timing_info_dump(const av1_timing_info_t *restrict header, mlog_s *restrict mlog)
{
	mlog_printf(mlog, "num_units_in_display_tick[0, 2^32-1]: %u\n", header->num_units_in_display_tick);
	mlog_printf(mlog, "time_scale[0, 2^32-1]:                %u\n", header->time_scale);
	mlog_printf(mlog, "num_ticks_per_picture[0, 2^32-1]:     %u\n", header->num_ticks_per_picture);
}
