#ifndef _note_details_h_
#define _note_details_h_

#include <stdint.h>
#include <refer.h>

typedef struct note_details_saq_s {
	double sa;
	double sq;
} note_details_saq_s;

typedef struct note_details_s {
	uint32_t used;
	uint32_t max;
	note_details_saq_s saq[];
} note_details_s;

note_details_s* note_details_alloc(uint32_t max);
void note_details_gen(double *v, uint32_t n, note_details_s *nd, uint32_t l, double a, double q);
void note_details_get(note_details_s *nd, double *v, uint32_t frames);

#endif
