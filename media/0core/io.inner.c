#include "io.h"
#include <memory.h>

struct media_io_s* media_io_inner_padding_align(struct media_io_s *restrict io, uint64_t offset, uintptr_t align, uint8_t value)
{
	uint8_t padding[64];
	uint64_t cpos;
	uintptr_t need_padding, n;
	if (align <= 1) goto label_okay;
	if ((cpos = media_io_offset(io, NULL)) >= offset)
	{
		if ((need_padding = (align - (cpos - offset) % align) % align))
		{
			for (n = 0; n < need_padding && n < sizeof(padding); ++n)
				padding[n] = value;
			do {
				n = (need_padding <= sizeof(padding))?need_padding:sizeof(padding);
				if (media_io_write(io, padding, n) != n)
					goto label_fail;
				need_padding -= n;
			} while (need_padding);
		}
		label_okay:
		return io;
	}
	label_fail:
	return NULL;
}
