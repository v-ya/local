#include "parser.h"

struct jpeg_segment_sos_channel_t {
	uint8_t channel_id;
	uint8_t da_of_huffman;  // bits 4-7: dc_of_huffman, bits 0-3: ac_of_huffman
} __attribute__ ((packed));

struct jpeg_segment_sos_head_t {
	uint8_t channel_number;
	struct jpeg_segment_sos_channel_t c[];
} __attribute__ ((packed));

struct jpeg_segment_sos_tail_t {
	uint8_t start_of_selection;
	uint8_t end_of_selection;
	uint8_t hl_of_approximation_bit_position;
} __attribute__ ((packed));

jpeg_parser_s* jpeg_parser_segment__sos(jpeg_parser_s *restrict p, jpeg_parser_target_t *restrict t, uintptr_t size)
{
	const struct jpeg_segment_sos_head_t *restrict d;
	const struct jpeg_segment_sos_tail_t *restrict e;
	uintptr_t i, n;
	if (size >= sizeof(struct jpeg_segment_sos_head_t))
	{
		d = (const struct jpeg_segment_sos_head_t *) (t->data + t->pos);
		mlog_printf(p->m, "channel_number: %u\n", d->channel_number);
		t->pos += sizeof(struct jpeg_segment_sos_head_t);
		size -= sizeof(struct jpeg_segment_sos_head_t);
		n = (uintptr_t) d->channel_number;
		i = size / sizeof(struct jpeg_segment_sos_channel_t);
		if (n > i) n = i;
		for (i = 0; i < n; ++i)
		{
			mlog_printf(p->m, "c[%zu].channel_id:    %u\n", i, d->c[i].channel_id);
			mlog_printf(p->m, "c[%zu].da_of_huffman: (%u, %u)\n", i, parser_debits_be_4_4(d->c[i].da_of_huffman));
			t->pos += sizeof(struct jpeg_segment_sos_channel_t);
			size -= sizeof(struct jpeg_segment_sos_channel_t);
		}
		if (i == (uintptr_t) d->channel_number && size >= sizeof(struct jpeg_segment_sos_tail_t))
		{
			e = (const struct jpeg_segment_sos_tail_t *) (t->data + t->pos);
			mlog_printf(p->m, "start_of_selection:               %u\n", e->start_of_selection);
			mlog_printf(p->m, "end_of_selection:                 %u\n", e->end_of_selection);
			mlog_printf(p->m, "hl_of_approximation_bit_position: (%u, %u)\n", parser_debits_be_4_4(e->hl_of_approximation_bit_position));
			t->pos += sizeof(struct jpeg_segment_sos_tail_t);
			size -= sizeof(struct jpeg_segment_sos_tail_t);
			return p;
		}
	}
	return NULL;
}
