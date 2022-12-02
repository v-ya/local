#include "codec.jpeg.h"
#include "../0bits/bits.h"
#include <memory.h>

// codec

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
				q->q[i] = (uint32_t) data[i];
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
		if (table_class == 0) target = &jc->h_dc;
		else if (table_class == 1) target = &jc->h_dc;
		else goto label_fail;
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

// scan

struct mi_jpeg_scan_data_t {
	uint64_t offset;
	enum mi_jpeg_segment_type_t type;
	uint32_t mark;
};

static void* media_codec_jpeg_scan_func(struct mi_jpeg_scan_data_t *restrict scan, uint64_t offset, const uint8_t *restrict data, uintptr_t size)
{
	uintptr_t i;
	for (i = 0; i < size; ++i)
	{
		if (!scan->mark)
		{
			if (data[i] == mi_jpeg_segment_type__mark)
				scan->mark = 1;
		}
		else
		{
			switch (data[i])
			{
				case mi_jpeg_segment_type__nul:
				case mi_jpeg_segment_type__rst0:
				case mi_jpeg_segment_type__rst1:
				case mi_jpeg_segment_type__rst2:
				case mi_jpeg_segment_type__rst3:
				case mi_jpeg_segment_type__rst4:
				case mi_jpeg_segment_type__rst5:
				case mi_jpeg_segment_type__rst6:
				case mi_jpeg_segment_type__rst7:
					scan->mark = 0;
					// fall through
				case mi_jpeg_segment_type__mark:
					break;
				default:
					scan->offset = offset + i + 1;
					scan->type = data[i];
					scan->mark = 0;
					return scan;
			}
		}
	}
	return NULL;
}

void media_codec_jpeg_scan_initial(struct mi_jpeg_scan_t *restrict scan)
{
	scan->offset_segment_start = 0;
	scan->offset_segment_data = 0;
	scan->offset_segment_next = 0;
	scan->segment_data_length = 0;
	scan->type = mi_jpeg_segment_type__nul;
}

struct mi_jpeg_scan_t* media_codec_jpeg_scan_fetch(struct mi_jpeg_scan_t *restrict scan, struct media_io_s *restrict io)
{
	struct mi_jpeg_scan_data_t data;
	uintptr_t size;
	uint16_t length;
	data.offset = scan->offset_segment_next;
	data.type = mi_jpeg_segment_type__nul;
	data.mark = 0;
	if (media_io_offset(io, &data.offset) == data.offset &&
		media_io_scan(io, (media_io_user_scan_f) media_codec_jpeg_scan_func, &data) &&
		media_io_offset(io, &data.offset) == data.offset)
	{
		switch (data.type)
		{
			case mi_jpeg_segment_type__tem:
			case mi_jpeg_segment_type__rst0:
			case mi_jpeg_segment_type__rst1:
			case mi_jpeg_segment_type__rst2:
			case mi_jpeg_segment_type__rst3:
			case mi_jpeg_segment_type__rst4:
			case mi_jpeg_segment_type__rst5:
			case mi_jpeg_segment_type__rst6:
			case mi_jpeg_segment_type__rst7:
			case mi_jpeg_segment_type__soi:
			case mi_jpeg_segment_type__eoi:
				size = 0;
				break;
			default:
				if (media_io_read(io, &length, sizeof(length)) != sizeof(length))
					goto label_fail;
				size = (uintptr_t) media_n2be_16(length);
				if (size < sizeof(length))
					goto label_fail;
				break;
		}
		scan->offset_segment_start = data.offset - 2;
		scan->offset_segment_data = data.offset;
		if (size)
		{
			scan->offset_segment_data += sizeof(length);
			size -= sizeof(length);
		}
		scan->offset_segment_next = scan->offset_segment_data + size;
		scan->segment_data_length = size;
		scan->type = data.type;
		return scan;
	}
	label_fail:
	return NULL;
}
