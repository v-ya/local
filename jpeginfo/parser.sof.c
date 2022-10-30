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
	uint8_t channels;
	struct jpeg_segment_sof_channel_t c[];
} __attribute__ ((packed));

jpeg_parser_s* jpeg_parser_segment__sof(jpeg_parser_s *restrict p, jpeg_parser_target_t *restrict t, uintptr_t size)
{
	const struct jpeg_segment_sof_t *restrict d;
	uintptr_t i, n;
	if (size >= sizeof(struct jpeg_segment_sof_t))
	{
		d = (const struct jpeg_segment_sof_t *) (t->data + t->pos);
		t->pos += size;
		mlog_printf(p->m, "sample bits: %u\n", d->sample_bits);
		mlog_printf(p->m, "frame size:  %ux%u\n", bits_n2be_16(d->width), bits_n2be_16(d->height));
		mlog_printf(p->m, "channels:    %u\n", d->channels);
		n = (size - sizeof(struct jpeg_segment_sof_t)) / sizeof(struct jpeg_segment_sof_channel_t);
		if (n > (uintptr_t) d->channels) n = (uintptr_t) d->channels;
		for (i = 0; i < n; ++i)
		{
			mlog_printf(p->m, "c[%zu].channel_id:            %u\n", i, d->c[i].channel_id);
			mlog_printf(p->m, "c[%zu].hv_per_mcu:            %ux%u\n", i, (d->c[i].hv_per_mcu >> 4) & 0xf, d->c[i].hv_per_mcu & 0xf);
			mlog_printf(p->m, "c[%zu].quantization_table_id: %u\n", i, d->c[i].quantization_table_id);
		}
		return p;
	}
	return NULL;
}
