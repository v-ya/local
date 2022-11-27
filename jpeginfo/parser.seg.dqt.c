#include "parser.h"

static rbtree_t* jpeg_parser_segment__dqt_add_table(jpeg_parser_s *restrict p, uintptr_t id, uintptr_t precision, const void *restrict q64)
{
	quantization_s *restrict q;
	rbtree_t *restrict r;
	r = NULL;
	if ((q = quantization_alloc((!precision)?8:16, q64)))
	{
		r = jpeg_parser_add_table(&p->q, (uint64_t) id, q);
		refer_free(q);
	}
	return r;
}

jpeg_parser_s* jpeg_parser_segment__dqt(jpeg_parser_s *restrict p, jpeg_parser_target_t *restrict t, uintptr_t size)
{
	static const char *const sp[3] = {"8-bits", "16-bits", "unknow"};
	const uint8_t *restrict d;
	uintptr_t precision, n;
	if (size >= sizeof(uint8_t))
	{
		d = t->data + t->pos;
		if ((precision = (uintptr_t) parser_debits_4h(*d)) > 2)
			precision = 2;
		mlog_printf(p->m, "precision:       %u (%s)\n", parser_debits_4h(*d), sp[precision]);
		mlog_printf(p->m, "quantization_id: %u\n", parser_debits_4l(*d));
		t->pos += sizeof(uint8_t);
		size -= sizeof(uint8_t);
		if (precision < 2 && (n = (precision + 1) * 64) <= size)
		{
			mlog_printf(p->m, "quantization(%ux%u):\n", 8, 8);
			tmlog_add(p->td, 1);
			if (!precision) jpeg_parser_print_mat_u8(p->m, d + 1, 8, 8);
			else jpeg_parser_print_mat_u16(p->m, (const uint16_t *) (d + 1), 8, 8);
			tmlog_sub(p->td, 1);
			t->pos += n;
			size -= n;
			jpeg_parser_segment__dqt_add_table(p, (uintptr_t) parser_debits_4l(*d), precision, d + 1);
			return p;
		}
	}
	return NULL;
}
