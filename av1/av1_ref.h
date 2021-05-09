#ifndef _av1_ref_h_
#define _av1_ref_h_

#include <stdint.h>
#include "obu/define.h"

typedef struct av1_ref_t {
	uint8_t RefValid[NUM_REF_FRAMES];
	uint8_t RefFrameType[NUM_REF_FRAMES];
	uint8_t RefOrderHint[NUM_REF_FRAMES];
} av1_ref_t;

av1_ref_t* av1_ref_init(av1_ref_t *restrict ref);

#endif
