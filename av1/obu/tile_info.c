#include "tile_info.h"
#include "obu_sequence_header.h"
#include "frame_render_size.h"

av1_tile_info_t* av1_tile_info_init(av1_tile_info_t *restrict tinfo)
{
	return (av1_tile_info_t *) memset(tinfo, 0, sizeof(*tinfo));
}

av1_tile_info_t* av1_tile_info_read(av1_tile_info_t *restrict tinfo, av1_bits_reader_t *restrict reader, const struct av1_frame_render_size_t *restrict frsize, uint8_t use_128x128_superblock)
{
	uint64_t v;
	uint32_t cols, rows, shift, size;
	uint32_t max_tile_width, max_tile_area;
	uint32_t min_log2_tile_cols, max_log2_tile_cols, max_log2_tile_rows, min_log2_tiles;
	uint32_t i, p, L, n;
	uint8_t f;
	shift = use_128x128_superblock?5:4;
	size = shift + 2;
	i = (1u << shift) - 1;
	cols = (frsize->MiCols + i) >> shift;
	rows = (frsize->MiRows + i) >> shift;
	max_tile_width = MAX_TILE_WIDTH >> size;
	max_tile_area = MAX_TILE_AREA >> (size << 1);
	min_log2_tile_cols = av1_tile_log2(max_tile_width, cols);
	max_log2_tile_cols = av1_tile_log2(1, (cols < MAX_TILE_COLS)?cols:MAX_TILE_COLS);
	max_log2_tile_rows = av1_tile_log2(1, (rows < MAX_TILE_ROWS)?rows:MAX_TILE_ROWS);
	if ((min_log2_tiles = av1_tile_log2(max_tile_area, rows * cols)) < min_log2_tile_cols)
		min_log2_tiles = min_log2_tile_cols;
	// (1) uniform_tile_spacing_flag
	if (!av1_bits_flag_read(reader, &tinfo->uniform_tile_spacing_flag))
		goto label_fail;
	if (tinfo->uniform_tile_spacing_flag)
	{
		i = 0;
		p = min_log2_tile_cols;
		while (p < max_log2_tile_cols)
		{
			// (1) increment_tile_cols_log2
			if (!av1_bits_flag_read(reader, &f))
				goto label_fail;
			++i;
			if (f) ++p;
			else break;
		}
		tinfo->increment_tile_cols_log2_bits = i;
		tinfo->TileColsLog2 = p;
		L = (cols + ((1u << p) - 1)) >> p;
		for (i = p = 0; p < cols; p += L)
			tinfo->MiColStarts[i++] = p << shift;
		tinfo->MiColStarts[i] = frsize->MiCols;
		tinfo->TileCols = i;
		i = p = 0;
		if (min_log2_tiles > tinfo->TileColsLog2)
			p = min_log2_tiles - tinfo->TileColsLog2;
		while (p < max_log2_tile_rows)
		{
			// (1) increment_tile_rows_log2
			if (!av1_bits_flag_read(reader, &f))
				goto label_fail;
			++i;
			if (f) ++p;
			else break;
		}
		tinfo->increment_tile_rows_log2_bits = i;
		tinfo->TileRowsLog2 = p;
		L = (rows + ((1u << p) - 1)) >> p;
		for (i = p = 0; p < rows; p += L)
			tinfo->MiRowStarts[i++] = p << shift;
		tinfo->MiRowStarts[i] = frsize->MiRows;
		tinfo->TileRows = i;
	}
	else
	{
		for (i = p = L = 0; p < cols; ++i)
		{
			tinfo->MiColStarts[i] = p << shift;
			if ((n = cols - p) > max_tile_width)
				n = max_tile_width;
			// (ns(n)) width_in_sbs_minus_1
			if (!av1_bits_ns_read(reader, &v, n))
				goto label_fail;
			if ((tinfo->width_in_sbs[i] = (uint32_t) (v + 1)) > L)
				L = tinfo->width_in_sbs[i];
			p += tinfo->width_in_sbs[i];
		}
		tinfo->MiColStarts[i] = frsize->MiCols;
		tinfo->TileCols = i;
		tinfo->TileColsLog2 = av1_tile_log2(1, i);
		if (min_log2_tiles > 0)
			L = ((rows * cols) >> (min_log2_tiles + 1)) / L;
		else L = (rows * cols) / L;
		if (L < 1) L = 1;
		for (i = p = 0; p < rows; ++i)
		{
			tinfo->MiRowStarts[i] = p << shift;
			if ((n = rows - p) > L)
				n = L;
			// (ns(n)) height_in_sbs_minus_1
			if (!av1_bits_ns_read(reader, &v, n))
				goto label_fail;
			tinfo->height_in_sbs[i] = (uint32_t) (v + 1);
			p += tinfo->height_in_sbs[i];
		}
		tinfo->MiRowStarts[i] = frsize->MiRows;
		tinfo->TileRows = i;
		tinfo->TileRowsLog2 = av1_tile_log2(1, i);
	}
	if (tinfo->TileColsLog2 > 0 || tinfo->TileRowsLog2 > 0)
	{
		// n = TileRowsLog2 + TileColsLog2
		// (n) context_update_tile_id
		if (!av1_bits_uint_read(reader, &v, tinfo->TileColsLog2 + tinfo->TileRowsLog2))
			goto label_fail;
		tinfo->context_update_tile_id = (uint32_t) v;
		// (2) tile_size_bytes_minus_1
		if (!av1_bits_uint_read(reader, &v, 2))
			goto label_fail;
		tinfo->tile_size_bytes = (uint8_t) (v + 1);
	}
	else tinfo->context_update_tile_id = 0;
	return tinfo;
	label_fail:
	return NULL;
}

const av1_tile_info_t* av1_tile_info_write(const av1_tile_info_t *restrict tinfo, av1_bits_writer_t *restrict writer, const struct av1_frame_render_size_t *restrict frsize, uint8_t use_128x128_superblock)
{
	uint32_t cols, rows, shift, size;
	uint32_t max_tile_width, max_tile_area;
	uint32_t min_log2_tile_cols, min_log2_tiles;
	uint32_t i, p, L, n;
	shift = use_128x128_superblock?5:4;
	size = shift + 2;
	i = (1u << shift) - 1;
	cols = (frsize->MiCols + i) >> shift;
	rows = (frsize->MiRows + i) >> shift;
	max_tile_width = MAX_TILE_WIDTH >> size;
	max_tile_area = MAX_TILE_AREA >> (size << 1);
	min_log2_tile_cols = av1_tile_log2(max_tile_width, cols);
	if ((min_log2_tiles = av1_tile_log2(max_tile_area, rows * cols)) < min_log2_tile_cols)
		min_log2_tiles = min_log2_tile_cols;
	// (1) uniform_tile_spacing_flag
	if (!av1_bits_flag_write(writer, tinfo->uniform_tile_spacing_flag))
		goto label_fail;
	if (tinfo->uniform_tile_spacing_flag)
	{
		// (increment_tile_cols_log2_bits) increment_tile_cols_log2
		if (tinfo->increment_tile_cols_log2_bits)
		{
			if (!av1_bits_uint_write(writer, ~(uint64_t) 1, tinfo->increment_tile_cols_log2_bits))
				goto label_fail;
		}
		// (increment_tile_rows_log2_bits) increment_tile_rows_log2
		if (tinfo->increment_tile_rows_log2_bits)
		{
			if (!av1_bits_uint_write(writer, ~(uint64_t) 1, tinfo->increment_tile_rows_log2_bits))
				goto label_fail;
		}
	}
	else
	{
		for (i = p = L = 0; p < cols; ++i)
		{
			if ((n = cols - p) > max_tile_width)
				n = max_tile_width;
			// (ns(n)) width_in_sbs_minus_1
			if (!av1_bits_ns_write(writer, tinfo->width_in_sbs[i], n))
				goto label_fail;
			if (tinfo->width_in_sbs[i] > L)
				L = tinfo->width_in_sbs[i];
			p += tinfo->width_in_sbs[i];
		}
		if (min_log2_tiles > 0)
			L = ((rows * cols) >> (min_log2_tiles + 1)) / L;
		else L = (rows * cols) / L;
		if (L < 1) L = 1;
		for (i = p = 0; p < rows; ++i)
		{
			if ((n = rows - p) > L)
				n = L;
			// (ns(n)) height_in_sbs_minus_1
			if (!av1_bits_ns_write(writer, tinfo->height_in_sbs[i], n))
				goto label_fail;
			p += tinfo->height_in_sbs[i];
		}
	}
	if (tinfo->TileColsLog2 > 0 || tinfo->TileRowsLog2 > 0)
	{
		// n = TileRowsLog2 + TileColsLog2
		// (n) context_update_tile_id
		if (!av1_bits_uint_write(writer, tinfo->context_update_tile_id, tinfo->TileColsLog2 + tinfo->TileRowsLog2))
			goto label_fail;
		// (2) tile_size_bytes_minus_1
		if (!av1_bits_uint_write(writer, tinfo->tile_size_bytes - 1, 2))
			goto label_fail;
	}
	return tinfo;
	label_fail:
	return NULL;
}

uint64_t av1_tile_info_bits(const av1_tile_info_t *restrict tinfo, const struct av1_frame_render_size_t *restrict frsize, uint8_t use_128x128_superblock)
{
	uint64_t rsize;
	uint32_t cols, rows, shift, size;
	uint32_t max_tile_width, max_tile_area;
	uint32_t min_log2_tile_cols, min_log2_tiles;
	uint32_t i, p, L, n;
	rsize = 0;
	shift = use_128x128_superblock?5:4;
	size = shift + 2;
	i = (1u << shift) - 1;
	cols = (frsize->MiCols + i) >> shift;
	rows = (frsize->MiRows + i) >> shift;
	max_tile_width = MAX_TILE_WIDTH >> size;
	max_tile_area = MAX_TILE_AREA >> (size << 1);
	min_log2_tile_cols = av1_tile_log2(max_tile_width, cols);
	if ((min_log2_tiles = av1_tile_log2(max_tile_area, rows * cols)) < min_log2_tile_cols)
		min_log2_tiles = min_log2_tile_cols;
	rsize += 1;
	if (tinfo->uniform_tile_spacing_flag)
	{
		// (increment_tile_cols_log2_bits) increment_tile_cols_log2
		// (increment_tile_rows_log2_bits) increment_tile_rows_log2
		rsize += tinfo->increment_tile_cols_log2_bits + tinfo->increment_tile_rows_log2_bits;
	}
	else
	{
		for (i = p = L = 0; p < cols; ++i)
		{
			if ((n = cols - p) > max_tile_width)
				n = max_tile_width;
			// (ns(n)) width_in_sbs_minus_1
			rsize += av1_bits_ns_bits(tinfo->width_in_sbs[i], n);
			if (tinfo->width_in_sbs[i] > L)
				L = tinfo->width_in_sbs[i];
			p += tinfo->width_in_sbs[i];
		}
		if (min_log2_tiles > 0)
			L = ((rows * cols) >> (min_log2_tiles + 1)) / L;
		else L = (rows * cols) / L;
		if (L < 1) L = 1;
		for (i = p = 0; p < rows; ++i)
		{
			if ((n = rows - p) > L)
				n = L;
			// (ns(n)) height_in_sbs_minus_1
			rsize += av1_bits_ns_bits(tinfo->height_in_sbs[i], n);
			p += tinfo->height_in_sbs[i];
		}
	}
	if (tinfo->TileColsLog2 > 0 || tinfo->TileRowsLog2 > 0)
	{
		// n = TileRowsLog2 + TileColsLog2
		// (n) context_update_tile_id
		// (2) tile_size_bytes_minus_1
		rsize += tinfo->TileRowsLog2 + tinfo->TileColsLog2 + 2;
	}
	return rsize;
}

void av1_tile_info_dump(const av1_tile_info_t *restrict tinfo, mlog_s *restrict mlog)
{
	uint32_t i;
	mlog_printf(mlog, "uniform_tile_spacing_flag[0, 1]:     %u\n", tinfo->uniform_tile_spacing_flag);
	mlog_printf(mlog, "tile_size_bytes[1, 4]:               %u\n", tinfo->tile_size_bytes);
	mlog_printf(mlog, "increment_tile_cols_log2_bits[0, ?]: %u\n", tinfo->increment_tile_cols_log2_bits);
	mlog_printf(mlog, "increment_tile_rows_log2_bits[0, ?]: %u\n", tinfo->increment_tile_rows_log2_bits);
	mlog_printf(mlog, "context_update_tile_id:              %08x\n", tinfo->context_update_tile_id);
	mlog_printf(mlog, "width_in_sbs[1, ?]:                  [");
	for (i = 0; i < tinfo->TileCols; ++i)
		mlog_printf(mlog, i?", %u":"%u", tinfo->width_in_sbs[i]);
	mlog_printf(mlog, "]\n");
	mlog_printf(mlog, "height_in_sbs[1, ?]:                 [");
	for (i = 0; i < tinfo->TileRows; ++i)
		mlog_printf(mlog, i?", %u":"%u", tinfo->height_in_sbs[i]);
	mlog_printf(mlog, "]\n");
	mlog_printf(mlog, "TileColsLog2:                        %u\n", tinfo->TileColsLog2);
	mlog_printf(mlog, "TileRowsLog2:                        %u\n", tinfo->TileRowsLog2);
	mlog_printf(mlog, "TileCols:                            %u\n", tinfo->TileCols);
	mlog_printf(mlog, "TileRows:                            %u\n", tinfo->TileRows);
	mlog_printf(mlog, "MiColStarts:                         [");
	for (i = 0; i <= tinfo->TileCols; ++i)
		mlog_printf(mlog, i?", %u":"%u", tinfo->MiColStarts[i]);
	mlog_printf(mlog, "]\n");
	mlog_printf(mlog, "MiRowStarts:                         [");
	for (i = 0; i <= tinfo->TileRows; ++i)
		mlog_printf(mlog, i?", %u":"%u", tinfo->MiRowStarts[i]);
	mlog_printf(mlog, "]\n");
}
