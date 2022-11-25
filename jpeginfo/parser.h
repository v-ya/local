#ifndef _jpeg_parser_h_
#define _jpeg_parser_h_

#include "jpeg.h"
#include "tmlog.h"
#include "huffman.h"
#include <hashmap.h>

typedef struct jpeg_parser_target_t jpeg_parser_target_t;
typedef struct jpeg_parser_segment_s jpeg_parser_segment_s;
typedef struct jpeg_parser_s jpeg_parser_s;

typedef jpeg_parser_s* (*jpeg_parser_segment_parse_f)(jpeg_parser_s *restrict p, jpeg_parser_target_t *restrict t, uintptr_t size);

struct jpeg_parser_target_t {
	const uint8_t *data;
	uintptr_t size;
	uintptr_t pos;
};

struct jpeg_parser_segment_s {
	refer_string_t name;
	refer_string_t desc;
	jpeg_parser_segment_parse_f parse;
};

struct jpeg_parser_s {
	hashmap_t type2parser;  // (enum jpeg_segment_type_t) => (jpeg_parser_segment_s *)
	mlog_s *m;
	tmlog_data_s *td;
};

jpeg_parser_s* jpeg_parser_alloc(mlog_s *restrict m, tmlog_data_s *restrict td);
void jpeg_parser_done(jpeg_parser_s *restrict p, jpeg_parser_target_t *restrict t);

// inner

jpeg_parser_target_t* jpeg_parser_scan_segment(jpeg_parser_target_t *restrict t, enum jpeg_segment_type_t *restrict rt);
jpeg_parser_target_t* jpeg_parser_get_segment_length(jpeg_parser_target_t *restrict t, enum jpeg_segment_type_t type, uintptr_t *restrict length);

jpeg_parser_s* jpeg_parser_add_segment(jpeg_parser_s *restrict p, enum jpeg_segment_type_t type, const char *restrict name, const char *restrict desc, jpeg_parser_segment_parse_f parse);
const jpeg_parser_segment_s* jpeg_parser_get_segment(jpeg_parser_s *restrict p, enum jpeg_segment_type_t type);

void jpeg_parser_print_rawdata(mlog_s *restrict mlog, const char *restrict name, const void *restrict data, uintptr_t size);
void jpeg_parser_print_mat_u8(mlog_s *restrict mlog, const uint8_t *restrict p, uintptr_t row, uintptr_t col);
void jpeg_parser_print_mat_u16(mlog_s *restrict mlog, const uint16_t *restrict p, uintptr_t row, uintptr_t col);
void jpeg_parser_print_mat_x8(mlog_s *restrict mlog, const uint8_t *restrict p, uintptr_t row, uintptr_t col);

// segment parser ...

jpeg_parser_s* jpeg_parser_segment__dht(jpeg_parser_s *restrict p, jpeg_parser_target_t *restrict t, uintptr_t size);
jpeg_parser_s* jpeg_parser_segment__sof(jpeg_parser_s *restrict p, jpeg_parser_target_t *restrict t, uintptr_t size);
#define jpeg_parser_segment__sof0   jpeg_parser_segment__sof
#define jpeg_parser_segment__sof1   jpeg_parser_segment__sof
#define jpeg_parser_segment__sof2   jpeg_parser_segment__sof
#define jpeg_parser_segment__sof3   jpeg_parser_segment__sof
#define jpeg_parser_segment__sof9   jpeg_parser_segment__sof
#define jpeg_parser_segment__sof10  jpeg_parser_segment__sof
#define jpeg_parser_segment__sof11  jpeg_parser_segment__sof
#define jpeg_parser_segment__dhp    jpeg_parser_segment__sof
#define jpeg_parser_segment__rst0   NULL
#define jpeg_parser_segment__rst1   NULL
#define jpeg_parser_segment__rst2   NULL
#define jpeg_parser_segment__rst3   NULL
#define jpeg_parser_segment__rst4   NULL
#define jpeg_parser_segment__rst5   NULL
#define jpeg_parser_segment__rst6   NULL
#define jpeg_parser_segment__rst7   NULL
#define jpeg_parser_segment__soi    NULL
#define jpeg_parser_segment__eoi    NULL
jpeg_parser_s* jpeg_parser_segment__sos(jpeg_parser_s *restrict p, jpeg_parser_target_t *restrict t, uintptr_t size);
jpeg_parser_s* jpeg_parser_segment__dqt(jpeg_parser_s *restrict p, jpeg_parser_target_t *restrict t, uintptr_t size);

// inner marco

#define parser_debits_4l(_v)      (_v & 0xf)
#define parser_debits_4h(_v)      ((_v >> 4) & 0xf)
#define parser_debits_be_4_4(_v)  parser_debits_4h(_v), parser_debits_4l(_v)

// temp
// MCU: Zig-zag sequence of quantized DCT coefficients
//  0  1  5  6 14 15 27 28
//  2  4  7 13 16 26 29 42
//  3  8 12 17 25 30 41 43
//  9 11 18 24 31 40 44 53
// 10 19 23 32 39 45 52 54
// 20 22 33 38 46 51 55 60
// 21 34 37 47 50 56 59 61
// 35 36 48 49 57 58 62 63

#endif
