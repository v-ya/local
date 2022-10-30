#ifndef _jpeg_type_h_
#define _jpeg_type_h_

#include <stdint.h>

enum jpeg_segment_type_t {
	// JPEG reserved
	jpeg_segment_type__nul   = 0x00, // JPEG reserved
	jpeg_segment_type__tem   = 0x01, // temporary marker for arithmetic coding
	jpeg_segment_type__mark  = 0xff, // segment mark code && padding
	// JPEG 1994 (ITU T.81 | ISO IEC 10918-1)
	jpeg_segment_type__sof0  = 0xc0, // start of frame (baseline jpeg)
	jpeg_segment_type__sof1  = 0xc1, // start of frame (extended sequential, huffman)
	jpeg_segment_type__sof2  = 0xc2, // start of frame (progressive, huffman)
	jpeg_segment_type__sof3  = 0xc3, // start of frame (lossless, huffman)
	jpeg_segment_type__dht   = 0xc4, // define huffman tables
	jpeg_segment_type__sof5  = 0xc5, // start of frame (differential sequential, huffman)
	jpeg_segment_type__sof6  = 0xc6, // start of frame (differential progressive, huffman)
	jpeg_segment_type__sof7  = 0xc7, // start of frame (differential lossless, huffman)
	jpeg_segment_type__jpg   = 0xc8, // reserved for JPEG extension
	jpeg_segment_type__sof9  = 0xc9, // start of frame (extended sequential, arithmetic)
	jpeg_segment_type__sof10 = 0xca, // start of frame (progressive, arithmetic)
	jpeg_segment_type__sof11 = 0xcb, // start of frame (lossless, arithmetic)
	jpeg_segment_type__dac   = 0xcc, // define arithmetic coding conditioning
	jpeg_segment_type__sof13 = 0xcd, // start of frame (differential sequential, arithmetic)
	jpeg_segment_type__sof14 = 0xce, // start of frame (differential progressive, arithmetic)
	jpeg_segment_type__sof15 = 0xcf, // start of frame (differential lossless, arithmetic)
	jpeg_segment_type__rst0  = 0xd0, // restart marker 0 (NO-PARAMS)
	jpeg_segment_type__rst1  = 0xd1, // restart marker 1 (NO-PARAMS)
	jpeg_segment_type__rst2  = 0xd2, // restart marker 2 (NO-PARAMS)
	jpeg_segment_type__rst3  = 0xd3, // restart marker 3 (NO-PARAMS)
	jpeg_segment_type__rst4  = 0xd4, // restart marker 4 (NO-PARAMS)
	jpeg_segment_type__rst5  = 0xd5, // restart marker 5 (NO-PARAMS)
	jpeg_segment_type__rst6  = 0xd6, // restart marker 6 (NO-PARAMS)
	jpeg_segment_type__rst7  = 0xd7, // restart marker 7 (NO-PARAMS)
	jpeg_segment_type__soi   = 0xd8, // start of image (NO-PARAMS)
	jpeg_segment_type__eoi   = 0xd9, // end of image (NO-PARAMS)
	jpeg_segment_type__sos   = 0xda, // start of scan
	jpeg_segment_type__dqt   = 0xdb, // define quantization table(s)
	jpeg_segment_type__dnl   = 0xdc, // define number of lines
	jpeg_segment_type__dri   = 0xdd, // define restart interval
	jpeg_segment_type__dhp   = 0xde, // define hierarchical progression
	jpeg_segment_type__exp   = 0xdf, // expand reference components
	jpeg_segment_type__com   = 0xfe, // extension data (comment)
	// JPEG 1997 (ITU T.84 | ISO IEC 10918-3)
	jpeg_segment_type__app0  = 0xe0, // application segment 0  (JFIF (len >=14) / JFXX (len >= 6) / AVI MJPEG)
	jpeg_segment_type__app1  = 0xe1, // application segment 1  (EXIF/XMP/XAP)
	jpeg_segment_type__app2  = 0xe2, // application segment 2  (FlashPix / ICC)
	jpeg_segment_type__app3  = 0xe3, // application segment 3  (Kodak/...)
	jpeg_segment_type__app4  = 0xe4, // application segment 4  (FlashPix/...)
	jpeg_segment_type__app5  = 0xe5, // application segment 5  (Ricoh...)
	jpeg_segment_type__app6  = 0xe6, // application segment 6  (GoPro...)
	jpeg_segment_type__app7  = 0xe7, // application segment 7  (Pentax/Qualcomm)
	jpeg_segment_type__app8  = 0xe8, // application segment 8  (Spiff)
	jpeg_segment_type__app9  = 0xe9, // application segment 9  (MediaJukebox)
	jpeg_segment_type__app10 = 0xea, // application segment 10 (PhotoStudio)
	jpeg_segment_type__app11 = 0xeb, // application segment 11 (HDR)
	jpeg_segment_type__app12 = 0xec, // application segment 12 (photoshoP ducky / savE foR web)
	jpeg_segment_type__app13 = 0xed, // application segment 13 (photoshoP savE As)
	jpeg_segment_type__app14 = 0xee, // application segment 14 ("adobe" (length = 12))
	jpeg_segment_type__app15 = 0xef, // application segment 15 (GraphicConverter)
	jpeg_segment_type__jpg0  = 0xf0, // extension data 0
	jpeg_segment_type__jpg1  = 0xf1, // extension data 1
	jpeg_segment_type__jpg2  = 0xf2, // extension data 2
	jpeg_segment_type__jpg3  = 0xf3, // extension data 3
	jpeg_segment_type__jpg4  = 0xf4, // extension data 4
	jpeg_segment_type__jpg5  = 0xf5, // extension data 5
	jpeg_segment_type__jpg6  = 0xf6, // extension data 6
	jpeg_segment_type__sof48 = 0xf7, // start of frame (lossless)
	jpeg_segment_type__lse   = 0xf8, // extension parameters (lossless)
	jpeg_segment_type__jpg9  = 0xf9, // extension data 9
	jpeg_segment_type__jpg10 = 0xfa, // extension data 10
	jpeg_segment_type__jpg11 = 0xfb, // extension data 11
	jpeg_segment_type__jpg12 = 0xfc, // extension data 12
	jpeg_segment_type__jpg13 = 0xfd, // extension data 13
	// JPEG 2000 (JP2) (IEC 15444-1 JPEG 2000)
	jpeg_segment_type__jp2_soc   = 0x4f, // start of codestream
	jpeg_segment_type__jp2_siz   = 0x51, // image and tile size
	jpeg_segment_type__jp2_cod   = 0x52, // coding style default
	jpeg_segment_type__jp2_coc   = 0x53, // coding style component
	jpeg_segment_type__jp2_tlm   = 0x55, // tile-part lengths
	jpeg_segment_type__jp2_plm   = 0x57, // packet length (main header)
	jpeg_segment_type__jp2_plt   = 0x58, // packet length (tile-part header)
	jpeg_segment_type__jp2_qcd   = 0x5c, // quantization default
	jpeg_segment_type__jp2_qcc   = 0x5d, // quantization component
	jpeg_segment_type__jp2_rng   = 0x5e, // region of interest
	jpeg_segment_type__jp2_poc   = 0x5f, // progression order change
	jpeg_segment_type__jp2_ppm   = 0x60, // packed packet headers (main header)
	jpeg_segment_type__jp2_ppt   = 0x61, // packed packet headers (tile-part header)
	jpeg_segment_type__jp2_crg   = 0x63, // component registration
	jpeg_segment_type__jp2_com   = 0x64, // comment
	jpeg_segment_type__jp2_sec   = 0x65, // secured codestream
	jpeg_segment_type__jp2_epb   = 0x66, // error protection block
	jpeg_segment_type__jp2_esd   = 0x67, // error sensitivity descriptor
	jpeg_segment_type__jp2_epc   = 0x68, // error protection capability
	jpeg_segment_type__jp2_red   = 0x69, // residual error descriptor
	jpeg_segment_type__jp2_mct   = 0x74, // multiple component transform
	jpeg_segment_type__jp2_mcc   = 0x75, // multiple component collection
	jpeg_segment_type__jp2_mco   = 0x77, // multiple component transformation ordering
	jpeg_segment_type__jp2_cbd   = 0x78, // component bit depth definition
	jpeg_segment_type__jp2_sop   = 0x91, // start of packet
	jpeg_segment_type__jp2_eph   = 0x92, // end of packet header
	jpeg_segment_type__jp2_insec = 0x94, // insecured codestream
	jpeg_segment_type__jp2_eoc   = 0xd9, // end of codestream (overlaps EOI)
	jpeg_segment_type__jp2_sot   = 0x90, // start of tile
	jpeg_segment_type__jp2_sod   = 0x93, // start of ...?
};

// MUST: sof0(0xc0) sof1(0xc1) dht(0xc4) soi(0xd8) eoi(0xd9) sos(0xda) dqt(0xdb) dri(0xdd) app0(0xe0) com(0xfe)
// native data is big-order

#endif
