#include "media.h"
#include "0core/media.h"
#include "media.frame.h"

const media_s* media_alloc(void)
{
	media_s *restrict r;
	if ((r = media_alloc_empty()))
	{
		// register ...
		return r;
		// refer_free(r);
	}
	return NULL;
}
