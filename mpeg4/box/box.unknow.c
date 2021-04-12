#include "box.include.h"
#include "inner.bindata.h"

#define unknow_max_dump_data_length  32

mpeg4$define$dump(box, unknow)
{
	mlog_level_dump("unknow: %u bytes\n", size);
	if (size > unknow_max_dump_data_length)
		size = unknow_max_dump_data_length;
	mpeg4$define(inner, bindata, dump)(mlog, data, size, 0, level + 1);
	return inst;
}
