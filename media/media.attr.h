#ifndef _media_attr_h_
#define _media_attr_h_

#define media_nai_width   ".width"
#define media_nai_height  ".height"
#define media_nai_bpp     ".bpp"     // bits per pixel

// image bmp

#define media_nas_bmp_magic          "bmp.magic"          // "BM", "BA", "CI", "CP", "IC", "PT"
#define media_nai_bmp_version        "bmp.version"        // 12, 16, 40, 52, 56, 64, 108, 124
#define media_nai_bmp_color_plane    "bmp.color_plane"    // 1
#define media_nai_bmp_compression    "bmp.compression"    // 0(RGB), 1(RLE8), 2(RLE4), 3(BITFIELDS), 4(JPEG), 5(PNG), 6(ALPHABITFIELDS), 11(CMYK), 12(CMYKRLE8), 13(CMYKRLE4)
#define media_nai_bmp_xppm           "bmp.xppm"           // x: pixel per metre
#define media_nai_bmp_yppm           "bmp.yppm"           // y: pixel per metre
#define media_nai_bmp_color_palette  "bmp.color_palette"  // 0 and 2^n
#define media_nai_bmp_used_color     "bmp.used_color"     // 0 is every color
#define media_nai_bmp_pixels_align   "bmp.pixels_align"   // [1, 16]

#endif
