#ifndef _mpeg4_inner_trackid_h_
#define _mpeg4_inner_trackid_h_

#include "box.include.h"

typedef struct inner_trackid_32_t {
	uint32_t creation_time;
	uint32_t modification_time;
	uint32_t track_id;
	uint32_t reserved;
	uint32_t duration;
} __attribute__ ((packed)) inner_trackid_32_t;

typedef struct inner_trackid_64_t {
	uint64_t creation_time;
	uint64_t modification_time;
	uint32_t track_id;
	uint32_t reserved;
	uint64_t duration;
} __attribute__ ((packed)) inner_trackid_64_t;

typedef struct inner_trackid_t {
	uint64_t creation_time;
	uint64_t modification_time;
	uint32_t track_id;
	uint64_t duration;
} inner_trackid_t;

inner_trackid_t* mpeg4$define(inner, trackid32, get)(inner_trackid_t *restrict r, const uint8_t *restrict *restrict data, uint64_t *restrict size);
inner_trackid_t* mpeg4$define(inner, trackid64, get)(inner_trackid_t *restrict r, const uint8_t *restrict *restrict data, uint64_t *restrict size);

#endif
