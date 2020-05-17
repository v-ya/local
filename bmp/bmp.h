#ifndef _wav_h_
#define _wav_h_

#include <stdint.h>

typedef struct bmp_header_t {
	uint16_t bm;
	uint32_t size;
	uint32_t res;
	uint32_t offset;
	uint32_t header_size;
	uint32_t width;
	int32_t height;
	uint16_t planes;
	uint16_t bits;
	uint32_t compression;
	uint32_t image_size;
	int32_t xppm;
	int32_t yppm;
	uint32_t color_used;
	uint32_t color_important;
	uint8_t res_algin[2];
} __attribute__((packed)) bmp_header_t;

int bmp_save_bgra(const char *path, uint32_t width, int32_t height, uint32_t *data);

#endif
