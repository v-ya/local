#include "segmentation_params.h"

static const uint32_t segmentation_feature_bits[SEG_LVL_MAX] = {
	8, 6, 6, 6, 6, 3, 0, 0
};
static const uint32_t segmentation_feature_signed[SEG_LVL_MAX] = {
	1, 1, 1, 1, 1, 0, 0, 0
};
static const int32_t segmentation_feature_max[SEG_LVL_MAX] = {
	255, MAX_LOOP_FILTER, MAX_LOOP_FILTER, MAX_LOOP_FILTER, MAX_LOOP_FILTER, 7, 0, 0
};

av1_segmentation_params_t* av1_segmentation_params_init(av1_segmentation_params_t *restrict sparam)
{
	return (av1_segmentation_params_t *) memset(sparam, 0, sizeof(*sparam));
}

av1_segmentation_params_t* av1_segmentation_params_read(av1_segmentation_params_t *restrict sparam, av1_bits_reader_t *restrict reader, uint8_t primary_ref_frame)
{
	uint64_t u;
	int64_t s;
	uintptr_t i, j;
	uint32_t bits;
	int32_t value, limit;
	uint8_t feature_enabled;
	// (1) segmentation_enabled
	if (!av1_bits_flag_read(reader, &sparam->segmentation_enabled))
		goto label_fail;
	if (sparam->segmentation_enabled)
	{
		if (primary_ref_frame == PRIMARY_REF_NONE)
		{
			sparam->segmentation_update_map = 1;
			sparam->segmentation_temporal_update = 0;
			sparam->segmentation_update_data = 1;
		}
		else
		{
			// (1) segmentation_update_map
			if (!av1_bits_flag_read(reader, &sparam->segmentation_update_map))
				goto label_fail;
			if (sparam->segmentation_update_map)
			{
				// (1) segmentation_temporal_update
				if (!av1_bits_flag_read(reader, &sparam->segmentation_temporal_update))
					goto label_fail;
			}
			// (1) segmentation_update_data
			if (!av1_bits_flag_read(reader, &sparam->segmentation_update_data))
				goto label_fail;
		}
		if (sparam->segmentation_update_data)
		{
			for (i = 0; i < MAX_SEGMENTS; ++i)
			{
				for (j = 0; j < SEG_LVL_MAX; ++j)
				{
					value = 0;
					// (1) feature_enabled
					if (!av1_bits_flag_read(reader, &feature_enabled))
						goto label_fail;
					sparam->feature_enabled[i][j] = feature_enabled;
					if (feature_enabled)
					{
						bits = segmentation_feature_bits[j];
						limit = segmentation_feature_max[j];
						if (segmentation_feature_signed[j])
						{
							// (su(1+bits)) feature_value
							if (!av1_bits_int_read(reader, &s, bits + 1))
								goto label_fail;
							value = (int32_t) s;
							if (value < -limit)
								value = -limit;
							if (value > limit)
								value = limit;
						}
						else if (bits)
						{
							// (f(bits)) feature_value
							if (!av1_bits_uint_read(reader, &u, bits))
								goto label_fail;
							value = (int32_t) u;
							if (value > limit)
								value = limit;
						}
					}
					sparam->feature_data[i][j] = value;
				}
			}
		}
	}
	else
	{
		// clear feature_enabled && feature_data
		memset(sparam->feature_enabled, 0, sizeof(sparam->feature_enabled));
		memset(sparam->feature_data, 0, sizeof(sparam->feature_data));
	}
	sparam->SegIdPreSkip = 0;
	sparam->LastActiveSegId = 0;
	for (i = 0; i < MAX_SEGMENTS; ++i)
	{
		for (j = 0; j < SEG_LVL_MAX; ++j)
		{
			if (sparam->feature_enabled[i][j])
			{
				sparam->LastActiveSegId = i;
				if (j >= SEG_LVL_REF_FRAME)
					sparam->SegIdPreSkip = 1;
			}
		}
	}
	return sparam;
	label_fail:
	return NULL;
}

const av1_segmentation_params_t* av1_segmentation_params_write(const av1_segmentation_params_t *restrict sparam, av1_bits_writer_t *restrict writer, uint8_t primary_ref_frame)
{
	uintptr_t i, j;
	// (1) segmentation_enabled
	if (!av1_bits_flag_write(writer, sparam->segmentation_enabled))
		goto label_fail;
	if (sparam->segmentation_enabled)
	{
		if (primary_ref_frame != PRIMARY_REF_NONE)
		{
			// (1) segmentation_update_map
			if (!av1_bits_flag_write(writer, sparam->segmentation_update_map))
				goto label_fail;
			if (sparam->segmentation_update_map)
			{
				// (1) segmentation_temporal_update
				if (!av1_bits_flag_write(writer, sparam->segmentation_temporal_update))
					goto label_fail;
			}
			// (1) segmentation_update_data
			if (!av1_bits_flag_write(writer, sparam->segmentation_update_data))
				goto label_fail;
		}
		if (sparam->segmentation_update_data)
		{
			for (i = 0; i < MAX_SEGMENTS; ++i)
			{
				for (j = 0; j < SEG_LVL_MAX; ++j)
				{
					// (1) feature_enabled
					if (!av1_bits_flag_write(writer, sparam->feature_enabled[i][j]))
						goto label_fail;
					if (sparam->feature_enabled[i][j])
					{
						if (segmentation_feature_signed[j])
						{
							// (su(1+bits)) feature_value
							if (!av1_bits_int_write(writer, sparam->feature_data[i][j], segmentation_feature_bits[j] + 1))
								goto label_fail;
						}
						else if (segmentation_feature_bits[j])
						{
							// (f(bits)) feature_value
							if (!av1_bits_uint_write(writer, sparam->feature_data[i][j], segmentation_feature_bits[j]))
								goto label_fail;
						}
					}
				}
			}
		}
	}
	return sparam;
	label_fail:
	return NULL;
}

uint64_t av1_segmentation_params_bits(const av1_segmentation_params_t *restrict sparam, uint8_t primary_ref_frame)
{
	uintptr_t i, j;
	uint64_t size = 0;
	// (1) segmentation_enabled
	size += 1;
	if (sparam->segmentation_enabled)
	{
		if (primary_ref_frame != PRIMARY_REF_NONE)
		{
			// (1) segmentation_update_map
			size += 1;
			if (sparam->segmentation_update_map)
			{
				// (1) segmentation_temporal_update
				size += 1;
			}
			// (1) segmentation_update_data
			size += 1;
		}
		if (sparam->segmentation_update_data)
		{
			for (i = 0; i < MAX_SEGMENTS; ++i)
			{
				for (j = 0; j < SEG_LVL_MAX; ++j)
				{
					// (1) feature_enabled
					size += 1;
					if (sparam->feature_enabled[i][j])
					{
						if (segmentation_feature_signed[j])
						{
							// (su(1+bits)) feature_value
							size += segmentation_feature_bits[j] + 1;
						}
						else if (segmentation_feature_bits[j])
						{
							// (f(bits)) feature_value
							size += segmentation_feature_bits[j];
						}
					}
				}
			}
		}
	}
	return size;
}

void av1_segmentation_params_dump(const av1_segmentation_params_t *restrict sparam, mlog_s *restrict mlog)
{
	uintptr_t i, j;
	mlog_printf(mlog, "segmentation_enabled[0, 1]:         %u\n", sparam->segmentation_enabled);
	mlog_printf(mlog, "segmentation_update_map[0, 1]:      %u\n", sparam->segmentation_update_map);
	mlog_printf(mlog, "segmentation_temporal_update[0, 1]: %u\n", sparam->segmentation_temporal_update);
	mlog_printf(mlog, "segmentation_update_data[0, 1]:     %u\n", sparam->segmentation_update_data);
	mlog_printf(mlog, "feature_enabled[0, 1]:              {\n");
	for (i = 0; i < MAX_SEGMENTS; ++i)
	{
		mlog_printf(mlog, "\t");
		for (j = 0; j < SEG_LVL_MAX; ++j)
			mlog_printf(mlog, "%4u, ", sparam->feature_enabled[i][j]);
		mlog_printf(mlog, "\n");
	}
	mlog_printf(mlog, "}\n");
	mlog_printf(mlog, "feature_data:                       {\n");
	for (i = 0; i < MAX_SEGMENTS; ++i)
	{
		mlog_printf(mlog, "\t");
		for (j = 0; j < SEG_LVL_MAX; ++j)
			mlog_printf(mlog, "%4d, ", sparam->feature_data[i][j]);
		mlog_printf(mlog, "\n");
	}
	mlog_printf(mlog, "}\n");
	mlog_printf(mlog, "SegIdPreSkip:                       %u\n", sparam->SegIdPreSkip);
	mlog_printf(mlog, "LastActiveSegId:                    %u\n", sparam->LastActiveSegId);
}
