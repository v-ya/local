#include "frame_render_size.h"
#include "obu_sequence_header.h"

av1_frame_render_size_t* av1_frame_render_size_init(av1_frame_render_size_t *restrict frsize)
{
	frsize->frame_width = 1;
	frsize->frame_height = 1;
	frsize->render_width = 1;
	frsize->render_height = 1;
	frsize->superres_denom = SUPERRES_NUM;
	frsize->UpscaledWidth = 1;
	frsize->FrameWidth = 1;
	frsize->FrameHeight = 1;
	frsize->RenderWidth = 1;
	frsize->RenderHeight = 1;
	frsize->MiCols = 2;
	frsize->MiRows = 2;
	return frsize;
}

av1_frame_render_size_t* av1_frame_render_size_read_superres_params(av1_frame_render_size_t *restrict frsize, av1_bits_reader_t *restrict reader, const struct av1_obu_sequence_header_t *restrict header)
{
	uint64_t v;
	uint8_t use_superres;
	use_superres = 0;
	if (header->enable_superres)
	{
		// (1) use_superres
		if (!av1_bits_flag_read(reader, &use_superres))
			goto label_fail;
	}
	frsize->superres_denom = SUPERRES_NUM;
	if (use_superres)
	{
		// (SUPERRES_DENOM_BITS) coded_denom
		if (!av1_bits_uint_read(reader, &v, SUPERRES_DENOM_BITS))
			goto label_fail;
		frsize->superres_denom = (uint32_t) (v + SUPERRES_DENOM_MIN);
	}
	frsize->UpscaledWidth = frsize->FrameWidth;
	frsize->FrameWidth = (frsize->UpscaledWidth * SUPERRES_NUM + (frsize->superres_denom >> 1)) / frsize->superres_denom;
	frsize->MiCols = ((frsize->FrameWidth + 7) >> 3) << 1;
	frsize->MiRows = ((frsize->FrameHeight + 7) >> 3) << 1;
	return frsize;
	label_fail:
	return NULL;
}

av1_frame_render_size_t* av1_frame_render_size_read(av1_frame_render_size_t *restrict frsize, av1_bits_reader_t *restrict reader, const struct av1_obu_sequence_header_t *restrict header, uint8_t frame_size_override_flag)
{
	uint64_t v;
	uint8_t render_and_frame_size_different;
	if (frame_size_override_flag)
	{
		// n = frame_width_bits
		// (n) frame_width_minus_1
		if (!av1_bits_uint_read(reader, &v, header->frame_width_bits))
			goto label_fail;
		frsize->frame_width = (uint32_t) (v + 1);
		// n = frame_height_bits
		// (n) frame_height_minus_1
		if (!av1_bits_uint_read(reader, &v, header->frame_height_bits))
			goto label_fail;
		frsize->frame_height = (uint32_t) (v + 1);
	}
	else
	{
		frsize->frame_width = header->max_frame_width;
		frsize->frame_height = header->max_frame_height;
	}
	frsize->FrameWidth = frsize->frame_width;
	frsize->FrameHeight = frsize->frame_height;
	if (!av1_frame_render_size_read_superres_params(frsize, reader, header))
		goto label_fail;
	// (1) render_and_frame_size_different
	if (!av1_bits_flag_read(reader, &render_and_frame_size_different))
		goto label_fail;
	if (render_and_frame_size_different)
	{
		// (16) render_width_minus_1
		// (16) render_height_minus_1
		if (!av1_bits_uint_read(reader, &v, 32))
			goto label_fail;
		frsize->render_width = (uint32_t) ((v >> 16) + 1);
		frsize->render_height = (uint32_t) ((v & 0xffff) + 1);
	}
	else
	{
		frsize->render_width = frsize->UpscaledWidth;
		frsize->render_height = frsize->FrameHeight;
	}
	frsize->RenderWidth = frsize->render_width;
	frsize->RenderHeight = frsize->render_height;
	return frsize;
	label_fail:
	return NULL;
}

const av1_frame_render_size_t* av1_frame_render_size_write_superres_params(const av1_frame_render_size_t *restrict frsize, av1_bits_writer_t *restrict writer, const struct av1_obu_sequence_header_t *restrict header)
{
	uint8_t use_superres;
	use_superres = 0;
	if (header->enable_superres)
	{
		// (1) use_superres
		if (!av1_bits_flag_write(writer, use_superres = frsize->superres_denom != SUPERRES_NUM))
			goto label_fail;
	}
	if (use_superres)
	{
		// (SUPERRES_DENOM_BITS) coded_denom
		if (!av1_bits_uint_write(writer, frsize->superres_denom - SUPERRES_DENOM_MIN, SUPERRES_DENOM_BITS))
			goto label_fail;
	}
	return frsize;
	label_fail:
	return NULL;
}

const av1_frame_render_size_t* av1_frame_render_size_write(const av1_frame_render_size_t *restrict frsize, av1_bits_writer_t *restrict writer, const struct av1_obu_sequence_header_t *restrict header, uint8_t frame_size_override_flag)
{
	uint8_t render_and_frame_size_different;
	if (frame_size_override_flag)
	{
		// n = frame_width_bits
		// (n) frame_width_minus_1
		if (!av1_bits_uint_write(writer, frsize->frame_width - 1, header->frame_width_bits))
			goto label_fail;
		// n = frame_height_bits
		// (n) frame_height_minus_1
		if (!av1_bits_uint_write(writer, frsize->frame_height - 1, header->frame_height_bits))
			goto label_fail;
	}
	if (!av1_frame_render_size_write_superres_params(frsize, writer, header))
		goto label_fail;
	// (1) render_and_frame_size_different
	if (!av1_bits_flag_write(writer, render_and_frame_size_different =
		(frsize->render_width != frsize->UpscaledWidth || frsize->render_height != frsize->FrameHeight)))
		goto label_fail;
	if (render_and_frame_size_different)
	{
		// (16) render_width_minus_1
		// (16) render_height_minus_1
		if (!av1_bits_uint_write(writer,
			((uint64_t) (frsize->render_width - 1) << 16) |
			(uint64_t) (frsize->render_height - 1),
			32))
			goto label_fail;
	}
	return frsize;
	label_fail:
	return NULL;
}

uint64_t av1_frame_render_size_bits_superres_params(const av1_frame_render_size_t *restrict frsize, const struct av1_obu_sequence_header_t *restrict header)
{
	return header->enable_superres?
		(frsize->superres_denom != SUPERRES_NUM?
			(1 + SUPERRES_DENOM_BITS):
			1):
		0;
}

uint64_t av1_frame_render_size_bits(const av1_frame_render_size_t *restrict frsize, const struct av1_obu_sequence_header_t *restrict header, uint8_t frame_size_override_flag)
{
	uint64_t size = 0;
	if (frame_size_override_flag)
		size += header->frame_width_bits + header->frame_height_bits;
	size += av1_frame_render_size_bits_superres_params(frsize, header);
	size += 1;
	if (frsize->render_width != frsize->UpscaledWidth || frsize->render_height != frsize->FrameHeight)
		size += 32;
	return size;
}

void av1_frame_render_size_dump(const av1_frame_render_size_t *restrict frsize, mlog_s *restrict mlog)
{
	mlog_printf(mlog, "frame_width[1, 65536]:   %u\n", frsize->frame_width);
	mlog_printf(mlog, "frame_height[1, 65536]:  %u\n", frsize->frame_height);
	mlog_printf(mlog, "render_width[1, 65536]:  %u\n", frsize->render_width);
	mlog_printf(mlog, "render_height[1, 65536]: %u\n", frsize->render_height);
	mlog_printf(mlog, "superres_denom[8, 16]:   %u\n", frsize->superres_denom);
	mlog_printf(mlog, "UpscaledWidth:           %u\n", frsize->UpscaledWidth);
	mlog_printf(mlog, "FrameWidth:              %u\n", frsize->FrameWidth);
	mlog_printf(mlog, "FrameHeight:             %u\n", frsize->FrameHeight);
	mlog_printf(mlog, "RenderWidth:             %u\n", frsize->RenderWidth);
	mlog_printf(mlog, "RenderHeight:            %u\n", frsize->RenderHeight);
	mlog_printf(mlog, "MiCols:                  %u\n", frsize->MiCols);
	mlog_printf(mlog, "MiRows:                  %u\n", frsize->MiRows);
}
