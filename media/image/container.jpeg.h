#ifndef _media_image_container_jpeg_h_
#define _media_image_container_jpeg_h_

#include "container.h"

struct media_container_id_jpeg_s {
	struct media_container_id_s id;
};

enum mi_jpeg_segment_type_t {
// JPEG reserved
	mi_jpeg_segment_type__nul   = 0x00, //  JPEG reserved
	mi_jpeg_segment_type__tem   = 0x01, //* temporary marker for arithmetic coding
	mi_jpeg_segment_type__mark  = 0xff, //  segment mark code && padding
// JPEG 1994 (ITU T.81 | ISO IEC 10918-1)
	// Start Of Frame markers, non-differential, Huffman coding
	mi_jpeg_segment_type__sof0  = 0xc0, //  start of frame (baseline DCT)
	mi_jpeg_segment_type__sof1  = 0xc1, //  start of frame (extended sequential DCT)
	mi_jpeg_segment_type__sof2  = 0xc2, //  start of frame (progressive DCT)
	mi_jpeg_segment_type__sof3  = 0xc3, //  start of frame (lossless (sequential))
	// Start Of Frame markers, differential, Huffman coding
	mi_jpeg_segment_type__sof5  = 0xc5, //  start of frame (differential sequential DCT)
	mi_jpeg_segment_type__sof6  = 0xc6, //  start of frame (differential progressive DCT)
	mi_jpeg_segment_type__sof7  = 0xc7, //  start of frame (differential lossless (sequential))
	// Start Of Frame markers, non-differential, arithmetic coding
	mi_jpeg_segment_type__jpg   = 0xc8, //  reserved for JPEG extension
	mi_jpeg_segment_type__sof9  = 0xc9, //  start of frame (extended sequential DCT)
	mi_jpeg_segment_type__sof10 = 0xca, //  start of frame (progressive DCT)
	mi_jpeg_segment_type__sof11 = 0xcb, //  start of frame (lossless (sequential))
	// Start Of Frame markers, differential, arithmetic coding
	mi_jpeg_segment_type__sof13 = 0xcd, //  start of frame (differential sequential DCT)
	mi_jpeg_segment_type__sof14 = 0xce, //  start of frame (differential progressive DCT)
	mi_jpeg_segment_type__sof15 = 0xcf, //  start of frame (differential lossless (sequential))
	// Huffman table specification
	mi_jpeg_segment_type__dht   = 0xc4, //  define huffman table(s)
	// Arithmetic coding conditioning specification
	mi_jpeg_segment_type__dac   = 0xcc, //  define arithmetic coding conditioning(s)
	// Restart interval termination
	mi_jpeg_segment_type__rst0  = 0xd0, //* restart marker 0
	mi_jpeg_segment_type__rst1  = 0xd1, //* restart marker 1
	mi_jpeg_segment_type__rst2  = 0xd2, //* restart marker 2
	mi_jpeg_segment_type__rst3  = 0xd3, //* restart marker 3
	mi_jpeg_segment_type__rst4  = 0xd4, //* restart marker 4
	mi_jpeg_segment_type__rst5  = 0xd5, //* restart marker 5
	mi_jpeg_segment_type__rst6  = 0xd6, //* restart marker 6
	mi_jpeg_segment_type__rst7  = 0xd7, //* restart marker 7
	// Other markers
	mi_jpeg_segment_type__soi   = 0xd8, //* start of image
	mi_jpeg_segment_type__eoi   = 0xd9, //* end of image
	mi_jpeg_segment_type__sos   = 0xda, //  start of scan
	mi_jpeg_segment_type__dqt   = 0xdb, //  define quantization table(s)
	mi_jpeg_segment_type__dnl   = 0xdc, //  define number of lines
	mi_jpeg_segment_type__dri   = 0xdd, //  define restart interval
	mi_jpeg_segment_type__dhp   = 0xde, //  define hierarchical progression
	mi_jpeg_segment_type__exp   = 0xdf, //  expand reference component(s)
	mi_jpeg_segment_type__com   = 0xfe, //  comment
	// JPEG 1997 (ITU T.84 | ISO IEC 10918-3)
	mi_jpeg_segment_type__app0  = 0xe0, //  application segment 0  (JFIF (len >=14) / JFXX (len >= 6) / AVI MJPEG)
	mi_jpeg_segment_type__app1  = 0xe1, //  application segment 1  (EXIF/XMP/XAP)
	mi_jpeg_segment_type__app2  = 0xe2, //  application segment 2  (FlashPix / ICC)
	mi_jpeg_segment_type__app3  = 0xe3, //  application segment 3  (Kodak/...)
	mi_jpeg_segment_type__app4  = 0xe4, //  application segment 4  (FlashPix/...)
	mi_jpeg_segment_type__app5  = 0xe5, //  application segment 5  (Ricoh...)
	mi_jpeg_segment_type__app6  = 0xe6, //  application segment 6  (GoPro...)
	mi_jpeg_segment_type__app7  = 0xe7, //  application segment 7  (Pentax/Qualcomm)
	mi_jpeg_segment_type__app8  = 0xe8, //  application segment 8  (Spiff)
	mi_jpeg_segment_type__app9  = 0xe9, //  application segment 9  (MediaJukebox)
	mi_jpeg_segment_type__app10 = 0xea, //  application segment 10 (PhotoStudio)
	mi_jpeg_segment_type__app11 = 0xeb, //  application segment 11 (HDR)
	mi_jpeg_segment_type__app12 = 0xec, //  application segment 12 (photoshoP ducky / savE foR web)
	mi_jpeg_segment_type__app13 = 0xed, //  application segment 13 (photoshoP savE As)
	mi_jpeg_segment_type__app14 = 0xee, //  application segment 14 ("adobe" (length = 12))
	mi_jpeg_segment_type__app15 = 0xef, //  application segment 15 (GraphicConverter)
	mi_jpeg_segment_type__jpg0  = 0xf0, //  extension data 0
	mi_jpeg_segment_type__jpg1  = 0xf1, //  extension data 1
	mi_jpeg_segment_type__jpg2  = 0xf2, //  extension data 2
	mi_jpeg_segment_type__jpg3  = 0xf3, //  extension data 3
	mi_jpeg_segment_type__jpg4  = 0xf4, //  extension data 4
	mi_jpeg_segment_type__jpg5  = 0xf5, //  extension data 5
	mi_jpeg_segment_type__jpg6  = 0xf6, //  extension data 6
	mi_jpeg_segment_type__jpg7  = 0xf7, //  extension data 7
	mi_jpeg_segment_type__jpg8  = 0xf8, //  extension data 8
	mi_jpeg_segment_type__jpg9  = 0xf9, //  extension data 9
	mi_jpeg_segment_type__jpg10 = 0xfa, //  extension data 10
	mi_jpeg_segment_type__jpg11 = 0xfb, //  extension data 11
	mi_jpeg_segment_type__jpg12 = 0xfc, //  extension data 12
	mi_jpeg_segment_type__jpg13 = 0xfd, //  extension data 13
};

// MUST: sof0(0xc0) sof1(0xc1) dht(0xc4) soi(0xd8) eoi(0xd9) sos(0xda) dqt(0xdb) dri(0xdd) app0(0xe0) com(0xfe)
// native data is big-order

// container

#define media_container__initial_judge__jpeg NULL
#define media_container__create_pri__jpeg NULL
d_media_container__parse_head(jpeg);
#define media_container__parse_tail__jpeg NULL
#define media_container__build_head__jpeg NULL
#define media_container__build_tail__jpeg NULL

// stream

#endif