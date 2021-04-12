#include "box.include.h"
#include "inner.bindata.h"

#define free_max_dump_data_length  128

mpeg4$define$dump(box, free_skip)
{
	mlog_level_dump("free space: %lu bytes\n", size);
	if (size > free_max_dump_data_length)
		size = free_max_dump_data_length;
	mpeg4$define(inner, bindata, dump)(mlog, data, size, 0, level + 1);
	return inst;
}
