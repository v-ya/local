#ifndef _jpeg_parser_h_
#define _jpeg_parser_h_

#include "jpeg.h"
#include "tmlog.h"
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

// segment parser ...

#define jpeg_parser_segment__soi  NULL
#define jpeg_parser_segment__eoi  NULL

#endif
