#ifndef _media_image_container_bmp_h_
#define _media_image_container_bmp_h_

#include "container.h"

struct media_container_id_bmp_s {
	struct media_container_id_s id;
	hashmap_t magic;
};

struct media_container_pri_bmp_s {
	const struct media_container_id_bmp_s *restrict id;
	uint32_t magic;
	uint32_t version;
	uint32_t pixel_offset;
	uint32_t image_size;
	// info ...
	int32_t width;
	int32_t height;
	uint16_t color_plane;
	uint16_t bpp;
	uint32_t compression;
	uint32_t xppm;
	uint32_t yppm;
	uint32_t color_palette;
	uint32_t used_color;
};

// packed
//     file-header    (mi_bmp_header_t)  must
//     info-size      (uint32_t)         must
//     info-data      (mi_bmp_info_*_t)  must
//     bit-mask       (uint32_t[])       maybe  (compression_method == BITFIELDS | ALPHABITFIELDS) => ([3] | [4])
//     color-table    *                  maybe  color depth <= 8
// insert
//     pixel-array    *                  must   pixels data, must linesize padding to 4 * n
//     color-profile  *                  maybe  ICC color profile

struct mi_bmp_header_t {
	uint16_t magic;
	uint32_t bmp_file_size;
	uint16_t reserve1;
	uint16_t reserve2;
	uint32_t pixel_offset;
} __attribute__ ((packed));

struct mi_bmp_info_12_t {
	uint16_t width;
	uint16_t height;
	uint16_t color_plane;
	uint16_t bpp;
} __attribute__ ((packed));

struct mi_bmp_info_40_t {
	int32_t width;
	int32_t height;
	uint16_t color_plane;
	uint16_t bpp;                 // 1, 4, 8, 16, 24, 32
	uint32_t compression_method;  // 0(RGB), 1(RLE8), 2(RLE4), 3(BITFIELDS), 4(JPEG), 5(PNG), 6(ALPHABITFIELDS), 11(CMYK), 12(CMYKRLE8), 13(CMYKRLE4)
	uint32_t image_size;          // size of the raw bitmap data (if RGB, image_size = 0)
	uint32_t xppm;
	uint32_t yppm;
	uint32_t color_palette;       // 0 and 2^n
	uint32_t used_color;          // 0 is every color
} __attribute__ ((packed));

// container

d_media_container__initial_judge(bmp);
d_media_container__create_pri(bmp);
d_media_container__parse_head(bmp);
#define media_container__parse_tail__bmp NULL
#define media_container__build_head__bmp NULL
#define media_container__build_tail__bmp NULL

// stream

#endif
