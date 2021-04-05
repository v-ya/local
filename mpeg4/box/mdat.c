#include "box.func.h"

mpeg4$define$dump(box, mdat)
{
	#define mlevel(_n)  (1.0 / (1u << _n))
	mlog_level_dump("media data size: %lu B = %.1f KiB = %.2f MiB = %.2f GiB\n", size, size * mlevel(10), size * mlevel(20), size * mlevel(30));
	return inst;
}
