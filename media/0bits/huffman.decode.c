#include "huffman.h"

static media_huffman_index_t media_huffman_decode_find_empty_jumper(struct media_huffman_decode_s *restrict r, struct media_huffman_decode_t *restrict d)
{
	const struct media_huffman_jumper_t *restrict jp;
	media_huffman_index_t jumper_index, jn;
	jumper_index = d->edge_jump2;
	if (d->jumper_number)
	{
		jp = r->jumper;
		jn = r->jumper_number;
		while (jumper_index < jn)
		{
			if (!~jp[jumper_index].bit_jump2[0])
				break;
			if (!~jp[jumper_index].bit_jump2[1])
				break;
			jumper_index = jp[jumper_index].last_jump4;
		}
		if (jumper_index >= jn)
			return media_huffman_decode_empty;
	}
	return jumper_index;
}

static media_huffman_index_t media_huffman_decode_append_jumper(struct media_huffman_decode_s *restrict r, struct media_huffman_decode_t *restrict d, media_huffman_index_t last_jump4)
{
	struct media_huffman_jumper_t *restrict jp;
	struct media_huffman_jumper_t jumper;
	media_huffman_index_t target;
	jumper.last_jump4 = last_jump4;
	jumper.bits_count = 1;
	jumper.bit_jump2[0] = media_huffman_decode_empty;
	jumper.bit_jump2[1] = media_huffman_decode_empty;
	jumper.bit_value[0] = NULL;
	jumper.bit_value[1] = NULL;
	target = r->jumper_number;
	if ((!~last_jump4 || last_jump4 < target) && target < media_huffman_decode_tail &&
		(jp = (struct media_huffman_jumper_t *) exbuffer_append(&r->jumper_buffer, &jumper, sizeof(jumper))))
	{
		r->jumper = jp;
		if (~last_jump4)
		{
			jp[target].bits_count += jp[last_jump4].bits_count;
			if (!~jp[last_jump4].bit_jump2[0])
				jp[last_jump4].bit_jump2[0] = target;
			else if (!~jp[last_jump4].bit_jump2[1])
				jp[last_jump4].bit_jump2[1] = target;
			else goto label_fail;
		}
		if (!~d->root_jump2) d->root_jump2 = target;
		d->edge_jump2 = target;
		d->jumper_number += 1;
		r->jumper_number += 1;
		return target;
		label_fail:
		r->jumper_buffer.used -= sizeof(jumper);
	}
	return media_huffman_decode_empty;
}

static void media_huffman_decode_free_func(struct media_huffman_decode_s *restrict r)
{
	exbuffer_uini(&r->jumper_buffer);
}

struct media_huffman_decode_s* media_huffman_decode_alloc(media_huffman_index_t decode_number)
{
	struct media_huffman_decode_s *restrict r;
	uintptr_t size;
	media_huffman_index_t i;
	size = sizeof(struct media_huffman_decode_s) + sizeof(struct media_huffman_decode_t) * decode_number;
	if (decode_number && (r = (struct media_huffman_decode_s *) refer_alloz(size)))
	{
		refer_set_free(r, (refer_free_f) media_huffman_decode_free_func);
		r->decode_number = decode_number;
		for (i = 0; i < decode_number; ++i)
		{
			r->decode[i].root_jump2 = media_huffman_decode_empty;
			r->decode[i].edge_jump2 = media_huffman_decode_empty;
		}
		return r;
	}
	return NULL;
}

struct media_huffman_decode_s* media_huffman_decode_add_bits(struct media_huffman_decode_s *restrict hmdc, media_huffman_index_t index)
{
	struct media_huffman_decode_t *restrict d;
	if (index < hmdc->decode_number)
	{
		d = hmdc->decode + index;
		index = media_huffman_decode_find_empty_jumper(hmdc, d);
		if ((~index || !d->jumper_number) &&
			~media_huffman_decode_append_jumper(hmdc, d, index))
		{
			d->bits_count += 1;
			return hmdc;
		}
	}
	return NULL;
}

struct media_huffman_decode_s* media_huffman_decode_add_value(struct media_huffman_decode_s *restrict hmdc, media_huffman_index_t index, const void *value)
{
	struct media_huffman_decode_t *restrict d;
	struct media_huffman_jumper_t *restrict jp;
	if (index < hmdc->decode_number)
	{
		d = hmdc->decode + index;
		if (d->bits_count && ~(index = media_huffman_decode_find_empty_jumper(hmdc, d)))
		{
			jp = hmdc->jumper;
			while (jp[index].bits_count < d->bits_count)
			{
				if (!~(index = media_huffman_decode_append_jumper(hmdc, d, index)))
					goto label_fail;
				jp = hmdc->jumper;
			}
			if (!~jp[index].bit_jump2[0])
			{
				jp[index].bit_value[0] = value;
				jp[index].bit_jump2[0] = media_huffman_decode_tail;
			}
			else if (!~jp[index].bit_jump2[1])
			{
				jp[index].bit_value[1] = value;
				jp[index].bit_jump2[1] = media_huffman_decode_tail;
			}
			else goto label_fail;
			return hmdc;
		}
	}
	label_fail:
	return NULL;
}

const void* media_huffman_decode_bits(const struct media_huffman_decode_s *restrict hmdc, media_huffman_index_t index, struct media_bits_reader_t *restrict reader)
{
	const struct media_huffman_decode_t *restrict d;
	const struct media_huffman_jumper_t *restrict jp;
	const struct media_huffman_jumper_t *restrict jv;
	uintptr_t p, jn;
	media_bits_t v;
	if (index < hmdc->decode_number)
	{
		d = hmdc->decode + index;
		jp = hmdc->jumper;
		jn = hmdc->jumper_number;
		p = d->root_jump2;
		if (p < jn)
		{
			do {
				if (!media_bits_reader_read_bits(reader, 1, &v))
					goto label_fail;
				jv = jp + p;
				p = jv->bit_jump2[v];
			} while (p < jn);
			if (p == media_huffman_decode_tail)
				return jv->bit_value[v];
		}
	}
	label_fail:
	return NULL;
}
