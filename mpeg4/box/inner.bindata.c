#include "inner.bindata.h"

void mpeg4$define(inner, bindata, dump)(mlog_s *restrict mlog, const uint8_t *restrict data, uint64_t size, uintptr_t n, uint32_t level)
{
	uintptr_t i;
	if (!n) n = 16;
	if (n > 64) n = 64;
	while (size >= n)
	{
		mlog_level_dump("");
		for (i = 0; i < n; ++i)
			mlog_printf(mlog, "%02x ", data[i]);
		mlog_printf(mlog, "| ");
		for (i = 0; i < n; ++i)
			mlog_printf(mlog, "%c", (data[i] >= ' ' && data[i] <= '~')?data[i]:'.');
		mlog_printf(mlog, "\n");
		data += n;
		size -= n;
	}
	if (!size) goto label_return;
	mlog_level_dump("");
	for (i = 0; i < size; ++i)
		mlog_printf(mlog, "%02x ", data[i]);
	mlog_printf(mlog, "%*c| ", (n - size) * 3 + 1, 0);
	for (i = 0; i < size; ++i)
		mlog_printf(mlog, "%c", (data[i] >= ' ' && data[i] <= '~')?data[i]:'.');
	mlog_printf(mlog, "\n");
	label_return:
	return ;
}
