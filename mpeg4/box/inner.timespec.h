#ifndef _mpeg4_inner_timespec_h_
#define _mpeg4_inner_timespec_h_

#include "box.include.h"

typedef struct inner_timespec_32_t {
	uint32_t creation_time;
	uint32_t modification_time;
	uint32_t timescale;
	uint32_t duration;
} __attribute__ ((packed)) inner_timespec_32_t;

typedef struct inner_timespec_64_t {
	uint64_t creation_time;
	uint64_t modification_time;
	uint32_t timescale;
	uint64_t duration;
} __attribute__ ((packed)) inner_timespec_64_t;

typedef struct inner_timespec_t {
	uint64_t creation_time;
	uint64_t modification_time;
	uint32_t timescale;
	uint64_t duration;
} inner_timespec_t;

inner_timespec_t* mpeg4$define(inner, timespec32, get)(inner_timespec_t *restrict r, const uint8_t *restrict *restrict data, uint64_t *restrict size);
inner_timespec_t* mpeg4$define(inner, timespec64, get)(inner_timespec_t *restrict r, const uint8_t *restrict *restrict data, uint64_t *restrict size);

// buffer[32]
const char* mpeg4$define(inner, time1904, string)(char *restrict buffer, uint64_t t1904);
// buffer[64]
const char* mpeg4$define(inner, duration, string)(char *restrict buffer, double duration);

#endif
