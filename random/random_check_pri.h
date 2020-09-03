#ifndef _random_check_pri_h_
#define _random_check_pri_h_

#include "random_check.h"
#include <memory.h>
#include <stdio.h>
#include <math.h>

typedef struct random_check_layer_s random_check_layer_s;
typedef void (*random_check_layer_check_f)(random_check_layer_s *restrict layer, uint32_t r);
typedef void (*random_check_layer_clear_f)(random_check_layer_s *restrict layer);
typedef void (*random_check_layer_dump_f)(random_check_layer_s *restrict layer, mlog_s *ml);

struct random_check_s {
	random_check_layer_s *layer;
	random_check_layer_s **pnext;
	mlog_s *ml;
};

struct random_check_layer_s {
	random_check_layer_s *next;
	random_check_layer_check_f check;
	random_check_layer_clear_f clear;
	random_check_layer_dump_f dump;
	// private data ...
};

void random_check_dump_statistics(mlog_s *ml, register const uint32_t *restrict data, register uint32_t number);

#define random_check_define_layer_check(_name)  static void random_check_layer_##_name##_check(register struct random_check_layer_##_name##_s *restrict r, register uint32_t x)
#define random_check_define_layer_clear(_name)  static void random_check_layer_##_name##_clear(register struct random_check_layer_##_name##_s *restrict r)
#define random_check_define_layer_dump(_name)   static void random_check_layer_##_name##_dump(register struct random_check_layer_##_name##_s *restrict r, mlog_s *ml)
#define random_check_append_layer(_rc, _layer, _name)  {\
		(_layer).next = NULL;\
		(_layer).check = (random_check_layer_check_f) random_check_layer_##_name##_check;\
		(_layer).clear= (random_check_layer_clear_f) random_check_layer_##_name##_clear;\
		(_layer).dump = (random_check_layer_dump_f) random_check_layer_##_name##_dump;\
		*(_rc)->pnext = &(_layer);\
		(rc)->pnext = &(_layer).next;\
	}

#endif
