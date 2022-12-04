#ifndef _media_attr_h_
#define _media_attr_h_

// container

#define media_naci_width   "c.width"
#define media_naci_height  "c.height"
#define media_naci_bpp     "c.bpp"     // bits per pixel

#define media_nacs_bmp_magic          "c.bmp.magic"          // "BM", "BA", "CI", "CP", "IC", "PT"
#define media_naci_bmp_version        "c.bmp.version"        // 12, 16, 40, 52, 56, 64, 108, 124
#define media_naci_bmp_color_plane    "c.bmp.color_plane"    // 1
#define media_naci_bmp_compression    "c.bmp.compression"    // 0(RGB), 1(RLE8), 2(RLE4), 3(BITFIELDS), 4(JPEG), 5(PNG), 6(ALPHABITFIELDS), 11(CMYK), 12(CMYKRLE8), 13(CMYKRLE4)
#define media_naci_bmp_xppm           "c.bmp.xppm"           // x: pixel per metre
#define media_naci_bmp_yppm           "c.bmp.yppm"           // y: pixel per metre
#define media_naci_bmp_color_palette  "c.bmp.color_palette"  // 0 and 2^n
#define media_naci_bmp_used_color     "c.bmp.used_color"     // 0 is every color
#define media_naci_bmp_pixels_align   "c.bmp.pixels_align"   // [1, 16]

// stream

#define media_nasi_width   "s.width"
#define media_nasi_height  "s.height"
#define media_nasi_bpc     "s.bpc"     // bits per channel

#define media_nasi_jpeg_channel  "s.jpeg.channel"  // 3

// frame

// transform

#endif
