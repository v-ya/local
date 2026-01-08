#include "bvh.sah.h"

static void iphyee_bvh_sah_bucket_free_func(iphyee_bvh_sah_bucket_s *restrict r)
{
	iphyee_bvh_sah_input_s **restrict p;
	uintptr_t i, n;
	p = r->bucket_array;
	n = r->bucket_count;
	for (i = 0; i < n; ++i)
		refer_free(p[i]);
	exbuffer_uini(&r->bucket);
}

iphyee_bvh_sah_bucket_s* iphyee_bvh_sah_bucket_alloc(void)
{
	iphyee_bvh_sah_bucket_s *restrict r;
	if ((r = (iphyee_bvh_sah_bucket_s *) refer_alloz(sizeof(iphyee_bvh_sah_bucket_s))))
	{
		refer_set_free(r, (refer_free_f) iphyee_bvh_sah_bucket_free_func);
		if (exbuffer_init(&r->bucket, 0))
		{
			iphyee_bvh_box_initial(&r->box);
			return r;
		}
		refer_free(r);
	}
	return NULL;
}

iphyee_bvh_sah_bucket_s* iphyee_bvh_sah_bucket_append(iphyee_bvh_sah_bucket_s *restrict r, iphyee_bvh_sah_input_s *restrict input)
{
	if (input && exbuffer_append(&r->bucket, (const void *) &input, sizeof(iphyee_bvh_sah_input_s *)))
	{
		r->bucket_array = (iphyee_bvh_sah_input_s **) r->bucket.data;
		r->bucket_count += 1;
		refer_save(input);
		iphyee_bvh_box_append(&r->box, &input->box);
		return r;
	}
	return NULL;
}

void iphyee_bvh_sah_bucket_clear(iphyee_bvh_sah_bucket_s *restrict r)
{
	iphyee_bvh_sah_input_s **restrict p;
	uintptr_t i, n;
	p = r->bucket_array;
	n = r->bucket_count;
	for (i = 0; i < n; ++i)
		refer_free(p[i]);
	r->bucket_array = NULL;
	r->bucket_count = 0;
	exbuffer_clear(&r->bucket);
	iphyee_bvh_box_initial(&r->box);
}

iphyee_bvh_sah_input_s** iphyee_bvh_sah_bucket_list(iphyee_bvh_sah_bucket_s *restrict r, iphyee_bvh_sah_input_s **restrict ptail)
{
	iphyee_bvh_sah_input_s **restrict p;
	iphyee_bvh_sah_input_s **restrict pnext;
	uintptr_t i, n;
	p = r->bucket_array;
	n = r->bucket_count;
	for (i = 0; i < n; ++i)
	{
		pnext = &p[i]->next;
		*pnext = *ptail;
		*ptail = p[i];
		ptail = pnext;
	}
	return ptail;
}
