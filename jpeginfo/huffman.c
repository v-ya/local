#include "huffman.h"

static huffman_decode_s* huffman_decode_find_empty_jumper(huffman_decode_s *restrict r, uintptr_t *restrict rji)
{
	const huffman_jumper_t *restrict jp;
	uintptr_t jumper_index, jn;
	jumper_index = ~(uintptr_t) 0;
	if ((jn = r->jumper_number))
	{
		jp = r->jumper;
		jumper_index = jn - 1;
		while (jumper_index < jn)
		{
			if (!~jp[jumper_index].bit_0_jumpto)
				break;
			if (!~jp[jumper_index].bit_1_jumpto)
				break;
			jumper_index = jp[jumper_index].jumper_index;
		}
		if (jumper_index >= jn)
			return NULL;
	}
	if (rji) *rji = jumper_index;
	return r;
}

static huffman_decode_s* huffman_decode_append_jumper(huffman_decode_s *restrict r, uintptr_t jumper_index)
{
	huffman_jumper_t *restrict jp;
	huffman_jumper_t jumper;
	jumper.jumper_index = jumper_index;
	jumper.bits_count = 1;
	jumper.bit_0_jumpto = ~(uintptr_t) 0;
	jumper.bit_1_jumpto = ~(uintptr_t) 0;
	jumper.bit_0_value = 0;
	jumper.bit_1_value = 0;
	if ((!~jumper_index || jumper_index < r->jumper_number) &&
		(jp = (huffman_jumper_t *) exbuffer_append(&r->jumper_buffer, &jumper, sizeof(jumper))))
	{
		r->jumper = jp;
		if (~jumper_index)
		{
			jp[r->jumper_number].bits_count += jp[jumper_index].bits_count;
			if (!~jp[jumper_index].bit_0_jumpto)
				jp[jumper_index].bit_0_jumpto = r->jumper_number;
			else if (!~jp[jumper_index].bit_1_jumpto)
				jp[jumper_index].bit_1_jumpto = r->jumper_number;
			else goto label_fail;
		}
		r->jumper_number += 1;
		return r;
		label_fail:
		r->jumper_buffer.used -= sizeof(jumper);
	}
	return NULL;
}

static void huffman_decode_free_func(huffman_decode_s *restrict r)
{
	exbuffer_uini(&r->jumper_buffer);
}

huffman_decode_s* huffman_decode_alloc(void)
{
	huffman_decode_s *restrict r;
	if ((r = (huffman_decode_s *) refer_alloz(sizeof(huffman_decode_s))))
	{
		refer_set_free(r, (refer_free_f) huffman_decode_free_func);
		r->jumper = (huffman_jumper_t *) r->jumper_buffer.data;
		return r;
	}
	return NULL;
}

huffman_decode_s* huffman_decode_add_bits(huffman_decode_s *restrict r)
{
	uintptr_t jumper_index;
	if (huffman_decode_find_empty_jumper(r, &jumper_index) &&
		huffman_decode_append_jumper(r, jumper_index))
	{
		r->bits_count += 1;
		return r;
	}
	return NULL;
}

huffman_decode_s* huffman_decode_add_value(huffman_decode_s *restrict r, uintptr_t value)
{
	huffman_jumper_t *restrict jp;
	uintptr_t jumper_index;
	if (r->bits_count && huffman_decode_find_empty_jumper(r, &jumper_index) && ~jumper_index)
	{
		jp = r->jumper;
		while (jp[jumper_index].bits_count < r->bits_count)
		{
			if (!huffman_decode_append_jumper(r, jumper_index))
				goto label_fail;
			jumper_index = r->jumper_number - 1;
			jp = r->jumper;
		}
		if (!~jp[jumper_index].bit_0_jumpto)
		{
			jp[jumper_index].bit_0_value = value;
			jp[jumper_index].bit_0_jumpto -= 1;
		}
		else if (!~jp[jumper_index].bit_1_jumpto)
		{
			jp[jumper_index].bit_1_value = value;
			jp[jumper_index].bit_1_jumpto -= 1;
		}
		else goto label_fail;
		return r;
	}
	label_fail:
	return NULL;
}
