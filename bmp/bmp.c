#include "bmp.h"
#include <stdio.h>
#include <memory.h>

int bmp_save_bgra(const char *path, uint32_t width, int32_t height, uint32_t *data)
{
	static const bmp_header_t header = {
		.bm = 'MB',
		.size = 0,
		.res = 0,
		.offset = sizeof(bmp_header_t),
		.header_size = 40,
		.width = 0,
		.height = 0,
		.planes = 1,
		.bits = 32,
		.compression = 0,
		.image_size = 0,
		.xppm = 0,
		.yppm = 0,
		.color_used = 0,
		.color_important = 0,
		.res_algin = {0, 0}
	};
	bmp_header_t h;
	FILE *fp;
	uint32_t n;
	int r;
	r = -1;
	memcpy(&h, &header, sizeof(h));
	h.width = width;
	h.height = height;
	if (height < 0) height = -height;
	n = width * (uint32_t) height;
	if (n)
	{
		h.size = (n << 2) + sizeof(bmp_header_t);
		h.image_size = n << 2;
		fp = fopen(path, "wb");
		if (fp)
		{
			if (fwrite(&h, 1, sizeof(h), fp) == sizeof(h))
			{
				if (fwrite(data, sizeof(uint32_t), n, fp) == n)
					r = 0;
			}
			fclose(fp);
		}
	}
	return r;
}
