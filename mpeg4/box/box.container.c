#include "box.include.h"

mpeg4$define$dump(box, container)
{
	mpeg4_dump_data_t ud = *unidata;
	return mpeg4_dump_level(inst, mlog, data, size, &ud, level);
}
