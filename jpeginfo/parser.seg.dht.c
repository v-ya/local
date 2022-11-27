#include "parser.h"

static const char* jpeg_parser_segment__dht_print_mapping__inc_value(char *restrict binary, uintptr_t length)
{
	while (length)
	{
		length -= 1;
		switch (binary[length])
		{
			case '1':
				binary[length] = '0';
				continue;
			case '0':
				binary[length] = '1';
				// fall through
			default: return binary;
		}
	}
	return NULL;
}

static const char* jpeg_parser_segment__dht_print_mapping__inc_length(char *restrict binary, uintptr_t length)
{
	binary[length] = '0';
	binary[length + 1] = 0;
	return binary;
}

static void jpeg_parser_segment__dht_print_mapping(mlog_s *restrict m, const uint8_t *restrict L, const uint8_t *restrict v, uintptr_t max_col)
{
	uintptr_t i, j, c, n;
	char binary[32];
	if (!max_col) max_col = 1;
	i = 0;
	while (i < 16)
	{
		n = (uintptr_t) L[i];
		jpeg_parser_segment__dht_print_mapping__inc_length(binary, i++);
		for (j = c = 0; j < n; ++j)
		{
			if (++c >= max_col || j + 1 >= n)
				c = 0;
			mlog_printf(m, "(%-16s => %02x)%s", binary, v[j], c?", ":"\n");
			jpeg_parser_segment__dht_print_mapping__inc_value(binary, i);
		}
		v += n;
	}
}

// static void jpeg_parser_segment__dht_print_huffman(mlog_s *restrict m, const uint8_t *restrict L, const uint8_t *restrict v)
// {
// 	huffman_decode_s *restrict hd;
// 	const huffman_jumper_t *restrict jp;
// 	uintptr_t i, in, j, n;
// 	if ((hd = huffman_decode_alloc()))
// 	{
// 		i = 0;
// 		in = 16;
// 		while (in && !L[in - 1]) --in;
// 		while (i < in)
// 		{
// 			n = (uintptr_t) L[i++];
// 			if (!huffman_decode_add_bits(hd))
// 				goto label_fail;
// 			for (j = 0; j < n; ++j)
// 			{
// 				if (!huffman_decode_add_value(hd, (uintptr_t) v[j]))
// 					goto label_fail;
// 			}
// 			v += n;
// 		}
// 		mlog_printf(m, "jumper:\n");
// 		jp = hd->jumper;
// 		n = hd->jumper_number;
// 		for (i = 0; i < n; ++i)
// 		{
// 			mlog_printf(m, "jumper[%2zu]: ", i);
// 			if (jp[i].jumper_index < n)
// 				mlog_printf(m, "ji(%2zu), ", jp[i].jumper_index);
// 			else mlog_printf(m, "ji(--), ");
// 			mlog_printf(m, "bc(%2zu), ", jp[i].bits_count);
// 			if (jp[i].bit_0_jumpto < n)
// 				mlog_printf(m, "j0(%2zu), ", jp[i].bit_0_jumpto);
// 			else mlog_printf(m, "j0(--), ");
// 			if (jp[i].bit_1_jumpto < n)
// 				mlog_printf(m, "j1(%2zu), ", jp[i].bit_1_jumpto);
// 			else mlog_printf(m, "j1(--), ");
// 			if (jp[i].bit_0_jumpto == ~(uintptr_t) 1)
// 				mlog_printf(m, "v0(%02zx), ", jp[i].bit_0_value);
// 			else mlog_printf(m, "v0(--), ");
// 			if (jp[i].bit_1_jumpto == ~(uintptr_t) 1)
// 				mlog_printf(m, "v1(%02zx)\n", jp[i].bit_1_value);
// 			else mlog_printf(m, "v1(--)\n");
// 		}
// 		refer_free(hd);
// 	}
// 	return ;
// 	label_fail:
// 	mlog_printf(m, "create (huffman_decode_s) fail\n");
// 	return ;
// }

static rbtree_t* jpeg_parser_segment__dht_add_table(jpeg_parser_s *restrict p, uintptr_t table_class, uintptr_t id, const uint8_t *restrict L, const uint8_t *restrict v)
{
	huffman_decode_s *restrict hd;
	rbtree_t *restrict r, *restrict *restrict prb;
	uintptr_t i, in, j, n;
	r = NULL;
	switch (table_class)
	{
		case 0: prb = &p->h_dc; break;
		case 1: prb = &p->h_ac; break;
		default: prb = NULL; break;
	}
	if (prb && (hd = huffman_decode_alloc()))
	{
		i = 0;
		in = 16;
		while (in && !L[in - 1]) --in;
		while (i < in)
		{
			n = (uintptr_t) L[i++];
			if (!huffman_decode_add_bits(hd))
				goto label_fail;
			for (j = 0; j < n; ++j)
			{
				if (!huffman_decode_add_value(hd, (uintptr_t) v[j]))
					goto label_fail;
			}
			v += n;
		}
		r = jpeg_parser_add_table(prb, id, hd);
		label_fail:
		refer_free(hd);
	}
	return r;
}

jpeg_parser_s* jpeg_parser_segment__dht(jpeg_parser_s *restrict p, jpeg_parser_target_t *restrict t, uintptr_t size)
{
	static const char *const sc[3] = {"DC", "AC", "unknow"};
	const uint8_t *restrict d, *restrict L;
	uintptr_t table_class, id, i, n;
	if (size >= sizeof(uint8_t))
	{
		d = t->data + t->pos;
		if ((table_class = (uintptr_t) parser_debits_4h(*d)) > 2)
			table_class = 2;
		mlog_printf(p->m, "table_class:    %u (%s)\n", parser_debits_4h(*d), sc[table_class]);
		mlog_printf(p->m, "huffman_dst_id: %zu\n", id = (uintptr_t) parser_debits_4l(*d));
		t->pos += sizeof(uint8_t);
		size -= sizeof(uint8_t);
		if (size >= 16)
		{
			L = d + 1;
			for (i = n = 0; i < 16; ++i)
				n += L[i];
			t->pos += 16;
			size -= 16;
			mlog_printf(p->m, "count_of_codes[length = (1 ... 16)]: all codes = %zu\n", n);
			tmlog_add(p->td, 1);
			jpeg_parser_print_mat_u8(p->m, L, 1, 16);
			tmlog_sub(p->td, 1);
			if (size >= n)
			{
				t->pos += n;
				size -= n;
				mlog_printf(p->m, "huffman:\n");
				tmlog_add(p->td, 1);
				d = L + 16;
				for (i = 0; i < 16; ++i)
				{
					n = (uintptr_t) L[i];
					mlog_printf(p->m, "(bits: %2zu, count: %3zu): ", i + 1, n);
					if (n) jpeg_parser_print_mat_x8(p->m, d, 1, n);
					else mlog_printf(p->m, "\n");
					d += n;
				}
				mlog_printf(p->m, "mapping-table:\n");
				tmlog_add(p->td, 1);
				jpeg_parser_segment__dht_print_mapping(p->m, L, L + 16, 4);
				// jpeg_parser_segment__dht_print_huffman(p->m, L, L + 16);
				tmlog_sub(p->td, 2);
				jpeg_parser_segment__dht_add_table(p, table_class, id, L, L + 16);
				return p;
			}
		}
	}
	return NULL;
}
