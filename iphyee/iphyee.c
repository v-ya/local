#include "iphyee.pri.h"
#include "bonex/bonex.h"

static void iphyee_free_func(iphyee_s *restrict r)
{
	if (r->bonex) refer_free(r->bonex);
}

iphyee_s* iphyee_alloc(void)
{
	iphyee_s *restrict r;
	if ((r = (iphyee_s *) refer_alloz(sizeof(iphyee_s))))
	{
		refer_set_free(r, (refer_free_f) iphyee_free_func);
		if ((r->bonex = vattr_alloc()))
			return r;
		refer_free(r);
	}
	return NULL;
}

const iphyee_bonex_s* iphyee_create_bonex(iphyee_s *restrict r, const iphyee_param_bonex_t *restrict param)
{
	return iphyee_bonex_create(r->bonex, param);
}
