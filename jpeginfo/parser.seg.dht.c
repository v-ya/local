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

jpeg_parser_s* jpeg_parser_segment__dht(jpeg_parser_s *restrict p, jpeg_parser_target_t *restrict t, uintptr_t size)
{
	static const char *const sc[3] = {"DC", "AC", "unknow"};
	const uint8_t *restrict d, *restrict L;
	uintptr_t table_class, i, n;
	if (size >= sizeof(uint8_t))
	{
		d = t->data + t->pos;
		if ((table_class = (uintptr_t) parser_debits_4h(*d)) > 2)
			table_class = 2;
		mlog_printf(p->m, "table_class:    %u (%s)\n", parser_debits_4h(*d), sc[table_class]);
		mlog_printf(p->m, "huffman_dst_id: %u\n", parser_debits_4l(*d));
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
				tmlog_sub(p->td, 2);
				return p;
			}
		}
	}
	return NULL;
}
