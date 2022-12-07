#include "codec.jpeg.h"
#include "../media.frame.h"
#include <memory.h>

static void mi_jpeg_codec_free_func(struct mi_jpeg_codec_s *restrict r)
{
	if (r->sof) refer_free(r->sof);
	if (r->sos) refer_free(r->sos);
	rbtree_clear(&r->q);
	rbtree_clear(&r->h_dc);
	rbtree_clear(&r->h_ac);
}

struct mi_jpeg_codec_s* mi_jpeg_codec_alloc(void)
{
	struct mi_jpeg_codec_s *restrict r;
	if ((r = (struct mi_jpeg_codec_s *) refer_alloz(sizeof(struct mi_jpeg_codec_s))))
	{
		refer_set_free(r, (refer_free_f) mi_jpeg_codec_free_func);
		return r;
	}
	return NULL;
}

void mi_jpeg_codec_clear(struct mi_jpeg_codec_s *restrict jc)
{
	mi_jpeg_codec_free_func(jc);
	jc->sof = NULL;
	jc->sos = NULL;
}

static void mi_jpeg_codec_rbtree_free_func(rbtree_t *restrict rbv)
{
	if (rbv->value) refer_free(rbv->value);
}

struct mi_jpeg_codec_s* mi_jpeg_codec_load_q(struct mi_jpeg_codec_s *restrict jc, struct media_io_s *restrict io, uintptr_t size)
{
	struct mi_jpeg_quantization_s *restrict q;
	uintptr_t i, n;
	uint32_t precision, id;
	uint8_t qi;
	q = NULL;
	n = 64;
	while (size >= sizeof(qi))
	{
		if (media_io_read(io, &qi, sizeof(qi)) != sizeof(qi))
			goto label_fail;
		size -= sizeof(qi);
		precision = (uint32_t) (qi >> 4);
		id = (uint32_t) (qi & 15);
		if (rbtree_find(&jc->q, NULL, (uint64_t) id))
			goto label_fail;
		if (!(q = (struct mi_jpeg_quantization_s *) refer_alloc(sizeof(*q))))
			goto label_fail;
		if (precision == 0)
		{
			uint8_t data[64];
			if (size < sizeof(data))
				goto label_fail;
			if (media_io_read(io, &data, sizeof(data)) != sizeof(data))
				goto label_fail;
			size -= sizeof(data);
			for (i = 0; i < n; ++i)
				q->q[i] = (uint32_t) data[i];
		}
		else if (precision == 1)
		{
			uint16_t data[64];
			if (size < sizeof(data))
				goto label_fail;
			if (media_io_read(io, &data, sizeof(data)) != sizeof(data))
				goto label_fail;
			size -= sizeof(data);
			for (i = 0; i < n; ++i)
				q->q[i] = (uint32_t) media_n2be_16(data[i]);
		}
		else goto label_fail;
		if (!rbtree_insert(&jc->q, NULL, (uint64_t) id, q, mi_jpeg_codec_rbtree_free_func))
			goto label_fail;
		q = NULL;
	}
	if (!size)
		return jc;
	label_fail:
	if (q) refer_free(q);
	return NULL;
}

struct mi_jpeg_codec_s* mi_jpeg_codec_load_h(struct mi_jpeg_codec_s *restrict jc, struct media_io_s *restrict io, uintptr_t size)
{
	struct mi_jpeg_huffman_s *restrict h;
	rbtree_t *restrict *restrict target;
	uintptr_t i, n, vn;
	uint8_t L[16];
	uint32_t table_class, id;
	uint8_t hi;
	h = NULL;
	n = sizeof(L);
	while (size >= (sizeof(hi) + sizeof(L)))
	{
		if (media_io_read(io, &hi, sizeof(hi)) != sizeof(hi))
			goto label_fail;
		size -= sizeof(hi);
		table_class = (uint32_t) (hi >> 4);
		id = (uint32_t) (hi & 15);
		switch (table_class)
		{
			case 0: target = &jc->h_dc; break;
			case 1: target = &jc->h_ac; break;
			default: goto label_fail;
		}
		if (rbtree_find(target, NULL, (uint64_t) id))
			goto label_fail;
		if (media_io_read(io, L, sizeof(L)) != sizeof(L))
			goto label_fail;
		size -= sizeof(L);
		for (i = vn = 0; i < n; ++i)
			vn += (uintptr_t) L[i];
		if (size < vn)
			goto label_fail;
		if (!(h = (struct mi_jpeg_huffman_s *) refer_alloc(sizeof(*h) + vn)))
			goto label_fail;
		h->size = sizeof(h->L) + vn;
		memcpy(h->L, L, sizeof(L));
		if (media_io_read(io, h->v, vn) != vn)
			goto label_fail;
		size -= vn;
		if (!rbtree_insert(target, NULL, (uint64_t) id, h, mi_jpeg_codec_rbtree_free_func))
			goto label_fail;
		h = NULL;
	}
	if (!size)
		return jc;
	label_fail:
	if (h) refer_free(h);
	return NULL;
}

struct mi_jpeg_segment_sof_t {
	uint8_t sample_bits;
	uint16_t height;
	uint16_t width;
	uint8_t channel_number;
} __attribute__ ((packed));

struct mi_jpeg_segment_sof_channel_t {
	uint8_t channel_id;
	uint8_t hv_per_mcu;  // bits 4-7: h_per_mcu, bits 0-3: v_per_mcu
	uint8_t quantization_id;
} __attribute__ ((packed));

static void mi_jpeg_sof_free_func(struct mi_jpeg_sof_s *restrict r)
{
	rbtree_clear(&r->ch_finder);
}

struct mi_jpeg_codec_s* mi_jpeg_codec_load_sof(struct mi_jpeg_codec_s *restrict jc, struct media_io_s *restrict io, uintptr_t size)
{
	struct mi_jpeg_sof_s *restrict sof;
	struct mi_jpeg_segment_sof_t data_sof;
	struct mi_jpeg_segment_sof_channel_t data_ch;
	uintptr_t i, n;
	sof = NULL;
	if (size >= sizeof(data_sof))
	{
		if (media_io_read(io, &data_sof, sizeof(data_sof)) != sizeof(data_sof))
			goto label_fail;
		size -= sizeof(data_sof);
		n = (uintptr_t) data_sof.channel_number;
		if (size >= (sizeof(data_ch) * n) &&
			(sof = (struct mi_jpeg_sof_s *) refer_alloz(sizeof(struct mi_jpeg_sof_s) + sizeof(struct mi_jpeg_ch_t) * n)))
		{
			refer_set_free(sof, (refer_free_f) mi_jpeg_sof_free_func);
			sof->width = (uint32_t) media_n2be_16(data_sof.width);
			sof->height = (uint32_t) media_n2be_16(data_sof.height);
			sof->ch_depth = (uint32_t) data_sof.sample_bits;
			sof->ch_number = 0;
			for (i = 0; i < n; ++i)
			{
				if (media_io_read(io, &data_ch, sizeof(data_ch)) != sizeof(data_ch))
					goto label_fail;
				size -= sizeof(data_ch);
				sof->ch[i].cid = (uint32_t) data_ch.channel_id;
				sof->ch[i].qid = (uint32_t) data_ch.quantization_id;
				sof->ch[i].mcu_nh = (uint32_t) data_ch.hv_per_mcu >> 4;
				sof->ch[i].mcu_nv = (uint32_t) data_ch.hv_per_mcu & 15;
				if (!rbtree_insert(&sof->ch_finder, NULL, (uint64_t) sof->ch[i].cid, &sof->ch[i], NULL))
					goto label_fail;
				sof->ch_number += 1;
			}
			if (!size)
			{
				if (jc->sof) refer_free(jc->sof);
				jc->sof = sof;
				sof = NULL;
				return jc;
			}
		}
	}
	label_fail:
	if (sof) refer_free(sof);
	return NULL;
}

struct jpeg_segment_sos_head_t {
	uint8_t channel_number;
} __attribute__ ((packed));

struct mi_jpeg_segment_sos_channel_t {
	uint8_t channel_id;
	uint8_t da_of_huffman;  // bits 4-7: dc_of_huffman, bits 0-3: ac_of_huffman
} __attribute__ ((packed));

struct mi_jpeg_segment_sos_tail_t {
	uint8_t start_of_selection;
	uint8_t end_of_selection;
	uint8_t hl_of_approximation_bit_position;
} __attribute__ ((packed));

struct mi_jpeg_codec_s* mi_jpeg_codec_load_sos(struct mi_jpeg_codec_s *restrict jc, struct media_io_s *restrict io, uintptr_t size)
{
	struct mi_jpeg_sos_s *restrict sos;
	const struct mi_jpeg_ch_t *restrict ch;
	struct jpeg_segment_sos_head_t data_head;
	struct mi_jpeg_segment_sos_tail_t data_tail;
	struct mi_jpeg_segment_sos_channel_t data_ch;
	uintptr_t i, n;
	sos = NULL;
	if (size >= sizeof(data_head) + sizeof(data_tail))
	{
		if (media_io_read(io, &data_head, sizeof(data_head)) != sizeof(data_head))
			goto label_fail;
		size -= sizeof(data_head);
		n = (uintptr_t) data_head.channel_number;
		if (size >= (sizeof(data_ch) * n + sizeof(data_tail)) &&
			(sos = (struct mi_jpeg_sos_s *) refer_alloz(sizeof(struct mi_jpeg_sos_s) + sizeof(struct mi_jpeg_ch_scan_t) * n)))
		{
			sos->ch_number = 0;
			for (i = 0; i < n; ++i)
			{
				if (media_io_read(io, &data_ch, sizeof(data_ch)) != sizeof(data_ch))
					goto label_fail;
				size -= sizeof(data_ch);
				if (!(ch = jc->sof?mi_jpeg_sof_find_ch(jc->sof, (uint32_t) data_ch.channel_id):NULL))
					goto label_fail;
				sos->ch[i].cid = (uint32_t) data_ch.channel_id;
				sos->ch[i].qid = ch->qid;
				sos->ch[i].hdcid = (uint32_t) data_ch.da_of_huffman >> 4;
				sos->ch[i].hacid = (uint32_t) data_ch.da_of_huffman & 15;
				sos->ch_number += 1;
			}
			if (media_io_read(io, &data_tail, sizeof(data_tail)) != sizeof(data_tail))
				goto label_fail;
			size -= sizeof(data_tail);
			sos->selection_start = (uint32_t) data_tail.start_of_selection;
			sos->selection_end = (uint32_t) data_tail.end_of_selection;
			sos->abp_high = (uint32_t) data_tail.hl_of_approximation_bit_position >> 4;
			sos->abp_low = (uint32_t) data_tail.hl_of_approximation_bit_position & 15;
			if (!size)
			{
				if (jc->sos) refer_free(jc->sos);
				jc->sos = sos;
				sos = NULL;
				return jc;
			}
		}
	}
	label_fail:
	if (sos) refer_free(sos);
	return NULL;
}

static void mi_jpeg_frame_info_free_func(struct mi_jpeg_frame_info_s *restrict r)
{
	if (r->f) refer_free(r->f);
	if (r->q) refer_free(r->q);
	if (r->h) refer_free(r->h);
}

static void mi_jpeg_codec_create_frame_info_func_write_qh(rbtree_t *restrict rbv, const void *argv[2])
{
	const struct mi_jpeg_frame_info_s *restrict r;
	const struct mi_jpeg_codec_s *restrict jc;
	const struct mi_jpeg_quantization_s *restrict q;
	const struct mi_jpeg_huffman_s *restrict h;
	r = (const struct mi_jpeg_frame_info_s *) argv[0];
	jc = (const struct mi_jpeg_codec_s *) argv[1];
	switch ((uint32_t) (rbv->key_index >> 32))
	{
		case 1: // q
			q = mi_jpeg_codec_find_quantization(jc, (uint32_t) rbv->key_index);
			memcpy(r->q + (uintptr_t) rbv->value, q, sizeof(*q));
			break;
		case 2: // h dc
			h = mi_jpeg_codec_find_huffman_dc(jc, (uint32_t) rbv->key_index);
			memcpy(r->h + (uintptr_t) rbv->value, h->L, h->size);
			break;
		case 3: // h ac
			h = mi_jpeg_codec_find_huffman_ac(jc, (uint32_t) rbv->key_index);
			memcpy(r->h + (uintptr_t) rbv->value, h->L, h->size);
			break;
		default:
			break;
	}
}

struct mi_jpeg_frame_info_s* mi_jpeg_codec_create_frame_info(const struct mi_jpeg_codec_s *restrict jc)
{
	struct mi_jpeg_frame_info_s *restrict r;
	const struct mi_jpeg_sof_s *restrict sof;
	const struct mi_jpeg_sos_s *restrict sos;
	const struct mi_jpeg_ch_t *restrict ch;
	const struct mi_jpeg_quantization_s *restrict q;
	const struct mi_jpeg_huffman_s *restrict h;
	struct mi_jpeg_frame_t *restrict f;
	rbtree_t *hit, *restrict rbv;
	uint64_t key;
	uint32_t i, n;
	const void *argv[2];
	r = NULL;
	if ((sof = jc->sof) && (sos = jc->sos))
	{
		hit = NULL;
		// fill frame, ch
		if ((r = (struct mi_jpeg_frame_info_s *) refer_alloz(sizeof(struct mi_jpeg_frame_info_s))))
		{
			refer_set_free(r, (refer_free_f) mi_jpeg_frame_info_free_func);
			r->f_size = sizeof(struct mi_jpeg_frame_t) + sizeof(struct mi_jpeg_frame_ch_t) * sos->ch_number;
			if (!(r->f = f = (struct mi_jpeg_frame_t *) refer_alloc(r->f_size)))
				goto label_fail;
			f->width = sof->width;
			f->height = sof->height;
			f->depth = sof->ch_depth;
			f->channel = n = sos->ch_number;
			for (i = 0; i < n; ++i)
			{
				if (!(ch = mi_jpeg_sof_find_ch(sof, sos->ch[i].cid)))
					goto label_fail;
				f->ch[i].mcu_nh = ch->mcu_nh;
				f->ch[i].mcu_nv = ch->mcu_nv;
				if (!(q = mi_jpeg_codec_find_quantization(jc, sos->ch[i].qid)))
					goto label_fail;
				if (!(rbv = rbtree_find(&hit, NULL, key = ((uint64_t) 1 << 32) | sos->ch[i].qid)))
				{
					if (!(rbv = rbtree_insert(&hit, NULL, key, (const void *) r->q_size, NULL)))
						goto label_fail;
					r->q_size += sizeof(*q);
				}
				f->ch[i].q_offset = (uintptr_t) rbv->value;
				f->ch[i].q_size = sizeof(*q);
				if (!(h = mi_jpeg_codec_find_huffman_dc(jc, sos->ch[i].hdcid)))
					goto label_fail;
				if (!(rbv = rbtree_find(&hit, NULL, key = ((uint64_t) 2 << 32) | sos->ch[i].hdcid)))
				{
					if (!(rbv = rbtree_insert(&hit, NULL, key, (const void *) r->h_size, NULL)))
						goto label_fail;
					r->h_size += h->size;
				}
				f->ch[i].hdc_offset = (uintptr_t) rbv->value;
				f->ch[i].hdc_size = h->size;
				if (!(h = mi_jpeg_codec_find_huffman_ac(jc, sos->ch[i].hacid)))
					goto label_fail;
				if (!(rbv = rbtree_find(&hit, NULL, key = ((uint64_t) 3 << 32) | sos->ch[i].hacid)))
				{
					if (!(rbv = rbtree_insert(&hit, NULL, key, (const void *) r->h_size, NULL)))
						goto label_fail;
					r->h_size += h->size;
				}
				f->ch[i].hac_offset = (uintptr_t) rbv->value;
				f->ch[i].hac_size = h->size;
			}
			if (!(r->q = (uint8_t *) refer_alloc(r->q_size)) ||
				!(r->h = (uint8_t *) refer_alloc(r->h_size)))
				goto label_fail;
			argv[0] = r;
			argv[1] = jc;
			rbtree_call(&hit, (rbtree_func_call_f) mi_jpeg_codec_create_frame_info_func_write_qh, argv);
		}
		label_fail_clear:
		rbtree_clear(&hit);
	}
	return r;
	label_fail:
	refer_free(r);
	r = NULL;
	goto label_fail_clear;
}

static inline const void* mi_jpeg_rbtree_find_by_id(rbtree_t *const restrict *restrict rbv, uint32_t id)
{
	rbtree_t *restrict v;
	if ((v = rbtree_find(rbv, NULL, (uint64_t) id)))
		return v->value;
	return NULL;
}

const struct mi_jpeg_quantization_s* mi_jpeg_codec_find_quantization(const struct mi_jpeg_codec_s *restrict jc, uint32_t qid)
{
	return (const struct mi_jpeg_quantization_s *) mi_jpeg_rbtree_find_by_id(&jc->q, qid);
}

const struct mi_jpeg_huffman_s* mi_jpeg_codec_find_huffman_dc(const struct mi_jpeg_codec_s *restrict jc, uint32_t hid)
{
	return (const struct mi_jpeg_huffman_s *) mi_jpeg_rbtree_find_by_id(&jc->h_dc, hid);
}

const struct mi_jpeg_huffman_s* mi_jpeg_codec_find_huffman_ac(const struct mi_jpeg_codec_s *restrict jc, uint32_t hid)
{
	return (const struct mi_jpeg_huffman_s *) mi_jpeg_rbtree_find_by_id(&jc->h_ac, hid);
}

const struct mi_jpeg_ch_t* mi_jpeg_sof_find_ch(const struct mi_jpeg_sof_s *restrict sof, uint32_t cid)
{
	return (const struct mi_jpeg_ch_t *) mi_jpeg_rbtree_find_by_id(&sof->ch_finder, cid);
}

const char* mi_jpeg_test_frame_name(const struct mi_jpeg_sof_s *restrict sof, const struct mi_jpeg_sos_s *restrict sos)
{
	const struct mi_jpeg_ch_t *restrict ch1, *restrict ch2, *restrict ch3;
	if (sof && sos)
	{
		if (sos->ch_number == 3)
		{
			ch1 = mi_jpeg_sof_find_ch(sof, sos->ch[0].cid);
			ch2 = mi_jpeg_sof_find_ch(sof, sos->ch[1].cid);
			ch3 = mi_jpeg_sof_find_ch(sof, sos->ch[2].cid);
			if (ch1 && ch2 && ch3 && ch1->cid == 1 && ch2->cid == 2 && ch3->cid == 3)
			{
				if (ch1->mcu_nh == ch2->mcu_nh * 2 && ch1->mcu_nv == ch2->mcu_nv * 2 &&
					ch1->mcu_nh == ch3->mcu_nh * 2 && ch1->mcu_nv == ch3->mcu_nv * 2)
				{
					// yuv 2x2:1x1:1x1
					if (sof->ch_depth == 8)
						return media_nf_jpeg_yuv_8_411;
				}
				else if (ch1->mcu_nh == ch2->mcu_nh && ch1->mcu_nv == ch2->mcu_nv &&
					ch1->mcu_nh == ch3->mcu_nh && ch1->mcu_nv == ch3->mcu_nv)
				{
					// yuv 1x1:1x1:1x1
					if (sof->ch_depth == 8)
						return media_nf_jpeg_yuv_8_111;
				}
			}
		}
	}
	return NULL;
}
