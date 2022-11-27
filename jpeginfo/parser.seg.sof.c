#include "parser.h"

struct jpeg_segment_sof_channel_t {
	uint8_t channel_id;
	uint8_t hv_per_mcu;             // bits 4-7: h_per_mcu, bits 0-3: v_per_mcu
	uint8_t quantization_table_id;  // link `jpeg_segment_type__dqt`
} __attribute__ ((packed));

struct jpeg_segment_sof_t {
	uint8_t sample_bits;
	uint16_t height;
	uint16_t width;
	uint8_t channel_number;
	struct jpeg_segment_sof_channel_t c[];
} __attribute__ ((packed));

static jpeg_parser_s* jpeg_parser_segment__sof_add_info(jpeg_parser_s *restrict p, const struct jpeg_segment_sof_t *restrict sof)
{
	jpeg_parser_s *restrict r;
	frame_info_s *restrict info;
	uint32_t i, n;
	r = NULL;
	if ((info = frame_info_alloc(bits_n2be_16(sof->width), bits_n2be_16(sof->height), sof->sample_bits, n = sof->channel_number)))
	{
		for (i = 0; i < n; ++i)
		{
			if (!frame_info_set_channel(info, i, sof->c[i].channel_id, parser_debits_be_4_4(sof->c[i].hv_per_mcu), sof->c[i].quantization_table_id))
				goto label_fail;
		}
		if (p->info) refer_free(p->info);
		p->info = (const frame_info_s *) refer_save(info);
		r = p;
		label_fail:
		refer_free(info);
	}
	return r;
}

jpeg_parser_s* jpeg_parser_segment__sof(jpeg_parser_s *restrict p, jpeg_parser_target_t *restrict t, uintptr_t size)
{
	const struct jpeg_segment_sof_t *restrict d;
	uintptr_t i, n;
	if (size >= sizeof(struct jpeg_segment_sof_t))
	{
		d = (const struct jpeg_segment_sof_t *) (t->data + t->pos);
		mlog_printf(p->m, "sample_bits:    %u\n", d->sample_bits);
		mlog_printf(p->m, "frame_size:     %ux%u\n", bits_n2be_16(d->width), bits_n2be_16(d->height));
		mlog_printf(p->m, "channel_number: %u\n", d->channel_number);
		t->pos += sizeof(struct jpeg_segment_sof_t);
		size -= sizeof(struct jpeg_segment_sof_t);
		n = (uintptr_t) d->channel_number;
		i = size / sizeof(struct jpeg_segment_sof_channel_t);
		if (n > i) n = i;
		for (i = 0; i < n; ++i)
		{
			mlog_printf(p->m, "c[%zu].channel_id:            %u\n", i, d->c[i].channel_id);
			mlog_printf(p->m, "c[%zu].hv_per_mcu:            %ux%u\n", i, parser_debits_be_4_4(d->c[i].hv_per_mcu));
			mlog_printf(p->m, "c[%zu].quantization_table_id: %u\n", i, d->c[i].quantization_table_id);
			t->pos += sizeof(struct jpeg_segment_sof_channel_t);
			size -= sizeof(struct jpeg_segment_sof_channel_t);
		}
		if (i == (uintptr_t) d->channel_number)
		{
			jpeg_parser_segment__sof_add_info(p, d);
			return p;
		}
	}
	return NULL;
}
