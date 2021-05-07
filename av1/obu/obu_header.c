#include "obu_header.h"

av1_obu_header_t* av1_obu_header_init(av1_obu_header_t *restrict header)
{
	return (av1_obu_header_t *) memset(header, 0, sizeof(*header));
}

av1_obu_header_t* av1_obu_header_read(av1_obu_header_t *restrict header, av1_bits_reader_t *restrict reader)
{
	uint64_t v;
	// (1) obu_forbidden_bit == 0
	// (4) obu_type
	// (1) obu_extension_flag
	// (1) obu_has_size_field
	// (1) obu_reserved_1bit == 0
	if (!av1_bits_uint_read(reader, &v, 8))
		goto label_fail;
	if (v & 0x80)
		goto label_fail;
	header->type = (av1_obu_type_t) ((v >> 3) & 0x0f);
	header->extension_flag = !!(v & 0x04);
	header->has_size_field = !!(v & 0x02);
	if (header->extension_flag)
	{
		// (3) temporal_id
		// (2) spatial_id
		// (3) extension_header_reserved_3bits
		if (!av1_bits_uint_read(reader, &v, 8))
			goto label_fail;
		header->temporal_id = (uint8_t) ((v >> 5) & 0x07);
		header->spatial_id = (uint8_t) ((v >> 3) & 0x03);
	}
	if (header->has_size_field)
	{
		// (leb128) obu_size
		if (!av1_bits_leb128_read(reader, &header->size))
			goto label_fail;
	}
	else header->size = (reader->saver.rbits + 7) >> 3;
	return header;
	label_fail:
	return NULL;
}

const av1_obu_header_t* av1_obu_header_write(const av1_obu_header_t *restrict header, av1_bits_writer_t *restrict writer)
{
	// (1) obu_forbidden_bit == 0
	// (4) obu_type
	// (1) obu_extension_flag
	// (1) obu_has_size_field
	// (1) obu_reserved_1bit == 0
	if (!av1_bits_uint_write(writer,
		((uint64_t) header->type << 3) |
		((uint64_t) header->extension_flag << 2) |
		((uint64_t) header->has_size_field << 1)
		, 8))
		goto label_fail;
	if (header->extension_flag)
	{
		// (3) temporal_id
		// (2) spatial_id
		// (3) extension_header_reserved_3bits
		if (!av1_bits_uint_write(writer,
			((uint64_t) header->temporal_id << 5) |
			((uint64_t) header->spatial_id << 3)
			, 8))
			goto label_fail;
	}
	if (header->has_size_field)
	{
		// (leb128) obu_size
		if (!av1_bits_leb128_write(writer, header->size))
			goto label_fail;
	}
	return header;
	label_fail:
	return NULL;
}

uint64_t av1_obu_header_bits(const av1_obu_header_t *restrict header)
{
	return 8 + (header->extension_flag?8:0) + (header->has_size_field?av1_bits_leb128_bits(header->size):0);
}

void av1_obu_header_dump(const av1_obu_header_t *restrict header, mlog_s *restrict mlog)
{
	static const char *s_type[16] = {
		[av1_obu_type_reserved] = "reserved",
		[av1_obu_type_sequence_header] = "sequence_header",
		[av1_obu_type_temporal_delimiter] = "temporal_delimiter",
		[av1_obu_type_frame_header] = "frame_header",
		[av1_obu_type_tile_group] = "tile_group",
		[av1_obu_type_metadata] = "metadata",
		[av1_obu_type_frame] = "frame",
		[av1_obu_type_redundant_frame_header] = "redundant_frame_header",
		[av1_obu_type_tile_list] = "tile_list",
		[av1_obu_type_padding] = "padding"
	};
	mlog_printf(mlog, "obu_type[0, 15]:          %u (%s)\n", header->type, s_type[header->type]?s_type[header->type]:s_type[0]);
	mlog_printf(mlog, "obu_extension_flag[0, 1]: %u\n", header->extension_flag);
	mlog_printf(mlog, "obu_has_size_field[0, 1]: %u\n", header->has_size_field);
	if (header->extension_flag)
	{
		mlog_printf(mlog, "temporal_id[0, 7]:        %u\n", header->temporal_id);
		mlog_printf(mlog, "spatial_id[0, 3]:         %u\n", header->spatial_id);
	}
	if (header->has_size_field)
	{
		mlog_printf(mlog, "obu_size[0, 2^56-1]:      %lu\n", header->size);
	}
}
